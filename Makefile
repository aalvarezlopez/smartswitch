CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld

UNIT_TEST_REPORT := ./unit_test_report.txt

TESTSRC := ./APP/utest_fluid_ctrl.c
TESTOBJ := $(TESTSRC:%.c=%.o)

SRCS := ./main.c
SRCS += ./ECUM/EcuM.c ./BSW/SPI/spi.c ./BSW/uart/uart.c ./BSW/io/io.c
SRCS += ./BSW/errorlog/errorlog.c
SRCS += ./BSW/isr/isr.c
SRCS += ./BSW/rtc/rtc.c
SRCS += ./BSW/flash/efc.c
SRCS += ./BSW/flash/flash_efc.c
SRCS += ./BSW/nvm/nvm.c
SRCS += ./Drivers/USB_CDC/udi_cdc.c ./Drivers/USB_CDC/udi_cdc_desc.c
SRCS += ./Drivers/USB_CDC/udc.c ./Drivers/USB_CDC/udp_device.c
SRCS += ./Drivers/DS18B20/ds18b20.c ./Drivers/DS18B20/ds18b20_ll.c
SRCS += $(wildcard ./cmsis/*.c)
SRCS += ./APP/fluid_ctrl.c
SRCS += ./misc/str.c
SRCS += ./misc/delays.c
OBJECTS := $(SRCS:%.c=%.o)
INCLUDES := -I./cmsis/
INCLUDES += -I./cmsis/component/
INCLUDES += -I./cmsis/instance/
INCLUDES += -I./ECUM/
INCLUDES += -I./BSW/SPI/
INCLUDES += -I./BSW/io/
INCLUDES += -I./BSW/uart
INCLUDES += -I./BSW/isr
INCLUDES += -I./BSW/errorlog
INCLUDES += -I./BSW/rtc
INCLUDES += -I./BSW/flash
INCLUDES += -I./BSW/nvm
INCLUDES += -I./Drivers/USB_CDC
INCLUDES += -I./Drivers/DS18B20
INCLUDES += -I./APP/
INCLUDES += -I./misc/

LD_SCRIPT = ./flash.ld
CFLAGS = -ggdb -mthumb -mcpu=cortex-m4 -D__SAM4S4A__
CFLAGS += -Os -MD -std=c99 -c
CFLAGS += $(INCLUDES)
LDFLAGS = -T $(LD_SCRIPT) -e Reset_Handler --print-memory-usage

.PHONY: all clean test style

all: $(OBJECTS)
	@echo "Building"
	$(LD) -o ./smartSwitch.elf $(OBJECTS) $(LDFLAGS)

-include $(OBJECTS:.o=.d)

clean:
	@find ./ -name "*.o" -delete
	@rm -f ./smartSwitch.elf

%.o: %.c
	@echo "Compiling $@"
	$(CC) $(CFLAGS)  $< -o $@

test:
	@echo "Unit test execution date " > $(UNIT_TEST_REPORT)
	@echo $(shell date) >> $(UNIT_TEST_REPORT)
	@find ./ -name "*.o" -delete
	@echo '' >> $(UNIT_TEST_REPORT)
	@make -C ./APP/ report_file=$(shell realpath $(UNIT_TEST_REPORT))

style:
	@find ./ -name "*.c" | xargs -I {} -n 1 astyle {} --project
	@find ./ -name "*.h" | xargs -I {} -n 1 astyle {} --project

testall:
	@rm -rf ./cov_report/*
	@mkdir -p ./cov_report
	@echo "Unit test execution date " > $(UNIT_TEST_REPORT)
	@echo $(shell date) >> $(UNIT_TEST_REPORT)
	@find ./ -name "utest*.c" | xargs -t -n 1 -I {} bash -c 'make -C $$(dirname {}) report_file=$$(realpath $(UNIT_TEST_REPORT))'
	@echo "--------" >> $(UNIT_TEST_REPORT)
	@find ./ -name "*.o" -delete

testreport:
	@find ./ -name "*.gcda" | xargs -n 1 -I {} cp {} ./cov_report/
	@find ./ -name "*.gcno" | xargs -n 1 -I {} cp {} ./cov_report/
	@lcov --capture --directory ./cov_report --output-file coverage.info
	@genhtml coverage.info --output-directory cov_report
