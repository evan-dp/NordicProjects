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

#include <stdint.h>
#include "nrf_bootloader_app_start.h"
#include "nrf_log.h"
#include "nrf_dfu_mbr.h"

#if defined(SOFTDEVICE_PRESENT)
#include "nrf_sdm.h"
#endif

extern void nrf_bootloader_app_start_impl(uint32_t start_addr);

void nrf_bootloader_app_start(uint32_t start_addr)
{
    NRF_LOG_DEBUG("Running nrf_bootloader_app_start with address: 0x%08x\r\n", start_addr);
    uint32_t err_code;

    //NRF_LOG_INFO("Initializing SD in mbr\r\n");
    err_code = nrf_dfu_mbr_init_sd();
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed running nrf_dfu_mbr_init_sd\r\n");
        return;
    }

    // Disable interrupts
    NRF_LOG_DEBUG("Disabling interrupts\r\n");

    NVIC->ICER[0]=0xFFFFFFFF;
#if defined(__NRF_NVIC_ISER_COUNT) && __NRF_NVIC_ISER_COUNT == 2
    NVIC->ICER[1]=0xFFFFFFFF;
#endif

    // Set the sd softdevice vector table base address
    NRF_LOG_DEBUG("Setting SD vector table base: 0x%08x\r\n", start_addr);
    err_code = sd_softdevice_vector_table_base_set(start_addr);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("Failed running sd_softdevice_vector_table_base_set\r\n");
        return;
    }

    // Run application
    nrf_bootloader_app_start_impl(start_addr);
}

