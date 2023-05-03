#include "sensor_ble.h"

#include "ble_configuration_service.h"
#include "ble_cycling_speed_cadence.h"
#include "ble_gpio_asm.h"
#include "app_error.h"
#include "ble_dis.h"
#include "nrf_delay.h"
#include "feature_config.h"
#include "ble_hid.h"
#include "peer_manager.h"
#include "ble_conn_state.h"
#include "fds.h"
#include "sleep.h"

ble_gap_adv_params_t m_adv_params;
ble_advdata_t advdata;

ble_dfu_t dfu;

bool is_advertising = false;

uint16_t connection_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */

uint16_t advertising_interval = APP_ADV_INTERVAL_SLOW;


void peer_manager_event_handler(pm_evt_t const *p_evt)
{
    ret_code_t err_code;
    switch (p_evt->evt_id)
    {
    case PM_EVT_BONDED_PEER_CONNECTED:
        NRF_LOG_DEBUG("PM_EVT_BONDED_PEER_CONNECTED\n");
        // Update the rank of the peer.
        err_code = pm_peer_rank_highest(p_evt->peer_id);
        break;
    case PM_EVT_CONN_SEC_START:
        NRF_LOG_DEBUG("PM_EVT_CONN_SEC_START\n");
        break;
    case PM_EVT_CONN_SEC_SUCCEEDED:
        // Update the rank of the peer.
        NRF_LOG_DEBUG("PM_EVT_CONN_SEC_SUCCEEDED\n");
        err_code = pm_peer_rank_highest(p_evt->peer_id);
        break;
    case PM_EVT_CONN_SEC_FAILED:
        // In some cases, when securing fails, it can be restarted directly. Sometimes it can be
        // restarted, but only after changing some Security Parameters. Sometimes, it cannot be
        // restarted until the link is disconnected and reconnected. Sometimes it is impossible
        // to secure the link, or the peer device does not support it. How to handle this error
        // is highly application-dependent.
        NRF_LOG_DEBUG("PM_EVT_CONN_SEC_FAILED\n");
        break;
    case PM_EVT_CONN_SEC_CONFIG_REQ:
    {
        // A connected peer (central) is trying to pair, but the Peer Manager already has a bond
        // for that peer. Setting allow_repairing to false rejects the pairing request.
        // If this event is ignored (pm_conn_sec_config_reply is not called in the event
        // handler), the Peer Manager assumes allow_repairing to be false.
        NRF_LOG_DEBUG("PM_EVT_CONN_SEC_CONFIG_REQ\n");
        pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
        pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
    }
    break;
    case PM_EVT_STORAGE_FULL:
        // Run garbage collection on the flash.
        NRF_LOG_DEBUG("PM_EVT_STORAGE_FULL\n");
        err_code = fds_gc();
        if (err_code == FDS_ERR_BUSY || err_code == FDS_ERR_NO_SPACE_IN_QUEUES)
        {
            // Retry.
        }
        else
        {
            APP_ERROR_CHECK(err_code);
        }
        break;
    case PM_EVT_ERROR_UNEXPECTED:
        // Assert.
        NRF_LOG_DEBUG("PM_EVT_ERROR_UNEXPECTED\n");
        APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        break;
    case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        NRF_LOG_DEBUG("PM_EVT_PEER_DATA_UPDATE_SUCCEEDED\n");
        break;
    case PM_EVT_PEER_DATA_UPDATE_FAILED:
        // Assert.
        NRF_LOG_DEBUG("PM_EVT_PEER_DATA_UPDATE_FAILED\n");
        APP_ERROR_CHECK_BOOL(false);
        break;
    case PM_EVT_PEER_DELETE_SUCCEEDED:
        NRF_LOG_DEBUG("PM_EVT_PEER_DELETE_SUCCEEDED\n");
        break;
    case PM_EVT_PEER_DELETE_FAILED:
        // Assert.
        NRF_LOG_DEBUG("PM_EVT_PEER_DELETE_FAILED\n");
        APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        break;
    case PM_EVT_PEERS_DELETE_SUCCEEDED:
        // At this point it is safe to start advertising or scanning.
        NRF_LOG_DEBUG("PM_EVT_PEERS_DELETE_SUCCEEDED\n");
        break;
    case PM_EVT_PEERS_DELETE_FAILED:
        // Assert.
        NRF_LOG_DEBUG("PM_EVT_PEERS_DELETE_FAILED\n");
        APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        break;
    case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        NRF_LOG_DEBUG("PM_EVT_LOCAL_DB_CACHE_APPLIED\n");
        break;
    case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        // The local database has likely changed, send service changed indications.
        NRF_LOG_DEBUG("PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED\n");
        pm_local_database_has_changed();
        break;
    case PM_EVT_SERVICE_CHANGED_IND_SENT:
        NRF_LOG_DEBUG("PM_EVT_SERVICE_CHANGED_IND_SENT\n");
        break;
    case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        NRF_LOG_DEBUG("PM_EVT_SERVICE_CHANGED_IND_CONFIRMED");
        break;
    }
}

void peer_manager_init()
{
    ret_code_t err_code;
    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    uint32_t caps = BLE_GAP_IO_CAPS_NONE;
    bool mitm = false;

    #if STATIC_PASSKEY_ENABLED == 1
    if(strlen(BLE_BONDIG_PASSKEY) != 6){
        NRF_LOG_ERROR("Passkey needs to be six digits long");
    }
    mitm = true;
    caps = BLE_GAP_IO_CAPS_DISPLAY_ONLY;
    static ble_opt_t passkey_opt;
    passkey_opt.gap_opt.passkey.p_passkey = (uint8_t*) BLE_BONDIG_PASSKEY;
    err_code = sd_ble_opt_set(BLE_GAP_OPT_PASSKEY, &passkey_opt);
    APP_ERROR_CHECK(err_code);
    #endif

    ble_gap_sec_params_t sec_param = {
        .bond = true,
        .mitm = mitm,
        .lesc = false,
        .keypress = false,
        .io_caps = caps,
        .min_key_size = 7,
        .max_key_size = 16,
        .kdist_own.enc = 1,
        .kdist_own.id = 1,
        .kdist_peer.enc = 1,
        .kdist_peer.id = 1,
    };

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(peer_manager_event_handler);
    APP_ERROR_CHECK(err_code);
}



// Simple event handler to handle errors during initialization.
void fds_evt_handler(fds_evt_t const *const p_fds_evt)
{
    switch (p_fds_evt->id)
    {
    case FDS_EVT_INIT:
        if (p_fds_evt->result == FDS_SUCCESS)
        {
            NRF_LOG_DEBUG("fds init success\n");
        }
        else
        {
            NRF_LOG_ERROR("fds init error: %d\n", p_fds_evt->result);
        }
        break;
    case FDS_EVT_WRITE:
        if (p_fds_evt->result == FDS_SUCCESS)
        {
            NRF_LOG_DEBUG("fds write success\n");
        }
        else
        {
            NRF_LOG_ERROR("fds write error: %d\n", p_fds_evt->result);
        }
        break;
    default:
        break;
    }
}

void filesystem_init()
{
    ret_code_t err_code = fds_register(fds_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = fds_init();
    APP_ERROR_CHECK(err_code);
}

void ble_init() {
    uint8_t device_name[LENGTH_DEVICE_NAME];
    uint32_t device_name_length;

    storage_read_device_name(device_name, &device_name_length);

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
    // allow flash operation to complete. 
    // Shitty solution, but for some reason there is no sys_evt fired to indicate a finished flash operation
    nrf_delay_ms(3); 
    advertising_init();

    #if FEATURE_ENABLED(BLE_BONDING)
    filesystem_init();
    peer_manager_init();
    #endif
}

void ble_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
    #if FEATURE_ENABLED(AUTOMATION_IO)
    ble_aio_handle_input_change(index, config);
    #endif

    #if FEATURE_ENABLED(GPIO_ASM)
    ble_gpio_asm_handle_input_change(index, config);
    #endif

    #if FEATURE_ENABLED(CYCLING_SPEED_CADENCE)
    ble_csc_handle_input_change(index, config);
    #endif

    #if FEATURE_ENABLED(HID)
    ble_hid_handle_input_change(index, config);
    #endif

    bool is_connected = (connection_handle != BLE_CONN_HANDLE_INVALID);
    if(!is_connected && !is_advertising){
        // awoke from light sleep mode
        advertising_start();
    }
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
            is_advertising = false;
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
            // Only pass through to PM if feature is enabled
            #if FEATURE_ENABLED(BLE_BONDING)
            break;
            #endif

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
    ble_conn_state_on_ble_evt(p_ble_evt);
    pm_on_ble_evt(p_ble_evt);

    on_ble_evt(p_ble_evt);

    #if FEATURE_ENABLED(AUTOMATION_IO)
    ble_aio_on_ble_evt(p_ble_evt);
    #endif

    #if FEATURE_ENABLED(BINARY_SENSOR)
    ble_bss_on_ble_evt(p_ble_evt);
    #endif

    ble_conn_params_on_ble_evt(p_ble_evt);

    if(p_ble_evt->header.evt_id == BLE_GAP_EVT_DISCONNECTED){
        // disallow advertising if the sleep module forbids it
        bool can_advertise = true;
        #if FEATURE_ENABLED(SLEEP_MODE)
            can_advertise = sleep_get_allow_advertise();
        #endif
        if(can_advertise){
            ble_advertising_on_ble_evt(p_ble_evt);
        }
    }else{
        ble_advertising_on_ble_evt(p_ble_evt);
    }

    ble_dfu_on_ble_evt(&dfu, p_ble_evt);

    #if FEATURE_ENABLED(BATTERY_PROFILE)
    ble_bas_on_ble_evt(p_ble_evt);
    #endif

    ble_configuration_on_ble_event(p_ble_evt);

    #if FEATURE_ENABLED(GPIO_ASM)
    ble_gpio_asm_on_ble_evt(p_ble_evt);
    #endif

    #if FEATURE_ENABLED(CYCLING_SPEED_CADENCE)
    ble_csc_on_ble_evt(p_ble_evt);
    #endif

    #if FEATURE_ENABLED(HID)
    ble_hid_on_ble_evt(p_ble_evt);
    #endif
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

    uint8_t uuid_len = 0;
    ble_uuid_t uuids[4]; // size may be updated ini the future

    #if FEATURE_ENABLED(BINARY_SENSOR)
        uuids[uuid_len].uuid = UUID_BINARY_SENSOR_SERVICE;
        uuids[uuid_len].type = BLE_UUID_TYPE_BLE;
        uuid_len++;
    #endif

    #if FEATURE_ENABLED(AUTOMATION_IO)
        uuids[uuid_len].uuid = UUID_AUTOMATION_IO_SERVICE;
        uuids[uuid_len].type = BLE_UUID_TYPE_BLE;
        uuid_len++;
    #endif

    #if FEATURE_ENABLED(CYCLING_SPEED_CADENCE)
        uuids[uuid_len].uuid = UUID_CSC_SERVICE;
        uuids[uuid_len].type = BLE_UUID_TYPE_BLE;
        uuid_len++;
    #endif

    #if FEATURE_ENABLED(HID)
        uuids[uuid_len].uuid = BLE_UUID_HID_SERVICE;
        uuids[uuid_len].type = BLE_UUID_TYPE_BLE;
        uuid_len++;
    #endif

    ble_advdata_t advertisement_data = {
      .name_type = BLE_ADVDATA_FULL_NAME,
      .include_appearance = true,
      .flags = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED | BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE,
      .uuids_complete = {
          .uuid_cnt = uuid_len,
          .p_uuids = uuids
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
    ble_advertising_on_sys_evt(sys_evt);
}

void ble_disable_rf(){
    if(connection_handle != BLE_CONN_HANDLE_INVALID){
        sd_ble_gap_disconnect(connection_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        connection_handle = BLE_CONN_HANDLE_INVALID;
    }
    advertising_stop();
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

    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
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
        NRF_LOG_WARNING("Connection params not configured\n");
    }

    gap_conn_params.min_conn_interval = BLE_DEFAULT_MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = BLE_DEFAULT_MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = BLE_DEFAULT_SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = BLE_DEFAULT_CONN_SUP_TIMEOUT;
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);

    #if FEATURE_ENABLED(HID)
    // set appearance to gamepad
    err_code = sd_ble_gap_appearance_set(0x03C4);
    APP_ERROR_CHECK(err_code);
    #elif FEATURE_ENABLED(CYCLING_SPEED_CADENCE)
    // set appearance to speed sensor
    // may include cadence sensor later
    err_code = sd_ble_gap_appearance_set(0x0482);
    APP_ERROR_CHECK(err_code);
    #endif
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
        NRF_LOG_DEBUG("udpate success\n");
    }
    // seems to lock up the chip
    // APP_ERROR_CHECK(err_code);
}

ret_code_t dis_init(){
    char *manufacturer = "https://github.com/dakhnod/nRF51-GPIO-BLE-Bridge";
    char *version_fw = FIRMWARE_VERSION;
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

    #if FEATURE_ENABLED(BATTERY_PROFILE)
    err_code = bas_init();
    APP_ERROR_CHECK(err_code);
    #endif

    err_code = ble_configuration_service_init(ble_handle_connection_parameters_configuration_update);
    APP_ERROR_CHECK(err_code);

    #if FEATURE_ENABLED(AUTOMATION_IO)
    err_code = ble_aio_init();
    APP_ERROR_CHECK(err_code);
    #endif

    #if FEATURE_ENABLED(GPIO_ASM)
    ble_gpio_asm_init();
    #endif

    #if FEATURE_ENABLED(HID)
    err_code = ble_hid_init();
    APP_ERROR_CHECK(err_code);
    #endif

    err_code = dfu_init();
    APP_ERROR_CHECK(err_code);

    #if FEATURE_ENABLED(CYCLING_SPEED_CADENCE)
    err_code = ble_csc_init();
    APP_ERROR_CHECK(err_code);
    #endif

    #if FEATURE_ENABLED(HID)
    err_code = ble_hid_init();
    APP_ERROR_CHECK(err_code);
    #endif
}

void
advertising_stop() {
    if (!is_advertising) {
        return;
    }
    uint32_t err = sd_ble_gap_adv_stop();
    APP_ERROR_CHECK(err);
    is_advertising = false;
}
