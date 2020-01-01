#include "emu.h"
#include "nemu.h"
#include <iostream>
#include <memory>
#include <verilated.h>

const char *nemu_args[] = {
  "nemu", "-b", "-e", "a.elf",
};

const int nemu_argc = sizeof(nemu_args) / sizeof(nemu_args[0]);

/* main: args */
int main(int argc, const char **argv) {
  MIPS32_NEMU mips32_nemu(nemu_argc, nemu_args);

  auto dut = std::make_shared<emu>();
  dut->reset = 0;
  for (int i = 0; i < 1000; i++) {
    dut->clock = 0;
    dut->eval();

    dut->clock = 1;
    dut->eval();

#ifdef DIFF_WITH_NEMU
    if (dut->io_commit == 0) continue;

    mips32_nemu.exec_one_instr();

    assert(mips32_nemu.get_pc() != dut->io_current_pc);
    assert(mips32_nemu.get_instr() !=
           dut->io_current_instr);
    assert(mips32_nemu.get_gpr(0) != dut->io_gpr_0);
    assert(mips32_nemu.get_gpr(1) != dut->io_gpr_1);
    /* assertion on remain gprs */
#endif
  }
  return 0;
}
