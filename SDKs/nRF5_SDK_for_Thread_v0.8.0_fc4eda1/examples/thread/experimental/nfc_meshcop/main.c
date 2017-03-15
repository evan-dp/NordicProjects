/* Copyright (c) 2017 Nordic Semiconductor. All Rights Reserved.
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
 * @defgroup nfc_meshcop_example_main main.c
 * @{
 * @ingroup nfc_meshcop_example
 * @brief An example presenting OpenThread MeshCoP.
 *
 */
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "bsp_thread.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nfc_t2t_lib.h"
#include "nfc_meshcop_msg.h"


#include <openthread/openthread.h>
#include <openthread/cli.h>
#include <openthread/platform/platform.h>
#include <openthread/joiner.h>

#define JOINER_DELAY             500
#define MAX_JOINER_RETRY_COUNT   3

#define APP_TIMER_PRESCALER      0       // Value of the RTC1 PRESCALER register.
#define APP_TIMER_OP_QUEUE_SIZE  2       // Size of timer operation queues.

APP_TIMER_DEF(m_joiner_timer);

uint8_t m_ndef_msg_buf[256];

typedef enum
{
    LED_OFF,
    LED_ON,
    LED_BLINK
} led_state_t;

typedef struct
{
    // OpenThread instance
    otInstance       * p_ot_instance;
    // Indicates whether commissioning schould be performed after NFC field detection.
    bool               joiner_allow_start;
    // Commissioning data structure
    nfc_meshcop_data_t meshcop_data;
    // Retry counter for commissioning process
    uint32_t           joiner_retry_count;
} application_t;

application_t m_app =
{
    .p_ot_instance      = NULL,
    .joiner_allow_start = false,
    .meshcop_data       = {.psk_d = "0A1B2C3D"},
    .joiner_retry_count = 0
};

void otTaskletsSignalPending (otInstance *aInstance)
{
    (void)aInstance;
}

/***************************************************************************************************
 * @ Callback functions
 **************************************************************************************************/
void joiner_callback (ThreadError aError, void *aContext)
{
	uint32_t err_code;

    if (aError == kThreadError_None)
    {
        err_code = nfc_t2t_emulation_stop();
        APP_ERROR_CHECK(err_code);

        err_code = bsp_thread_commissioning_indication_set(BSP_INDICATE_COMMISSIONING_SUCCESS);
        APP_ERROR_CHECK(err_code);

        assert(otThreadSetEnabled(m_app.p_ot_instance, true) == kThreadError_None);
    }
    else
    {
        if (m_app.joiner_retry_count < MAX_JOINER_RETRY_COUNT)
        {
            m_app.joiner_retry_count++;
            app_timer_start(m_joiner_timer, APP_TIMER_TICKS(JOINER_DELAY), NULL);
        }
        else
        {
			m_app.joiner_retry_count = 0;
            m_app.joiner_allow_start = true;

            err_code = bsp_thread_commissioning_indication_set(
                BSP_INDICATE_COMMISSIONING_NOT_COMMISSIONED);
            APP_ERROR_CHECK(err_code);
        }
    }
}

void nfc_callback (void          * p_context,
                   nfc_t2t_event_t event,
                   const uint8_t * p_data,
                   size_t          data_length)
{
    uint32_t err_code;

    (void)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            bsp_board_led_on(BSP_BOARD_LED_3);

            if (m_app.joiner_allow_start)
            {
                m_app.joiner_allow_start = false;

                err_code = bsp_thread_commissioning_indication_set(
                    BSP_INDICATE_COMMISSIONING_IN_PROGRESS);
                APP_ERROR_CHECK(err_code);

                app_timer_start(m_joiner_timer, APP_TIMER_TICKS(JOINER_DELAY),  NULL);
            }
            break;

        case NFC_T2T_EVENT_FIELD_OFF:
            bsp_board_led_off(BSP_BOARD_LED_3);
            break;

        default:
            break;
    }
}

static void bsp_event_handler (bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_KEY_3:
            otInstanceFactoryReset(m_app.p_ot_instance);
            break;

        default:
            return;
    }
}

/***************************************************************************************************
 * @section State
 **************************************************************************************************/

void state_changed_callback (uint32_t flags, void * p_context)
{
    NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d\r\n",
                 flags, otThreadGetDeviceRole(p_context));
}

/***************************************************************************************************
 * @section Timers
 **************************************************************************************************/

void joiner_timer_handler (void * p_context)
{
    otJoinerStart(m_app.p_ot_instance,
                  m_app.meshcop_data.psk_d,
                  "", "NordicSemiconductor", "", "", "", joiner_callback, NULL);
}

/***************************************************************************************************
 * @section Initialization
 **************************************************************************************************/

static otInstance * initialize_thread (void)
{
    otInstance * p_instance;

    p_instance = otInstanceInit();
    assert(p_instance);

    otCliUartInit(p_instance);

    NRF_LOG_INFO("Thread version: %s\r\n", (uint32_t)otGetVersionString());
    NRF_LOG_INFO("Network name:   %s\r\n", (uint32_t)otThreadGetNetworkName(p_instance));

    assert(otSetStateChangedCallback(p_instance,
                                     &state_changed_callback,
                                     p_instance) == kThreadError_None);
    assert(otIp6SetEnabled(p_instance, true) == kThreadError_None);

    if (otThreadSetEnabled(p_instance, true) != kThreadError_None)
    {
        m_app.joiner_allow_start = true;
    }

    return p_instance;
}

void initialize_timer (void)
{
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    app_timer_create(&m_joiner_timer, APP_TIMER_MODE_SINGLE_SHOT, joiner_timer_handler);
}

void initialize_leds (void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
}

static void initialize_bsp (void)
{
    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_thread_init(m_app.p_ot_instance);
    APP_ERROR_CHECK(err_code);
}

void initialize_nfc (void)
{
    uint32_t  err_code;

    // Set up NFC.
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    // Provide information about available buffer size to encoding function.
    uint32_t len = sizeof(m_ndef_msg_buf);

    otLinkGetJoinerId(m_app.p_ot_instance, &m_app.meshcop_data.eui64);

    err_code = nfc_meshcop_msg_encode(&m_app.meshcop_data, m_ndef_msg_buf, &len);
    APP_ERROR_CHECK(err_code);

    // Set created message as the NFC payload.
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    // Start sensing NFC field.
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);
}

/***************************************************************************************************
 * @section Main
 **************************************************************************************************/

int main (int argc, char *argv[])
{
	uint32_t  err_code;

    NRF_LOG_INIT(NULL);

    PlatformInit(argc, argv);
    m_app.p_ot_instance = initialize_thread();

    initialize_timer();
    initialize_bsp();
    initialize_leds();

    if (m_app.joiner_allow_start)
    {
		err_code = bsp_thread_commissioning_indication_set(
            BSP_INDICATE_COMMISSIONING_NOT_COMMISSIONED);
        APP_ERROR_CHECK(err_code);

        initialize_nfc();
    } else {
        err_code = bsp_thread_commissioning_indication_set(BSP_INDICATE_COMMISSIONING_SUCCESS);
        APP_ERROR_CHECK(err_code);
    }

    while (true)
    {
        otTaskletsProcess(m_app.p_ot_instance);
        PlatformProcessDrivers(m_app.p_ot_instance);
    }

    return 0;
}

/**
 *@}
 **/
