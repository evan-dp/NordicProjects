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
#include "sdk_config.h"
#if APP_USBD_HID_GENERIC_ENABLED
#include <string.h>

#include "sdk_common.h"
#include "app_usbd_hid_generic.h"
#include "app_util_platform.h"


/**
 * @ingroup app_usbd_hid_generic
 *
 * Module with types, definitions and API used by HID generic.
 * @{
 */

/**
 * @brief Auxiliary function to access HID generic context data.
 *
 * @param[in] p_generic HID generic instance.
 *
 * @return HID generic class instance data context.
 */
static inline app_usbd_hid_generic_ctx_t *
hid_generic_ctx_get(app_usbd_hid_generic_t const * p_generic)
{
    ASSERT(p_generic != NULL);
    ASSERT(p_generic->specific.p_data != NULL);
    return &p_generic->specific.p_data->ctx;
}

/**
 * @brief Auxiliary function to access HID generic instance data.
 *
 * @param[in] p_inst Class instance data.
 *
 * @return HID generic class instance data.
 */
static inline app_usbd_hid_generic_t const *
hid_generic_get(app_usbd_class_inst_t const * p_inst)
{
    ASSERT(p_inst != NULL);
    return (app_usbd_hid_generic_t const *)p_inst;
}


/**
 * @brief Returns report ID buffer descriptor.
 *
 * @param[in] p_generic Internal HID generic context.
 *
 * @return HID report buffer.
 */
static inline app_usbd_hid_report_buffer_t *
hid_generic_rep_buffer_get(app_usbd_hid_generic_t const * p_generic)
{
    ASSERT(p_generic != NULL);
    app_usbd_hid_inst_t const * p_hinst = &p_generic->specific.inst.hid_inst;

    return app_usbd_hid_rep_buff_in_get(p_hinst);
}

/**
 * @brief Auxiliary function to prepare report transfer buffer to next transfer.
 *
 * @param[in] p_generic HID generic instance.
 *
 * @retval true  Next transfer required.
 * @retval false Next transfer not required.
 */
static inline bool hid_generic_transfer_next(app_usbd_hid_generic_t const * p_generic)
{
    nrf_queue_t const * p_rep_in_queue = p_generic->specific.inst.p_rep_in_queue;
    return !nrf_queue_is_empty(p_rep_in_queue);
}


/**
 * @brief Triggers IN endpoint transfer.
 *
 * @param[in] p_generic HID generic instance.
 *
 * @return Standard error code.
 */
static inline ret_code_t hid_generic_transfer_set(app_usbd_hid_generic_t const * p_generic)
{
    app_usbd_class_inst_t const * p_inst = (app_usbd_class_inst_t const *)p_generic;
    app_usbd_hid_generic_ctx_t *  p_generic_ctx = hid_generic_ctx_get(p_generic);

    nrf_drv_usbd_ep_t ep_addr = app_usbd_hid_epin_addr_get(p_inst);
    app_usbd_hid_state_flag_clr(&p_generic_ctx->hid_ctx,
                                APP_USBD_HID_STATE_FLAG_TRANS_IN_PROGRESS);

    if (!hid_generic_transfer_next(p_generic))
    {
        return NRF_SUCCESS;
    }

    app_usbd_hid_report_buffer_t * p_rep_buff = hid_generic_rep_buffer_get(p_generic);
    nrf_queue_t const * p_rep_in_queue = p_generic->specific.inst.p_rep_in_queue;

    ret_code_t ret = nrf_queue_pop(p_rep_in_queue, p_rep_buff);
    ASSERT(ret == NRF_SUCCESS);

    NRF_DRV_USBD_TRANSFER_IN(transfer, p_rep_buff->p_buff, p_rep_buff->size);
    CRITICAL_REGION_ENTER();
    ret = app_usbd_core_ep_transfer(ep_addr, &transfer);
    if (ret == NRF_SUCCESS)
    {
        app_usbd_hid_state_flag_set(&p_generic_ctx->hid_ctx,
                                    APP_USBD_HID_STATE_FLAG_TRANS_IN_PROGRESS);
    }
    CRITICAL_REGION_EXIT();

    return ret;
}

ret_code_t app_usbd_hid_generic_in_report_set(app_usbd_hid_generic_t const * p_generic,
                                              const void * p_buff,
                                              size_t size)
{
    app_usbd_hid_generic_ctx_t * p_generic_ctx = hid_generic_ctx_get(p_generic);
    nrf_queue_t const * p_rep_in_queue = p_generic->specific.inst.p_rep_in_queue;
    const app_usbd_hid_report_buffer_t rep_buff = {
        .p_buff = (void *)p_buff,
        .size = size,
    };

    if (nrf_queue_push(p_rep_in_queue, &rep_buff) != NRF_SUCCESS)
    {
        return NRF_ERROR_BUSY;
    }

    ret_code_t ret = NRF_SUCCESS;
    if (app_usbd_hid_trans_required(&p_generic_ctx->hid_ctx))
    {
        ret = hid_generic_transfer_set(p_generic);
    }

    return ret;
}

const void * app_usbd_hid_generic_in_report_get(app_usbd_hid_generic_t const * p_generic,
                                                size_t * p_size)
{
    app_usbd_hid_inst_t const * p_hinst = &p_generic->specific.inst.hid_inst;
    *p_size = p_hinst->p_rep_buffer_in->size;
    return p_hinst->p_rep_buffer_in->p_buff;
}

const void * app_usbd_hid_generic_out_report_get(app_usbd_hid_generic_t const * p_generic,
                                                 size_t * p_size)
{
    app_usbd_hid_inst_t const * p_hinst = &p_generic->specific.inst.hid_inst;
    *p_size = p_hinst->p_rep_buffer_out->size;
    return p_hinst->p_rep_buffer_out->p_buff;
}

/**
 * @brief @ref app_usbd_hid_interface_t::on_get_report
 */
static ret_code_t hid_generic_on_get_report(app_usbd_class_inst_t const * p_inst,
                                            app_usbd_setup_evt_t const * p_setup_ev)
{
    return NRF_ERROR_NOT_SUPPORTED;
}


static ret_code_t hid_generic_on_set_report_data_cb(nrf_drv_usbd_ep_status_t status,
                                                    void * p_context)
{
    app_usbd_hid_user_ev_handler_t handler;
    app_usbd_hid_generic_t const * p_generic = (app_usbd_hid_generic_t const *)p_context;

    if (status != NRF_USBD_EP_OK)
    {
        return NRF_ERROR_INTERNAL;
    }

    handler = p_generic->specific.inst.hid_inst.user_event_handler;
    handler((app_usbd_class_inst_t const *)p_generic,
            APP_USBD_HID_USER_EVT_OUT_REPORT_READY);
    return NRF_SUCCESS;
}



/**
 * @brief @ref app_usbd_hid_interface_t::on_set_report
 */
static ret_code_t hid_generic_on_set_report(app_usbd_class_inst_t const * p_inst,
                                            app_usbd_setup_evt_t const * p_setup_ev)
{
    app_usbd_hid_generic_t const *  p_generic = hid_generic_get(p_inst);

    /*Request setup data*/
    app_usbd_hid_report_buffer_t const * p_rep_buff;
    p_rep_buff = app_usbd_hid_rep_buff_out_get(&p_generic->specific.inst.hid_inst);

    p_rep_buff->p_buff[0] = p_setup_ev->setup.wValue.lb;
    NRF_DRV_USBD_TRANSFER_OUT(transfer, p_rep_buff->p_buff + 1, p_rep_buff->size - 1);

    ret_code_t ret;
    CRITICAL_REGION_ENTER();
    ret = app_usbd_core_setup_data_transfer(NRF_DRV_USBD_EPOUT0, &transfer);
    if (ret == NRF_SUCCESS)
    {
        app_usbd_core_setup_data_handler_desc_t desc = {
                .handler = hid_generic_on_set_report_data_cb,
                .p_context = (void*)p_generic
        };

        ret = app_usbd_core_setup_data_handler_set(NRF_DRV_USBD_EPOUT0, &desc);
    }
    CRITICAL_REGION_EXIT();

    return ret;
}

/**
 * @brief @ref app_usbd_hid_interface_t::ep_transfer_in
 */
static ret_code_t hid_generic_ep_transfer_in(app_usbd_class_inst_t const * p_inst)
{
    app_usbd_hid_generic_t const * p_generic = hid_generic_get(p_inst);
    app_usbd_hid_generic_ctx_t *   p_generic_ctx = hid_generic_ctx_get(p_generic);

    nrf_queue_t const * p_rep_in_queue = p_generic->specific.inst.p_rep_in_queue;

    if (nrf_queue_is_empty(p_rep_in_queue))
    {
        app_usbd_hid_state_flag_clr(&p_generic_ctx->hid_ctx,
                                    APP_USBD_HID_STATE_FLAG_TRANS_IN_PROGRESS);
        return NRF_SUCCESS;
    }

    /* Get next report to send */
    return hid_generic_transfer_set((app_usbd_hid_generic_t const *)p_inst);
}

/**
 * @brief @ref app_usbd_hid_interface_t::ep_transfer_out
 */
static ret_code_t hid_generic_ep_transfer_out(app_usbd_class_inst_t const * p_inst)
{

    app_usbd_hid_generic_t const * p_generic = hid_generic_get(p_inst);
    nrf_drv_usbd_ep_t ep_addr = app_usbd_hid_epout_addr_get(p_inst);

    /*Request setup data*/
    app_usbd_hid_report_buffer_t const * p_rep_buff;
    p_rep_buff = app_usbd_hid_rep_buff_out_get(&p_generic->specific.inst.hid_inst);
    NRF_DRV_USBD_TRANSFER_OUT(transfer, p_rep_buff->p_buff, p_rep_buff->size);

    return app_usbd_core_ep_transfer(ep_addr, &transfer);
}

/**
 * @brief @ref app_usbd_class_interface_t::event_handler
 */
static ret_code_t hid_generic_event_handler(app_usbd_class_inst_t const * p_inst,
                                            app_usbd_complex_evt_t const * p_event)
{
    ASSERT(p_inst != NULL);
    ASSERT(p_event != NULL);

    app_usbd_hid_generic_t const * p_generic = hid_generic_get(p_inst);
    app_usbd_hid_inst_t const *    p_hinst = &p_generic->specific.inst.hid_inst;
    app_usbd_hid_generic_ctx_t *   p_generic_ctx = hid_generic_ctx_get(p_generic);
    app_usbd_hid_ctx_t *           p_hid_ctx = &p_generic_ctx->hid_ctx;

    /*Try handle event by generic HID event handler*/
    return app_usbd_hid_event_handler(p_inst, p_hinst, p_hid_ctx, p_event);
}

/**
 * @brief @ref app_usbd_class_interface_t::get_descriptors
 */
static const void * hid_generic_get_descriptors(app_usbd_class_inst_t const * p_inst,
                                                size_t * p_size)
{
    ASSERT(p_size != NULL);
    app_usbd_hid_generic_t const * p_generic = hid_generic_get(p_inst);
    app_usbd_hid_inst_t const *    p_hinst = &p_generic->specific.inst.hid_inst;

    *p_size = p_hinst->raw_desc_size;
    return p_hinst->p_raw_desc;
}

/** @} */

const app_usbd_hid_methods_t app_usbd_hid_generic_methods = {
        .on_get_report = hid_generic_on_get_report,
        .on_set_report = hid_generic_on_set_report,
        .ep_transfer_in = hid_generic_ep_transfer_in,
        .ep_transfer_out = hid_generic_ep_transfer_out,
};

const app_usbd_class_methods_t app_usbd_generic_class_methods = {
        .event_handler = hid_generic_event_handler,
        .get_descriptors = hid_generic_get_descriptors,
};

#endif // APP_USBD_HID_GENERIC_ENABLED
