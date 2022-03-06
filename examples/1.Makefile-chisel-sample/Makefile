.PHONY: all emu clean-all run-emu bitcode

OBJ_DIR := output
SCALA_DIR := src
SCALA_FILES := $(shell find $(SCALA_DIR) -name "*.scala")

EMU_SRC_DIR := emu
EMU_TOP_MODULE := ALU
EMU_TOP_V := $(OBJ_DIR)/emu_top.v
EMU_MK := $(OBJ_DIR)/emu.mk
EMU_BIN := $(OBJ_DIR)/emulator
EMU_BC := $(OBJ_DIR)/emulator.bc
LLVM_LINK := llvm-link-10
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

$(EMU_BC): $(EMU_MK) $(EMU_CXXFILES)
	@echo + $(EMU_BC)
	@cd $(@D) && \
	  make -s -nB -f $(notdir $<) | \
	  sed 's/^g++/clang -fno-rtti -fno-exceptions -g -emit-llvm/g' | \
	  sed 's/\(\w*\)\.[oa]/\1.bc/g' | \
	  sed 's/-Wno-unused-but-set-variable//g' | \
	  sed 's/ar -cr/$(LLVM_LINK) -o/g' | \
	  sed 's/clang.*-emit-llvm\(.*\)-o.*-lm -lstdc\+\+.*/$(LLVM_LINK) \1 -o emulator.bc/g' | \
	  sed 's/^ranlib/#ranlib/g' | sh -

bitcode: $(EMU_BC)

clean-all:
	rm -rf $(OBJ_DIR)
