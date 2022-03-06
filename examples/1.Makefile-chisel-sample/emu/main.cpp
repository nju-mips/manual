#include "emu.h"
#include <iostream>
#include <memory>
#include <stdint.h>
#include <verilated.h>

extern "C" {
void klee_make_symbolic(void *, int, const char *);
void klee_assume(int);
}

int main(int argc, char **argv) {
  emu dut{};
  dut.reset = 0;

  uint32_t symbols[3];
  klee_make_symbolic(symbols, sizeof(symbols), "input");

  dut.io_in_valid = 1;
  dut.io_in_bits_op = symbols[0];
  dut.io_in_bits_a = symbols[1];
  dut.io_in_bits_b = symbols[2];
  klee_assume(symbols[0] < 8);

  while (true) {
    dut.clock = 0;
    dut.eval();
    dut.clock = 1;
    dut.eval();

    if (dut.io_out_bits_c == 0x12345678) break;
  }
  assert(0);
  return 0;
}
