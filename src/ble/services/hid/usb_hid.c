#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "app_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd_hid_generic.h"
#include "app_usbd_hid_mouse.h"
#include "app_usbd_hid_kbd.h"
#include "nrf_log.h"
#include "app_timer.h"
#include "ble_helpers.h"
#include "sdk_common.h"
#include "ble_srv_common.h"
#include "nrf_gpio.h"

static void hid_user_mouse_ev_handler(app_usbd_class_inst_t const * p_inst, app_usbd_hid_user_event_t event);
static void hid_user_keyboard_ev_handler(app_usbd_class_inst_t const * p_inst, app_usbd_hid_user_event_t event);

APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(keyboard_desc, APP_USBD_HID_KBD_REPORT_DSC());
APP_USBD_HID_GENERIC_SUBCLASS_REPORT_DESC(mouse_desc,    {
    0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // Usage (Mouse)
    0xA1, 0x01,        // Collection (Application)
    0x09, 0x01,        //   Usage (Pointer)
    0xA1, 0x00,        //   Collection (Physical)
    
    // --- 3 Buttons (Bits 0-2) + 5 Bits Padding ---
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01 - Button 1 / Left)
    0x29, 0x03,        //     Usage Maximum (0x03 - Button 3 / Middle)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x03,        //     Report Count (3)
    0x75, 0x01,        //     Report Size (1 bit)
    0x81, 0x02,        //     Input (Data, Var, Abs)
    
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x05,        //     Report Size (5 bits)
    0x81, 0x03,        //     Input (Const, Var, Abs) - Padding bits
    
    // --- Absolute X and Y (16-bit unsigned each: 0 to 32767) ---
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x00,        //     Logical Minimum (0)
    0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
    0x75, 0x10,        //     Report Size (16 bits)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x02,        //     Input (Data, Var, Abs) - Absolute coordinates
    
    0xC0,              //   End Collection (Physical)
    0xC0               // End Collection (Application)
});

static const app_usbd_hid_subclass_desc_t * reps_mouse[] = {&mouse_desc};
static const app_usbd_hid_subclass_desc_t * reps_keyboard[] = {&keyboard_desc};

#define REPORT_OUT_MAXSIZE_MOUSE  0
#define REPORT_IN_QUEUE_SIZE_MOUSE    4
#define HID_GENERIC_INTERFACE_MOUSE  0

APP_USBD_HID_GENERIC_GLOBAL_DEF(m_app_hid_generic_mouse,
                                HID_GENERIC_INTERFACE_MOUSE,
                                hid_user_mouse_ev_handler,
                                (NRF_DRV_USBD_EPIN1),
                                reps_mouse,
                                REPORT_IN_QUEUE_SIZE_MOUSE,
                                REPORT_OUT_MAXSIZE_MOUSE,
                                APP_USBD_HID_SUBCLASS_NONE,
                                APP_USBD_HID_PROTO_GENERIC);



#define REPORT_OUT_MAXSIZE_KEYBOARD  1
#define REPORT_IN_QUEUE_SIZE_KEYBOARD    2
#define HID_GENERIC_INTERFACE_KEYBOARD  1

APP_USBD_HID_GENERIC_GLOBAL_DEF(m_app_hid_generic_keyboard,
                                HID_GENERIC_INTERFACE_KEYBOARD,
                                hid_user_keyboard_ev_handler,
                                (NRF_DRV_USBD_EPIN2/*, NRF_DRV_USBD_EPOUT1*/),
                                reps_keyboard,
                                REPORT_IN_QUEUE_SIZE_KEYBOARD,
                                REPORT_OUT_MAXSIZE_KEYBOARD,
                                APP_USBD_HID_SUBCLASS_BOOT,
                                APP_USBD_HID_PROTO_KEYBOARD);

APP_TIMER_DEF(input_timer);

#define UUID_HID_INJECTOR_BASE { 0xa5, 0xd8, 0x1d, 0x75, 0x40, 0xfe, 0x40, 0xea, 0x8d, 0x6d, 0x44, 0xd4, 0xdb, 0xca, 0x1a, 0xab }

uint8_t ble_hid_injector_custom_uuid_type;
uint16_t ble_hid_injector_service_handle;
uint16_t ble_hid_injector_characteristic_keyboard_value_handle;
uint16_t ble_hid_injector_characteristic_mouse_value_handle;
uint16_t ble_usb_hid_connection_handle;

uint8_t mouse_data[5];
uint8_t keyboard_data[8] = { 0 };

static void hid_user_mouse_ev_handler(app_usbd_class_inst_t const * p_inst, app_usbd_hid_user_event_t event){
    switch (event)
    {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
        {
            /* No output report defined for this example.*/
            NRF_LOG_DEBUG("mouse IN report ready");
            size_t report_size;
            (void)app_usbd_hid_generic_out_report_get(&m_app_hid_generic_mouse, &report_size);
            // ASSERT(0);
            break;
        }
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
        {
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
        {
            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
            NRF_LOG_INFO("mouse SET_BOOT_PROTO");
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
        {
            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
            NRF_LOG_INFO("mouse SET_REPORT_PROTO");
            break;
        }
        default:
            break;
    }
}


static void hid_user_keyboard_ev_handler(app_usbd_class_inst_t const * p_inst, app_usbd_hid_user_event_t event){
    switch (event)
    {
        case APP_USBD_HID_USER_EVT_OUT_REPORT_READY:
        {
            /* No output report defined for this example.*/
            NRF_LOG_DEBUG("keyboard IN report ready");
            size_t report_size;
            (void)app_usbd_hid_generic_out_report_get(&m_app_hid_generic_keyboard, &report_size);
            // ASSERT(0);
            break;
        }
        case APP_USBD_HID_USER_EVT_IN_REPORT_DONE:
        {
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_BOOT_PROTO:
        {
            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
            NRF_LOG_INFO("keyboard SET_BOOT_PROTO");
            break;
        }
        case APP_USBD_HID_USER_EVT_SET_REPORT_PROTO:
        {
            UNUSED_RETURN_VALUE(hid_generic_clear_buffer(p_inst));
            NRF_LOG_INFO("keyboard SET_REPORT_PROTO");
            break;
        }
        default:
            break;
    }
}

static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SOF: {
            // NRF_LOG_INFO("APP_USBD_EVT_DRV_SOF");
            break;
        }
        case APP_USBD_EVT_DRV_RESET:
            NRF_LOG_INFO("APP_USBD_EVT_DRV_RESET");
            break;
        case APP_USBD_EVT_DRV_SUSPEND:
            NRF_LOG_INFO("APP_USBD_EVT_DRV_SUSPEND");
            app_usbd_suspend_req(); // Allow the library to put the peripheral into sleep mode
            break;
        case APP_USBD_EVT_DRV_RESUME:
            NRF_LOG_INFO("APP_USBD_EVT_DRV_RESUME");
            break;
        case APP_USBD_EVT_STARTED:
            NRF_LOG_INFO("APP_USBD_EVT_STARTED");
            break;
        case APP_USBD_EVT_STOPPED:
            NRF_LOG_INFO("APP_USBD_EVT_STOPPED");
            app_usbd_disable();
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            NRF_LOG_INFO("USB power detected");
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            NRF_LOG_INFO("USB power removed");
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            NRF_LOG_INFO("USB ready");
            app_usbd_start();
            break;
        default:
            break;
    }
}

static ret_code_t idle_handle(app_usbd_class_inst_t const * p_inst, uint8_t report_id)
{
    /*
    switch (report_id)
    {
        case 0:
        {
            uint8_t report[] = {0xBE, 0xEF};
            return app_usbd_hid_generic_in_report_set(
              &m_app_hid_generic,
              report,
              sizeof(report));
        }
        default:
            return NRF_ERROR_NOT_SUPPORTED;
    }
    */
   return 0;
}

void on_input_timer_timeout(void *d) {
    NRF_LOG_DEBUG("input timer tick");
}

void usb_init() {
    ret_code_t err_code;

    NRF_LOG_DEBUG("USB init");

    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler
    };

    APP_ERROR_CHECK(app_usbd_init(&usbd_config));

    NRF_LOG_DEBUG("after init");

    app_usbd_class_inst_t const 
        *class_inst_generic_mouse = app_usbd_hid_generic_class_inst_get(&m_app_hid_generic_mouse),
        *class_inst_generic_keyboard = app_usbd_hid_generic_class_inst_get(&m_app_hid_generic_keyboard);
    
    // err_code = hid_generic_idle_handler_set(class_inst_generic, idle_handle);
    //APP_ERROR_CHECK(err_code);
    UNUSED_PARAMETER(idle_handle);

    err_code = app_usbd_class_append(class_inst_generic_keyboard);
    APP_ERROR_CHECK(err_code);
    err_code = app_usbd_class_append(class_inst_generic_mouse);
    APP_ERROR_CHECK(err_code);

    app_usbd_enable();
    app_usbd_start();

    UNUSED_PARAMETER(input_timer);

    // app_timer_create(&input_timer, APP_TIMER_MODE_REPEATED, on_input_timer_timeout);
    // app_timer_start(input_timer, APP_TIMER_TICKS(1000), NULL);
}

void hid_injector_ble_init() {
    ret_code_t err_code;

    ble_uuid128_t vs_uuid = {
        .uuid128 = UUID_HID_INJECTOR_BASE
    };
    err_code = sd_ble_uuid_vs_add(&vs_uuid, &ble_hid_injector_custom_uuid_type);
    APP_ERROR_CHECK(err_code);

    ble_uuid_t uuid_service = {
        .uuid = 0x0000,
        .type = ble_hid_injector_custom_uuid_type
    };

    err_code = sd_ble_gatts_service_add(
        BLE_GATTS_SRVC_TYPE_PRIMARY,
        &uuid_service,
        &ble_hid_injector_service_handle
    );
    APP_ERROR_CHECK(err_code);

    ble_helper_characteristic_init_t ble_injector_mouse_characteristic = {
        .service_handle = ble_hid_injector_service_handle,
        .uuid = 0x0001,
        .uuid_type = ble_hid_injector_custom_uuid_type,
        .description_str = "HID report data for mouse",
        .is_writable = true,
        .max_length = 5,
        .value_handle = &ble_hid_injector_characteristic_mouse_value_handle
    };
    err_code = ble_helper_characteristic_add(&ble_injector_mouse_characteristic);
    APP_ERROR_CHECK(err_code);

    ble_helper_characteristic_init_t ble_injector_keyboard_characteristic = {
        .service_handle = ble_hid_injector_service_handle,
        .uuid = 0x0002,
        .uuid_type = ble_hid_injector_custom_uuid_type,
        .description_str = "HID report data for keyboard",
        .is_writable = true,
        .max_length = 8,
        .value_handle = &ble_hid_injector_characteristic_keyboard_value_handle
    };
    err_code = ble_helper_characteristic_add(&ble_injector_keyboard_characteristic);
    APP_ERROR_CHECK(err_code);
}

void usb_hid_init() {
    usb_init();
    hid_injector_ble_init();
}

void ble_usb_hid_on_connect(const ble_evt_t *p_ble_evt)
{
    ble_usb_hid_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

void ble_usb_hid_on_disconnect(const ble_evt_t *p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    ble_usb_hid_connection_handle = BLE_CONN_HANDLE_INVALID;
}

void ble_usb_hid_on_write(const ble_evt_t *p_ble_evt)
{
    const ble_gatts_evt_write_t *write_evt = &p_ble_evt
                                            ->evt
                                            .gatts_evt
                                            .params
                                            .write;

    uint16_t handle = write_evt->handle;

    if (handle == ble_hid_injector_characteristic_mouse_value_handle)
    {
        if (write_evt->len != 5) {
            return;
        }
        // static uint8_t report[4] = {0, 10, 10, 0};
        ret_code_t err_code = app_usbd_hid_generic_in_report_set(
            &m_app_hid_generic_mouse,
            write_evt->data,
            write_evt->len
        );
        NRF_LOG_DEBUG("mouse ret: %i %i", write_evt->len, err_code);

        if (write_evt->data[0] != 0x00) {
            memcpy(mouse_data, write_evt->data, write_evt->len);
            mouse_data[0] = 0x00;
            ret_code_t err_code = app_usbd_hid_generic_in_report_set(
                &m_app_hid_generic_mouse,
                mouse_data,
                write_evt->len
            );
            UNUSED_VARIABLE(err_code);
        }
        return;
    }

    if (handle == ble_hid_injector_characteristic_keyboard_value_handle) {
        if (write_evt->len != 8) {
            return;
        }

        // static uint8_t press_report[8]   = { 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00 };
        // static uint8_t release_report[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

        ret_code_t err_code = app_usbd_hid_generic_in_report_set(
            &m_app_hid_generic_keyboard,
            write_evt->data,
            write_evt->len
        );
        NRF_LOG_DEBUG("keyboard ret: %i %i", write_evt->len, err_code);
        UNUSED_PARAMETER(err_code);

        err_code = app_usbd_hid_generic_in_report_set(
            &m_app_hid_generic_keyboard,
            keyboard_data,
            8
        );
        NRF_LOG_DEBUG("keyboard ret: %i %i", write_evt->len, err_code);

        // release_keyboard = true;
    }
}

void ble_usb_hid_on_ble_evt(const ble_evt_t *p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        ble_usb_hid_on_connect(p_ble_evt);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        ble_usb_hid_on_disconnect(p_ble_evt);
        break;

    case BLE_GATTS_EVT_WRITE:
        ble_usb_hid_on_write(p_ble_evt);
        break;

    default:
        // No implementation needed.
        break;
    }
}