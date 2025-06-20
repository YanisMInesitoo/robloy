# Robloy - Makefile para Game Boy Advance

TARGET := 0000robloy_1.0.1.2
BUILD := build
SRC := src
OBJ := $(BUILD)/main.o

# Ruta a devkitARM
DEVKITARM ?= $(DEVKITARM)

CC := $(DEVKITARM)/bin/arm-none-eabi-gcc
CFLAGS := -mthumb-interwork -mthumb -O2 -Wall -Wno-unused-function -ffast-math -mcpu=arm7tdmi -mtune=arm7tdmi -I$(DEVKITPRO)/libgba/include
LDFLAGS := -specs=gba.specs -L$(DEVKITPRO)/libgba/lib -lgba -lm

all: $(BUILD) $(TARGET).gba

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TARGET).gba: $(TARGET).elf
	@$(DEVKITPRO)/tools/bin/gbafix $< -tROBLOY -cRL01 -mYM -r00
	@cp $< $@

clean:
	rm -rf $(BUILD) *.elf *.gba
