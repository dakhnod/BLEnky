#include "sensor_ble.h"

#include "ble_configuration_service.h"
#include "ble_gpio_asm.h"
#include "app_error.h"
#include "ble_dis.h"

ble_gap_adv_params_t m_adv_params;
ble_advdata_t advdata;

ble_dfu_t dfu;

bool is_advertising = false;

uint16_t connection_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

uint16_t advertising_interval = APP_ADV_INTERVAL_FAST;

void ble_init() {
    ble_stack_init();

    uint8_t device_name[LENGTH_DEVICE_NAME];
    uint32_t device_name_length;

    storage_read_device_name(device_name, &device_name_length);

    NRF_LOG_INFO("device name length: %d\n", device_name_length);

    if(device_name_length == 0){
        // set default device name
        gap_params_init(
            (uint8_t*) DEVICE_NAME,
            strlen(DEVICE_NAME)
        );
    }else {
        gap_params_init(
            device_name,
            device_name_length
        );
    }
    conn_params_init();
    services_init();
    advertising_init();
}

void ble_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
    ble_aio_handle_input_change(index, config);
    ble_gpio_asm_handle_input_change(index, config);
}

void ble_handle_device_name_write(ble_gatts_evt_write_t *write_evt){
    uint16_t len = write_evt->len;
    uint8_t *data = write_evt->data;

    storage_store_device_name(data, len);
}

void ble_on_write(ble_evt_t *p_ble_evt) {
  ble_gatts_evt_write_t *write_evt = &p_ble_evt
    ->evt
    .gatts_evt
    .params
    .write;

  uint16_t handle = write_evt->handle;
  uint16_t uuid = write_evt->uuid.uuid;

  UNUSED_PARAMETER(handle);

  if (uuid == BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME) {
    ble_handle_device_name_write(write_evt);
    return;
  }
}

void on_ble_evt(ble_evt_t *p_ble_evt) {
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("connected\r\n");
            connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
            break; // BLE_GAP_EVT_CONNECTED

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("disconnected\r\n");
            connection_handle = BLE_CONN_HANDLE_INVALID;
            break; // BLE_GAP_EVT_DISCONNECTED


        case BLE_GATTS_EVT_WRITE:
            ble_on_write(p_ble_evt);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(connection_handle,
                BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP,
                NULL,
                NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GAP_EVT_SEC_PARAMS_REQUEST

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(connection_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_SYS_ATTR_MISSING

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTC_EVT_TIMEOUT

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.\r\n");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_TIMEOUT

        case BLE_EVT_USER_MEM_REQUEST:
            err_code = sd_ble_user_mem_reply(p_ble_evt->evt.gattc_evt.conn_handle, NULL);
            APP_ERROR_CHECK(err_code);
            break; // BLE_EVT_USER_MEM_REQUEST

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID) {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)) {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE) {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    err_code = sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                        &auth_reply);
                    APP_ERROR_CHECK(err_code);
                }
            }
        }
        break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

#if (NRF_SD_BLE_API_VERSION == 3)
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            err_code = sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle,
                NRF_BLE_MAX_MTU_SIZE);
            APP_ERROR_CHECK(err_code);
            break; // BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST
#endif

        default:
            // No implementation needed.
            break;
    }
}

void ble_evt_dispatch(ble_evt_t *p_ble_evt) {
    on_ble_evt(p_ble_evt);
    ble_aio_on_ble_evt(p_ble_evt);
    ble_bss_on_ble_evt(p_ble_evt);
    ble_conn_params_on_ble_evt(p_ble_evt);
    ble_advertising_on_ble_evt(p_ble_evt);
    ble_dfu_on_ble_evt(&dfu, p_ble_evt);
    ble_bas_on_ble_evt(p_ble_evt);
    ble_configuration_on_ble_event(p_ble_evt);
    ble_gpio_asm_on_ble_evt(p_ble_evt);
}


void power_manage(void) {
    uint32_t err_code = sd_app_evt_wait();

    APP_ERROR_CHECK(err_code);
}


void on_conn_params_evt(ble_conn_params_evt_t *p_evt) {
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED) {
        err_code = sd_ble_gap_disconnect(connection_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

void conn_params_error_handler(uint32_t nrf_error) {
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void) {
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = false;
    cp_init.evt_handler = on_conn_params_evt;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

void advertising_event_handler(ble_adv_evt_t event) {
    is_advertising = event != BLE_ADV_EVT_IDLE;
    switch (event) {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_DEBUG("advertising mode BLE_ADV_EVT_FAST\n");
            break;
        case BLE_ADV_EVT_SLOW:
            NRF_LOG_DEBUG("advertising mode BLE_ADV_EVT_SLOW\n");
            break;
        case BLE_ADV_EVT_IDLE:
            NRF_LOG_DEBUG("advertising mode BLE_ADV_EVT_IDLE\n");
            break;
        default:
            NRF_LOG_DEBUG("advertising mode UNKNOWN\n");
            break;
    }
}

void advertising_init() {
    ret_code_t err_code;

    ble_adv_modes_config_t advertising_modes_config = {
      .ble_adv_whitelist_enabled = false,
      .ble_adv_directed_enabled = false,
      .ble_adv_directed_slow_enabled = false,
      .ble_adv_fast_enabled = true,
      .ble_adv_slow_enabled = true,

      .ble_adv_fast_interval = APP_ADV_INTERVAL_FAST,
      .ble_adv_fast_timeout = APP_ADV_TIMEOUT_FAST_SECS,
      .ble_adv_slow_interval = advertising_interval,
      .ble_adv_slow_timeout = APP_ADV_TIMEOUT_SLOW_SECS,
    };

    ble_uuid_t uuid_bss[] = {
        {
            .uuid = UUID_BINARY_SENSOR_SERVICE,
            .type = BLE_UUID_TYPE_BLE
        }, {
            .uuid = UUID_AUTOMATION_IO_SERVICE,
            .type = BLE_UUID_TYPE_BLE
        }, {
            .uuid = UUID_BINARY_SENSOR_SERVICE,
            .type = BLE_UUID_TYPE_BLE
        }
    };

    ble_advdata_t advertisement_data = {
      .name_type = BLE_ADVDATA_FULL_NAME,
      .include_appearance = false,
      .flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE,
      .uuids_complete = {
          .uuid_cnt = 3,
          .p_uuids = uuid_bss
      }
        // clearly something forgotten here
    };

    err_code = ble_advertising_init(
        &advertisement_data,
        NULL,
        &advertising_modes_config,
        advertising_event_handler,
        NULL
    );

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for starting advertising.
 */
void advertising_start() {
    ret_code_t err_code;

    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

void sys_evt_dispatch(uint32_t sys_evt) {
    storage_on_sys_evt(sys_evt);
}

void ble_stack_init(void) {
    uint32_t err_code;

    nrf_clock_lf_cfg_t clock_lf_cfg = NRF_CLOCK_LFCLKSRC;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(&clock_lf_cfg, NULL);

    ble_enable_params_t ble_enable_params;
    err_code = softdevice_enable_get_default_config(CENTRAL_LINK_COUNT,
        PERIPHERAL_LINK_COUNT,
        &ble_enable_params);
    ble_enable_params.common_enable_params.vs_uuid_count = 3;
    APP_ERROR_CHECK(err_code);

    //Check the ram settings against the used number of links
    CHECK_RAM_START_ADDR(CENTRAL_LINK_COUNT, PERIPHERAL_LINK_COUNT);

    // Enable BLE stack.
#if (NRF_SD_BLE_API_VERSION == 3)
    ble_enable_params.gatt_enable_params.att_mtu = NRF_BLE_MAX_MTU_SIZE;
#endif
    err_code = softdevice_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Subscribe for BLE events.
    err_code = softdevice_ble_evt_handler_set(ble_evt_dispatch);
    APP_ERROR_CHECK(err_code);

    softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(uint8_t *device_name, uint32_t device_name_length) {
    uint32_t                err_code;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
        device_name,
        device_name_length
    );
    APP_ERROR_CHECK(err_code);

    ble_gap_conn_params_t gap_conn_params;

    uint8_t params_data[10];
    storage_read_connection_params_configuration(params_data);

    if (params_data[0] != 0xff) {
        ble_configuration_connection_params_packet_t *params =
            (ble_configuration_connection_params_packet_t *)params_data;

        gap_conn_params.min_conn_interval = MSEC_TO_UNITS(params->min_conn_interval, UNIT_1_25_MS);
        gap_conn_params.max_conn_interval = MSEC_TO_UNITS(params->max_conn_interval, UNIT_1_25_MS);
        gap_conn_params.slave_latency = params->slave_latency;
        gap_conn_params.conn_sup_timeout = MSEC_TO_UNITS(params->conn_sup_timeout, UNIT_10_MS);

        advertising_interval = MSEC_TO_UNITS(params->advertising_interval, UNIT_0_625_MS);

        err_code = sd_ble_gap_ppcp_set(&gap_conn_params);

        if (err_code == NRF_SUCCESS) {
            return;
        }
        NRF_LOG_ERROR("failed setting stored connection parameters: %s\n", (uint32_t)ERR_TO_STR(err_code));
    }
    else {
        NRF_LOG_INFO("Connection params not configured\n");
    }

    gap_conn_params.min_conn_interval = BLE_DEFAULT_MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = BLE_DEFAULT_MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = BLE_DEFAULT_SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = BLE_DEFAULT_CONN_SUP_TIMEOUT;
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


uint32_t bas_init() {
    return ble_bas_init(&bas_init);
}


uint32_t dfu_init() {
    ble_dfu_init_t init = {
        .evt_handler = NULL
    };
    return ble_dfu_init(&dfu, &init);
}

void ble_handle_connection_parameters_configuration_update(ble_configuration_connection_params_packet_t *packet) {
    // should to some validation here

    ble_gap_conn_params_t real_params = {
        .min_conn_interval = MSEC_TO_UNITS(packet->min_conn_interval, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(packet->max_conn_interval, UNIT_1_25_MS),
        .slave_latency = packet->slave_latency,
        .conn_sup_timeout = MSEC_TO_UNITS(packet->conn_sup_timeout, UNIT_10_MS),
    };

    NRF_LOG_DEBUG("min interval: %d\n", packet->min_conn_interval);
    NRF_LOG_DEBUG("max interval: %d\n", packet->max_conn_interval);
    NRF_LOG_DEBUG("slave latency: %d\n", packet->slave_latency);
    NRF_LOG_DEBUG("sup timeout: %d\n", packet->conn_sup_timeout);
    NRF_LOG_DEBUG("adv interval: %d\n", packet->advertising_interval);

    ret_code_t err_code;

    err_code = ble_conn_params_change_conn_params(
        &real_params
    );

    if (err_code != NRF_SUCCESS) {
        NRF_LOG_ERROR("update failed: %s\n", (uint32_t)ERR_TO_STR(err_code));
    }
    else {
        NRF_LOG_INFO("udpate success\n");
    }
    // seems to lock up the chip
    // APP_ERROR_CHECK(err_code);
}

ret_code_t dis_init(){
    char *manufacturer = "https://github.com/dakhnod/nRF51-GPIO-BLE-Bridge";
    char *version_fw = "0.5.1";
    ble_srv_security_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&sec_mode.write_perm);
    ble_dis_init_t init = {
        .dis_attr_md = sec_mode,
        .fw_rev_str = {
            .p_str = (uint8_t*) version_fw,
            .length = strlen(version_fw)
        },
        .manufact_name_str = {
            .p_str = (uint8_t*) manufacturer,
            .length = strlen(manufacturer)
        }
    };

    return ble_dis_init(&init);
}

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void) {
    ret_code_t err_code;

    err_code = dis_init();
    APP_ERROR_CHECK(err_code);

    err_code = bas_init();
    APP_ERROR_CHECK(err_code);

    err_code = ble_configuration_service_init(ble_handle_connection_parameters_configuration_update);
    APP_ERROR_CHECK(err_code);

    err_code = ble_aio_init();
    APP_ERROR_CHECK(err_code);

    ble_gpio_asm_init();

    err_code = dfu_init();
    APP_ERROR_CHECK(err_code);
}

void
advertising_stop() {
    if (!is_advertising) {
        return;
    }
    uint32_t err = sd_ble_gap_adv_stop();
    APP_ERROR_CHECK(err);
}
