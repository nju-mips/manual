# verilator

verilator可以将我们的verilog代码使用C++模拟，从而使用C++语言可以编写测试程序。verilator将我们的verilog代码编译成一个类，我们只需要在我们的测试的.c文件中include其生成的头文件即可。

## 前置说明
verilator的用法，在样例工程中均有体现，建议从样例工程入手，辅之以手册博客

## 使用makefile组织verilator
具体例子见 [example](../examples/1.Makefile-chisel-sample),组织一个工程的makefile,首先得知道这个工程的编译流程以及需要的项目目录的组织(将源代码和生成的代码放置在不同的文件夹中),其次将编译过程中的重要的中间文件确定出来,将其固定命名,这样可以使得makefile的流程更加简单易维护,然后按照这些中间文件的编译顺序来编写makefile,确定依赖,使用知道的编译命令逐步编译.

## 助记符列表
|符号|含义|
|:--|:--|
|$(@D)|当前规则目标所在目录|
|$@|当前规则目标|
|$<|当前规则依赖项的第一个|
|$^|当前规则的所有依赖项|
|$(notdir xxx)|xxx文件去除其目录|


## Makefile代码解析
```
.PHONY: xxx
```
.PHONY定义的伪目标的作用是能够使得makefile在每次make的时候将这个目标下的指令重新做一遍,因为当依赖没有改变时,makefile会默认这个过程不需要再执行一次,但有时候我们需要这个目标作为一个常用的调用执行,这样可以将它定义为伪目标

```
SCALA_FILES := $(shell find $(SCALA_DIR) -name "*.scala")
```
这个指令的作用是将**SCALA_DIR**目录下寻找所有的**scala**后缀的文件.
```
$(EMU_TOP_V): $(SCALA_FILES)
	@mkdir -p $(@D)
	@sbt "run MainDriver -tn $(EMU_TOP_MODULE) -td $(@D) --output-file $@"
```
这一段是将SCALA文件使用sbt编译成对应的.v文件,前面的@标记会使得这条命令不出现在命令行中.
```
$(EMU_MK): $(EMU_TOP_V) $(EMU_CXXFILES)
	@verilator --cc --exe --top-module $(EMU_TOP_MODULE) \
	  -o $(notdir $(EMU_BIN)) -Mdir $(@D) \
	  --prefix $(basename $(notdir $(EMU_MK))) $^ 
```
这一段将之前生成的.v文件和我们编写的cpp文件一起编译成verilator的bin文件, --cc是说明使用C++, --exe是说明和cpp一起编译生成一个可执行工程, --top-module是用于指定出需要编译的顶层模块. -o指定最后生成的可执行文件名, -Mdir指明verilator生成文件所在目录, --prefix指明使用的.mk文件.

```
$(EMU_BIN): $(EMU_MK) $(EMU_CXXFILES)
	@cd $(@D) && make -s -f $(notdir $<)
```
这一段将verilator工具生成的代码使用其规则来make出最后的可执行文件.

## emu/main.cpp解释
```
#include "emu.h"
#include <memory>
#include <iostream>
#include <verilated.h>

int main(int argc, char **argv) {

/*emu是verilator工具为我们的verilog代码生成的一个类。*/
  auto dut = std::make_shared<emu>();	

/*dut->reset表示的是我们的verilog代码中的一个输入信号。*/

  dut->reset = 0;					
  for (int i = 0; i < 10; i++) {
    dut->io_in_valid = 1;
    dut->io_in_bits_op = i;
    dut->io_in_bits_a = 123;
    dut->io_in_bits_b = 456;

/*以下4句代码表示了一个周期的变化，clock置0再置1，即为一个周期的变化*/

    dut->clock = 0;							
    dut->eval();								//更新类中各个信号的信息

    dut->clock = 1;
    dut->eval();

/*打印出电路模拟的时候的某个信号的信息，可以写相关代码自动判断信号是否出错*/
	printf ("RECEIVE: %d\n", dut->io_out_bits_c);
  }
  return 0;
}
```