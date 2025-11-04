# Rishav (2022/05/15)

## Start Edit ##

TARGET = camera
BUILD_DIR = build
DEBUG = 0

# Cflags 
CPU = -mcpu=cortex-m3
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)
OPT = -Og

# C sources
C_SOURCES =  \
core/startup/crt.c \
core/startup/vectors_stm32f10x_md.c \
core/cmsis/core_support/core_cm3.c \
core/cmsis/device_support/system_stm32f10x.c \
core/std_periph_lib/src/stm32f10x_usart.c \
core/std_periph_lib/src/stm32f10x_flash.c \
core/std_periph_lib/src/stm32f10x_gpio.c \
core/std_periph_lib/src/stm32f10x_rcc.c \
core/std_periph_lib/src/stm32f10x_spi.c \
core/std_periph_lib/src/stm32f10x_i2c.c \
core/std_periph_lib/src/stm32f10x_rtc.c \
core/std_periph_lib/src/stm32f10x_bkp.c \
core/std_periph_lib/src/stm32f10x_pwr.c \
core/std_periph_lib/src/stm32f10x_dma.c \
core/std_periph_lib/src/stm32f10x_iwdg.c \
core/std_periph_lib/src/stm32f10x_tim.c \
core/std_periph_lib/src/stm32f10x_exti.c \
core/std_periph_lib/src/misc.c \
src/camera/ArduCAM.c \
src/camera/delay.c \
src/camera/sccb_bus.c \
src/camera/spi.c \
src/camera/sys.c \
src/camera/usart.c \
src/ssdv/rs8.c \
src/ssdv/ssdv.c \
src/flash/flash.c \
src/payload/payload.c \
src/payload/payload_config.c \
src/watchdog/watchdog.c \
src/i2c/i2c.c \
src/i2c/stm32f10x_it.c \
main.c

# ASM sources
ASM_SOURCES =  \

# Macros for gcc
AS_DEFS = 
C_DEFS = -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER

# Includes
AS_INCLUDES = 

C_INCLUDES =  \
-Icore/std_periph_lib/inc \
-Icore/std_periph_lib \
-Icore/cmsis/device_support \
-Icore/cmsis/core_support \
-Isrc/camera \
-Isrc/flash \
-Isrc/ssdv \
-Isrc/payload \
-Isrc/watchdog \
-Isrc/i2c

# Link script
LDSCRIPT = core/linker/stm32f103xb_flash.ld

# Libraries
LIBS = -lc -lm -lnosys
LIBDIR = 

## Stop Edit ##

# Compilers
GCC_PATH =
PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" # Generate dependency information

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Link flags
LDFLAGS = $(MCU) -nostartfiles -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections -u _printf_float,--no-warn-mismatch

# Default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


# Build the application #
# List of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o))) 
vpath %.c $(sort $(dir $(C_SOURCES)))

# List of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@ #

# Flash the binary
flash: all
	st-flash write $(BUILD_DIR)/$(TARGET).bin 0x8000000

# Delete build directory
clean:
	rm -r -d $(BUILD_DIR)

# Dependencies
-include $(wildcard $(BUILD_DIR)/*.d)
