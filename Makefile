BUILD_DIR=build
include $(N64_INST)/include/n64.mk

src = main.c things.c collision.c timer.c
assets_png = $(wildcard assets/*.png)

assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))

MKSPRITE_FLAGS ?=

LDFLAGS += -lraylib
all: AirHockey64.z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	echo @$(N64_MKSPRITE) -v -f RGBA16 --compress -o "$(dir $@)" "$<"
	@cp "$<" "$(dir $@)"

$(BUILD_DIR)/AirHockey64.dfs: $(assets_conv)
$(BUILD_DIR)/AirHockey64.elf: $(src:%.c=$(BUILD_DIR)/%.o)

AirHockey64.z64: N64_ROM_TITLE="AirHockey64"
AirHockey64.z64: $(BUILD_DIR)/AirHockey64.dfs

clean:
	rm -rf $(BUILD_DIR) filesystem/ AirHockey64.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
