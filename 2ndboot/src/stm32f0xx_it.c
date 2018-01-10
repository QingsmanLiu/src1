#include "stm32f0xx_it.h"
#include <appl.h>


#define JUMP_TO_APP_ISR(off)      ((JUMP_FUNC)(*((U32 *)(APPL_START_ADDR + off))))();

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void)
{
  // 00000008
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0008);
  }
}


void HardFault_Handler(void)
{
  // 0000000C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x000C);
  }
}


void SVC_Handler(void)
{
  //0000002C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x002C);
  }
}


void PendSV_Handler(void)
{
  // 00000038
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0038);
  }
}


void SysTick_Handler( void )  // TIMER
{
  // 0000003C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x003C);
  }
}


//-----------------------------------------------------------------------------
// IRQ Handler
//-----------------------------------------------------------------------------
void WWDG_IRQHandler(void)
{
  // 00000040
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0040);
  }
}


void PVD_VDDIO2_IRQHandler(void)
{
  // 00000044
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0044);
  }
}


void RTC_IRQHandler(void)
{
  // 00000048
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0048);
  }
}


void FLASH_IRQHandler(void)
{
  // 0000004C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x004C);
  }
}


void RCC_CRS_IRQHandler(void)
{
  // 00000050
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0050);
  }
}


void EXTI0_1_IRQHandler(void)     // EXTI0 -- ACC
{
  // 00000054
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0054);
  }
}


void EXTI2_3_IRQHandler(void)
{
  // 00000058
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0058);
  }
}


void EXTI4_15_IRQHandler(void)    // EXTI13 -- IR
{
  // 0000005C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x005C);
  }
}


void TSC_IRQHandler(void)
{
  // 00000060
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0060);
  }
}


void DMA1_Channel1_IRQHandler(void)
{
  // 00000064
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0064);
  }
}


void DMA1_Channel2_3_IRQHandler(void)
{
  // 00000068
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0068);
  }
}


void DMA1_Channel4_5_6_7_IRQHandler(void)
{
  // 0000006C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x006C);
  }
}


void ADC1_COMP_IRQHandler(void)   // ADC
{
  // 00000070
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0070);
  }
}


void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
  // 00000074
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0074);
  }
}


void TIM1_CC_IRQHandler(void)
{
  // 00000078
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0078);
  }
}


void TIM2_IRQHandler(void)       // Beep
{
  // 0000007C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x007C);
  }
}


void TIM3_IRQHandler(void)       // LCD PWM
{
  // 00000080
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0080);
  }
}


void TIM6_DAC_IRQHandler(void)
{
  // 00000084
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0084);
  }
}


void TIM7_IRQHandler(void)
{
  // 00000088
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0088);
  }
}


void TIM14_IRQHandler(void)
{
  // 0000008C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x008C);
  }
}

 
void TIM15_IRQHandler(void)
{
  // 00000090
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0090);
  }
}


void TIM16_IRQHandler(void)
{
  // 00000094
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0094);
  }
}


void TIM17_IRQHandler(void)
{
  // 00000098
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x0098);
  }
}


void I2C1_IRQHandler(void)
{
  // 0000009C
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x009C);
  }
}


void I2C2_IRQHandler(void)
{
  // 000000A0
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00A0);
  }
}


void SPI1_IRQHandler(void)
{
  // 000000A4
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00A4);
  }
}


void SPI2_IRQHandler(void)
{
  // 000000A8
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00A8);
  }
}


void USART1_IRQHandler(void)    // UART1
{
  // 000000AC
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00AC);
  }
}


void USART2_IRQHandler(void)    // UART2
{
  // 000000B0
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00B0);
  }
}


void USART3_4_IRQHandler(void)  // UART3, UART4
{
  // 000000B4
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00B4);
  }
}


void CEC_CAN_IRQHandler(void)
{
  // 000000B8
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00B8);
  }
}


void USB_IRQHandler(void)
{
  // 000000BC
  if( IS_APP_MODE() )
  {
    JUMP_TO_APP_ISR(0x00BC);
  }
}
 

/*
void PPP_IRQHandler(void)       // Not used
{
}
*/

