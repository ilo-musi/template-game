GAME_NAME = Template
ROM_PACKER = ~/iloMusiRomPacker.py
IMAGE_CONVERTER = ~/iloMusiImageConverter.py
INCLUDE_DIR = ./include
SRC_DIR = ./src
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as

API_MAPPER = ./misc/externFunctionAddress.sym
OUTPUT_DIR = ./bin
OUTPUT_ELF = $(GAME_NAME).elf
OUTPUT_BIN = $(GAME_NAME).bin
OUTPUT_ILM = $(GAME_NAME).ilm
LINKER_SCRIPT = ./misc/linkerScript.ld
ASM_SCRIPTS = ./external/startup_stm32f030x6.s
ASM_OBJS = $(OUTPUT_DIR)/startup_stm32f030x6.o
DEPS = $(INCLUDE_DIR)/*
OBJS = $(OUTPUT_DIR)/main.o $(OUTPUT_DIR)/gameObject.o

WIN_OPENOCD_DIR = C:/Users/User/Desktop/openocd-0.10.0


LIBS = -lc -lrdimon -lm
CFLAGS = -g -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -Og -g -Wall -Wextra -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -mabi=aapcs -I$(INCLUDE_DIR)

ASFLAGS = -mcpu=cortex-m0 -mthumb -mfloat-abi=soft
LINKER_FLAGS = -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -specs=nosys.specs -specs=nano.specs -specs=rdimon.specs -lc -lrdimon -Wl,-Map=output.map -Wl,--just-symbols=$(API_MAPPER) -Wl,--gc-sections -mabi=aapcs

$(OUTPUT_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUTPUT_DIR)/%.o: $(ASM_SCRIPTS)
	@mkdir -p $(@D)
	$(AS) $(ASFLAGS) -o $@ $<

$(OUTPUT_DIR)/%.elf: $(OBJS) $(ASM_OBJS)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(LINKER_FLAGS) $(LIBS) -T$(LINKER_SCRIPT)

$(OUTPUT_BIN): $(OUTPUT_DIR)/$(OUTPUT_ELF)
	@mkdir -p $(@D)
	arm-none-eabi-objcopy -O binary "$(OUTPUT_DIR)/$(OUTPUT_ELF)" "$(OUTPUT_DIR)/$(OUTPUT_BIN)"

all: $(OUTPUT_BIN)

.PHONY: run clean
.PRECIOUS: $(OUTPUT_DIR)/%.o

run: all
	openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f0x.cfg -c init -c "reset init" -c "flash write_image erase \"$(OUTPUT_DIR)/$(OUTPUT_BIN)\" 0x08001000" -c init -c "reset run"

run-windows-x86: all
	"$(WIN_OPENOCD_DIR)/bin/openocd" -f "$(WIN_OPENOCD_DIR)/scripts/interface/stlink-v2.cfg" -f "$(WIN_OPENOCD_DIR)/scripts/target/stm32f0x.cfg" -c init -c "reset init" -c "flash write_image erase \"$(OUTPUT_DIR)/$(OUTPUT_BIN)\" 0x08001000" -c init -c "reset run"

run-windows-x64: all
	"$(WIN_OPENOCD_DIR)/bin-x64/openocd" -f "$(WIN_OPENOCD_DIR)/scripts/interface/stlink-v2.cfg" -f "$(WIN_OPENOCD_DIR)/scripts/target/stm32f0x.cfg" -c init -c "reset init" -c "flash write_image erase \"$(OUTPUT_DIR)/$(OUTPUT_BIN)\" 0x08001000" -c init -c "reset run"

pack: all
	python3 $(IMAGE_CONVERTER) "./assets/src/images.txt"
	python3 $(ROM_PACKER) "./assets/src/metadata.csv" "$(OUTPUT_DIR)/$(OUTPUT_ILM)"


clean:
	rm -f $(OUTPUT_DIR)/*
