# 红外寻迹小车项目

## 项目说明

我们这个项目是一个红外寻迹小车项目，主要的功能是参考2024年全国电子设计大赛H题自动行驶小车。

资料里安装了所有的有用资料。安装使用说明以及成品图

只供学习使用

## 代码解释

>  两种寻迹模式，一个角度寻迹，二是红外寻迹

**红外寻迹**

> 然后gd1,和gd5记录方向分别是左和右。gd2，和gd4是不断的红外寻迹控制电机转动
>
> 通过==key==的选择来选择不同的赛题。

**角度寻迹**

> 本角度寻迹是通过PID精准控制某个方向，通过调整target_angle来确定小车角度的方向，具体的KP，KI，KD这个值通过测量得出来的，建议参考网上，如果你不需要特别精准那么采用我们的PID值也是可以的

这个我只是粗说代码，具体请看代码

## 现象解释

这里会遇到一些莫名的现象我来解释一下

**小车为什么转的方向不是想要的**

> 这个是需要调整代码的target_angle 控制角度的方向

**小车为什么红外寻迹没有完成就开始直走或者完成了还在直走**

> 这个是因为代码中没有调整angle角度。仔细观察代码中会发现判断红外寻迹结束的标志不光光是判断小车寻迹到白线，因为红外寻迹代码是g2和g4来判断方向，哪个为1就放哪个方向转，如果同时为0或者为1直走，既然同时为1了说明同时扫描到黑线或者0白线都会直走，然后这个过程不断地循环往复。所以红外寻迹因为走了直线非常有可能在黑线边边也会扫到白线，但是代码中是个&& 符号限制还要有angle这个角度。没有达到一定的angle不会进入下一个阶段。如果你把这个angle调小了，那么还没寻迹完成就会进入下一个阶段。如果调大了那么可能寻迹完成还在走直线，因为角度angle没有达到条件，所以具体的值需要正好转好了去测量这个angle这个角度

**小车为什么走完红外寻迹就开始转圈**

> 这是因为红外寻迹过程中达到了一定的角度结束了红外寻迹，但在黑线里头角度寻迹直接结束，然后马上走红外寻迹，红外寻迹黑线完遇到白点马上开始通过PID调节角度，就开始转圈去找那个target_angle那个角度也就是会发生转圈现象

**小车如何停下来**

> 观察代码就会发现小车代码停止要调整参数，把参数调好就行了

**小车为什么遇到黑线不开始红外寻迹呢反而直走**

> 这个问题有多种可能性，一方面可能是gd1，gd5同时扫到黑线就会直走，这个问题解决只能通过调整出发点的角度尽量不要让走的方向和目标黑线的切线方向垂直。
>
> 另一方面可能就是你的红外传感器不敏感，观察红外传感器的灯去观察遇到黑线是否灭了，调整敏感度

qq：2842001726（有偿指导）