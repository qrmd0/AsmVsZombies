<!--
 * @Coding: utf-8
 * @Author: vector-wlc
 * @Date: 2021-09-25 15:09:04
 * @Description: AvZ 起步
-->

# 起步

## 本教程只完全适配 `AvZ 210820` 版本，如果根据本教程编写的脚本出现语法错误，请先检查是否与该教程的版本相对应

AvZ 版本号按照以下方式查找
```C++
// AvZ根目录->inc->libavz.h 
#define __AVZ_VERSION__ 210820
```

欢迎学习 AsmVsZombies 键控框架，AsmVsZombies 是由我和 [yuchenxi0_0](https://www.bilibili.com/video/BV1WJ41177a3) 两人合作完成，yuchenxi0_0 主要完成了反汇编注入工作，我主要完成了键控逻辑。我相信这套框架会带给脚本编写者焕然一新的使用体验。

本教程总体将分为两大部分：基础和进阶。


在基础部分，我们将会了解到键控所需的必备接口，例如用卡、用炮、时间设定等，因此，不论你是何方神圣，是必须将基础部分刷完的，这样在编写脚本时才不会遇到疑惑。


在进阶部分，我们将深入讨论 AvZ 的键控逻辑核心——操作队列，理解透操作队列的实现原理非常重要，然后介绍阻塞函数，动态插入函数，以及内存读取功能。


**如果只是想编写炮阵的脚本，看完基础部分是足够的，但是如果想把 AvZ 玩出花来，甚至想用 AvZ 搞一搞无炮键控，那么进阶部分是必须要看的。**

## 准备

**在使用 AvZ 之前，我们需要进行一些准备，这些内容我放在了下面这个视频中，请一定耐心观看，不要错过任何一个细节。**

接下来给出一些传送门

VSCode版下载：https://gitee.com/vector-wlc/AvZTools/blob/master/bin/AsmVsZombies_VSCode_Setup.exe

更新：https://gitee.com/vector-wlc/AsmVsZombies/tree/master/release

准备阶段完成之后，请继续结合视频教程完成自己的第一脚本 ：经典十二炮。

最后，希望 AvZ 能够在键控方面帮助到您！