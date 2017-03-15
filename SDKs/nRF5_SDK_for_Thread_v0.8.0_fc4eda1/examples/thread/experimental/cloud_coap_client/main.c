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
 * @defgroup cloud_coap_client_example_main main.c
 * @{
 * @ingroup cloud_coap_client_example_example
 * @brief Simple Cloud CoAP Client Example Application main file.
 *
 * @details This example demonstrates a CoAP client application that sends emulated
 *          temperature value to the thethings.io cloud. Example uses NAT64 on the
 *          Nordic's Thread Border Router soulution for IPv4 connectivity.
 *
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

#include <openthread/openthread.h>
#include <openthread/diag.h>
#include <openthread/coap.h>
#include <openthread/cli.h>
#include <openthread/platform/platform.h>
#include <openthread/platform/alarm.h>

#define LED_INTERVAL              200

#define CLOUD_ADDRESS             "fd00:64:123:4567::68c7:55d3"  /**< NAT64 address of the thethings.io cloud. */
#define CLOUD_URI_PATH            "v2/things/{THING-TOKEN}"      /**< Put your things URI here. */
#define CLOUD_THING_RESOURCE      "temp"                         /**< Thing resource name. */
#define CLOUD_COAP_CONTENT_FORMAT 50                             /**< Use application/json content format type. */

#define TEMPERATURE_INIT          22                             /**< The initial value of temperature. */
#define TEMPERATURE_MIN           15                             /**< Minimal possible temperature value. */
#define TEMPERATURE_MAX           30                             /**< Maximum possible temperature value. */

typedef struct
{
    otInstance   * p_ot_instance;       /**< A pointer to the OpenThread instance. */
    uint16_t       temperature;         /**< The value of emulated temperature. */
} application_t;

application_t m_app =
{
    .p_ot_instance      = NULL,
    .temperature        = TEMPERATURE_INIT,
};

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

/***************************************************************************************************
 * @section Cloud CoAP
 **************************************************************************************************/

 static void cloud_data_send(otInstance * p_instance,
                             const char * p_uri_path,
                             char       * p_payload)
 {
     ThreadError error = kThreadError_NoBufs;
     otCoapHeader header;
     otCoapOption content_format_option;
     otMessage * p_request;
     otMessageInfo message_info;
     uint8_t content_format = CLOUD_COAP_CONTENT_FORMAT;

     do
     {
         content_format_option.mNumber = kCoapOptionContentFormat;
         content_format_option.mLength = 1;
         content_format_option.mValue  = &content_format;

         otCoapHeaderInit(&header, kCoapTypeNonConfirmable, kCoapRequestPost);
         otCoapHeaderAppendUriPathOptions(&header, p_uri_path);
         otCoapHeaderAppendOption(&header, &content_format_option);
         otCoapHeaderSetPayloadMarker(&header);

         p_request = otCoapNewMessage(p_instance, &header);
         if (p_request == NULL)
         {
             NRF_LOG_INFO("Failed to allocate message for CoAP Request\r\n");
             break;
         }

         error = otMessageAppend(p_request, p_payload, strlen(p_payload));
         if (error != kThreadError_None)
         {
             break;
         }

         memset(&message_info, 0, sizeof(message_info));
         message_info.mInterfaceId = OT_NETIF_INTERFACE_ID_THREAD;
         message_info.mPeerPort = OT_DEFAULT_COAP_PORT;
         otIp6AddressFromString(CLOUD_ADDRESS, &message_info.mPeerAddr);

         error = otCoapSendRequest(p_instance, p_request, &message_info, NULL, NULL);

     } while (false);

     if (error != kThreadError_None && p_request != NULL)
     {
         NRF_LOG_INFO("Failed to send CoAP Request: %d\r\n", error);
         otMessageFree(p_request);
     }
 }

 static void cloud_data_update(void)
 {
     char payload_buffer[64];

     sprintf(payload_buffer,
             "{\"values\":[{\"key\":\"%s\",\"value\":\"%d\"}]}",
             CLOUD_THING_RESOURCE, m_app.temperature);

     cloud_data_send(m_app.p_ot_instance, CLOUD_URI_PATH, payload_buffer);
 }

/***************************************************************************************************
 * @section State
 **************************************************************************************************/

void state_changed_callback(uint32_t flags, void * p_context)
{
    NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d\r\n", flags, otThreadGetDeviceRole(p_context));
}

/***************************************************************************************************
 * @section Buttons
 **************************************************************************************************/

void bsp_event_handler(bsp_event_t event)
{
    switch (event)
    {
        case BSP_EVENT_KEY_0:
            if (m_app.temperature == TEMPERATURE_MIN)
            {
                // The minimal temperature has been already reached.
                break;
            }

            // Decrement temperature value.
            m_app.temperature -= 1;

            cloud_data_update();
            break;

        case BSP_EVENT_KEY_1:
            if (m_app.temperature == TEMPERATURE_MAX)
            {
                // The maximum temperature has been already reached.
                break;
            }

            // Increment temperature value.
            m_app.temperature += 1;

            cloud_data_update();
            break;

        case BSP_EVENT_KEY_2:
            break;

        case BSP_EVENT_KEY_3:
            break;

        default:
            return; // no implementation needed
    }
}

/***************************************************************************************************
 * @section Initialization
 **************************************************************************************************/

static otInstance * initialize_thread(void)
{
    otInstance *p_instance;

    p_instance = otInstanceInit();
    assert(p_instance);

    otCliUartInit(p_instance);

    NRF_LOG_INFO("Thread version: %s\r\n", (uint32_t)otGetVersionString());
    NRF_LOG_INFO("Network name:   %s\r\n", (uint32_t)otThreadGetNetworkName(p_instance));

    assert(otSetStateChangedCallback(p_instance, &state_changed_callback, p_instance) == kThreadError_None);

    assert(otLinkSetChannel(p_instance, THREAD_CHANNEL) == kThreadError_None);
    assert(otLinkSetPanId(p_instance, THREAD_PANID) == kThreadError_None);
    assert(otIp6SetEnabled(p_instance, true) == kThreadError_None);
    assert(otThreadSetEnabled(p_instance, true) == kThreadError_None);

    return p_instance;
}

void initialize_timer(void)
{
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

void initialize_leds(void)
{
    LEDS_CONFIGURE(LEDS_MASK);
    LEDS_OFF(LEDS_MASK);
}

void initialize_bsp(void)
{
    uint32_t err_code = bsp_init(BSP_INIT_LED | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_thread_init(m_app.p_ot_instance);
    APP_ERROR_CHECK(err_code);
}

/***************************************************************************************************
 * @section Main
 **************************************************************************************************/

int main(int argc, char *argv[])
{
    NRF_LOG_INIT(NULL);

    PlatformInit(argc, argv);
    m_app.p_ot_instance = initialize_thread();

    initialize_timer();
    initialize_bsp();
    initialize_leds();

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
