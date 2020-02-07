.PHONY: all emu clean-all run-emu

export ARCH=mips32-npc
export CROSS_COMPILE=mips-linux-gnu-
export AM_HOME=$(abspath $(shell pwd)/../../../nexus-am)
export MIPS32_NEMU_HOME=$(abspath $(shell pwd)/../../../nemu-mips32)
export INSTTEST_HOME=$(abspath $(shell pwd)/../../../insttest)

.DEFAULT_GOAL=emu

OBJ_DIR := output
SCALA_DIR := src
SCALA_FILES := $(shell find $(SCALA_DIR) -name "*.scala")

EMU_SRC_DIR := emu
EMU_TOP_MODULE := ALU
EMU_TOP_V := $(OBJ_DIR)/emu_top.v
EMU_MK := $(OBJ_DIR)/emu.mk
EMU_BIN := $(OBJ_DIR)/emulator
EMU_CXXFILES := $(shell find $(EMU_SRC_DIR) -name "*.cpp")

MIPS32_NEMU := $(MIPS32_NEMU_HOME)/build/nemu
MIPS32_NEMU_LIB := $(MIPS32_NEMU_HOME)/build/nemu.a

nemu: $(MIPS32_NEMU) $(MIPS32_NEMU_LIB)
emu: $(EMU_BIN)
run-emu: emu
	$(EMU_BIN)

$(EMU_TOP_V): $(SCALA_FILES)
	@mkdir -p $(@D)
	@sbt "run MainDriver -tn $(EMU_TOP_MODULE) -td $(@D) --output-file $@"

$(EMU_MK): $(EMU_TOP_V) $(EMU_CXXFILES)
	@verilator --cc --exe --top-module $(EMU_TOP_MODULE) \
	  -o $(notdir $(EMU_BIN)) -Mdir $(@D) \
		-CFLAGS "-I $(MIPS32_NEMU_HOME)/include" \
		-LDFLAGS "$(MIPS32_NEMU_LIB) -lpthread -lreadline -lSDL" \
		--prefix $(basename $(notdir $(EMU_MK))) $^ 

$(EMU_BIN): $(EMU_MK) $(EMU_CXXFILES)
	@cd $(@D) && make -s -f $(notdir $<)
	@touch $<

$(MIPS32_NEMU) $(MIPS32_NEMU_LIB): $(shell find $(MIPS32_NEMU_HOME) -name "*.c" -or -name "*.h")
	@make -s -C $(MIPS32_NEMU_HOME) ARCH=mips32-npc

clean-all:
	rm -rf $(OBJ_DIR)

include rules/test-all.mk
