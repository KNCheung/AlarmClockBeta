/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JJL
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL VARIABLES
*********************************************************************************************************
*/
static    OS_TCB       AppTaskStart_TCB;
static    OS_TCB       AppTaskREG_TCB;
static    OS_TCB       AppTaskTemp_TCB;
static    OS_TCB       AppTaskTime_TCB;
static    OS_TCB       AppTaskIR_TCB;

static    CPU_STK      AppTaskStart_Stk[APP_TASK_START_STK_SIZE];
static    CPU_STK      AppTaskREG_Stk[APP_TASK_REG_STK_SIZE];
static    CPU_STK      AppTaskTemp_Stk[APP_TASK_TEMP_STK_SIZE];
static    CPU_STK      AppTaskTime_Stk[APP_TASK_TIME_STK_SIZE];
static    CPU_STK      AppTaskIR_Stk[APP_TASK_IR_STK_SIZE];

static    CPU_INT32U   AppCPU_ClkFreq_Hz;

          OS_SEM       RegUpdateAllowed;
          OS_SEM       SemREG;
          
          uint8_t      FlagIR;





/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void         AppTaskStart        (void *p_arg);
static  void         AppTaskREG          (void *p_arg);
static  void         AppTaskTemp         (void *p_arg);
static  void         AppTaskTime         (void *p_arg);
static  void         AppTaskIR           (void *p_arg);

static  void         AppTaskCreate      (void);
static  void         AppObjCreate       (void);




/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
int  LoadSystem(void);

int  LoadSystem (void)
{
    OS_ERR  err;


    BSP_IntDisAll();                                            /* Disable all interrupts.                              */

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStart_TCB,               /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR )AppTaskStart,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStart_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart_()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{  
    CPU_INT32U  cnts;
    OS_ERR      err;
    
    

   (void)p_arg;

    BSP_Init();                                                       /* Initialize BSP functions                          */
    CPU_Init();                                                       /* Initialize the uC/CPU services                    */
    
    AppCPU_ClkFreq_Hz = BSP_CPU_ClkFreq();                            /* Determine SysTick reference freq.                 */                                                                        
    cnts          = AppCPU_ClkFreq_Hz / (CPU_INT32U)OSCfg_TickRate_Hz;/* Determine nbr SysTick increments                  */
    OS_CPU_SysTickInit(cnts);                                         /* Init uC/OS periodic time src (SysTick).           */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                                     /* Compute CPU capacity with no task running         */
#endif

#ifdef  CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    AppObjCreate();                                                   /* Create kernel objects (semaphore, queue, etc.)    */
    AppTaskCreate();                                                  /* Create application tasks                          */

    OSTaskDel(&AppTaskStart_TCB,&err);

    while (DEF_TRUE) {                                                /* Task body, always written as an infinite loop.    */
      OSTimeDlyHMSM(10,59,59,59,OS_OPT_TIME_HMSM_STRICT,&err);
    }
}


/*
*********************************************************************************************************
*                                        CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    OS_ERR  err;

    OSTaskCreate((OS_TCB     *)&AppTaskREG_TCB,
                 (CPU_CHAR   *)"REG Task",
                 (OS_TASK_PTR )AppTaskREG,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_REG_PRIO,
                 (CPU_STK    *)&AppTaskREG_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_REG_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_REG_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
                 /*
    OSTaskCreate((OS_TCB     *)&AppTaskTemp_TCB,
                 (CPU_CHAR   *)"Temperature Task",
                 (OS_TASK_PTR )AppTaskTemp,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_TEMP_PRIO,
                 (CPU_STK    *)&AppTaskTemp_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_TEMP_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_TEMP_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err); 
                 */
    OSTaskCreate((OS_TCB     *)&AppTaskTime_TCB,
                 (CPU_CHAR   *)"Clock Task",
                 (OS_TASK_PTR )AppTaskTime,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_TIME_PRIO,
                 (CPU_STK    *)&AppTaskTime_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_TIME_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_TIME_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
                 /*
    OSTaskCreate((OS_TCB     *)&AppTaskIR_TCB,
                 (CPU_CHAR   *)"IR Task",
                 (OS_TASK_PTR )AppTaskIR,
                 (void       *)0,
                 (OS_PRIO     )APP_TASK_IR_PRIO,
                 (CPU_STK    *)&AppTaskIR_Stk[0],
                 (CPU_STK_SIZE)APP_TASK_IR_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_IR_STK_SIZE,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
                 */
}


/*
*********************************************************************************************************
*                                    CREATE APPLICATION KERNEL OBJECTS
*
* Description:  This function creates the application kernel objects such as semaphore, queue, etc.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
  OS_ERR err;
  OSSemCreate(&SemREG,"Sem of REG",1,&err);
}

static void AppTaskREG(void * p_arg)
{
  OS_ERR err;
  (void)p_arg;
  while (DEF_TRUE)
  {
    REG_Update();
    OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err);
  }
}

static void AppTaskTemp(void *p_arg)
{
  OS_ERR err;
  CPU_TS ts;
  float temp;
  (void)p_arg;
  
  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_NON_STRICT,&err);
  DS18B20_StartConvTemp();
  OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
  temp=DS18B20_ReadTemp();
  OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err);
  while (DEF_TRUE)
  {
    OSTimeDlyHMSM(1,1,1,1,OS_OPT_TIME_HMSM_STRICT,&err);
 //   REG_Set(0x27,0xBF,0xBF,0xBF,0,1);
    DS18B20_StartConvTemp();
    OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    
    OSSemPend(&SemREG,0,OS_OPT_PEND_BLOCKING,&ts,&err);
    temp=DS18B20_ReadTemp();
    REG_Set(0x27,REG_HexToReg(((uint8_t)(temp*10))%10),REG_HexToReg((uint8_t)temp%10),REG_HexToReg((uint8_t)temp/10%10),0,1);
    OSTimeDlyHMSM(0,0,5,0,OS_OPT_TIME_HMSM_STRICT,&err);
    OSSemPost(&SemREG,OS_OPT_POST_1,&err);
  }
} 

static void AppTaskTime(void * p_arg)
{
  OS_ERR err;
  CPU_TS ts;
  static uint8_t h,m,s=59;
  CPU_SR_ALLOC();
  
  (void)p_arg;
  while (DEF_TRUE)
  {
    OSSemPend(&SemREG,0,OS_OPT_PEND_BLOCKING,&ts,&err);
    OS_CRITICAL_ENTER();
    h=IIC_Read(0xD0,0x02);
    m=IIC_Read(0xD0,0x01);
    s=IIC_Read(0xD0,0x00);
    s=(s>>4)*10+s%16;
    REG_Set(REG_HexToReg(m%16),REG_HexToReg(m>>4),REG_HexToReg(h%16),REG_HexToReg(h>>4),1,0);
    OS_CRITICAL_EXIT();
    OSSemPost(&SemREG,OS_OPT_POST_1,&err);
    OSTimeDlyHMSM(0,0,(((59-s)>0)?59-s:1),0,OS_OPT_TIME_HMSM_NON_STRICT,&err);
  }
}

extern unsigned long IRCode;
static void AppTaskIR(void* p_arg)
{
  OS_ERR err;
  CPU_TS ts;   

  while (DEF_TRUE)
  {
    while (FlagIR==0)
      OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_NON_STRICT,&err);
    FlagIR=0;
    OSSemPend(&SemREG,0,OS_OPT_PEND_BLOCKING,&ts,&err);
    REG_Set(REG_HexToReg(IRCode%16),REG_HexToReg((IRCode>>4)%16),REG_HexToReg((IRCode>>16)%16),REG_HexToReg((IRCode>>20)%16),0,0);
    OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_NON_STRICT,&err);
    OSSemPost(&SemREG,OS_OPT_POST_1,&err);
  }
}
