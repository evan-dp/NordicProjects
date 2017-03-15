/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include "es_adv_timing_resolver.h"
#include "sdk_macros.h"

/**@brief Function for finding delay to use after each non-eTLM advertisement.
 *
 * @param[in] adv_interval          Configured advertisement interval.
 * @param[in] num_slots_configured  Number of configured slots.
 * @param[in] eTLM_required         Is there an eTLM slot.
 */
static uint16_t get_adv_delay(uint16_t adv_interval,
                              uint8_t  num_slots_configured,
                              bool     eTLM_required)
{
    // If eTLM is required, don't count this when calculating delay.
    return adv_interval / (num_slots_configured - (eTLM_required ? 1 : 0));
}


/**@brief Function for checking if given slot_no is an EID slot.
 *
 * @param[in] slot_no                   Slot number to check.
 * @param[in] p_eid_slots_configured    Pointer to list of configured EID slots.
 * @param[in] num_eid_slots_configured  Number of EID slots configured.
 */
static bool is_eid(uint8_t slot_no, const uint8_t * p_eid_slots_configured, uint8_t num_eid_slots_configured)
{
    for (uint32_t i = 0; i < num_eid_slots_configured; ++i)
    {
        if (slot_no == p_eid_slots_configured[i])
        {
            return true;
        }
    }

    return false;
}

ret_code_t es_adv_timing_resolve(es_adv_timing_resolver_input_t * p_input)
{
    VERIFY_PARAM_NOT_NULL(p_input);
    
    uint8_t  result_index  = 0;
    bool     eTLM_required = p_input->tlm_configured && p_input->num_eid_slots_configured > 0;
    uint16_t base_delay;

    if (p_input->num_slots_configured == 0)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    base_delay = get_adv_delay(p_input->adv_interval, p_input->num_slots_configured, eTLM_required);

    for (uint32_t i = 0; i < p_input->num_slots_configured; ++i)
    {
        uint8_t slot_no = p_input->p_slots_configured[i];

        if (!(eTLM_required && slot_no == p_input->tlm_slot))
        {
            uint8_t last_non_etlm_slot_index = (p_input->num_slots_configured - (eTLM_required ? 1 : 0)) - 1;
            es_adv_timing_resolver_adv_timing_t * p_current_result = &p_input->p_result->timing_results[result_index];
            p_current_result->slot_no = slot_no;
            p_current_result->is_etlm = false;

            if (i < last_non_etlm_slot_index)
            {
                // If an eTLM is to be advertised for this frame, this value will be changed.
                p_current_result->delay_ms = base_delay;
            }

            result_index++;

            if (eTLM_required &&
                is_eid(slot_no, p_input->p_eid_slots_configured, p_input->num_eid_slots_configured))
            {
                es_adv_timing_resolver_adv_timing_t * p_eTLM_timing_result =
                    &p_input->p_result->timing_results[result_index];

                p_current_result->delay_ms = APP_CONFIG_ADV_FRAME_ETLM_SPACING_MS; // Update delay from EID to eTLM frame.

                p_eTLM_timing_result->slot_no = slot_no; // Point to EID slot-no, as this will be
                                                         // used for finding the correct EIK.
                p_eTLM_timing_result->is_etlm = true;    // Configure as eTLM frame.

                if (base_delay > APP_CONFIG_ADV_FRAME_ETLM_SPACING_MS)
                {
                    p_eTLM_timing_result->delay_ms =
                        base_delay -
                        APP_CONFIG_ADV_FRAME_ETLM_SPACING_MS; // Set delay of eTLM frame.
                }

                else
                {
                    p_eTLM_timing_result->delay_ms = APP_CONFIG_ADV_FRAME_SPACING_MS_MIN;
                }

                result_index++;
            }
        }
    }

    p_input->p_result->len_timing_results = result_index; // Note: index has been increased to equal length of result.
    
    if(p_input->p_result->len_timing_results > 0)
    {
        p_input->p_result->timing_results[p_input->p_result->len_timing_results - 1].delay_ms = 0; // Last Slot does not need delay.
    }

    return NRF_SUCCESS;
}
