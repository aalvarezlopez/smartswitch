CC = /opt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc
LD = /opt/gcc-arm-none-eabi/bin/arm-none-eabi-ld

UNIT_TEST_REPORT := ./unit_test_report.txt

TESTSRC := ./APP/utest_smartswitch.c
TESTOBJ := $(TESTSRC:%.c=%.o)

SRCS := ./main.c
SRCS += ./ECUM/EcuM.c ./BSW/SPI/spi.c ./BSW/uart/uart.c ./BSW/io/io.c ./BSW/I2C/i2c.c
SRCS += ./BSW/errorlog/errorlog.c
SRCS += ./BSW/isr/isr.c
SRCS += ./BSW/rtc/rtc.c
SRCS += ./BSW/Eth/EtherCard.c
SRCS += ./BSW/Eth/tcpip.c
SRCS += ./BSW/Eth/arp.c
SRCS += ./Drivers/DS18B20/ds18b20.c ./Drivers/DS18B20/ds18b20_ll.c
SRCS += ./Drivers/Display/display.c
SRCS += ./Drivers/Enc/enc28j60.c
SRCS += $(wildcard ./cmsis/*.c)
SRCS += ./APP/smartswitch.c
SRCS += ./misc/str.c
SRCS += ./misc/delays.c
SRCS += ./misc/graphics.c
OBJECTS := $(SRCS:%.c=%.o)
INCLUDES := -I./cmsis/
INCLUDES += -I./cmsis/component/
INCLUDES += -I./cmsis/instance/
INCLUDES += -I./ECUM/
INCLUDES += -I./BSW/SPI/
INCLUDES += -I./BSW/I2C/
INCLUDES += -I./BSW/io/
INCLUDES += -I./BSW/uart
INCLUDES += -I./BSW/isr
INCLUDES += -I./BSW/errorlog
INCLUDES += -I./BSW/rtc
INCLUDES += -I./BSW/flash
INCLUDES += -I./BSW/nvm
INCLUDES += -I./BSW/Eth
INCLUDES += -I./Drivers/USB_CDC
INCLUDES += -I./Drivers/DS18B20
INCLUDES += -I./Drivers/Display
INCLUDES += -I./Drivers/Enc
INCLUDES += -I./APP/
INCLUDES += -I./misc/

LD_SCRIPT = ./flash.ld
CFLAGS = -ggdb -mthumb -mcpu=cortex-m4 -D__SAM4S4A__
CFLAGS += -O0 -MD -std=c99 -c -fno-builtin
CFLAGS += $(INCLUDES)
LDFLAGS = -T $(LD_SCRIPT) -e Reset_Handler

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
