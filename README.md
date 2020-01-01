# 2019 manual

# [CPU core](manuals/NOOP.pdf)
* 单周期CPU
* 多周期CPU（考虑信号的阻塞）
* 一般流水线CPU（标准5段、score board）
  * 长延迟指令（访存，乘除法）
* 高级流水线CPU (分支预测器、ICache、DCache)
  * CP0 (中断、异常)
  * L2 cache
  * TLB
* 乱序流水线CPU（rename table、issue queue、ls queue、rob）
* 乱序双发射CPU

# MIPS standard ([1](manuals/MIPS_Vol1.pdf), [2](manuals/MIPS_Vol2.pdf), [3](manuals/MIPS_Vol3.pdf))
* ALU: 算术运算指令(确定延迟)
  * add, addu, addi, addiu, sub, subu, and, andi, clz, or, ori, xor, xori, nor, slt, sltu, slti, sltiu, sll, sllv, srl, srlv, sra, srav, lui, movn, movz
  * 注意：clz有高效的递归[实现方案](docs/clz.md)，采用低效方案容易形成关键路径
* BRU: 分支跳转指令
  * beq, bgtz, blez, bltz, bgez, bltzal, bgezal, bne, j, jal, jalr, jr
  * 注意：所有的分支指令都有延迟槽，即分支后一条指令一定执行
* LSU: 访存指令(不定延迟)
  * 对齐访存指令: lb, lbu, lh, lhu, lw, sb, sh, sw
  * 不对齐访存指令: lwl, lwr, swl, swr
  * 原子性读写指令: ll, sc
* MDU: 乘除法指令(长延迟)
  * mul, mult, multu, div, divu, mfhi, mflo, mthi, mtlo
  * 建议：hi和lo寄存器随通用寄存器一起进行转发，如果单独放在MDU里面，后续高级流水线的时候需要两套处理方案
* PRU: 特权指令(该单元又名CP0)，重点看[3](manuals/MIPS_Vol3.pdf)
  * syscall: 发起系统调用的指令
  * eret: 系统调用执行完从内核态返回用户态的指令
  * mfc0: cp0有自己的一套标准规定的寄存器，这条指令用于读cp0寄存器到一般寄存器
  * mtc0: 用于将一般寄存器的值写入cp0
  * tlbp: 用于查询一个虚拟地址的页表项是否存在
  * tlbr: 用于读取一个页表项(cp0维护虚拟地址到物理地址的映射,形式有标准规定)
  * tlbw: 用于写入一个页表项
  * tlbwr: 由硬件随机淘汰一个旧页表项，并将新表项写入
  * cache: 用于控制cache，包括将cache的一行写回，标记为无效等操作
  * pref: 内存预取指令，用于加速，具体实现的时候可以什么都不做
  * sync: 多核之间同步的指令，单核可以什么都不做
  * break: 抛异常就行了
  * tlt, tge, tltu, tgeu, tlti, teqi, tgei, tnei, tltiu, tgeiu, tne, teq: 自陷指令，条件满足的时候让cpu停住，不要抛异常了，直接在开发板上点个灯，正常执行是不会有这条指令的
* CP0: 0号协处理器
  * 维护页目录，页目录缺失由操作系统回填，硬件抛异常就行了
  * 维护cp0寄存器，以优先级排序：
    * status, cause, epc (实现系统调用必须)
	* badvaddr (实现异常必须，用于指示最后一个出错的访存地址)
	* index, pagemask, context, entry\_lo0, entry\_lo1, entry\_hi (实现虚存必须)
	* count, compare (实现时间中断必须)
	* prid, config, config1 (运行u-boot和linux必需)
	* base (可选，旧标准内没有，用于调整异常向量表的基址，缺省为0xbfc00000)
	  * cpu启动时也是0xbfc00000，所以异常向量一般需要后期写入
* CP0寄存器的布局
  * 寻址: (待续)

# CPU uncore
* [AXI4](manuals/AXI4.pdf): 最常用的核外总线协议
  * 几乎绝大多数硬件的IP核都会使用这个协议
* AXI4lite
  * 简化版的AXI4，但是不支持burst传输，一开始可以用这个
* Uartlite (用axi4-uartlite的IP核就行，无需自行实现)
  * 最简单的串口协议
  * 连接串口的简单[教程](docs/minicom.md)
* Emaclite (不需要深入理解、用就可以了)
  * 最简单的网口协议
* VGA (用往届的就可以了)
  * 数电课的VGA控制器包装成AXI4控制接口
* BlockRAM、RAM
  * 片上资源，BlockRAM是读写均同步，适合用来做cache
  * RAM，写同步，读异步，适合用来做register

# CPU调试常用的工具链
* [Makefile](docs/Makefile.md)
  * 用于将一键化各种功能
* tcl脚本
  * vivado的自动化脚本，可以调用vivado的功能
  * 如果需要一键创建vivado项目，这个是常用的工具
   * vivado可以将当前项目导出到tcl脚本
* block design
  * 用图形界面来连接模块和IP核
* [verilator](docs/verilator.md)
  * verilog的模拟器，非常快，可以方便你们回归测试
  * 到后期，用到IP核的时候，还需要用vivado的仿真工具
  * 样例工程在`examples/1.Makefile-chisel-sample`中
* chisel
  * 高级硬件描述语言
  * 样例工程在`examples/1.Makefile-chisel-sample`中
* ILA
  * 调试的终极大招，可以在板上电路中采样信号

