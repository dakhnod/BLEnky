#include "app_twi.h"
#include "app_error.h"
#include "nrf_log.h"
#include "app_timer.h"
#include "sensor_timer.h"


void twi_init_asdasd(void)
{
    uint32_t err_code;

    app_twi_transfer_t configs[] = {
        APP_TWI_WRITE(0x14, &register_address, 1, 0),
    };

    /*
    app_twi_transaction_t transaction = {
        .p_transfers = type_read,
        .number_of_transfers = 1
    };
    */

    err_code = app_twi_perform(&m_app_twi, type_read, 2, NULL);
}