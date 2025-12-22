
#include "nrf_log.h"
#include "nrf.h"
#include "app_error.h"
#include "watchdog.h"

#ifdef NRF52
#include "nrf_strerror.h"
#endif

#define DEAD_BEEF 0xDEADBEEF /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

void HardFault_Handler(){
  NRF_LOG_ERROR("HardFault\n");
#ifdef DEBUG
  for (;;);
#endif
  NVIC_SystemReset();
}

void
app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info) {
  unsigned int tmp = id;
  NRF_LOG_ERROR("app_error_print():\r\n");
  NRF_LOG_ERROR("Fault identifier:  0x%X\r\n", tmp);
  NRF_LOG_ERROR("Program counter:   0x%X\r\n", tmp = pc);
  NRF_LOG_ERROR("Fault information: 0x%X\r\n", tmp = info);

  switch (id) {
    case NRF_FAULT_ID_SDK_ASSERT:
      NRF_LOG_ERROR("Line Number: %u\r\n", tmp =
        ((assert_info_t *)(info))->line_num);
      NRF_LOG_ERROR("File Name:   %s\r\n",
        (uint32_t)((assert_info_t *)(info))->p_file_name);
      break;

    case NRF_FAULT_ID_SDK_ERROR:
      NRF_LOG_ERROR("Line Number: %u\r\n", tmp =
        ((error_info_t *)(info))->line_num);
      NRF_LOG_ERROR("File Name:   %s\r\n",
        (uint32_t)((error_info_t *)(info))->p_file_name);
      NRF_LOG_ERROR("Error Code:  0x%X\r\n", tmp =
        ((error_info_t *)(info))->err_code);

      #ifdef NRF51
      NRF_LOG_ERROR("Error description: %s\n", (uint32_t)ERR_TO_STR(((error_info_t *)(info))->err_code));
      #else
      NRF_LOG_ERROR("Error description: %s\n", (uint32_t)nrf_strerror_get(((error_info_t *)(info))->err_code));
      #endif
      break;
  }
#ifdef DEBUG
  for (;;) {
    watchdog_feed();
  };
#endif
}
