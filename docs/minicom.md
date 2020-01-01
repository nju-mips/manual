# minicom
minicom是一款终端串口连接软件，并且minicom自己有一套脚本语法，便于在不定延迟的串口上传送指令

## 基本的使用
将开发板上的uart接口和电脑的usb接口相连接，此时在你的ubuntu的/dev目录下会多出一个ttyUSB1文件，这个文件就是用于连接串口的钥匙
* 连接串口: `sudo minicom -D /dev/ttyUSB1 -b 115200`
  * `-D /dev/ttyUSB1`的含义显而易见
  * `-b 115200`用于指定串口的波特率，可以理解为传数据的频率，115200就是每秒传115200个比特
    * 注意：上板设置的波特率和minicom设置的波特率一定要一样，不然会乱码
* 开启escape控制码: `minicom -c on ...`
  * 一般情形你在终端里`printf "\e[32mHelloWorld\e[0m"`是会出现彩色文字的(因为这里使用了控制码`\e[32m`)，彩色文字便于在调试的时候快速定位关键信息，但是minicom从串口接受数据并显示的时候会把彩色(escape控制码)给过滤掉，你可以通过`-c on`开启彩色显示

## minicom脚本
一个你们开始上板时会频繁出现的需求，那就是自动化启动加载过程，因为从上板开始，到你们加载cpu完毕，你们需要在串口里敲很多命令，而自动化可以给你们省下很多时间：

一个简单的例子，如果你需要启动u-boot，然后从网口加载程序到cpu上，经常会敲的指令：
```
UBOOT> set serverip 192.168.1.104
UBOOT> set ipaddr 192.168.1.107
UBOOT> set gateway 192.168.1.104
UBOOT> tftpboot 0x82000000 nanos-pal
UBOOT> bootelf -p 0x82000000
```
这几条命令还有一个限制，那就是每次敲的时候都必须等待相应的提示符出现之后才能敲命令，过早的敲会被无效掉，而将这一过程写成脚本就是：
```
START:

gosub WAIT_PROMPT
send "set serverip 192.168.1.104"

gosub WAIT_PROMPT
send "set ipaddr 192.168.1.107"

gosub WAIT_PROMPT
send "set gateway 192.168.1.104"

gosub WAIT_PROMPT
send "tftpboot 0x82000000 nanos-pal"

gosub WAIT_PROMPT
send "bootelf -p 0x82000000"

exit

WAIT_PROMPT:
expect {
  "UBOOT> " break
  goto FAIL
}
return

FAIL:
```
然后下次你再连接串口的时候，使用指定这个脚本就行了(假设上述内容写入了minicom.script这个文件): `sudo minicom -D /dev/ttyUSB1 -b 115200 -c on -S minicom.script`

## minicom的日志记录功能
有的时候你可以需要让minicom把所有的内容都记下来，这个时候你加上一个`-C xx.log`选项就行了，这个选项会将所有内容追加到`xx.log`这个文件末尾。
