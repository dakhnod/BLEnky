#include <stdint.h>
#include <sdk_errors.h>
#include <i2c_service.h>
#include <nrf_log.h>
#include <sensor_timer.h>
#include <nrf_delay.h>
#include <sensor_gpio.h>
#include <math.h>
#include <feature_config.h>

#define BMA400_ADDRESS 0x14

#define LOG_XYZ(label, x, y, z)  NRF_LOG_DEBUG(label "  x: " NRF_LOG_FLOAT_MARKER ", y: " NRF_LOG_FLOAT_MARKER ", z: " NRF_LOG_FLOAT_MARKER "\n", NRF_LOG_FLOAT(x), NRF_LOG_FLOAT(y), NRF_LOG_FLOAT(z));

APP_TIMER_DEF(state_timer);

#define CHECK_ERROR(code) do{ \
    if(code != NRF_SUCCESS) { \
        return code;          \
    }                         \
}while(0)

gpio_input_change_handler_t gpio_change_handler;

ret_code_t bma400_write(uint8_t *data, uint8_t length) {
    return i2c_write(BMA400_ADDRESS, data, length);
}

ret_code_t bma400_read(uint8_t register_address, uint8_t *data, uint8_t length) {
    return i2c_read(BMA400_ADDRESS, register_address, data, length);
}

ret_code_t bma400_reset(){
    uint8_t data[] = {0x7e, 0xb6};
    return bma400_write(data, sizeof(data));
}

ret_code_t bma400_set_low_power(){
    uint8_t data[] = {0x19, 0x01};
    return bma400_write(data, sizeof(data));
}

ret_code_t bma400_set_auto_wakeup(){
    uint8_t wakeup_int_data[] = {0x2d, 0x02};
    CHECK_ERROR(bma400_write(wakeup_int_data, sizeof(wakeup_int_data)));

    uint8_t data[] = {
        0x2f, 0xe2, // WKUP_INT_CONFIG0, enable xyz, ref update every time
        0x30, 0x28, // WKUP_INT_CONFIG1, threshold 40
        // 0x31, 0x00, // WKUP_INT_CONFIG2 refx 0
        // 0x32, 0x00, // WKUP_INT_CONFIG3 refy 0
        // 0x33, 0x40  // WKUP_INT_CONFIG4 refz 64
    };

    return bma400_write(data, sizeof(data));
}

ret_code_t bma400_set_auto_low_power(){
    uint8_t autolowpower_int_data[] = {0x2b, 0x02};
    CHECK_ERROR(bma400_write(autolowpower_int_data, sizeof(autolowpower_int_data)));
    
    uint8_t data[] = {
        0x3f, 0xf9, // GEN1INT_CONFIG0 all axes, filter source acc_filt2, gen1_act_refu every time, gen1_act_hyst 24mg
        0x40, 0x01, // GEN1INT_CONFIG1 logical AND
        0x41, 0x05, // GEN1INT_CONFIG2 gen1_int_thres 5
        0x42, 0x00, // GEN1INT_CONFIG3 duration 0
    };

    CHECK_ERROR(bma400_write(data, sizeof(data)));

    uint8_t data2[] = {
        0x43, 0x32  // GEN1INT_CONFIG31 gen1_int_dur_7_0 32
    };
    CHECK_ERROR(bma400_write(data2, sizeof(data2)));

    return NRF_SUCCESS;
}

ret_code_t bma400_get_state(uint8_t *state){
    return bma400_read(0x03, state, 1);
}

void timeout_handler(){
    uint8_t state;

    bma400_get_state(&state);

    NRF_LOG_DEBUG("bma400 state: %x\n", state);
}

void bma400_handle_gpio_event(uint32_t index, gpio_config_input_digital_t *config){
    if(config->pin != BMA400_INTERRUPT_PIN) {
        return;
    }
    // uint8_t interrupt_reg;

    // bma400_read(0x0E, &interrupt_reg, 1);

    // NRF_LOG_DEBUG("int stat: %x\n", interrupt_reg);

    if(config->state != 1) {
        return;
    }

    uint8_t data[6];

    bma400_read(0x04, data, 6);

    int16_t x_raw = *((int16_t*)(data + 0));
    int16_t y_raw = *((int16_t*)(data + 2));
    int16_t z_raw = *((int16_t*)(data + 4));

    if(x_raw > 2047) {
        x_raw -= 4096;
    }
    if(y_raw > 2047) {
        y_raw -= 4096;
    }
    if(z_raw > 2047) {
        z_raw -= 4096;
    }

    NRF_LOG_DEBUG("raw measurements: %d %d %d\n", x_raw, y_raw, z_raw);

    double x = x_raw / 512.0;
    double y = y_raw / 512.0;
    double z = z_raw / 512.0;

    NRF_LOG_DEBUG("stopped!\n");
    NRF_LOG_DEBUG("x: " NRF_LOG_FLOAT_MARKER ", y: " NRF_LOG_FLOAT_MARKER ", z: " NRF_LOG_FLOAT_MARKER "\n", NRF_LOG_FLOAT(x), NRF_LOG_FLOAT(y), NRF_LOG_FLOAT(z));

    double orientations[] = ORIENTATION_VECTORS;

    int closest_point = -1;
    double smallest_distance;

    gpio_config_input_digital_t *highest_config = NULL;

    int count = (sizeof(orientations) / sizeof(double) / 3);

    for(int i = 0; i < count; i++) {
        double compared_x = orientations[(i * 3) + 0];
        double compared_y = orientations[(i * 3) + 1];
        double compared_z = orientations[(i * 3) + 2];

        double delta_x = fabs(compared_x - x);
        double delta_y = fabs(compared_y - y);
        double delta_z = fabs(compared_z - z);

        double distance = sqrtf((delta_x * delta_x) + (delta_y * delta_y) + (delta_z * delta_z));

        LOG_XYZ("comparing to", compared_x, compared_y, compared_z);
        NRF_LOG_DEBUG("distance: " NRF_LOG_FLOAT_MARKER "\n", NRF_LOG_FLOAT(distance));

        gpio_config_input_digital_t *current_config = gpio_find_input_by_index(i);
        current_config->changed = true;
        current_config->state = 0;

        if((closest_point == -1) || (distance < smallest_distance)) {
            smallest_distance = distance;
            closest_point = i;
            highest_config = current_config;
        }
    }

    highest_config->state = 1;

    char *labels[] = {
        "x up",
        "x down",
        "y up",
        "y down",
        "z up",
        "z down",
    };

    NRF_LOG_DEBUG("closest distance: " NRF_LOG_FLOAT_MARKER "   orientation: %s\n", NRF_LOG_FLOAT(smallest_distance), (uint32_t) (labels[closest_point]));

    // gpio_change_handler(count);
}

ret_code_t bma400_setup_orientation_detection(gpio_input_change_handler_t change_handler){
    gpio_change_handler = change_handler;

    NRF_LOG_INFO("bma400_reset...\n");
    CHECK_ERROR(bma400_reset());

    
    nrf_delay_ms(100);

    NRF_LOG_INFO("bma400_set_low_power...\n");
    CHECK_ERROR(bma400_set_low_power());
    NRF_LOG_INFO("bma400_set_auto_wakeup...\n");
    CHECK_ERROR(bma400_set_auto_wakeup());

    CHECK_ERROR(bma400_set_auto_low_power());

    uint8_t data[] = {
        0x20, 0x00, // INT_CONFIG1
        0x21, 0x04, // INT1_MAP gen1_int1
        0x22, 0x00, // INT2_MAP
        0x23, 0x00, // INT12_MAP
    };

    CHECK_ERROR(bma400_write(data, sizeof(data)));

    uint8_t data2[] = {
        0x24, 0x22, // INT12_IO_CTRL interrupt pin active-high
    };

    CHECK_ERROR(bma400_write(data2, sizeof(data2)));

    uint8_t gen1_int_en_data[] = {0x1f, 0x04};
    CHECK_ERROR(bma400_write(gen1_int_en_data, sizeof(gen1_int_en_data)));

    return NRF_SUCCESS;

    app_timer_create(
        &state_timer,
        APP_TIMER_MODE_REPEATED,
        timeout_handler
    );

    app_timer_start(
        state_timer,
        APP_TIMER_TICKS(500, APP_TIMER_PRESCALER),
        NULL
    );

    return NRF_SUCCESS;
}