#include <nrf_drv_wdt.h>
#include <app_error.h>

void watchdog_timeout_handler(){
    /* do nothing */
}

void watchdog_init(){
    ret_code_t err_code;

    err_code = nrf_drv_wdt_init(NULL /* configured trough sdk_config */, watchdog_timeout_handler /* required */);
    APP_ERROR_CHECK(err_code);

    nrf_drv_wdt_channel_alloc(NRF_WDT_RR0);
    APP_ERROR_CHECK(err_code);

    nrf_drv_wdt_enable();
    APP_ERROR_CHECK(err_code);
}

void watchdog_feed(){
    nrf_drv_wdt_channel_feed(NRF_WDT_RR0);
}