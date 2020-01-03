# 如何在nemu-mips32及noop上运行linux

## 基本的工具链依赖
```
sudo apt-get install gcc-mips-linux-gnu
sudo apt-get install flex bison
sudo apt-get install libncurses5
```

## 在nemu-mips32上运行linux
```
git clone git@github.com:nju-mips/u-boot
git clone git@github.com:nju-mips/linux
git clone git@github.com:nju-mips/nemu-mips32

export ARCH=mips
export CROSS_COMPILE=mips-linux-gnu-

cd u-boot
make noop_defconfig
make -j32
cd ..

cd linux
make noop_defconfig
make -j32 uImage
cd ..

cd nemu-mips32
make mips32r1_defconfig
make -b -e ../u-boot/u-boot --fifo-data xilinx-uartlite:serial/bootm.in --block-data ddr:0x04000000:../linux/arch/mips/boot/uImage.bin
```
