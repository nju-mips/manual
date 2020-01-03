# Makefile的基本用法

# 规则依赖

一个简单的例子：
```mk
a.o: a.c
  g++ a.c -o a.o

b.o: b.c
  g++ b.c -o b.o

binary: a.o b.o
  echo + binary
```

将上述内容写入到你当前目录的Makefile文件中，并保证`a.c`和`b.c`文件存在，然后在命令行里敲`make binary`，这条语句的意思是制造`binary`这个目标，make会读特定名称的文件来获取制造这个目标的规则(Makefile是make内置的一个特定名称)。make读取你的Makefile后会发现binary依赖于`a.o`和`b.o`，并进而计算得出`a.o`依赖于`a.c`，`b.o`依赖于`b.c`，最后按照你给定的生成规则`g++ a.c -o a.o`和`g++ b.c -o b.o`去生成相应的`a.o`和`b.o`。

敲完`make binary`之后，你binary并没有生成，因为你还没有为binary添加生成规则，所以如果你再敲一遍`make binary`，相应的`echo + binary`语句会被再执行一遍。

我们稍微改一下，让他变成下面这样：
```mk
a.o: a.c
  g++ a.c -o a.o

b.o: b.c
  g++ b.c -o b.o

binary: a.o b.o
  cat a.o b.o > binary
```

这个时候如果你在敲`make binary`，生成规则会将`a.o`与`b.o`拼接成binary文件，由于binary文件是最新生成的，它的时间戳大于`a.o`和`b.o`的时间戳，所以再下一次你敲`make binary`的时候，make会输出`'binary' is up to date`。

一般而言，我们并不希望我们所生成的中间文件和源码文件共用一个目录，假设我们希望将目录结构调整成这样：
```
+---src
|   +--- a.c
|   +--- b.c
|
+---build
    +--- a.o
    +--- b.o
    +--- binary
```

为了达成这个目的，我们需要对Makefile做一些微调：
```
build/a.o: src/a.c
  mkdir -p build
  g++ src/a.c -o build/a.o

build/b.o: b.c
  mkdir -p build
  g++ src/b.c -o build/b.o

build/binary: build/a.o build/b.o
  cat build/a.o build/b.o > build/binary
```

相应的，基于这个Makefile来生成binary，我们需要执行`make build/binary`，这条命令会按照你的规则去生成`build/a.o`和`build/b.o`两个文件，然后拼接成`build/binary`这个文件。

这个时候，你可能发现，make的时候连目录一起写进去太撒刁了，所以这个Makefile可以进一步改进：

```
.PHONY: binary

binary: build/binary

build/a.o: src/a.c
  mkdir -p build
  g++ src/a.c -o build/a.o

build/b.o: b.c
  mkdir -p build
  g++ src/b.c -o build/b.o

build/binary: build/a.o build/b.o
  cat build/a.o build/b.o > build/binary
```

`.PHONY`的作用是定义一个伪目标，什么是伪目标，直观的讲就是，这个目标在磁盘上不存在。之前Makefile里面的`a.o`、`build/binary`、`build/a.o`这些目标在磁盘上是会对应到一个实际存在的文件的，而伪目标不需要满足这个要求，同时相应的，make也不会去检查伪目标的时间戳，而是直接调用他的生成规则，比如：

```
.PHONY: hello

hello:
  touch hello
  echo hello world
```

在这个makefile中，无论你敲几次`make hello`，`touch hello`和`echo hello world`这两条生成规则都会被执行。

# 通用匹配%和生成规则简化

在上文提到的Makefile中，你会发现在编写a.o和b.o的生成规则的时候，你需要把`build/`和`a.o`这样的多余的东西再写一遍，这个信息明明已经包含在依赖目标和生成目标里了，为了简化规则的编写，makefile定义了一系列的简写符号，用这些符号我们可以将Makefile简化成：

```
.PHONY: binary

binary: build/binary

build/a.o: src/a.c
  mkdir -p $(@D)
  g++ $^ -o $@

build/b.o: b.c
  mkdir -p $(@D)
  g++ $^ -o $@

build/binary: build/a.o build/b.o
  cat $^ > $@
```

其中`$^`出现在生成规则中表示所有被依赖的目标(示例中$^会被替换成`build/a.o build/b.o`)，`$@`表示生成的目标(示例中上下两条生成规则里的`$@`分别会被替换成`build/a.o`和`build/b.o`)，`$(@D)`则是表示生成目标所在的目录，也就是`build/`，所以上面的Makefile其实与之前的Makefile完全等价。

> |符号|含义|
> |:--|:--|
> |$(@D)|当前规则目标所在目录|
> |$@|当前规则目标|
> |$<|当前规则依赖项的第一个|
> |$^|当前规则的所有依赖项|
> |$(notdir xxx)|xxx文件去除其目录|
> |$(basename xxx)|取文件名，去掉后缀名|

到这里，你会发现一件更撒刁的事，那就是`mkdir -p $(@D)`和`g++ $^ -o $@`这两条语句你在两个目标的生成规则中写了两遍，为了解决这样的问题，Makefile又提供了通配符机制，利用通配符，我们可以将Makefile进一步改写成：

```
.PHONY: binary

binary: build/binary

build/%.o: src/%.c
  mkdir -p $(@D)
  g++ $^ -o $@

build/binary: build/a.o build/b.o
  cat $^ > $@
```

在这个Makefile上，如果我们敲`make binary`，make会去查找`build/binary`所依赖的`build/a.o`和`build/b.o`的生成规则，当然make是找不到的，因为我们并没有明确的写这样的规则。但是make会进一步发现，它只要把`build/%.o`中的`%`这个字符替换成`a`，他就能匹配上`build/%.o: src/%.c`这条规则，于是它就把这条规则里面所有的`%`都替换成了`a`，便得到了`build/a.o`的生成规则，同理它也能获得`build/b.o`的生成规则。

# 变量和函数的使用

在上面的Makefile里面，我们还有着这样一个问题，就是如果我们不知道src下面究竟有多少`.c`文件，但是我们依旧希望Makefile能工作应该怎么办。举个简单的场景，你的项目正在如火如荼的发展中，每天都可能添加若干个`.c`文件或重命名若干个`.c`文件，如果你每次都要手动更新Makefile，未免太过撒刁了，为了解决这个问题，Makefile提供了函数机制。

回顾我们的需求，我们其实需要的是make能自动找到src下所有的.c文件，并将其重命名为build下的.o文件，最后放到`build/binary`的依赖目标中去：

```
.PHONY: binary

binary: build/binary

build/%.o: src/%.c
  mkdir -p $(@D)
  g++ $^ -o $@

build/binary: $(patsubst src/%.c,build/%.o,$(shell find src/ -name "*.c"))
  cat $^ > $@
```

新的Makefile中`build/binary`所依赖的目标已经被替换成了`$(patsubst src/%.c,build/%.o,$(shell find src/ -name "\*.c"))`，我们分开来讲这条语句，首先是`$(shell find src/ -name "\*.c")`，这条语句会调用你终端中的`find`命令，查找`src`目录下所有的`.c`文件，并将其替换成查找的结果，也就是说这条语句执行完毕之后原语句会变成`$(patsubst src/%.c,build/%.o,src/a.c src/b.c)`，`patsubst`是make的一个内置函数，它的作用是根据你指定的初始特征和结果特征，对你的输入进入转化，对应到这条语句，你给它的初始特征是`src/%.c`，结果特征是`build/%.o`，输入是`src/a.c src/b.c`，对于输入中的每一项，如`src/a.c`，`patsubst`函数会发现将`%`换成`a`就能匹配上，然后他讲结果特征中的`%`也换成`a`，并将结果替换掉`src/a.c`，也就是说，`src/a.c`会被替换成`build/a.o`，同理`src/b.c`也会被替换成`build/b.o`。

上述Makefile看着非常蛋疼，因为我们写了一个超长的语句，有没有一种机制，像编程语言一样，通过定义一些中间变量来拆分超长表达式呢？答案当然是有的，所以上面的Makefile又可以改成下面这样：

```
.PHONY: binary

SRC_DIR := src/
SRCS    := $(shell find $(SRC_DIR) -name "*.o")
OBJ_DIR := build/
OBJS    := $(patsubst src/%.c,build/%.o,$(SRCS))

binary: $(OBJ_DIR)binary

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
  mkdir -p $(@D)
  g++ $^ -o $@

$(OBJ_DIR)binary: $(OBJS)
  cat $^ > $@
```

上面的赋值使用了`:=`，你暂时不需要知道为什么是`:=`而不是`=`，如果你真的想了解，在docs目录下的Makefile.pdf可以给你答案，同时如果哪天你需要什么其它的功能，你也可以通过查阅`Makefile.pdf`来看看是否有相应的函数。

# 宏的使用与自动规则生成

假如你想把你的Makefile包装成一个功能库，其他人只要include你的Makefile就能获得上述所有的功能，那么你需要怎么做呢？一个简单的答案就是宏。
