OUTPUT_DIRECTORY := _build

BLE_ROOT ?= ..

APPLICATION_HEX ?= $(OUTPUT_DIRECTORY)/$(TARGETS).hex
KEY_FILE ?= $(BLE_ROOT)/private.pem
PROJECT_ID ?= $(shell basename `pwd`)
OUT_ZIP ?= $(PROJECT_ID)_$(FAMILY).zip

SHELL := /bin/bash

PROJ_DIR := .
CUSTOM_INCLUDES_DIR = $(PROJ_DIR)/src/common
ADB_TARGET ?= Pixel-5
ADB_DIRECTORY ?= /sdcard/dfu

FIRMWARE_VERSION := \"0.8.5\"

CONFIG_SUBDIR = $(shell echo $(FAMILY) | tr A-Z a-z)

# Source files common to all targets
SRC_FILES_COMMON += \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/uart/retarget.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/pwm/app_pwm.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
  $(CUSTOM_INCLUDES_DIR)/services/battery_service/battery.c \
  $(CUSTOM_INCLUDES_DIR)/services/dfu_service/ble_dfu.c \
  $(PROJ_DIR)/src/ble/services/binary_sensor/ble_binary_sensor_service.c \
  $(PROJ_DIR)/src/ble/services/automation_io/ble_automation_io_service.c \
  $(PROJ_DIR)/src/ble/services/configuration/ble_configuration_service.c \
  $(PROJ_DIR)/src/ble/services/ble_gpio_asm/ble_gpio_asm.c \
  $(PROJ_DIR)/src/ble/services/cycling_speed_cadence/ble_cycling_speed_cadence.c \
  $(PROJ_DIR)/src/ble/services/hid/ble_hid.c \
  $(PROJ_DIR)/src/ble/services/temperature/ble_temperature_service.c \
  $(PROJ_DIR)/src/ble/helpers/ble_helpers.c \
  $(PROJ_DIR)/src/ble/sensor_ble.c \
  $(PROJ_DIR)/src/gpio/sensor_gpio.c \
  $(PROJ_DIR)/src/helpers/encoding.c \
  $(PROJ_DIR)/src/timer/sensor_timer.c \
  $(PROJ_DIR)/src/error_handler/error_handler.c \
  $(PROJ_DIR)/src/gpioasm/gpioasm.c \
  $(PROJ_DIR)/src/persistence/pin_configuration.c \
  $(PROJ_DIR)/src/sleep/sleep.c \
  $(PROJ_DIR)/src/watchdog/watchdog.c \
  $(PROJ_DIR)/src/dfu/nrf_dfu_flash.c \
  $(PROJ_DIR)/src/main.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/libraries/fifo \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/drivers_nrf/twi_master \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(PROJ_DIR)/src/ble/services/automation_io/ \
  $(PROJ_DIR)/src/ble/services/binary_sensor/ \
  $(PROJ_DIR)/src/ble/services/configuration/ \
  $(PROJ_DIR)/src/ble/services/ble_gpio_asm/ \
  $(PROJ_DIR)/src/ble/services/cycling_speed_cadence/ \
  $(PROJ_DIR)/src/ble/services/hid/ \
  $(PROJ_DIR)/src/ble/services/temperature/ \
  $(PROJ_DIR)/src/ble/helpers/ \
  $(PROJ_DIR)/src/ble/ \
  $(PROJ_DIR)/src/helpers/ \
  $(PROJ_DIR)/src/config/$(CONFIG_SUBDIR)/ \
  $(PROJ_DIR)/src/config/ \
  $(PROJ_DIR)/src/gpio/ \
  $(PROJ_DIR)/src/storage/ \
  $(PROJ_DIR)/src/gpioasm/ \
  $(PROJ_DIR)/src/timer/ \
  $(PROJ_DIR)/src/persistence/ \
  $(PROJ_DIR)/src/watchdog/ \
  $(PROJ_DIR)/src/sleep/ \
  $(CUSTOM_INCLUDES_DIR)/services/battery_service \
  $(CUSTOM_INCLUDES_DIR)/boards \
  $(CUSTOM_INCLUDES_DIR)/services/dfu_service \

ifeq ($(CHIP), NRF51822)
FAMILY = NRF51
TARGETS = nrf51822_xxac
$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := src/linker/nrf51822_qfac.ld

SRC_FILES = \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SRC_FILES_COMMON) \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer_appsh.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/fstorage/fstorage.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_mutex.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master/nrf_drv_spi.c \
  $(SDK_ROOT)/components/drivers_nrf/ppi/nrf_drv_ppi.c \
  $(SDK_ROOT)/components/drivers_nrf/timer/nrf_drv_timer.c \
  $(SDK_ROOT)/components/drivers_nrf/wdt/nrf_drv_wdt.c \
  $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_nvmc.c \
  $(PROJ_DIR)/src/storage/storage.nrf51.c \

INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s130/headers \
  $(SDK_ROOT)/components/softdevice/s130/headers/nrf51 \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/i2s \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/drivers_nrf/wdt \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/drivers_nrf/twis_slave \
  $(SDK_ROOT)/components/drivers_nrf/pwm \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/drivers_nrf/rng \
  $(SDK_ROOT)/components/drivers_nrf/spi_slave \
  $(SDK_ROOT)/components/drivers_nrf/power \
  $(SDK_ROOT)/components/drivers_nrf/qdec \
  $(SDK_ROOT)/components/drivers_nrf/pdm \
  $(SDK_ROOT)/components/drivers_nrf/swi \
  $(SDK_ROOT)/components/drivers_nrf/lpcomp \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/drivers_nrf/usbd \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/saadc \
  $(SDK_ROOT)/components/drivers_nrf/comp \
  $(SDK_ROOT)/components/drivers_nrf/adc \
  $(SDK_ROOT)/components/libraries/twi \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/device \

BOARD ?= BLE400
SDK_ROOT ?= $(BLE_ROOT)/nRF5_SDK_12.3.0_d7731ad
SOFTDEVICE_HEX = $(SDK_ROOT)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex

CFLAGS += -DNRF51
CFLAGS += -DS130
CFLAGS += -DNRF51822
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -DFDS_VIRTUAL_PAGE_SIZE=256
CFLAGS += -DAPP_TIMER_TICKS_COMPAT\(time,prescaler\)=APP_TIMER_TICKS\(time,prescaler\)
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mfloat-abi=soft
CFLAGS += -DHARDWARE_PIN_COUNT=32

ASMFLAGS += -DNRF51
ASMFLAGS += -DS130
ASMFLAGS += -DNRF51822
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2

LDFLAGS += -mcpu=cortex-m0
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)

else ifeq ($(CHIP), NRF52832)
FAMILY = NRF52
TARGETS = nrf52832_xxac
$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := src/linker/nrf52832_qfaa.ld

SRC_FILES = \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls/mbedtls_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c \
  $(SDK_ROOT)/external/mbedtls/library/ecp.c \
  $(SDK_ROOT)/external/mbedtls/library/bignum.c \
  $(SDK_ROOT)/external/mbedtls/library/ecp_curves.c \
  $(SDK_ROOT)/external/mbedtls/library/platform.c \


INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52

SOFTDEVICE_HEX = $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex

CFLAGS += -DS132
CFLAGS += -DNRF52
CFLAGS += -DHARDWARE_PIN_COUNT=32
CFLAGS += -DNRF_CRYPTO_ALLOCATOR=3
CFLAGS += -DNRF_CRYPTO_BACKEND_MBEDTLS_ENABLED=1

ASMFLAGS += -DS132
ASMFLAGS += -DNRF52
else ifeq ($(CHIP), NRF52840)
FAMILY = NRF52
TARGETS = nrf52840_xxaa
$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := src/linker/nrf52840_qfaa.ld

SRC_FILES = \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_init.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_mutex.c \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_shared.c \

INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s140/headers \
  $(SDK_ROOT)/components/softdevice/s140/headers/nrf52

SOFTDEVICE_HEX = $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_6.1.1_softdevice.hex

CFLAGS += -DS140
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DHARDWARE_PIN_COUNT=64
CFLAGS += -DNRF_CRYPTO_BACKEND_CC310_ENABLED=1

ASMFLAGS += -DS140
ASMFLAGS += -DNRF52840_XXAA
else
$(error please specify CHIP=NRF51822 / NRF52832 / NRF52840)
endif

ifeq ($(FAMILY), NRF52)
SRC_FILES += \
  $(SRC_FILES_COMMON) \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_ppi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_timer.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_wdt.c \
  $(PROJ_DIR)/src/storage/storage.nrf52.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_nvmc.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_rng.c \
  $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c \
  $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/peer_manager/nrf_ble_lesc.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_ppi.c \
  $(SDK_ROOT)/modules/nrfx/hal/nrf_nvmc.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \


INC_FOLDERS += \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/modules/nrfx/hal \
  $(SDK_ROOT)/components/libraries/strerror \
  $(SDK_ROOT)/components/softdevice/common \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/memobj \
  $(SDK_ROOT)/components/libraries/balloc \
  $(SDK_ROOT)/components/libraries/svc \
  $(SDK_ROOT)/components/libraries/atomic \
  $(SDK_ROOT)/components/libraries/atomic_fifo \
  $(SDK_ROOT)/components/libraries/atomic_flags \
  $(SDK_ROOT)/components/libraries/delay \
  $(SDK_ROOT)/components/libraries/mutex \
  $(SDK_ROOT)/components/libraries/ringbuf \
  $(SDK_ROOT)/components/libraries/stack_info \
  $(SDK_ROOT)/components/libraries/mem_manager \
  $(SDK_ROOT)/components/libraries/crypto \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310 \
  $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl \
  $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls \
  $(SDK_ROOT)/components/libraries/crypto/backend/oberon \
  $(SDK_ROOT)/components/libraries/crypto/backend/optiga \
  $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw \
  $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw \
  $(SDK_ROOT)/components/libraries/crypto/backend/cifra \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt \
  $(SDK_ROOT)/modules/nrfx \
  $(SDK_ROOT)/modules/nrfx/drivers/include \
  $(SDK_ROOT)/integration/nrfx \
  $(SDK_ROOT)/integration/nrfx/legacy \
  $(SDK_ROOT)/external/fprintf \
  $(SDK_ROOT)/external/nrf_cc310/include \
  $(SDK_ROOT)/external/mbedtls/include \

LIB_FILES += \
  $(SDK_ROOT)/external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.12.a \
  $(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_2.0.7.a \
  $(SDK_ROOT)/components/nfc/t2t_lib/nfc_t2t_lib_gcc.a \

BOARD ?= HOLYIOT_17095

CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -DAPP_TIMER_TICKS_COMPAT\(time,prescaler\)=APP_TIMER_TICKS\(time\)
CFLAGS += -DNRF_DFU_SETTINGS_COMPATIBILITY_MODE=1
CFLAGS += -DCONFIG_NFCT_PINS_AS_GPIOS=1
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

CFLAGS += -DDEBUG

ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
LDFLAGS += -mthumb -mabi=aapcs -L $(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)

SDK_ROOT ?= $(BLE_ROOT)/nRF5_SDK_15.3.0_59ac345
endif

# Libraries common to all targets
LIB_FILES += \

# C flags common to all targets
CFLAGS += -DBOARD_$(BOARD)
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror -O3 -g3
# keep every function in separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums 
CFLAGS += -DNRF_DFU_SETTINGS_VERSION=1
CFLAGS += -DUSE_DFU
#CFLAGS += -DUSE_SPI
#CFLAGS += -DUSE_UART
ifeq ($(BOARD), BLE400)
CFLAGS += -DDEBUG
endif
CFLAGS += -DBUTTON_PIN=BUTTON_0
CFLAGS += -DFIRMWARE_VERSION=$(FIRMWARE_VERSION)

# C++ flags common to all targets
CXXFLAGS += \

# Assembler flags common to all targets
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DBOARD_$(BOARD)
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DSWI_DISABLE0

# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys


.PHONY: $(TARGETS) default all clean help flash flash_softdevice erase merge_softdevice applicaiton_zip push sign reset config feature_config

# Default target - first one defined
default: $(TARGETS)

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo 	$(TARGETS)

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc

include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

# Flash the program
flash: $(APPLICATION_HEX)
	@echo Flashing: $<
	nrfjprog --program $(APPLICATION_HEX) --verify --sectorerase -r
	# scp $(APPLICATION_HEX) home:ram
	# echo -e "program /home/home/ram/$(TARGETS).hex verify reset \n exit" | nc home 4444

# Flash softdevice
flash_softdevice:
	@echo Flashing: $(SOFTDEVICE_HEX)
	nrfjprog --program $(SOFTDEVICE_HEX) -f $(FAMILY) --verify --sectorerase
	nrfjprog --reset -f $(FAMILY)

erase:
	nrfjprog --eraseall -f $(FAMILY)

merge_softdevice: $(APPLICATION_HEX) $(SOFTDEVICE_HEX)
	mergehex -m \
		$(APPLICATION_HEX) \
		$(SOFTDEVICE_HEX) \
	-o application_with_softdevice.hex

$(OUT_ZIP): $(APPLICATION_HEX)
	rm -f $(OUT_ZIP)
	ls -lh $(APPLICATION_HEX)
	nrfutil pkg generate --application $(APPLICATION_HEX) --debug-mode $(OUT_ZIP) --key-file $(KEY_FILE)

sign: $(OUT_ZIP)

push: $(OUT_ZIP)
	adb connect $(ADB_TARGET)
	adb shell mkdir -p $(ADB_DIRECTORY)
	adb push $(OUT_ZIP) $(ADB_DIRECTORY)

config: src/config/sdk_config.h
	java -jar $(BLE_ROOT)/CMSIS_Configuration_Wizard.jar src/config/sdk_config.h

feature_config: src/config/feature_config.h
	java -jar $(BLE_ROOT)/CMSIS_Configuration_Wizard.jar src/config/feature_config.template.h
	
reset:
	nrfjprog --reset

bin: 
	rm -f without_crtystal.bin with_crtystal.bin nrf51822_xxac.bin
	make clean default sign BOARD=BOARD_BEACON_BIG
	unzip $(PROJECT_ID).zip nrf51822_xxac.bin
	mv nrf51822_xxac.bin "$(BIN_OUTPUT_FOLDER)$(BIN_OUTPUT_WITHOUT_CRYSTAL)"
	make clean default sign  BOARD=BOARD_BEACON_SMALL
	unzip $(PROJECT_ID).zip nrf51822_xxac.bin
	mv nrf51822_xxac.bin "$(BIN_OUTPUT_FOLDER)$(BIN_OUTPUT_WITH_CRYSTAL)"
	rm $(PROJECT_ID).zip
	make clean

rtt_viewer_start:
	sed -i -r 's/(Frame[XY]) = .*/\1 = 0/' ~/.config/SEGGERJLinkRTTViewerSettings.ini
	nohup JLinkRTTViewer --autoconnect &
	sleep 1

rtt_viewer_stop:
	killall JLinkRTTViewer || true
	sleep 0.5

python_script_run:
	test/venv/bin/python test/send.py

dongle.zip: $(APPLICATION_HEX)
	nrfutil pkg generate \
	--application $(APPLICATION_HEX) \
	--debug-mode \
	--hw-version 52 \
	--sd-id 0x00 \
	--sd-req 0x00 \
	--softdevice $(SOFTDEVICE_HEX) \
	dongle.zip

serial: dongle.zip
	nrfutil dfu usb-serial -pkg dongle.zip --port /dev/tty.usbmodemCDEEB788250C1
