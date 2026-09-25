BUILD_DIR=build
include $(N64_INST)/include/n64.mk

src = src/main.c
assets_png = $(wildcard assets/*.png)

assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite)))

MKSPRITE_FLAGS ?=

all: cube_engine.z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) -f RGBA16 -o "$(dir $@)" "$<"

$(BUILD_DIR)/cube_engine.dfs: $(assets_conv)
$(BUILD_DIR)/cube_engine.elf: $(src:%.c=$(BUILD_DIR)/%.o)

cube_engine.z64: N64_ROM_TITLE="cube_engine"
cube_engine.z64: $(BUILD_DIR)/cube_engine.dfs

clean:
	rm -rf $(BUILD_DIR) filesystem/ cube_engine.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
