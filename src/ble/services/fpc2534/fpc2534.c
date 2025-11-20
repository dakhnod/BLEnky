#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "sdk_common.h"
#include "ble_l2cap.h"
#include "app_error.h"
#include "ble_helpers.h"
#include "ble_srv_common.h"
#include "nrf_log.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"
#include "nrf_delay.h"
#include "nrf_queue.h"

#include "nrfx_spim.h"

#define CUSTOM_FPC2534_BASE_CONFIGURATION_SERVICE {0xc5, 0x09, 0x41, 0x58, 0xf1, 0x14, 0x30, 0x78, 0x15, 0xd8, 0x47, 0x79, 0x30, 0x2a, 0x3f, 0x38}
#define UUID_FPC2534 0x0000

#define PIN_SCK  NRF_GPIO_PIN_MAP(1, 11)
#define PIN_MISO NRF_GPIO_PIN_MAP(1, 13)
#define PIN_MOSI NRF_GPIO_PIN_MAP(1, 15)
#define PIN_CS   NRF_GPIO_PIN_MAP(0, 2)
#define PIN_RST  NRF_GPIO_PIN_MAP(0, 31)
#define PIN_IRQ  NRF_GPIO_PIN_MAP(0, 29)

uint8_t fpc2534_custom_uuid_type;

uint16_t fpc2534_connection_handle;

uint16_t fpc2534_service_handle;
uint16_t fpc2534_write_characteristic_handle;
uint16_t fpc2534_read_characteristic_handle;
uint16_t fpc2534_read_cccd_handle;

bool send_reads = false;
bool ignore_gpio_irq = true;
bool reading_header = false;

volatile bool waiting_for_transmission = false;

uint8_t spi_buf[251];
nrfx_spim_t spi = NRFX_SPIM_INSTANCE(0);

NRF_QUEUE_DEF(uint8_t, transmission_queue, 1024, NRF_QUEUE_MODE_NO_OVERFLOW);

void irq_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
    if(ignore_gpio_irq) {
        return;
    }

    NRF_LOG_DEBUG("data available! Reading header...");

    nrf_gpio_pin_clear(PIN_CS);
    nrf_delay_ms(1);

    reading_header = true;

    nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_RX(spi_buf, 8);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));
}

void spi_handler(nrfx_spim_evt_t const *p_event, void *p_context) {
    if(p_event->xfer_desc.rx_length > 0) {
        if(reading_header) {
            uint16_t length = spi_buf[6] + (spi_buf[7] << 8);
            NRF_LOG_DEBUG("Received header, indicating %d available", length);
            // NRF_LOG_HEXDUMP_DEBUG(spi_buf, 8);
            nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_RX(spi_buf + 8, length); // account for header (+8)

            reading_header = false;
            APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));
        }else{
            uint16_t payload_length = p_event->xfer_desc.rx_length + 8;

            NRF_LOG_DEBUG("Received %d payload", p_event->xfer_desc.rx_length);
            // NRF_LOG_HEXDUMP_DEBUG(spi_buf, payload_length);
            nrf_gpio_pin_set(PIN_CS);

            if(!send_reads) {
                return;
            }

            if(fpc2534_connection_handle == BLE_CONN_HANDLE_INVALID) {
                return;
            }

            if(waiting_for_transmission) {
                NRF_LOG_DEBUG("waiting for transmission, queueing");
                nrf_queue_write(&transmission_queue, spi_buf, payload_length);
                return;
            }

            ble_gatts_hvx_params_t params = {
                .handle = fpc2534_read_characteristic_handle,
                .type = BLE_GATT_HVX_INDICATION,
                .offset = 0,
                .p_len = &payload_length,
                .p_data = spi_buf
            };

            waiting_for_transmission = true;

            ret_code_t err_code = sd_ble_gatts_hvx(
                fpc2534_connection_handle,
                &params);
            APP_ERROR_CHECK(err_code);
        }
        return;
    }


    nrf_gpio_pin_set(PIN_CS);
}

void fpc2534_on_transmission(const ble_evt_t *p_ble_evt) {
    if(p_ble_evt->evt.gatts_evt.params.hvc.handle != fpc2534_read_characteristic_handle) {
        return;
    }

    NRF_LOG_DEBUG("transmission: %d %d", p_ble_evt->evt.gatts_evt.params.hvc.handle, fpc2534_read_characteristic_handle);

    if(nrf_queue_is_empty(&transmission_queue)) {
        NRF_LOG_DEBUG("no data queued");
        waiting_for_transmission = false;
        return;
    }

    // read header
    nrf_queue_read(&transmission_queue, spi_buf, 8);
    uint16_t length = spi_buf[6] + (spi_buf[7] << 8);
    NRF_LOG_DEBUG("another %d bytes queued", length);
    nrf_queue_read(&transmission_queue, spi_buf + 8, length);

    uint16_t payload_length = length + 8;

    ble_gatts_hvx_params_t params = {
        .handle = fpc2534_read_characteristic_handle,
        .type = BLE_GATT_HVX_INDICATION,
        .offset = 0,
        .p_len = &payload_length,
        .p_data = spi_buf
    };

    waiting_for_transmission = true;

    ret_code_t err_code = sd_ble_gatts_hvx(
        fpc2534_connection_handle,
        &params);
    APP_ERROR_CHECK(err_code);
}

void fpc2534_init(){
    nrf_drv_gpiote_in_config_t drv_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
    APP_ERROR_CHECK(nrf_drv_gpiote_in_init(PIN_IRQ, &drv_config, irq_handler));
    nrf_drv_gpiote_in_event_enable(PIN_IRQ, true);

    nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
    config.sck_pin = PIN_SCK;
    config.miso_pin = PIN_MISO;
    config.mosi_pin = PIN_MOSI;

    APP_ERROR_CHECK(nrfx_spim_init(&spi, &config, spi_handler, NULL));

    // reset
    nrf_gpio_cfg_output(PIN_RST);
    nrf_gpio_pin_clear(PIN_RST);
    nrf_delay_ms(10);
    nrf_gpio_pin_set(PIN_RST);
    nrf_delay_ms(200);

    // CSn
    nrf_gpio_cfg_output(PIN_CS);
    nrf_gpio_pin_clear(PIN_CS);

    nrf_delay_us(500);

    // request welcome message
    nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_RX(spi_buf, 20);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));

    ignore_gpio_irq = false;

    ret_code_t err_code;

    ble_uuid128_t vs_uuid = {
        .uuid128 = CUSTOM_FPC2534_BASE_CONFIGURATION_SERVICE
    };

    err_code = sd_ble_uuid_vs_add(&vs_uuid, &fpc2534_custom_uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t ble_uuid = {
        .type = fpc2534_custom_uuid_type,
        .uuid = UUID_FPC2534
    };
    
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &fpc2534_service_handle);
    APP_ERROR_CHECK(err_code);

    ble_helper_characteristic_init_t init = {
        .service_handle = fpc2534_service_handle,
        .uuid = 0x0001,
        .uuid_type = fpc2534_custom_uuid_type,
        .description_str = "SPI write",
        .is_writable = true,
        .max_length = sizeof(spi_buf),
        .value_handle = &fpc2534_write_characteristic_handle,
    };
    APP_ERROR_CHECK(ble_helper_characteristic_add(&init));

    ble_helper_characteristic_init_t init2 = {
        .service_handle = fpc2534_service_handle,
        .uuid = 0x0002,
        .uuid_type = fpc2534_custom_uuid_type,
        .description_str = "SPI response",
        .is_indicatable = true,
        .value_handle = &fpc2534_read_characteristic_handle,
        .cccd_handle = &fpc2534_read_cccd_handle,
        .max_length = 200,
    };
    APP_ERROR_CHECK(ble_helper_characteristic_add(&init2));
}

void fpc2534_on_write_send(const ble_gatts_evt_write_t *write_evt) {
    nrf_gpio_pin_clear(PIN_CS);
    nrf_delay_us(500);
    nrfx_spim_xfer_desc_t xfer = NRFX_SPIM_XFER_TX(write_evt->data, write_evt->len);
    APP_ERROR_CHECK(nrfx_spim_xfer(&spi, &xfer, 0));
}

void fpc2534_on_write(const ble_evt_t *p_ble_evt)
{
    const ble_gatts_evt_write_t *write_evt = &p_ble_evt
                                            ->evt
                                            .gatts_evt
                                            .params
                                            .write;

    uint16_t handle = write_evt->handle;

    if (handle == fpc2534_read_cccd_handle)
    {
        send_reads = ble_srv_is_indication_enabled(write_evt->data);
        NRF_LOG_DEBUG("Enabled FPC2534 reads: %d", send_reads);
        return;
    }
    if(handle == fpc2534_write_characteristic_handle) {
        fpc2534_on_write_send(write_evt);
        return;
    }
}

void fpc2534_on_ble_evt(const ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        fpc2534_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        fpc2534_connection_handle = BLE_CONN_HANDLE_INVALID;
        send_reads = false;
        break;

    case BLE_GATTS_EVT_HVC:
        fpc2534_on_transmission(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        fpc2534_on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}