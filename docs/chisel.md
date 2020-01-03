# chisel

chisel是由伯克利开发的一门硬件构建语言，嵌入在scala编程语言中。相比verilog，chisel有抽象的数据类型和接口，层次化+面向对象+功能化构造，可以很简单地实现工程的高度参数化。可以编译生成出verilog语言，相当于软件设计语言中的高级语言。

## chisel安装
使用chisel语言需要安装sbt
官方网站：https://www.scala-sbt.org/1.0/docs/Installing-sbt-on-Linux.html

```
$ echo "deb https://dl.bintray.com/sbt/debian /" | sudo tee -a /etc/apt/sources.list.d/sbt.list
$ sudo apt-key adv --keyserver hkps://keyserver.ubuntu.com:443 --recv 2EE0EA64E40A89B84B2DF73499E82A75642AC823
$ sudo apt-get update
$ sudo apt-get install sbt
```

## 用chisel编写硬件
```
git clone https://github.com/addrices/chisel-template.git
```
这是使用chisel语言的一个简单的例子，即本项目下 examples/Makefile-chisel-sample,可以直接从上面的仓库获得。
我们简单分析一下这个项目中的各个文件的作用：

这个简单的项目是使用chisel编写硬件并且使用verilator来进行测试。Makefile文件中命令的详细解释放置在verilator.md中。

```
import chisel3._
import chisel3.util._
import chisel3.iotesters.{ChiselFlatSpec, Driver, PeekPokeTester}

/*类似于c语言中的函数，可以在电路重复的地方复用*/
object GTimer {
  def apply(): UInt = {
    val (t, c) = Counter(true.B, 0x7fffffff)
    t
  }
}

/*类似于c语言中的结构体，将相关的信号线打包成一个结构体，其中的Output表示输出（Input是输入）*/
class ALU_IN extends Bundle {
  val op = Output(UInt(4.W))
  val b = Output(UInt(32.W))
  val a = Output(UInt(32.W))
}

class ALU_OUT extends Bundle {
  val c = Output(UInt(32.W))
}

class ALU extends Module {
    //表示当前ALU模块的输入输出的信号定义
  val io = IO(new Bundle {
   //这里的Flipped表示的是该结构体中所有Input信号变成Output信号，Output信号变成Input信号
    val in = Flipped(ValidIO(new ALU_IN))            
    val out = ValidIO(new ALU_OUT)
  })

  io.in := DontCare

  /* in is valid at the next cycle of valid io.in */
  val in_valid = RegNext(io.in.valid, init=false.B)
  val in = RegEnable(enable=io.in.valid, next=io.in.bits,
    init=0.U.asTypeOf(io.in.bits))

  val neg_b = Mux(in.op(3) === 0.U, in.b, ~in.b + 1.U)
  val sr = Mux(in.op(3) === 0.U,
    in.a >> (in.b(4, 0)),
    (in.a.asSInt >> (in.b(4, 0))).asUInt)

  io.out.valid := in_valid
  io.out.bits.c := Mux1H(Seq(
    (in.op(2, 0) === 0.U) -> (in.a + neg_b),
    (in.op(2, 0) === 1.U) -> (in.a << (in.b(4, 0))),
    (in.op(2, 0) === 2.U) -> (in.a.asSInt < in.b.asSInt).asUInt,
    (in.op(2, 0) === 3.U) -> (in.a < in.b).asUInt,
    (in.op(2, 0) === 4.U) -> (in.a ^ in.b),
    (in.op(2, 0) === 5.U) -> (sr),
    (in.op(2, 0) === 6.U) -> (in.a | in.b),
    (in.op(2, 0) === 7.U) -> (in.a & in.b )
  ))

  when (in_valid) {
    printf("CLOCK: %x, op: %x, a: %x, b: %x; c: %x\n",
      GTimer(), in.op, in.a, in.b, io.out.bits.c)
  }
}

//这个函数是为chisel代码生成verilog代码使用的。指定最后生成的顶层模块即可。
object MainDriver extends ChiselFlatSpec {
  def main(args: Array[String]): Unit = {
    chisel3.Driver.execute(args, () => new ALU)
  }
}
```
在根目录下输入
```
$ sbt
$ run MainDriver -tn [顶层模块] -td [源文件目录] --output-file [目的地址文件]
```
即可获得对应的.v文件
## 相关网站
伯克利的Chisel教程: https://github.com/ucb-bar/chisel-tutorial

chisel api查询: https://www.chisel-lang.org/api/chisel3/latest/
