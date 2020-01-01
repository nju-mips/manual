#include "emu.h"
#include <memory>
#include <iostream>
#include <verilated.h>

int main(int argc, char **argv) {
  auto dut = std::make_shared<emu>();
  dut->reset = 0;
  for (int i = 0; i < 10; i++) {
    dut->io_in_valid = 1;
    dut->io_in_bits_op = i;
    dut->io_in_bits_a = 123;
    dut->io_in_bits_b = 456;

    dut->clock = 0;
    dut->eval();

    dut->clock = 1;
    dut->eval();

	printf ("RECEIVE: %d\n", dut->io_out_bits_c);
  }
  return 0;
}
