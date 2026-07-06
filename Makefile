CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

MCU = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft

BUILD_DIR = build

SRCS = \
startup/startup.c \
applications/blink/main.c

LDSCRIPT = linker/linker.ld		


CFLAGS = $(MCU) -nostdlib -O0 -Wall -Wextra -g3
LDFLAGS = $(MCU) -nostdlib -T $(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/AuroraRT.map

TARGET = $(BUILD_DIR)/AuroraRT
OBJS = $(addprefix $(BUILD_DIR)/, $(notdir $(SRCS:.c=.o)))

all: $(BUILD_DIR) $(TARGET).elf $(TARGET).bin

$(BUILD_DIR):
	mkdir -p $@

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	$(SIZE) $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

VPATH = startup:applications/blink
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)