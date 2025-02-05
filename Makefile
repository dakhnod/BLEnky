OUTPUT_DIRECTORY := _build

BLE_ROOT ?= ../..
SDK_ROOT ?= $(BLE_ROOT)/nRF5_SDK_12.3.0_d7731ad

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

# Source files common to all targets
SRC_FILES += \
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
  $(SDK_ROOT)/components/ble/peer_manager/pm_mutex.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer_appsh.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/fstorage/fstorage.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/uart/retarget.c \
  $(SDK_ROOT)/components/libraries/util/sdk_errors.c \
  $(SDK_ROOT)/components/libraries/twi/app_twi.c \
  $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c \
  $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c \
  $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master/nrf_drv_spi.c \
  $(SDK_ROOT)/components/drivers_nrf/twi_master/nrf_drv_twi.c \
  $(SDK_ROOT)/external/segger_rtt/RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_flash.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_settings.c \
  $(SDK_ROOT)/components/drivers_nrf/hal/nrf_nvmc.c \
  $(SDK_ROOT)/components/libraries/crc32/crc32.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/pwm/app_pwm.c \
  $(SDK_ROOT)/components/drivers_nrf/ppi/nrf_drv_ppi.c \
  $(SDK_ROOT)/components/drivers_nrf/timer/nrf_drv_timer.c \
  $(SDK_ROOT)/components/drivers_nrf/wdt/nrf_drv_wdt.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c \
  $(CUSTOM_INCLUDES_DIR)/services/battery_service/battery.c \
  $(CUSTOM_INCLUDES_DIR)/services/dfu_service/ble_dfu.c \
  $(CUSTOM_INCLUDES_DIR)/libraries/gpiote/nrf_drv_gpiote.c \
  $(PROJ_DIR)/src/ble/services/binary_sensor/ble_binary_sensor_service.c \
  $(PROJ_DIR)/src/ble/services/automation_io/ble_automation_io_service.c \
  $(PROJ_DIR)/src/ble/services/configuration/ble_configuration_service.c \
  $(PROJ_DIR)/src/ble/services/ble_gpio_asm/ble_gpio_asm.c \
  $(PROJ_DIR)/src/ble/services/cycling_speed_cadence/ble_cycling_speed_cadence.c \
  $(PROJ_DIR)/src/ble/services/hid/ble_hid.c \
  $(PROJ_DIR)/src/ble/services/temperature/ble_temperature_service.c \
  $(PROJ_DIR)/src/ble/services/i2c/i2c_service.c \
  $(PROJ_DIR)/src/ble/helpers/ble_helpers.c \
  $(PROJ_DIR)/src/ble/sensor_ble.c \
  $(PROJ_DIR)/src/gpio/sensor_gpio.c \
  $(PROJ_DIR)/src/helpers/encoding.c \
  $(PROJ_DIR)/src/timer/sensor_timer.c \
  $(PROJ_DIR)/src/error_handler/error_handler.c \
  $(PROJ_DIR)/src/gpioasm/gpioasm.c \
  $(PROJ_DIR)/src/storage/storage.c \
  $(PROJ_DIR)/src/persistence/pin_configuration.c \
  $(PROJ_DIR)/src/sleep/sleep.c \
  $(PROJ_DIR)/src/watchdog/watchdog.c \
  $(PROJ_DIR)/src/twi/bma400.c \
  $(PROJ_DIR)/src/main.c \

# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/components/drivers_nrf/comp \
  $(SDK_ROOT)/components/drivers_nrf/twi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias_c \
  $(SDK_ROOT)/components/libraries/pwm \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/generic \
  $(SDK_ROOT)/components/libraries/usbd/class/msc \
  $(SDK_ROOT)/components/libraries/usbd/class/hid \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/libraries/log \
  $(SDK_ROOT)/components/ble/ble_services/ble_gls \
  $(SDK_ROOT)/components/libraries/fstorage \
  $(SDK_ROOT)/components/drivers_nrf/i2s \
  $(SDK_ROOT)/components/libraries/gpiote \
  $(SDK_ROOT)/components/drivers_nrf/gpiote \
  $(SDK_ROOT)/components/libraries/fifo \
  $(SDK_ROOT)/components/drivers_nrf/common \
  $(SDK_ROOT)/components/ble/ble_advertising \
  $(SDK_ROOT)/components/drivers_nrf/adc \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas_c \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c \
  $(SDK_ROOT)/components/libraries/queue \
  $(SDK_ROOT)/components/ble/ble_dtm \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c \
  $(SDK_ROOT)/components/drivers_nrf/uart \
  $(SDK_ROOT)/components/ble/common \
  $(SDK_ROOT)/components/ble/ble_services/ble_lls \
  $(SDK_ROOT)/components/drivers_nrf/wdt \
  $(SDK_ROOT)/components/libraries/bsp \
  $(SDK_ROOT)/components/ble/ble_services/ble_bas \
  $(SDK_ROOT)/components/libraries/experimental_section_vars \
  $(SDK_ROOT)/components/ble/ble_services/ble_ans_c \
  $(SDK_ROOT)/components/libraries/slip \
  $(SDK_ROOT)/external/segger_rtt \
  $(SDK_ROOT)/components/libraries/csense_drv \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/drivers_nrf/rtc \
  $(SDK_ROOT)/components/ble/ble_services/ble_ias \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse \
  $(SDK_ROOT)/components/drivers_nrf/ppi \
  $(SDK_ROOT)/components/drivers_nrf/twis_slave \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components/libraries/scheduler \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs \
  $(SDK_ROOT)/components/ble/ble_services/ble_hts \
  $(SDK_ROOT)/components/drivers_nrf/delay \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/libraries/crc16 \
  $(SDK_ROOT)/components/drivers_nrf/timer \
  $(SDK_ROOT)/components/libraries/util \
  $(SDK_ROOT)/components/drivers_nrf/pwm \
  $(SDK_ROOT)/components/libraries/usbd/class/cdc \
  $(SDK_ROOT)/components/libraries/csense \
  $(SDK_ROOT)/components/drivers_nrf/rng \
  $(SDK_ROOT)/components/libraries/low_power_pwm \
  $(SDK_ROOT)/components/libraries/hardfault \
  $(SDK_ROOT)/components/ble/ble_services/ble_cscs \
  $(SDK_ROOT)/components/libraries/uart \
  $(SDK_ROOT)/components/libraries/hci \
  $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd \
  $(SDK_ROOT)/components/drivers_nrf/spi_slave \
  $(SDK_ROOT)/components/drivers_nrf/lpcomp \
  $(SDK_ROOT)/components/libraries/timer \
  $(SDK_ROOT)/components/drivers_nrf/power \
  $(SDK_ROOT)/components/libraries/usbd/config \
  $(SDK_ROOT)/components/toolchain \
  $(SDK_ROOT)/components/libraries/led_softblink \
  $(SDK_ROOT)/components/drivers_nrf/qdec \
  $(SDK_ROOT)/components/ble/ble_services/ble_cts_c \
  $(SDK_ROOT)/components/drivers_nrf/spi_master \
  $(SDK_ROOT)/components/ble/ble_services/ble_hids \
  $(SDK_ROOT)/components/drivers_nrf/pdm \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(SDK_ROOT)/components/libraries/usbd/class/audio \
  $(SDK_ROOT)/components/ble/peer_manager \
  $(SDK_ROOT)/components/drivers_nrf/swi \
  $(SDK_ROOT)/components/ble/ble_services/ble_tps \
  $(SDK_ROOT)/components/ble/ble_services/ble_dis \
  $(SDK_ROOT)/components/device \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr \
  $(SDK_ROOT)/components/libraries/button \
  $(SDK_ROOT)/components/libraries/usbd \
  $(SDK_ROOT)/components/drivers_nrf/saadc \
  $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c \
  $(SDK_ROOT)/components/ble/ble_racp \
  $(SDK_ROOT)/components/toolchain/gcc \
  $(SDK_ROOT)/components/libraries/fds \
  $(SDK_ROOT)/components/libraries/twi \
  $(SDK_ROOT)/components/drivers_nrf/clock \
  $(SDK_ROOT)/components/ble/ble_services/ble_rscs \
  $(SDK_ROOT)/components/drivers_nrf/usbd \
  $(SDK_ROOT)/components/softdevice/common/softdevice_handler \
  $(SDK_ROOT)/components/ble/ble_services/ble_hrs \
  $(SDK_ROOT)/components/libraries/log/src \
  $(SDK_ROOT)/components/libraries/bootloader/ble_dfu \
  $(SDK_ROOT)/components/libraries/bootloader/dfu \
  $(SDK_ROOT)/components/libraries/bootloader \
  $(SDK_ROOT)/components/drivers_nrf/hal \
  $(SDK_ROOT)/components/libraries/crc32 \
  $(PROJ_DIR)/src/ble/services/automation_io/ \
  $(PROJ_DIR)/src/ble/services/binary_sensor/ \
  $(PROJ_DIR)/src/ble/services/configuration/ \
  $(PROJ_DIR)/src/ble/services/ble_gpio_asm/ \
  $(PROJ_DIR)/src/ble/services/cycling_speed_cadence/ \
  $(PROJ_DIR)/src/ble/services/hid/ \
  $(PROJ_DIR)/src/ble/services/temperature/ \
  $(PROJ_DIR)/src/ble/services/i2c/ \
  $(PROJ_DIR)/src/ble/helpers/ \
  $(PROJ_DIR)/src/ble/ \
  $(PROJ_DIR)/src/helpers/ \
  $(PROJ_DIR)/src/config/ \
  $(PROJ_DIR)/src/gpio/ \
  $(PROJ_DIR)/src/storage/ \
  $(PROJ_DIR)/src/gpioasm/ \
  $(PROJ_DIR)/src/timer/ \
  $(PROJ_DIR)/src/persistence/ \
  $(PROJ_DIR)/src/watchdog/ \
  $(PROJ_DIR)/src/sleep/ \
  $(PROJ_DIR)/src/twi/ \
  $(CUSTOM_INCLUDES_DIR)/services/battery_service \
  $(CUSTOM_INCLUDES_DIR)/boards \
  $(CUSTOM_INCLUDES_DIR)/services/dfu_service \

ifeq ($(FAMILY), NRF51)
TARGETS = nrf51822_xxac
$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := src/linker/nrf51822_qfac.ld

SRC_FILES += \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.S \
  $(SDK_ROOT)/components/toolchain/system_nrf51.c

INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s130/headers \
  $(SDK_ROOT)/components/softdevice/s130/headers/nrf51 \

BOARD ?= BLE400
SOFTDEVICE_HEX := $(SDK_ROOT)/components/softdevice/s130/hex/s130_nrf51_2.0.1_softdevice.hex

CFLAGS += -DNRF51
CFLAGS += -DS130
CFLAGS += -DNRF51822
CFLAGS += -DNRF_SD_BLE_API_VERSION=2
CFLAGS += -DFDS_VIRTUAL_PAGE_SIZE=256
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mfloat-abi=soft

ASMFLAGS += -DNRF51
ASMFLAGS += -DS130
ASMFLAGS += -DNRF51822
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=2

LDFLAGS += -mcpu=cortex-m0
else ifeq ($(FAMILY), NRF52)
TARGETS = nrf52832_xxac
$(OUTPUT_DIRECTORY)/$(TARGETS).out: \
  LINKER_SCRIPT  := src/linker/nrf52832_qfaa.ld

SRC_FILES += \
  $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S \
  $(SDK_ROOT)/components/toolchain/system_nrf52.c

INC_FOLDERS += \
  $(SDK_ROOT)/components/softdevice/s132/headers \
  $(SDK_ROOT)/components/softdevice/s132/headers/nrf52 \

BOARD ?= HOLYIOT_17095
SOFTDEVICE_HEX := $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_3.0.0_softdevice.hex

CFLAGS += -DNRF52
CFLAGS += -DS132
CFLAGS += -DNRF52832
CFLAGS += -DNRF_SD_BLE_API_VERSION=3
CFLAGS += -DFDS_VIRTUAL_PAGE_SIZE=1024
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16

ASMFLAGS += -DNRF52
ASMFLAGS += -DS132
ASMFLAGS += -DNRF52832
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=3

LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
else
$(error please specify FAMILY=NRF51 / NRF52)
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

# Linker flags
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
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
	scp $(APPLICATION_HEX) home:ram
	echo -e "program /home/home/ram/$(TARGETS).hex verify reset \n exit" | nc home 4444

# Flash softdevice
flash_softdevice: $(SOFTDEVICE_HEX)
	@echo Flashing: $(SOFTDEVICE_HEX)
	nrfjprog --program $(SOFTDEVICE_HEX) -f $(FAMILY) --verify
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
