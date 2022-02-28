.PHONY: run-% prj

run-%: $(MIPS32_NEMU) compile-%
	@cd $($*_OBJDIR) && $< -b -e ./$*.elf

prj: $(MIPS32_NEMU)
update: ;
clean: ;
