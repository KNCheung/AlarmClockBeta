;******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
;* File Name          : startup_stm32f10x_hd.s
;* Author             : MCD Application Team
;* Version            : V3.4.0
;* Date               : 10/15/2010
;* Description        : STM32F10x High Density Devices vector table for MDK-ARM 
;*                      toolchain. 
;*                      This module performs:
;*                      - Set the initial SP
;*                      - Set the initial PC == Reset_Handler
;*                      - Set the vector table entries with the exceptions ISR address
;*                      - Configure the clock system and also configure the external 
;*                        SRAM mounted on STM3210E-EVAL board to be used as data 
;*                        memory (optional, to be enabled by user)
;*                      - Branches to __main in the C library (which eventually
;*                        calls main()).
;*                      After Reset the CortexM3 processor is in Thread mode,
;*                      priority is Privileged, and the Stack is set to Main.
;* <<< Use Configuration Wizard in Context Menu >>>   
;*******************************************************************************
; THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
; WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
; AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
; INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
; CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
; INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
;*******************************************************************************

; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp
                                                  
; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB


			    IMPORT	OS_CPU_SysTickHandler
		        IMPORT	OS_CPU_PendSVHandler
; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp               ; Top of Stack
                DCD     Reset_Handler              ; Reset Handler
                DCD     App_NMI_ISR                ; NMI Handler
                DCD     App_Fault_ISR          ; Hard Fault Handler
                DCD     App_MemFault_ISR          ; MPU Fault Handler
                DCD     App_BusFault_ISR           ; Bus Fault Handler
                DCD     App_UsageFault_ISR         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     BSP_IntHandlerSVC                ; SVCall Handler
                DCD     BSP_IntHandlerDebugMon           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     OS_CPU_PendSVHandler       ; PendSV Handler
                DCD     OS_CPU_SysTickHandler      ; UCOS-II SysTick Handler

                ; External Interrupts
                DCD     BSP_IntHandlerWWDG            ; Window Watchdog
                DCD     BSP_IntHandlerPVD             ; PVD through EXTI Line detect
                DCD     BSP_IntHandlerTAMPER          ; Tamper
                DCD     BSP_IntHandlerRTC             ; RTC
                DCD     BSP_IntHandlerFLASH           ; Flash
                DCD     BSP_IntHandlerRCC             ; RCC
                DCD     BSP_IntHandlerEXTI0           ; EXTI Line 0
                DCD     BSP_IntHandlerEXTI1           ; EXTI Line 1
                DCD     BSP_IntHandlerEXTI2           ; EXTI Line 2
                DCD     BSP_IntHandlerEXTI3           ; EXTI Line 3
                DCD     BSP_IntHandlerEXTI4           ; EXTI Line 4
                DCD     BSP_IntHandlerDMA1_Channel1   ; DMA1 Channel 1
                DCD     BSP_IntHandlerDMA1_Channel2   ; DMA1 Channel 2
                DCD     BSP_IntHandlerDMA1_Channel3   ; DMA1 Channel 3
                DCD     BSP_IntHandlerDMA1_Channel4   ; DMA1 Channel 4
                DCD     BSP_IntHandlerDMA1_Channel5   ; DMA1 Channel 5
                DCD     BSP_IntHandlerDMA1_Channel6   ; DMA1 Channel 6
                DCD     BSP_IntHandlerDMA1_Channel7   ; DMA1 Channel 7
                DCD     BSP_IntHandlerADC1_2          ; ADC1 & ADC2
                DCD     BSP_IntHandlerUSB_HP_CAN1_TX  ; USB High Priority or CAN1 TX
                DCD     BSP_IntHandlerUSB_LP_CAN1_RX0 ; USB Low  Priority or CAN1 RX0
                DCD     BSP_IntHandlerCAN1_RX1        ; CAN1 RX1
                DCD     BSP_IntHandlerCAN1_SCE        ; CAN1 SCE
                DCD     BSP_IntHandlerEXTI9_5         ; EXTI Line 9..5
                DCD     BSP_IntHandlerTIM1_BRK        ; TIM1 Break
                DCD     BSP_IntHandlerTIM1_UP         ; TIM1 Update
                DCD     BSP_IntHandlerTIM1_TRG_COM    ; TIM1 Trigger and Commutation
                DCD     BSP_IntHandlerTIM1_CC         ; TIM1 Capture Compare
                DCD     BSP_IntHandlerTIM2            ; TIM2
                DCD     BSP_IntHandlerTIM3            ; TIM3
                DCD     BSP_IntHandlerTIM4            ; TIM4
                DCD     BSP_IntHandlerI2C1_EV         ; I2C1 Event
                DCD     BSP_IntHandlerI2C1_ER         ; I2C1 Error
                DCD     BSP_IntHandlerI2C2_EV         ; I2C2 Event
                DCD     BSP_IntHandlerI2C2_ER         ; I2C2 Error
                DCD     BSP_IntHandlerSPI1            ; SPI1
                DCD     BSP_IntHandlerSPI2            ; SPI2
                DCD     BSP_IntHandlerUSART1          ; USART1
                DCD     BSP_IntHandlerUSART2          ; USART2
                DCD     BSP_IntHandlerUSART3          ; USART3
                DCD     BSP_IntHandlerEXTI15_10       ; EXTI Line 15..10
                DCD     BSP_IntHandlerRTCAlarm        ; RTC Alarm through EXTI Line
                DCD     BSP_IntHandlerUSBWakeUp       ; USB Wakeup from suspend
                DCD     BSP_IntHandlerTIM8_BRK        ; TIM8 Break
                DCD     BSP_IntHandlerTIM8_UP         ; TIM8 Update
                DCD     BSP_IntHandlerTIM8_TRG_COM    ; TIM8 Trigger and Commutation
                DCD     BSP_IntHandlerTIM8_CC         ; TIM8 Capture Compare
                DCD     BSP_IntHandlerADC3            ; ADC3
                DCD     BSP_IntHandlerFSMC            ; FSMC
                DCD     BSP_IntHandlerSDIO            ; SDIO
                DCD     BSP_IntHandlerTIM5            ; TIM5
                DCD     BSP_IntHandlerSPI3            ; SPI3
                DCD     BSP_IntHandlerUART4           ; UART4
                DCD     BSP_IntHandlerUART5           ; UART5
                DCD     BSP_IntHandlerTIM6            ; TIM6
                DCD     BSP_IntHandlerTIM7            ; TIM7
                DCD     BSP_IntHandlerDMA2_Channel1   ; DMA2 Channel1
                DCD     BSP_IntHandlerDMA2_Channel2   ; DMA2 Channel2
                DCD     BSP_IntHandlerDMA2_Channel3   ; DMA2 Channel3
                DCD     BSP_IntHandlerDMA2_Channel4_5 ; DMA2 Channel4 & Channel5
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY
                
; Reset handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  LoadSystem
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0               
                LDR     R0, =LoadSystem
                BLX     R0
                ENDP
                
; Dummy Exception Handlers (infinite loops which can be modified)

App_NMI_ISR     PROC
                EXPORT  App_NMI_ISR                [WEAK]
                B       .
                ENDP
App_Fault_ISR\
                PROC
                EXPORT  App_Fault_ISR              [WEAK]
                B       .
                ENDP
App_MemFault_ISR\
                PROC
                EXPORT  App_MemFault_ISR           [WEAK]
                B       .
                ENDP
App_BusFault_ISR\
                PROC
                EXPORT  App_BusFault_ISR           [WEAK]
                B       .
                ENDP
App_UsageFault_ISR\
                PROC
                EXPORT  App_UsageFault_ISR         [WEAK]
                B       .
                ENDP
BSP_IntHandlerSVC     PROC
                EXPORT  BSP_IntHandlerSVC                [WEAK]
                B       .
                ENDP
BSP_IntHandlerDebugMon\
                PROC
                EXPORT  BSP_IntHandlerDebugMon           [WEAK]
                B       .
                ENDP
BSP_IntHandlerPendSV  PROC
                EXPORT  BSP_IntHandlerPendSV             [WEAK]
                B       .
                ENDP
BSP_IntHandlerSysTick PROC
                EXPORT  BSP_IntHandlerSysTick            [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  BSP_IntHandlerWWDG            [WEAK]
                EXPORT  BSP_IntHandlerPVD             [WEAK]
                EXPORT  BSP_IntHandlerTAMPER          [WEAK]
                EXPORT  BSP_IntHandlerRTC             [WEAK]
                EXPORT  BSP_IntHandlerFLASH           [WEAK]
                EXPORT  BSP_IntHandlerRCC             [WEAK]
                EXPORT  BSP_IntHandlerEXTI0           [WEAK]
                EXPORT  BSP_IntHandlerEXTI1           [WEAK]
                EXPORT  BSP_IntHandlerEXTI2           [WEAK]
                EXPORT  BSP_IntHandlerEXTI3           [WEAK]
                EXPORT  BSP_IntHandlerEXTI4           [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel1   [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel2   [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel3   [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel4   [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel5   [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel6   [WEAK]
                EXPORT  BSP_IntHandlerDMA1_Channel7   [WEAK]
                EXPORT  BSP_IntHandlerADC1_2          [WEAK]
                EXPORT  BSP_IntHandlerUSB_HP_CAN1_TX  [WEAK]
                EXPORT  BSP_IntHandlerUSB_LP_CAN1_RX0 [WEAK]
                EXPORT  BSP_IntHandlerCAN1_RX1        [WEAK]
                EXPORT  BSP_IntHandlerCAN1_SCE        [WEAK]
                EXPORT  BSP_IntHandlerEXTI9_5         [WEAK]
                EXPORT  BSP_IntHandlerTIM1_BRK        [WEAK]
                EXPORT  BSP_IntHandlerTIM1_UP         [WEAK]
                EXPORT  BSP_IntHandlerTIM1_TRG_COM    [WEAK]
                EXPORT  BSP_IntHandlerTIM1_CC         [WEAK]
                EXPORT  BSP_IntHandlerTIM2            [WEAK]
                EXPORT  BSP_IntHandlerTIM3            [WEAK]
                EXPORT  BSP_IntHandlerTIM4            [WEAK]
                EXPORT  BSP_IntHandlerI2C1_EV         [WEAK]
                EXPORT  BSP_IntHandlerI2C1_ER         [WEAK]
                EXPORT  BSP_IntHandlerI2C2_EV         [WEAK]
                EXPORT  BSP_IntHandlerI2C2_ER         [WEAK]
                EXPORT  BSP_IntHandlerSPI1            [WEAK]
                EXPORT  BSP_IntHandlerSPI2            [WEAK]
                EXPORT  BSP_IntHandlerUSART1          [WEAK]
                EXPORT  BSP_IntHandlerUSART2          [WEAK]
                EXPORT  BSP_IntHandlerUSART3          [WEAK]
                EXPORT  BSP_IntHandlerEXTI15_10       [WEAK]
                EXPORT  BSP_IntHandlerRTCAlarm        [WEAK]
                EXPORT  BSP_IntHandlerUSBWakeUp       [WEAK]
                EXPORT  BSP_IntHandlerTIM8_BRK        [WEAK]
                EXPORT  BSP_IntHandlerTIM8_UP         [WEAK]
                EXPORT  BSP_IntHandlerTIM8_TRG_COM    [WEAK]
                EXPORT  BSP_IntHandlerTIM8_CC         [WEAK]
                EXPORT  BSP_IntHandlerADC3            [WEAK]
                EXPORT  BSP_IntHandlerFSMC            [WEAK]
                EXPORT  BSP_IntHandlerSDIO            [WEAK]
                EXPORT  BSP_IntHandlerTIM5            [WEAK]
                EXPORT  BSP_IntHandlerSPI3            [WEAK]
                EXPORT  BSP_IntHandlerUART4           [WEAK]
                EXPORT  BSP_IntHandlerUART5           [WEAK]
                EXPORT  BSP_IntHandlerTIM6            [WEAK]
                EXPORT  BSP_IntHandlerTIM7            [WEAK]
                EXPORT  BSP_IntHandlerDMA2_Channel1   [WEAK]
                EXPORT  BSP_IntHandlerDMA2_Channel2   [WEAK]
                EXPORT  BSP_IntHandlerDMA2_Channel3   [WEAK]
                EXPORT  BSP_IntHandlerDMA2_Channel4_5 [WEAK]

BSP_IntHandlerWWDG
BSP_IntHandlerPVD
BSP_IntHandlerTAMPER
BSP_IntHandlerRTC
BSP_IntHandlerFLASH
BSP_IntHandlerRCC
BSP_IntHandlerEXTI0
BSP_IntHandlerEXTI1
BSP_IntHandlerEXTI2
BSP_IntHandlerEXTI3
BSP_IntHandlerEXTI4
BSP_IntHandlerDMA1_Channel1
BSP_IntHandlerDMA1_Channel2
BSP_IntHandlerDMA1_Channel3
BSP_IntHandlerDMA1_Channel4
BSP_IntHandlerDMA1_Channel5
BSP_IntHandlerDMA1_Channel6
BSP_IntHandlerDMA1_Channel7
BSP_IntHandlerADC1_2
BSP_IntHandlerUSB_HP_CAN1_TX
BSP_IntHandlerUSB_LP_CAN1_RX0
BSP_IntHandlerCAN1_RX1
BSP_IntHandlerCAN1_SCE
BSP_IntHandlerEXTI9_5
BSP_IntHandlerTIM1_BRK
BSP_IntHandlerTIM1_UP
BSP_IntHandlerTIM1_TRG_COM
BSP_IntHandlerTIM1_CC
BSP_IntHandlerTIM2
BSP_IntHandlerTIM3
BSP_IntHandlerTIM4
BSP_IntHandlerI2C1_EV
BSP_IntHandlerI2C1_ER
BSP_IntHandlerI2C2_EV
BSP_IntHandlerI2C2_ER
BSP_IntHandlerSPI1
BSP_IntHandlerSPI2
BSP_IntHandlerUSART1
BSP_IntHandlerUSART2
BSP_IntHandlerUSART3
BSP_IntHandlerEXTI15_10
BSP_IntHandlerRTCAlarm
BSP_IntHandlerUSBWakeUp
BSP_IntHandlerTIM8_BRK
BSP_IntHandlerTIM8_UP
BSP_IntHandlerTIM8_TRG_COM
BSP_IntHandlerTIM8_CC
BSP_IntHandlerADC3
BSP_IntHandlerFSMC
BSP_IntHandlerSDIO
BSP_IntHandlerTIM5
BSP_IntHandlerSPI3
BSP_IntHandlerUART4
BSP_IntHandlerUART5
BSP_IntHandlerTIM6
BSP_IntHandlerTIM7
BSP_IntHandlerDMA2_Channel1
BSP_IntHandlerDMA2_Channel2
BSP_IntHandlerDMA2_Channel3
BSP_IntHandlerDMA2_Channel4_5
                B       .

                ENDP

                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB
                
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
                
                 ELSE
                
                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap
                 
__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END

;******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE*****
