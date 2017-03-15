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

/** @file
 *
 * @defgroup thread_secure_dfu_example_main main.c
 * @{
 * @ingroup thread_secure_dfu_example
 * @brief Thread Secure DFU Example Application main file.
 *
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <openthread/openthread.h>
#include <openthread/diag.h>
#include <openthread/cli.h>
#include <openthread/platform/platform.h>
#include <openthread/platform/alarm.h>
#include <openthread/platform/random.h>
#include "app_util.h"
#include "app_timer.h"
#include "boards.h"
#include "bsp_thread.h"
#include "nrf_delay.h"
#include "mem_manager.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "coap_dfu.h"
#include "sdk_config.h"
#include "nrf_dfu_utils.h"

static otInstance     * m_Instance;
static otNetifAddress   m_addresses[10];
static bool             m_trigger_dfu       = false;
uint16_t                m_coap_message_id;

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

static void coap_error_handler(uint32_t error_code, coap_message_t * p_message)
{
    // If any response fill the p_response with a appropriate response message.
}

static void print_address(const otIp6Address *addr)
{
    char ipstr[40];
    snprintf(ipstr, sizeof(ipstr), "%x:%x:%x:%x:%x:%x:%x:%x",
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 0)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 1)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 2)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 3)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 4)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 5)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 6)),
             uint16_big_decode((uint8_t *)(addr->mFields.m16 + 7)));

    NRF_LOG_INFO("%s\r\n", (uint32_t)ipstr);
}

static void print_addresses(otInstance * aInstance)
{
    for (const otNetifAddress *addr = otIp6GetUnicastAddresses(aInstance); addr; addr = addr->mNext)
    {
        print_address(&addr->mAddress);
    }
}

static void state_changed_callback(uint32_t aFlags, void *aContext)
{
    if (aFlags & OT_THREAD_NETDATA_UPDATED)
    {
        otIp6SlaacUpdate(m_Instance,
                         m_addresses,
                         sizeof(m_addresses) / sizeof(m_addresses[0]),
                         otIp6CreateRandomIid,
                         NULL);

        print_addresses(m_Instance);
    }

    otDeviceRole role = otThreadGetDeviceRole(m_Instance);
    NRF_LOG_INFO("New role: %d\r\n", role);

    if (aFlags & OT_NET_ROLE)
    {
        switch(role)
        {
            case kDeviceRoleChild:
            case kDeviceRoleRouter:
            case kDeviceRoleLeader:
                m_trigger_dfu = true;
                break;

            case kDeviceRoleOffline:
            case kDeviceRoleDisabled:
            case kDeviceRoleDetached:
            default:
                break;
        }
    }
}

static void initialize_bsp(void)
{
    uint32_t err_code;
    err_code = bsp_init(BSP_INIT_LED, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_thread_init(m_Instance);
    APP_ERROR_CHECK(err_code);
}

static otInstance * initialize_thread(void)
{
    otInstance * p_instance = otInstanceInit();
    assert(p_instance != NULL);

    otCliUartInit(p_instance);

    otSetStateChangedCallback(p_instance, state_changed_callback, NULL);

    assert(otLinkSetPanId(p_instance, THREAD_PANID) == kThreadError_None);
    assert(otLinkSetChannel(p_instance, THREAD_CHANNEL) == kThreadError_None);
    assert(otIp6SetEnabled(p_instance, true) == kThreadError_None);
    assert(otThreadSetEnabled(p_instance, true) == kThreadError_None);

    NRF_LOG_INFO("Thread version: %s\r\n", (uint32_t)otGetVersionString());
    NRF_LOG_INFO("Network name:   %s\r\n", (uint32_t)otThreadGetNetworkName(p_instance));

    return p_instance;
}

static void initialize_coap(otInstance * p_instance)
{
    NRF_LOG_DEBUG("Init coap\r\n");
    coap_port_t local_port_list[COAP_PORT_COUNT] =
    {
        {.port_number = DFU_UDP_PORT}
    };

    coap_transport_init_t transport_params;
    transport_params.p_port_table = &local_port_list[0];
    transport_params.p_arg = p_instance;

    m_coap_message_id = otPlatRandomGet();
    coap_init(otPlatRandomGet(), &transport_params);
    coap_error_handler_register(coap_error_handler);
}

// Reset handler used after bootloader update, required by nrf_dfu_utils module.
static void reset_delay_timer_handler(void * p_context)
{
    NRF_LOG_DEBUG("Reset delay timer expired, resetting.\r\n");
#ifdef NRF_DFU_DEBUG_VERSION
    nrf_delay_ms(100);
#endif
    NVIC_SystemReset();
}

static void timers_init(void)
{
    APP_ERROR_CHECK(app_timer_init());
    APP_ERROR_CHECK(app_timer_create(&nrf_dfu_utils_reset_delay_timer, APP_TIMER_MODE_SINGLE_SHOT, reset_delay_timer_handler));
}

int main(int argc, char *argv[])
{
    NRF_LOG_INIT(NULL);
    nrf_mem_init();

    timers_init();

    PlatformInit(argc, argv);

    m_Instance = initialize_thread();
    initialize_bsp();
    initialize_coap(m_Instance);

    coap_dfu_init();

    uint32_t now, before = otPlatAlarmGetNow();
    while (1)
    {
        otTaskletsProcess(m_Instance);
        PlatformProcessDrivers(m_Instance);

        now = otPlatAlarmGetNow();
        if (now - before > 1000)
        {
            coap_time_tick();
            before = now;
        }

        if (m_trigger_dfu)
        {
            m_trigger_dfu = false;
            coap_dfu_trigger(NULL);
        }
    }

    otInstanceFinalize(m_Instance);
}

/** @} */
