#if FAMILY == 51
#define NRF_LOG_MODULE_NAME "APP"
#else
#define NRF_LOG_MODULE_NAME APP
#endif

#include <stdint.h>
#include <string.h>

#if FAMILY == 51
#include "fstorage.h"
#else
#include "nrf_fstorage.h"
#include "nrf_log_default_backends.h"
#endif
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "sensor_ble.h"
#include "sensor_gpio.h"
#include "sensor_timer.h"
#include "ble_configuration_service.h"
#include "sleep.h"
#include "feature_config.h"
#include "watchdog.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_delay.h"

#include "nrfx_spim.h"

nrfx_spim_t spi = NRFX_SPIM_INSTANCE(0);

uint8_t spi_buf[256];

#if FAMILY == 52
NRF_LOG_MODULE_REGISTER();
#endif

void main_handle_input_change(int highest_changed_index)
{
    #if FEATURE_ENABLED(SLEEP_MODE)
        sleep_handle_gpio_event();
    #endif
    ble_handle_input_change(highest_changed_index);
}

void irq_handldler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    if(nrf_gpio_pin_read(4)) {
        nrf_gpio_pin_clear(13);
    }else{
        nrf_gpio_pin_set(13);
    }

    NRF_LOG_DEBUG("interrupt!");
}

void spi_handler(nrfx_spim_evt_t const *p_event, void *p_context) {
    NRF_LOG_DEBUG("SPI handler! %d bytes transmitted", p_event->xfer_desc.tx_length);
}

void spi_init() {
    nrf_drv_gpiote_in_config_t drv_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(4, &drv_config, irq_handldler));
    nrf_drv_gpiote_in_event_enable(4, true);

    nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
    config.sck_pin = (0 + 30);
    config.miso_pin = (0 + 29);
    config.mosi_pin = (0 + 28);
    // config.ss_pin = (0 + 03);

    APP_ERROR_CHECK(nrfx_spim_init(&spi, &config, spi_handler, NULL));

    nrf_gpio_cfg_output(31);
    nrf_gpio_pin_clear(31);
    nrf_delay_ms(10);
    nrf_gpio_pin_set(31);
    nrf_delay_ms(200);

    nrf_gpio_cfg_output(3);
    nrf_gpio_pin_clear(3);

    nrf_delay_ms(1);

    nrfx_spim_xfer_desc_t xfer = {
        .p_rx_buffer = spi_buf,
        .tx_length = 0,
        .rx_length = 8,
    };
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));

    NRF_LOG_DEBUG("SPI done.");

    NRF_LOG_HEXDUMP_DEBUG(spi_buf, 8);

    int length = spi_buf[6] + (spi_buf[7] << 8);
    NRF_LOG_DEBUG("payload lemgth: %d", length);

    xfer.rx_length = length;
    xfer.p_rx_buffer += 8;
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));

    NRF_LOG_HEXDUMP_DEBUG(spi_buf, 22);

    nrf_delay_ms(1000);

    uint8_t data[] = {4, 0, 17, 0, 16, 0, 4, 0, 64, 0, 17, 0};
    xfer.p_tx_buffer = data;
    xfer.tx_length = sizeof(data);
    xfer.rx_length = 0;
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));

    nrf_delay_ms(100);

    xfer.p_rx_buffer = spi_buf;
    xfer.tx_length = 0;
    xfer.rx_length = 8;
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));

    NRF_LOG_HEXDUMP_DEBUG(spi_buf, 8);

    // nrf_gpio_pin_set(3);
}

int main(void) {
    (void) NRF_LOG_INIT(NULL);

    #ifdef NRF52840_XXAA
    // disable APPROTECT
    *((uint32_t*)0x40000558) = 0x5A;
    #endif

    #if FAMILY == 52
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    #endif

    NRF_LOG_INFO("starting\n");

    ble_stack_init();

    #if FAMILY == 51
    fs_init();
    #endif

    timer_init();

    #if FEATURE_ENABLED(SLEEP_MODE)
    sleep_init(ble_disable_rf);
    #endif

    gpio_init(main_handle_input_change);

    watchdog_init();

    ble_init();

    advertising_start();

    spi_init();

    NRF_LOG_INFO("setup done\n");

    // Enter main loop.
    for (;;) {
        // if (NRF_LOG_PROCESS() == false) {
            power_manage();
            watchdog_feed();
            app_sched_execute();
        // }
    }
}
