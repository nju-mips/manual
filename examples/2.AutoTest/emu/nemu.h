#ifndef __NEMU_H__
#define __NEMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "memory.h"
#include "cpu/reg.h"

extern nemu_state_t nemu_state;
extern work_mode_t work_mode;

/* APIs exported by nemu-mips32 */
extern CPU_state cpu;
extern void cpu_exec(uint64_t);
extern void print_registers(uint32_t);
extern void print_instr_queue(void);
extern work_mode_t init_monitor(int argc, char *argv[]);
extern void init_device(void);
extern uint32_t paddr_peek(vaddr_t addr, int len);
extern uint32_t get_current_instr();
/* APIs exported by nemu-mips32 */

#ifdef __cplusplus
}
#endif

// wrappers for nemu-riscv32 library
class MIPS32_NEMU {
  class GPRHelper {
  public:
    GPRHelper() = default;

    uint32_t operator[](const size_t i) {
      return cpu.gpr[i];
    }
  };

  GPRHelper gpr;

public:
  MIPS32_NEMU(int argc, const char *argv[]) {
    init_monitor(argc, const_cast<char **>(argv));
    // init_device();
  }

  void exec_one_instr() { cpu_exec(1); }

  void dump() {
    eprintf(
        "================nemu instrs=================\n");
    print_instr_queue();
    eprintf(
        "==============nemu registers================\n");
    print_registers(get_instr());
    eprintf(
        "==============nemu status end===============\n");
  }

  uint32_t get_pc() { return cpu.pc; }
  uint32_t get_gpr(uint32_t i) { return cpu.gpr[i]; }

  uint32_t get_instr() { return get_current_instr(); }
};

#endif
