/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/
 
#include <board.h>
#include <rtthread.h>

#include <REG.h>
#include <IIC.h>
#include <IR.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

ALIGN(RT_ALIGN_SIZE)

static rt_uint8_t reg_stack[256];
static struct rt_thread reg_thread;
void rt_thread_reg_entry(void* parameter);
#define PRIO_REG RT_THREAD_PRIORITY_MAX-3
#define EVENT_REG (1<<0)

static rt_uint8_t clock_stack[256];
static struct rt_thread clock_thread;
void rt_thread_clock_entry(void* parameter);
rt_timer_t update_clock_timer;
#define PRIO_CLOCK RT_THREAD_PRIORITY_MAX-5
#define EVENT_CLOCK (1<<2)
static rt_uint8_t ir_stack[256];
static struct rt_thread ir_thread;
void rt_thread_ir_entry(void* parameter);
#define PRIO_IR 1

static rt_uint8_t temp_clock_stack[256];
static struct rt_thread temp_clock_thread;
void rt_thread_temp_clock_entry(void* parameter);
#define PRIO_TEMP_CLOCK RT_THREAD_PRIORITY_MAX-5
#define EVENT_TEMP_CLOCK (1<<8)


static rt_uint8_t temp_stack[256];
static struct rt_thread temp_thread;
void rt_thread_temp_entry(void* parameter);
#define PRIO_TEMP 6
#define EVENT_TEMP (1<<3)

static rt_uint8_t date_stack[256];
static struct rt_thread date_thread;
void rt_thread_date_entry(void* parameter);
#define PRIO_DATE 7
#define EVENT_DATE (1<<4)

static rt_uint8_t pomodoro_stack[256];
static struct rt_thread pomodoro_thread;
void rt_thread_pomodoro_entry(void* parameter);
#define PRIO_POMODORO 3
#define EVENT_POMODORO (1<<5)
#define EVENT_POMODORO_BREAK (1<<6)
#define EVENT_POMODORO_REST (1<<7)
#define POMODORO_TIME 1
#define POMODORO_BREAK_TIME 2
#define POMODORO_REST_TIME 3

#define EVENT_PRESS (1<<1)
#define WAIT_FOR_RELEASE while(rt_event_recv(en_event,EVENT_PRESS,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_TICK_PER_SECOND/8,&e)==RT_EOK)

rt_mq_t ir_mq = RT_NULL;
rt_mutex_t reg_mutex = RT_NULL;
rt_event_t en_event = RT_NULL;

void reboot(void);
static void update_timeout(void* parameter);
void ToggleREG(void);
#define back_to_time_display() rt_event_send(en_event,EVENT_CLOCK)
void lp(void);
void fnDebug(void);

#define VAILDMASK 0
#define SETTING_LENGTH 16
#define SETTING_BASE 0x0000
uint8_t Setting[SETTING_LENGTH];
void Setting_Read(void);
void Setting_Write(void);

#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */


void rt_init_thread_entry(void* parameter)
{
    rt_err_t result;

#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

#ifdef  RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif  /* RT_USING_FINSH */

    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
        rt_kprintf("File System initialzation failed!\n");
#endif  /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    {
        extern void rt_hw_lcd_init();
        extern void rtgui_touch_hw_init(void);

        rt_device_t lcd;

        /* init lcd */
        rt_hw_lcd_init();

        /* init touch panel */
        rtgui_touch_hw_init();

        /* find lcd device */
        lcd = rt_device_find("lcd");

        /* set lcd device as rtgui graphic driver */
        rtgui_graphic_set_device(lcd);

#ifndef RT_USING_COMPONENTS_INIT
        /* init rtgui system server */
        rtgui_system_server_init();
#endif

        calibration_set_restore(cali_setup);
        calibration_set_after(cali_store);
        calibration_init();
    }
#endif /* #ifdef RT_USING_RTGUI */
    
    result = rt_thread_init(&reg_thread,
                            "REG",
                            rt_thread_reg_entry,
                            RT_NULL,
                            (rt_uint8_t*)reg_stack,
                            sizeof(reg_stack),
                            PRIO_REG,
                            2);
    if (result == RT_EOK)
      rt_thread_startup(&reg_thread);
    
    IIC_Init();
    do
    {
      Setting_Read();
      rt_thread_delay(RT_TICK_PER_SECOND/100);
    }while(Setting[0]!=42);

    result = rt_thread_init(&clock_thread,
                            "Clock",
                            rt_thread_clock_entry,
                            RT_NULL,
                            (rt_uint8_t*)clock_stack,
                            sizeof(clock_stack),
                            PRIO_CLOCK,
                            2);

    if (result == RT_EOK)
      rt_thread_startup(&clock_thread);
    
        result = rt_thread_init(&temp_clock_thread,
                            "TemClock",
                            rt_thread_temp_clock_entry,
                            RT_NULL,
                            (rt_uint8_t*)temp_clock_stack,
                            sizeof(temp_clock_stack),
                            PRIO_TEMP_CLOCK,
                            10);
    if (result == RT_EOK)
      rt_thread_startup(&temp_clock_thread);

    result = rt_thread_init(&ir_thread,
                            "IR",
                            rt_thread_ir_entry,
                            RT_NULL,
                            (rt_uint8_t*)ir_stack,
                            sizeof(ir_stack),
                            PRIO_IR,
                            5);
    if (result == RT_EOK)
      rt_thread_startup(&ir_thread);
    
    result = rt_thread_init(&temp_thread,
                            "Temp",
                            rt_thread_temp_entry,
                            RT_NULL,
                            (rt_uint8_t*)temp_stack,
                            sizeof(temp_stack),
                            PRIO_TEMP,
                            5);
    if (result == RT_EOK)
      rt_thread_startup(&temp_thread);
    
    result = rt_thread_init(&date_thread,
                            "Date",
                            rt_thread_date_entry,
                            RT_NULL,
                            (rt_uint8_t*)date_stack,
                            sizeof(date_stack),
                            PRIO_DATE,
                            5);
    if (result == RT_EOK)
      rt_thread_startup(&date_thread);
    
    result = rt_thread_init(&pomodoro_thread,
                            "Pomodoro",
                            rt_thread_pomodoro_entry,
                            RT_NULL,
                            (rt_uint8_t*)pomodoro_stack,
                            sizeof(pomodoro_stack),
                            PRIO_POMODORO,
                            5);
    if (result == RT_EOK)
      rt_thread_startup(&pomodoro_thread);
    

}

int rt_application_init(void)
{
    rt_thread_t init_thread;

    en_event = rt_event_create("Enable",RT_IPC_FLAG_FIFO);
  
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, RT_THREAD_PRIORITY_MAX-2, 1);

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/

void reboot(void)
{
  rt_kprintf("=====REBOOTED=====");
  rt_thread_delay(10);  
  NVIC_SystemReset();
  return;
}
FINSH_FUNCTION_EXPORT(reboot,Reboot the MCU)

void rt_thread_reg_entry(void* parameter)
{
  rt_uint32_t e;
  REG_Init();
  REG_On();
  rt_event_send(en_event,EVENT_REG);
  reg_mutex = rt_mutex_create("REG",RT_IPC_FLAG_FIFO);
  rt_mutex_release(reg_mutex);
  while (1)
  {
    rt_event_recv(en_event,EVENT_REG,RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&e);
    rt_thread_delay(1);
    REG_Update();
  }
}

void rt_thread_temp_clock_entry(void* parameter)
{
  uint8_t h,m,s;
  uint32_t set;
  rt_uint8_t REG_TEMP[4];
  extern rt_uint8_t REG_out[4];
  rt_uint32_t e;
  while (1)
  {
    rt_event_recv(en_event,EVENT_TEMP_CLOCK,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_enter_critical();
    h=IIC_Read(DS3231_ADDRESS,DS3231_HOUR);
    m=IIC_Read(DS3231_ADDRESS,DS3231_MINUTE);
    s=IIC_Read(DS3231_ADDRESS,DS3231_SECOND);
    rt_exit_critical();
    REG_TEMP[0]=REG_out[0];REG_TEMP[1]=REG_out[1];REG_TEMP[2]=REG_out[2];REG_TEMP[3]=REG_out[3];
    s=(s>>4)*10+s%16;
    set = RT_TICK_PER_SECOND * ((59==s)?1:59-s);
    REG_Set(REG_HexToReg(m%16),REG_HexToReg(m>>4),REG_HexToReg(h%16),REG_HexToReg(h>>4),1,0);
    rt_timer_control(update_clock_timer,RT_TIMER_CTRL_SET_TIME,&set);
    rt_timer_start(update_clock_timer);
    WAIT_FOR_RELEASE;
    REG_out[0]=REG_TEMP[0];REG_out[1]=REG_TEMP[1];REG_out[2]=REG_TEMP[2];REG_out[3]=REG_TEMP[3];
  }
}

void rt_thread_clock_entry(void* parameter)
{
  uint8_t h,m,s;
  uint32_t set;
  rt_uint32_t e;
  IIC_Init();
  update_clock_timer = rt_timer_create("Update", \
                                       update_timeout, \
                                       RT_NULL, \
                                       1, \
                                       RT_TIMER_FLAG_ACTIVATED | RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_HARD_TIMER);
  rt_timer_start(update_clock_timer);
  while (1)
  {
    rt_event_recv(en_event,EVENT_CLOCK,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_event_recv(en_event,EVENT_REG,RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&e);
    rt_mutex_take(reg_mutex,RT_WAITING_FOREVER);
    rt_enter_critical();
    h=IIC_Read(DS3231_ADDRESS,DS3231_HOUR);
    m=IIC_Read(DS3231_ADDRESS,DS3231_MINUTE);
    s=IIC_Read(DS3231_ADDRESS,DS3231_SECOND);
    rt_exit_critical();
    s=(s>>4)*10+s%16;
    set = RT_TICK_PER_SECOND * ((59==s)?1:59-s);
    REG_Set(REG_HexToReg(m%16),REG_HexToReg(m>>4),REG_HexToReg(h%16),REG_HexToReg(h>>4),1,0);
    rt_timer_control(update_clock_timer,RT_TIMER_CTRL_SET_TIME,&set);
    rt_timer_start(update_clock_timer);
    rt_mutex_release(reg_mutex);
  }
}

static void update_timeout(void* parameter)
{
  rt_event_send(en_event,EVENT_CLOCK);
  return;
}

void rt_thread_ir_entry(void* parameter)
{
  uint16_t TS;
  uint8_t state = 0;
  uint32_t IRCode;
  uint8_t RFlag;
  IR_Init();
  IR_Enable();
  ir_mq = rt_mq_create("IR",sizeof(uint16_t),64,RT_IPC_FLAG_FIFO);
  while (1)
  {
    rt_mq_recv(ir_mq,&TS,sizeof(uint16_t),RT_WAITING_FOREVER);
    switch (state)
    {
      case 0:
        if ((1336<=TS)&&(TS<=1344))
        {
          state = 1;
          IRCode = 0;
          RFlag = 0;
        }
        else if (RFlag && (((4000<=TS)&&(TS<=4050))||((9550<=TS)&&(TS<=9620))))
          state = 100;
        else
        {
          state = 0;
          RFlag = 0;
        }
        break;
      case  1:case  2:case  3:case  4:case  5:case  6:case  7:case  8:case  9:case 10:
      case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:case 20:
      case 21:case 22:case 23:case 24:case 25:case 26:case 27:case 28:case 29:case 30:
      case 31:case 32:
        if ((100<=TS)&&(TS<=120))
          IRCode*=2;
        else if ((210<=TS)&&(TS<=235))
          IRCode=IRCode*2+1;
        else 
        {
          state = 0;
          break;
        }
        state++;
        if (state==33)
        {
          state=0;
          RFlag =1 ;
          switch ((uint8_t)((IRCode>>8)%256))
          { //DECODE
            // CH-
            case 0xA2: lp(); break;
            // CH
            case 0x62: break;
            // CH+
            case 0xE2: fnDebug(); break;
            // PREV
            case 0x22: break;
            // NEXT
            case 0x02: break;
            // PLAY/PAUSE
            case 0xC2: break;
            // VOL-
            case 0xE0: break;
            // VOL+
            case 0xA8: break;
            // PLAY/PAUSE
            case 0x90: break;
            // 0
            case 0x68: reboot(); break;
            // 100+
            case 0x98: ToggleREG(); break;
            // 200+
            case 0xB0: rt_event_send(en_event,EVENT_TEMP_CLOCK); break;
            // 1
            case 0x30: rt_event_send(en_event,EVENT_DATE);  break;
            // 2
            case 0x18: rt_event_send(en_event,EVENT_TEMP); break;
            // 3
            case 0x7A: break;
            // 4
            case 0x10: break;
            // 5
            case 0x38: break;
            // 6
            case 0x5A: break;
            // 7
            case 0x42: rt_event_send(en_event,EVENT_POMODORO); break;
            // 8
            case 0x4A: rt_event_send(en_event,EVENT_POMODORO_BREAK); break;
            // 9
            case 0x52: rt_event_send(en_event,EVENT_POMODORO_REST); break;
            // ERROR
            default: rt_kprintf("INVALID CODE : %x\n",(uint8_t)((IRCode>>8)%256));break;
          }
        }
        break;
      case 100:
        if ((1110<=TS)&&(TS<=1130))
        {
          rt_event_send(en_event,EVENT_PRESS);
          RFlag=1;
        }
        state = 0;
        break;
      default:
        state=0;  
    }
  }
}

uint8_t time(void)
{
  rt_enter_critical();
  rt_kprintf("         Date:\t20%x-%x-%x\n         Time:\t%x:%x:%x\n  Temperature:\t%d C\n",\
                          IIC_Read(DS3231_ADDRESS,DS3231_YEAR),IIC_Read(DS3231_ADDRESS,DS3231_MONTH)&0x7f,IIC_Read(DS3231_ADDRESS,DS3231_DAY),\
                          IIC_Read(DS3231_ADDRESS,DS3231_HOUR),IIC_Read(DS3231_ADDRESS,DS3231_MINUTE),IIC_Read(DS3231_ADDRESS,DS3231_SECOND),\
                          IIC_Read(DS3231_ADDRESS,DS3231_TEMP_MSB));
  rt_exit_critical();
  return 0;
}
FINSH_FUNCTION_EXPORT(time,Read DS3231)

void ToggleREG(void)
{
  rt_uint32_t e;
  if (REG_State())
  {
    REG_Off();
    rt_event_recv(en_event,EVENT_REG, RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,0,&e);
    rt_kprintf("\n  REG is Turned Off\n");
  }else{
    rt_event_send(en_event,EVENT_REG);
    REG_On();
    rt_kprintf("\n  REG is Turned On\n");
  }
}

void rt_thread_temp_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t temp;
  IIC_Init();
  while (1)
  {
    rt_event_recv(en_event,EVENT_TEMP,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_event_recv(en_event,EVENT_REG,RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&e);
    rt_mutex_take(reg_mutex,RT_WAITING_FOREVER);
    
    temp = IIC_Read(DS3231_ADDRESS,DS3231_TEMP_MSB);
    REG_Set(0x27,REG_HexToReg(temp%10),REG_HexToReg(temp/10),REG_HexToReg(0xff),1,1);
    
    WAIT_FOR_RELEASE;
    back_to_time_display();
    rt_mutex_release(reg_mutex);
  }
}

void rt_thread_date_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t m,d;
  IIC_Init();
  while(1){
    rt_event_recv(en_event,EVENT_DATE,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_event_recv(en_event,EVENT_REG,RT_EVENT_FLAG_AND,RT_WAITING_FOREVER,&e);
    rt_mutex_take(reg_mutex,RT_WAITING_FOREVER);
    
    m = IIC_Read(DS3231_ADDRESS,DS3231_MONTH);
    d = IIC_Read(DS3231_ADDRESS,DS3231_DAY);
    REG_Set(REG_HexToReg(d%16),REG_HexToReg(d/16),REG_HexToReg(m%16),REG_HexToReg((m/16)&0x7),1,1);
      
    WAIT_FOR_RELEASE;
    
    back_to_time_display();
    rt_mutex_release(reg_mutex);
  }
}

void rt_thread_pomodoro_entry(void* parameter)
{
  rt_uint32_t e;
  uint8_t cnt = 0;
  uint8_t state = 0;
  while (1)
  {
    rt_event_recv(en_event,EVENT_POMODORO,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER,&e);
    rt_mutex_take(reg_mutex,RT_WAITING_FOREVER);
    state = 0;
    do 
    {
      switch (state)
      {
        case 0: //Initialization for Working
          rt_event_recv(en_event,EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0,&e);
          cnt = Setting[POMODORO_TIME]+1;
          state++;
        case 1: //Working
          cnt--;
          REG_Set(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(cnt%10),REG_HexToReg(cnt/10),1,1);
          if (cnt==0) 
          {
            state++;
            rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
          }
          else
          {
            rt_thread_delay(RT_TICK_PER_SECOND*60);
            break;
          }
        case 2: //Waiting for command & Blinking
          if (rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR,RT_TICK_PER_SECOND*4,&e)==RT_EOK)
          {
            state++;
            break;
          }
          REG_Set(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),1,1);
          rt_thread_delay(RT_TICK_PER_SECOND*1);
          REG_Set(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(0),REG_HexToReg(0),1,1);
          break;
        case 3: //Check commands & Initialization for Break or Rest
          rt_event_recv(en_event,EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,0,&e);
          if (e & EVENT_POMODORO) break;
          else if (e & EVENT_POMODORO_BREAK)
            cnt = Setting[POMODORO_BREAK_TIME] + 1;
          else 
            cnt = Setting[POMODORO_REST_TIME] + 1;
          state++;
        case 4: //Break or Rest
          cnt--;
          REG_Set(REG_HexToReg(cnt%10),REG_HexToReg(cnt/10),REG_HexToReg(22),REG_HexToReg(22),1,1);
          if (cnt==0)
          {
            state++;
            rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
          }
          else 
          {
            rt_thread_delay(RT_TICK_PER_SECOND*60);
            break;
          }
        case 5: //Waiting for command & Blinking
          if (rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR,RT_TICK_PER_SECOND*1,&e)==RT_EOK)
          {
            state++;
            break;
          }
          REG_Set(REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),REG_HexToReg(22),1,1);
          rt_thread_delay(RT_TICK_PER_SECOND*1);
          REG_Set(REG_HexToReg(0),REG_HexToReg(0),REG_HexToReg(22),REG_HexToReg(22),1,1);
          break;
        case 6: //Check Commands
          if (e & EVENT_POMODORO) break;
          else state = 0;
        default: 
          break;
      }
    }while(rt_event_recv(en_event,EVENT_POMODORO,RT_EVENT_FLAG_AND,0,&e) != RT_EOK);
    rt_event_recv(en_event, EVENT_POMODORO|EVENT_POMODORO_BREAK|EVENT_POMODORO_REST,RT_EVENT_FLAG_OR|RT_EVENT_FLAG_CLEAR,0,&e);
    rt_mutex_release(reg_mutex);
    back_to_time_display();
  }
}

void pomodoro_set(uint8_t a,uint8_t b,uint8_t c)
{
  Setting[POMODORO_TIME] = a;
  Setting[POMODORO_BREAK_TIME] = b;
  Setting[POMODORO_REST_TIME] = c;
  Setting_Write();
  return;
}
FINSH_FUNCTION_EXPORT_ALIAS(pomodoro_set,pomodoro,Set Interval of Pomodoro Clock(Working,Break,Rest))

void lp(void)
{
  REG_Off();
  NVIC_SystemLPConfig(NVIC_LP_SEVONPEND,ENABLE);
  PWR_WakeUpPinCmd(ENABLE);
  PWR_EnterSTANDBYMode();
  return;
}
FINSH_FUNCTION_EXPORT(lp , Low Power Mode)

void fnDebug(void)
{
  return;
}
FINSH_FUNCTION_EXPORT_ALIAS(fnDebug,debugg,Debug Function)

void Setting_Read(void)
{
  IIC_ReadSeq(AT24C32_ADDRESS,SETTING_BASE,Setting,SETTING_LENGTH);
  return;
}

void Setting_Write(void)
{
  uint8_t t;
  while (1)
  {
    IIC_WriteSeq(AT24C32_ADDRESS,SETTING_BASE,Setting,SETTING_LENGTH);
    rt_thread_delay(10);
    IIC_ReadSeq(AT24C32_ADDRESS,SETTING_BASE+1,&t,1);
    if (t!=Setting[1])
      rt_thread_delay(10);
    else
      break;
  }
  return;
}

void Print_Setting(void)
{
  uint8_t i;
  for (i=0;i<SETTING_LENGTH;i++)
    rt_kprintf("%d ",Setting[i]);
  rt_kprintf("\n");
  return;
}
FINSH_FUNCTION_EXPORT_ALIAS(Print_Setting,setting,Print Setting Values)
