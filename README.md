# NimBLE FreeRTOS
本软件包为移植到FreeRTOS上的Apache 蓝牙协议栈NimBLE的例程，软件包在将NIMBLE移植到FreeRTOS的同时添加了部分BLE例程。

* 操作系统：FreeRTOS
* NimBLE版本：1.10
* 开发板：PCA10040（nrf52832）
* 集成开发环境：keil5.26
* 调试工具：J-Link（板载）或者其它

注：
* 目前移植的各个例程功能正常，但是协议栈没有经过BQB测试，适合初步学习蓝牙协议栈，如有更高要求建议慎重考虑。
* 目前使用的是NimBLE的V1.10版本，最新的V1.20版本不适合ARM_CC编译器，所以暂时没用

例程名称：
* [bas_wss_dis]体重、设备信息服务
* [hr_sensor]心率、GAP服务
* [iBeacon]iBeacon例程，微信摇一摇发现商家

## NimBLE
NimBLE符合蓝牙核心规范5.0，这使其成为物联网（IoT）的理想无线技术。

* LE广播附加信息
* 2Msym / s PHY可获得更高的吞吐量
* LE远程编码PHY
* 高占空比不可连接广播
* 频道选择算法2以更有效的方式利用频道。
* LE Privacy 1.2，用于频繁更改设备地址，从而难以跟踪外部人员
* LE安全连接具有符合FIPS的算法。
* LE数据长度扩展可提高吞吐量


我的邮箱：yuzairun@126.com，欢迎交流。
