# 2012_Debugger

# OxFF 前言

- 功能介绍

![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608185769866-512de6c0-08cf-474f-beaa-522cb3bf2cdd.png)

# Ox00 基础功能

## Ox01 单步

```
- 入口点位置，测试单步步过断点
- call 0x4010ac（输出字符串"testStepIn"）
- call 0x40108f（输出字符串"testStepOut"）

g
p
p
g
```

## Ox02 硬件访问

```
- 硬件访问断点:（永久性断点会断下两次）
- 在 0x403000 上设置读取断点
- 在 0x403004 上设置写入断点

g
ba 403000
r
g
g

g
ba 403004
w
g
g
```

## Ox03 软件访问

```
- 内存访问断点:（永久性断点会断下两次）
- 在 0x403000 上设置读取断点
- 在 0x403004 上设置写入断点

g
bm 403000
r
g
g

g
bm 403004
w
g
g
```

## Ox04 软件执行

```
- 软件断点: 0040102F

g
bp 40102F
y
g
g
```

## Ox05 硬件执行

```
- 硬件执行断点: 00401029

g
ba 401029
e
g
g
```

## Ox06 内存执行

```
- 内存执行断点: 0040104F

g
ba 40104F
e
g
g
```

## Ox07 查看内存、寄存器、栈

```
g


u


d 4010CA
y


r


u
```

## Ox08 查看模块

```
lm
```

# Ox10 高级功能

## Ox11 条件断点

- 使用MFC界面添加计数条件断点

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608175527165-fe890855-8a3b-44f3-a4cb-39d92d365970.png)
  - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608175903703-6ca59f6b-8d71-4fbc-99ed-e9be66784fa5.png)
  - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608175929507-c4c2915f-afb5-4d0d-90c7-11634e6ef768.png)

- END

## Ox11 反反调试

- 使用插件注入

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183478470-6f8dc1fe-2ef3-4c0e-be94-8e848eea0ebf.png)
  - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183365936-05fef8f4-9b30-4c14-bb35-90a187383ba5.png)
  - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183497803-c7d04213-1f25-4db2-98fd-e3bba28d3201.png)

- END

## Ox11 插件功能

![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183403298-0e900170-342e-49d7-a9bb-77d26fbfc683.png)

# Ox20 附加功能

## Ox21 导入导出表

- 导入表

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183554602-477459ce-76c2-4ba8-8a0f-62a180952015.png)

- 导出表

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183571344-f7bd0a59-c095-462f-8265-36b34cd2ef17.png)

## Ox22 符号解析

- ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183644003-50255651-0f0d-4819-8cb0-93bad89c72a5.png)

## Ox23 源码调试

- 使用code加载.CPP源码

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183680313-ea2d757d-e654-4319-b03d-2ee600ee8959.png)

- 下断至main函数

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183755199-190b5078-ad5c-4513-ba6f-5f1832c1b2a6.png)

- 下断至cout

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608183879650-bf17d8f6-fd0b-4d59-83a7-82997c601f63.png)

- 对应响应断点

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608184083312-5f5b0dd9-c373-4ad1-824e-44dd7f0e64f2.png)

## Ox24 DUMP

- 使用dump命令导出文件

- - ![image.png](https://cdn.nlark.com/yuque/0/2020/png/2556867/1608184764193-86631dc2-0bb9-4784-894b-19fb37c44a1d.png)

## Ox25 其他

- API断点

------



# OxF0 后记

## OxF1 参考链接

> [[超级列表框\]启用Explorer主题_精易论坛](https://bbs.125.la/thread-14607694-1-1.html)
>
> [MFC中使用控制台窗口输入输出_Qsir的专栏-CSDN博客](https://blog.csdn.net/Qsir/article/details/78276411)