#include <nrf_drv_wdt.h>
#include <app_error.h>
#include <nrf_log.h>
#include <nrf_nvic.h>
#include <nrf_soc.h>

#include "watchdog.h"

void watchdog_timeout_handler(){
    /* do nothing */
}

/**
 * Hook onto radio events in order to reset the watchdog.
 * You would think that calling watchdog_feed() in main.c would be enough but SURPRISE, no.
 * Turns out that power_manage() does not return for pure BLE events, while the CPU ticks during those, angering the watchdog.
 * 
 * Hence, we also need to feed the watchdog on radio events that simply pass by the main loop.
 */

uint32_t radio_notification_init()
{
    uint32_t err_code;

    err_code = sd_nvic_ClearPendingIRQ(SWI1_IRQn);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = sd_nvic_SetPriority(SWI1_IRQn, 1);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    err_code = sd_nvic_EnableIRQ(SWI1_IRQn);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Configure the event
    return sd_radio_notification_cfg_set(NRF_RADIO_NOTIFICATION_TYPE_INT_ON_INACTIVE, NRF_RADIO_NOTIFICATION_DISTANCE_800US);
}

void SWI1_IRQHandler(bool radio_evt)
{
    watchdog_feed();
}

void watchdog_init(){
    ret_code_t err_code;

    err_code = radio_notification_init();
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_wdt_init(NULL /* configured trough sdk_config */, watchdog_timeout_handler /* required */);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_wdt_channel_alloc(NRF_WDT_RR0);
    APP_ERROR_CHECK(err_code);

    nrf_drv_wdt_enable();
}

void watchdog_feed(){
    nrf_drv_wdt_channel_feed(NRF_WDT_RR0);
}