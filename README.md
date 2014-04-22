# Alarm Clock #
Just an alarm clock with assistant functions.
Base on [STM32F103R8T6](http://www.st.com/web/catalog/mmc/FM141/SC1169/SS1031/LN1565) and [RT-Thread](http://www.rt-thread.org/) 1.2.1

## To-do List ##
- 使能RTC外设
- 添加倒计时器
- 增加唤醒功能（三极管反相电路）
- 增加闹钟外设接口
- 将ToggleREG、休眠键、重启键合并
- 保存当前工作状态至EEPROM
- 给DS3231增加一个电容，减少冲击电流
- 增加唤醒屏幕功能

## 2014-04-21 ##
- 增加电源电压监测，防止过放，在finsh的voltage可以读取 (tag:v1.3.0)
- 增加finsh的修改时间函数
- 坑爹啊！！！PA6烧掉啦啦啦！！！在加入一个测电源电压的接口时……不小心把12V直接接到了PA6（红外）上了……还好只是烧了个端口~~~~>_<~~~~
- 红外移至PB0
- 端口说明更新到PCBPrj里

## 2014-04-20 ##
- 添加计时器
- 去除重新开屏时产生的闪屏bug
- 计时器加入复位前暂停显示

## 2014-04-19 ##
- 大幅度改动系统结构，使之能按优先级，在番茄钟临时查看时钟、日期等信息。
- 番茄钟计时算法修改，按当前
- 添加RTC任务：先同步DS3231上的时间，再根据SysTick修改秒数。
- 稍微调整红外功能键

## 2014-04-18 ##
首次将代码保存到GitCafe，已完成RT-Thread红外遥控、数码管、时间、日历、番茄钟、节电模式
