.PHONY: compile-linux compile-u-boot

LINUX_OBJDIR := $(OBJ_DIR)/linux
LINUX_ELF    := $(LINUX_HOME)/vmlinux
U_BOOT_ELF   := $(U_BOOT_HOME)/u-boot

$(U_BOOT_ELF) $(LINUX_ELF):
	@ARCH=mips CROSS_COMPILE=mips-linux-gnu- make -s -C $(@D) -j32

compile-u-boot: $(U_BOOT_ELF)
compile-linux: $(LINUX_ELF)
compile-u-boot compile-linux:
	@mkdir -p $(LINUX_OBJDIR)
	@cd $(LINUX_OBJDIR) && ln -sf $^ . && \
	  mips-linux-gnu-objdump -d $^ > $(<F).S
