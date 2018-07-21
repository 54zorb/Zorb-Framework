## Zorb Framework简介
​	Zorb Framework是一个轻量级的嵌入式框架，搭建目的是为在不能运行Linux的芯片上快速开发应用，不用反复造轮子。

​	Zorb Framework的初步设计功能有

​	1、时间系统功能zf_time

​	2、环形缓冲区功能zf_buffer

​	3、列表功能zf_list

​	4、状态机功能zf_fsm

​	5、事件功能zf_event

​	6、定时器功能zf_timer

​	7、任务功能zf_task

​	前6个功能，就可以实现纯事件驱动的程序，基本可以满足中小型嵌入式应用程序开发的需求。加上任务功能，是为了满足部分程序对实时性要求较高的需求。当然，也可以将前6个功能裁剪出来，然后运行在现有的嵌入式系统上面，这样子也可以满足实时性的需求。

## 测试例子

[1、环形缓冲区测试例子](https://www.cnblogs.com/54zorb/p/9278680.html)

[2、列表测试例子](https://www.cnblogs.com/54zorb/p/9279805.html)

[3、状态机测试例子](https://www.cnblogs.com/54zorb/p/9285805.html)

[4、事件测试例子](https://www.cnblogs.com/54zorb/p/9325298.html)

[5、定时器测试例子](https://www.cnblogs.com/54zorb/p/9325510.html)

[6、任务测试例子](https://www.cnblogs.com/54zorb/p/9337754.html)

