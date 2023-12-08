#ifndef GPIOASM_H
#define GPIOASM_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifndef GPIOASM_BUFFER_SIZE
#define GPIOASM_BUFFER_SIZE 64
#endif

#ifndef GPIOASM_LOG_DEBUG
#define GPIOASM_LOG_DEBUG(...)
#endif

#ifndef GPIOASM_LOG_ERROR
#define GPIOASM_LOG_ERROR(...)
#endif

#ifndef MIN
#define MIN(a, b) a < b ? a : b
#endif

typedef void (*pin_digital_output_handler_t)(
    uint32_t index, 
    uint8_t state
);

typedef void (*pin_analog_output_handler_t)(
    uint32_t index,
    uint16_t duty_cycle
);

typedef bool (*pin_digital_input_provider_t)(
    uint32_t index
);

typedef void (*timer_handler_t)(
    uint64_t timeout,
    bool start
);

typedef struct {
    pin_digital_output_handler_t pin_digital_output_handler;
    pin_analog_output_handler_t pin_analog_output_handler;
    pin_digital_input_provider_t pin_digital_input_provider;
    timer_handler_t timer_handler;
} gpioasm_engine_init_t;

typedef struct {
    // code storage
    uint8_t instruction_buffer[GPIOASM_BUFFER_SIZE];

    // registers
    uint32_t instruction_index;
    uint8_t is_running;

    // transmission registers
    uint32_t write_index;
    uint32_t packet_index;

    // branch registers
    uint32_t jump_counter;
    uint32_t jump_target;
    uint32_t sleep_pin_data;
    enum {
        SLEEP_NO_CONDITION,
        SLEEP_MATCH_PINS_ALL,
        SLEEP_MATCH_PINS_ANY,
        SLEEP_MATCH_PINS_ALL_TIMEOUT,
        SLEEP_MATCH_PINS_ANY_TIMEOUT,
    } sleep_condition;

    // handlers
    gpioasm_engine_init_t init;
} gpioasm_engine_t;

typedef enum {
    PUSH_SUCCESS,
    PUSH_MISSED_PACKET,
    PUSH_OVERFLOW,
    PUSH_FIRST_PACKET,
    PUSH_FINAL_PACKET
} GPIOASM_PACKET_PUSH_RESULT;

typedef enum {
    DECODE_SUCCESS,
    DECODE_OVERFLOW
} GPIOASM_DECODE_RESULT;

void gpioasm_start(gpioasm_engine_t *engine);
void gpioasm_stop(gpioasm_engine_t *engine);
void gpioasm_init(gpioasm_engine_t *engine, gpioasm_engine_init_t *gpioasm_engine_init);
void gpioasm_step(gpioasm_engine_t *engine);
uint8_t gpioasm_is_running(gpioasm_engine_t *engine);
GPIOASM_PACKET_PUSH_RESULT gpioasm_push_packet(gpioasm_engine_t *engine, uint8_t *data, uint32_t length);
void gpioasm_handle_digital_input_update(gpioasm_engine_t *engine, uint32_t index, bool is_high);
void gpioasm_handle_timer_timeout(gpioasm_engine_t *engine);
bool gpioasm_read_has_reached_end(gpioasm_engine_t *engine);

#endif