.PHONY: all emu clean-all run-emu

OBJ_DIR := output
SCALA_DIR := src
SCALA_FILES := $(shell find $(SCALA_DIR) -name "*.scala")

EMU_SRC_DIR := emu
EMU_TOP_MODULE := ALU
EMU_TOP_V := $(OBJ_DIR)/emu_top.v
EMU_MK := $(OBJ_DIR)/emu.mk
EMU_BIN := $(OBJ_DIR)/emulator
EMU_CXXFILES := $(shell find $(EMU_SRC_DIR) -name "*.cpp")

emu: $(EMU_BIN)
run-emu: emu
	@$(EMU_BIN)

$(EMU_TOP_V): $(SCALA_FILES)
	@mkdir -p $(@D)
	@sbt "run MainDriver -tn $(EMU_TOP_MODULE) -td $(@D) --output-file $@"

$(EMU_MK): $(EMU_TOP_V) $(EMU_CXXFILES)
	@verilator --cc --exe --top-module $(EMU_TOP_MODULE) \
	  -o $(notdir $(EMU_BIN)) -Mdir $(@D) \
	  --prefix $(basename $(notdir $(EMU_MK))) $^ 

$(EMU_BIN): $(EMU_MK) $(EMU_CXXFILES)
	@cd $(@D) && make -s -f $(notdir $<)

clean-all:
	rm -rf $(OBJ_DIR)
