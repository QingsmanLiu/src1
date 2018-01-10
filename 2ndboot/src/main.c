#include "stm32f0xx.h"
#include <appl.h>
#include "frame.h"
#include "proc_playctrl.h"

//-----------------------------------------------------------------------------
// Options
//-----------------------------------------------------------------------------
#if 0 // def STD_IO_PORT
  #define DNLD_Printf(x)    printf x
#else
  #define DNLD_Printf(x)
#endif

#define ENABLE_UART_RX_TIMEOUT    1

#define STD_IO_BAUDRATE           115200

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#if (BOOT_POWER_ON_ALL)

  typedef enum
  {
    ARM_S_NOT_STARTED = 0,
    ARM_S_START_EXE   = 1,
    ARM_S_STARTED     = 2,

  } ARM_STATE;
  

  ARM_STATE gbArmBootStatus;
  U16       gwArmDelayCount;
  bool      gBootFlag;
  bool      gbDnldStarted;

  void  ARM_ProcessArmStart( void )
  {
    if( gwArmDelayCount )
      --gwArmDelayCount;

    switch( gbArmBootStatus )
    {
      case  ARM_S_NOT_STARTED :
            if( gwArmDelayCount==0 )
            {
              PWR_ArmStartPinLow();
              gbArmBootStatus = ARM_S_START_EXE;
              gwArmDelayCount = 35;
              //DNLD_Printf(("ArmStartPinLow()\r"));
            }
            break;

      case  ARM_S_START_EXE   :
            if( gwArmDelayCount==0 )
            {
              PWR_ArmStartPinHigh();
              gbArmBootStatus = ARM_S_STARTED;
              //DNLD_Printf(("ArmStartPinHigh()\r"));
            }
            break;

      case  ARM_S_STARTED    :
      default                :
            break;
    }
  }

#endif


void  COM_OnExtCommand( int c, U8 *pbRxBuf )
{
  // 为支持MCU升级失败后，还可以继续升级。
  static U8   gCmdReceived = FALSE;
  static U8   gExtCmdCnt = 0;
  
  if( gCmdReceived )
    return;

  if( c==0x55 )
  {
    if( gExtCmdCnt==8 )
    {
    /*
              55 28 00 02 0A 01 01 C9
      [<-ARM] 55 20 00 02 0A 01 01 D1 55    SetUpdateMode( MCU, 1 )
      [->ARM] 55 27 00 01 11 05 C1 55       Report LinkState( 5 )
    */
      if(  pbRxBuf[0]==0x55 &&
           (pbRxBuf[1]&0xF0)==CTRL_DATA_REQ &&
           pbRxBuf[2]==0 &&                 // APP_SYS=0
           pbRxBuf[3]==2 &&                 // len = 2
           pbRxBuf[4]==REQ_UPDATE_MODE &&   // cmd = 0x0A
           pbRxBuf[5]==UM_MCU_UPDATE   &&   // data[0] = UM_MCU_UPDATE = 0x01
           pbRxBuf[6]==TRUE )               // data[1] = TRUE
      {
        UART_SyncSend( DNLD_UART_PORT, "\x55\x21\x00\x01\x11\x05\xC7\x55", 8 );  // 38, C7
        UART_SyncSend( DNLD_UART_PORT, "\x55\x2F\x00\x01\x11\x05\xB8\x55", 8 );  // 47, B8
        // CTRL_DATA_REQ=0x20, APP_SYS=0, len=1, cmd=IND_LINK_STATUS=0x11, [0]=LINK_S_MCU_UPDATE=5
        gCmdReceived = TRUE;
        return;
      }
    }
    gExtCmdCnt = 0;
  }

  pbRxBuf[gExtCmdCnt] = c;
  if( ++gExtCmdCnt>=60 )
    gExtCmdCnt = 0;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DNLD_ERR  COMM_SendAnsPack( const PACKET *pCmdPack, const U8 *pcbCmdData )
{
  const U8 *p = (U8 *) pCmdPack;
  U16       nSize = GET_WORD( &(pCmdPack->wSize) );
  U16       i, wLen, wCheckSum;
  U8        abHeader[8];

  //DNLD_Printf(("{ COMM_SendAnsPack().\r"));
  wLen = sizeof(PACKET) + nSize;
  memcpy( abHeader, "\xFF\xFF\xFF\xFF\x29\x55\x00\x00", 6 );
  PUT_WORD( abHeader+6, wLen );
  UART_SyncSend( DNLD_UART_PORT, abHeader, 8 );
  UART_SyncSend( DNLD_UART_PORT,p, sizeof(PACKET) );
  
  wCheckSum = 0;
  for( i=0; i<sizeof(PACKET); i++ ) wCheckSum += *p++;
  for( i=0; i<nSize; i++ ) wCheckSum += pcbCmdData[i];

  if( nSize )
    UART_SyncSend( DNLD_UART_PORT,pcbCmdData, nSize );

  PUT_WORD( abHeader, wCheckSum  );
  UART_SyncSend( DNLD_UART_PORT,abHeader, 2 );
  //DNLD_Printf(("} COMM_SendAnsPack() = 0.\r"));
  return ERR_OK;
}


DNLD_ERR  COMM_RecvCmdPack( PACKET *pAnsPack, U8 *pbRxBuf )
{
  U8 *        pbAns = (U8 *) pAnsPack;
  U16         wAnsDataSize;
  U16         wCount = 0;
  U16         wLen = 0;
  U16         wSum = 0, wCheckSum;
  TPackStat   State = STATE_IDLE;
  int         c;
  DNLD_ERR    ErrCode;
  U32         dwIdleCount = 0;

  //DNLD_Printf(("{ COMM_RecvCmdPack().\r"));
  while (1)
  {
    c = UART_AsynPeek( DNLD_UART_PORT );
    if( c < 0 )
    {
      ++dwIdleCount;
      WDOG_Feed();
    }
    else
    {
      dwIdleCount = 0;
      //DNLD_Printf(("%02X ", c));
      switch( State )
      {
        case  STATE_IDLE   :
              if( c == 0xff )
                State = STATE_SYNCED;
              else
              { 
                COM_OnExtCommand( c, pbRxBuf );
              }
              break;

        case  STATE_SYNCED :
              if( c == 0x29 )
                State = STATE_COMMAND;
              else if( c != 0xff )
              { UART_SendString(STD_IO_PORT, ".");
                State = STATE_IDLE;
              }
              break;
    
        case  STATE_COMMAND:
              if( c == 0xAA ) 
                State = STATE_LENHIGH;
              else
              { UART_SendString(STD_IO_PORT, ";");
                State = STATE_IDLE;
              }
              break;
    
        case  STATE_LENHIGH:
              wLen = c << 8;
              State = STATE_LENLOW;
              break;
    
        case  STATE_LENLOW:
              wLen |= c;
              wCount = 0;
              wSum   = 0;
              State  = STATE_HEADER;
              break;
    
        case  STATE_HEADER:
              pbAns[wCount++] = c;
              wSum += c;
              if( wCount == sizeof(PACKET) )
              {
                wAnsDataSize = GET_WORD( &(pAnsPack->wSize) );
                if( wLen != ( sizeof(PACKET) + wAnsDataSize ) )
                { // length error
                  UART_SendString(STD_IO_PORT, "-");
                  ErrCode = ERR_MCU_DATA_SIZE;
                  goto label_to_exit;
                }

                wCount = 0;
                if( wAnsDataSize == 0 )
                  State = STATE_CHECKHIGH;
                else
                  State = STATE_DATA;
              }
              break;

        case  STATE_DATA:
              pbRxBuf[wCount++] = c;
              wSum += c;
              if( wCount >= wAnsDataSize )
              { wCount = 0;
                State = STATE_CHECKHIGH;
              }
              break;
    
        case  STATE_CHECKHIGH:
              wCheckSum = c << 8;
              State = STATE_CHECKLOW;
              break;
    
        case  STATE_CHECKLOW:
              wCheckSum |= c;
              if( wSum != wCheckSum )
              {
                UART_SendString(STD_IO_PORT, "?");
                ErrCode = ERR_MCU_CHECK_SUM;
                goto label_to_exit;
              }
              State = STATE_MATCHED;
              break;

        default:
              break;
      }
    }

    if( State == STATE_MATCHED )
    {
      ErrCode = ( pAnsPack->bStart != CMD_MAGIC ) ? ERR_MCU_MAGIC : ERR_OK;
      break;
    }

#if (ENABLE_UART_RX_TIMEOUT)
    if( dwIdleCount > 30000 )  // 100 ms
    { ErrCode = ERR_MCU_TIMEOUT;
      //UART_SendString(STD_IO_PORT, "#");
      break;
    }
#endif
  }

label_to_exit:
  //DNLD_Printf(("} COMM_RecvCmdPack() = %d.\r", ErrCode));
  return ErrCode;
}


DNLD_ERR  DNLD_OnCmdErasePage( PACKET *pCmdPack, U8 *pbBuff )
{
  U16       nDataSize;
  U16       wPageSize;
  U32       dwDnldAddr;
  DNLD_ERR  ErrCode;

  nDataSize  = GET_WORD( &(pCmdPack->wSize) );
  
  dwDnldAddr = GET_DWORD( pbBuff+0 );
  wPageSize  = GET_WORD( pbBuff+4 );

  //DNLD_Printf(("CMD Erase Page:\r"));
  //DNLD_Printf(("  dwDnldAddr = 0x%08X:\r", dwDnldAddr));
  //DNLD_Printf(("  wPageSize  = %d:\r", wPageSize ));

  if( nDataSize != 6 )
    ErrCode = ERR_MCU_DATA_SIZE;
  else if( wPageSize != FLASH_PAGE_SIZE )
    ErrCode = ERR_MCU_DNLD_SIZE;
  else if( (dwDnldAddr%wPageSize) != 0 || dwDnldAddr < FLASH_START ||
           (dwDnldAddr+wPageSize) > (FLASH_START+FLASH_SIZE) )
    ErrCode = ERR_MCU_DNLD_ADDR;
  else if( !FLASH_Erase( dwDnldAddr, 1 ) )
    ErrCode = ERR_MCU_FLASH_ERASE;
  else
    ErrCode = ERR_OK;

  //DNLD_Printf(("  ErrCode = %d.\r", ErrCode ));
  return ErrCode;
}


DNLD_ERR  DNLD_OnCmdDnldData( PACKET *pCmdPack, U8 *pbBuff )
{
  DNLD_ERR  ErrCode;
  U16       wDnldSize, wCheckSum, wCrc;
  U32       dwDnldAddr;
  U16       nDataSize;

  nDataSize  = GET_WORD( &(pCmdPack->wSize) );
  
  dwDnldAddr = GET_DWORD( pbBuff+0 );
  wDnldSize  = GET_WORD( pbBuff+4 );
  wCheckSum  = GET_WORD( pbBuff+6 );
  
  //DNLD_Printf(("CMD Download:\r"));
  //DNLD_Printf(("  dwDnldAddr = 0x%08X:\r", dwDnldAddr));
  //DNLD_Printf(("  wDnldSize  = %d:\r", wDnldSize ));
  //DNLD_Printf(("  wCheckSum  = 0x%04X:\r", wCheckSum ));

  if( nDataSize != (wDnldSize+8) )
    ErrCode = ERR_MCU_DATA_SIZE;
  else if( wDnldSize < 4 || wDnldSize > FLASH_PAGE_SIZE )
    ErrCode = ERR_MCU_DNLD_SIZE;
  else if( (dwDnldAddr%FLASH_PAGE_SIZE) != 0 || dwDnldAddr < FLASH_START ||
           (dwDnldAddr+wDnldSize) > (FLASH_START+FLASH_SIZE) )
    ErrCode = ERR_MCU_DNLD_ADDR;
  else
  {
    if( wCrc = CalculCrc16( pbBuff+8, wDnldSize ), wCrc != wCheckSum )
      ErrCode = ERR_MCU_CHECK_SUM;
    else if( !FLASH_Write( dwDnldAddr, pbBuff+8, wDnldSize ) )
      ErrCode = ERR_MCU_FLASH_WRITE;
    else if( wCrc = CalculCrc16( (U8 *)dwDnldAddr, wDnldSize ), wCrc != wCheckSum )
      ErrCode = ERR_MCU_CHECK_SUM;
    else
      ErrCode = ERR_OK;
  }
  //DNLD_Printf(("  ErrCode = %d.\r", ErrCode));
  return ErrCode;
}


DNLD_ERR  DNLD_SetAppMode( U32 dwDnldAddr, const U8 *pData, U16 wDnldSize )
{
  DNLD_ERR  ErrCode;

  if( !FLASH_Erase( dwDnldAddr & ~(FLASH_PAGE_SIZE-1), 1 ) )
    ErrCode = ERR_MCU_FLASH_ERASE;
  else if( !FLASH_Write( dwDnldAddr, pData, wDnldSize ) )
    ErrCode = ERR_MCU_FLASH_WRITE;
  else
    ErrCode = ERR_OK;
  return ErrCode;
}


DNLD_ERR  DNLD_OnCmdSetStartMode( PACKET *pCmdPack, U8 *pbBuff )
{
  DNLD_ERR  ErrCode;
  U16       wDnldSize, wCheckSum, wCrc;
  U32       dwDnldAddr;
  U16       nDataSize;

  nDataSize  = GET_WORD( &(pCmdPack->wSize) );
  
  dwDnldAddr = GET_DWORD( pbBuff+0 );
  wDnldSize  = GET_WORD( pbBuff+4 );
  wCheckSum  = GET_WORD( pbBuff+6 );

  /*
  DNLD_Printf(("CMD StartMode:\r"));
  DNLD_Printf(("  dwDnldAddr = 0x%08X:\r", dwDnldAddr));
  DNLD_Printf(("  wDnldSize  = %d:\r", wDnldSize ));
  DNLD_Printf(("  wCheckSum  = 0x%04X:\r", wCheckSum ));
  */

  if( nDataSize != (wDnldSize+8) )
    ErrCode = ERR_MCU_DATA_SIZE;
  else if( wDnldSize != 4 )
    ErrCode = ERR_MCU_DNLD_SIZE;
  else if( dwDnldAddr < FLASH_START || (dwDnldAddr+wDnldSize) > (FLASH_START+FLASH_SIZE) )
    ErrCode = ERR_MCU_DNLD_ADDR;
  else
  {
    if( wCrc = CalculCrc16( pbBuff+8, wDnldSize ), wCrc != wCheckSum )
      ErrCode = ERR_MCU_CHECK_SUM;
    else
    {
      ErrCode = DNLD_SetAppMode( dwDnldAddr, pbBuff+8, wDnldSize );
      //else if( wCrc = CalculCrc16( (U8 *)dwDnldAddr, wDnldSize ), wCrc != wCheckSum )
      //  ErrCode = ERR_MCU_CHECK_SUM;
      //else
      //  ErrCode = ERR_OK;
    }
  }
  // DNLD_Printf(("  ErrCode = %d.\r", ErrCode));
  return ErrCode;
}


void  DNLD_PowerOff( void )
{
//  U8  i;

  // Power off ARM, 8288, and Camera.
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE );
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOD, ENABLE );
/*
  for( i=0; i<5; i++ )
  { WDOG_Feed();
    if( i<2 )
      UART_SyncSend( DNLD_UART_PORT, "\x55\x2A\x00\x01\x3D\x01\x96\x55", 8 ); // 让ARM自己关机
    delay_ms(500);
  }
*/
  MUTE_PinSetOutput();        // PB7
  MUTE_Enable();              // 静音
  delay_ms(50);

  PWR_ArmPowerOff();
  PWR_ArmPowerPinSetOutput(); // PB1    ARM_POW       GPIO（输出）作为核心板电源开关，强制关电。

  PWR_CameraPowerOff();           // 摄像头关电
  PWR_CameraPowerPinSetOutput();  // PD2    CCD_POW       GPIO输出，摄像头电源控制脚

  PWR_SysPowerOff();          // 系统板(8288,Radio)关电
  PWR_SysPowerPinSetOutput(); // PB0    SYS_POW       GPIO（输出）作为8288电源开关，强制关电。

  PWR_ExtAmpPinOff();
  PWR_CameraPowerPinSetOutput();
}


void  DNLD_Process( void )
{
  static PACKET CmdPack;
  static PACKET AnsPack;
  DNLD_ERR      ErrCode;
  U8           *pBuff;
  
  pBuff = (U8 *) (0x20000000 + 8*1024);
  ErrCode = COMM_RecvCmdPack( &CmdPack, (U8 *)pBuff );
  if( ErrCode!=ERR_OK )
  { gbDnldStarted = FALSE;
    return;
  }

  if( !gbDnldStarted )
  { gbDnldStarted = TRUE;
    UART_SendString(STD_IO_PORT, "Upgrade:");
  }
  UART_SendString(STD_IO_PORT, ">" );

  memset( &AnsPack, 0, sizeof(AnsPack) );
  switch( CmdPack.bCmd )
  {
    case  DNLD_CMD_SHAKE_HAND :
          //DNLD_Printf(("CMD Shakehand OK.\r"));
          ErrCode = ERR_OK;
          break;

    case  DNLD_CMD_ERASE_PAGE:
          ErrCode = DNLD_OnCmdErasePage( &CmdPack, pBuff );
          break;

    case  DNLD_CMD_DOWNLOAD_DATA :
          ErrCode = DNLD_OnCmdDnldData( &CmdPack, pBuff );
          break;

    case  DNLD_CMD_SET_START_MODE:
          ErrCode = DNLD_OnCmdSetStartMode( &CmdPack, pBuff );
          FLASH_Final();
          break;
          
    default:
          ErrCode = ERR_MCU_COMMAND; 
          break;
  }

  WDOG_Feed();    // 2016.05.26

  AnsPack.bStart  = ANS_MAGIC;
  AnsPack.bCmd    = CmdPack.bCmd;
  AnsPack.bTag    = CmdPack.bTag;
  AnsPack.bStatus = ErrCode;
  AnsPack.wSize   = 0;
  COMM_SendAnsPack( &AnsPack, NULL );

  if( CmdPack.bCmd == DNLD_CMD_SET_START_MODE &&
      ErrCode == ERR_OK &&
      IS_APP_MODE() )
  {
    gbDnldStarted = FALSE;
    UART_SendString( STD_IO_PORT, "Done!\r");
    delay_ms( 50 );
    DNLD_PowerOff();
    delay_ms( 500 );
    MCU_Jump( BOOT_START_ADDR + 4 );  // reboot
    while(1) {};  // nerver reach here
  }
}


void  MCU_GetStartUpFlag( void )
{
#if 1
  U32  dwValue = 0;

  UART_SendString(STD_IO_PORT, "Resumed from ( ");
  if( RCC_GetFlagStatus(RCC_FLAG_OBLRST) != RESET )
  { UART_SendString(STD_IO_PORT, "OBL, ");
  }
  
  if( RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET )
  { UART_SendString(STD_IO_PORT, "RST-PIN, ");
  }

  if( RCC_GetFlagStatus(RCC_FLAG_V18PWRRSTF) != RESET )
  { UART_SendString(STD_IO_PORT, "1.8V-Domain, ");
  }

  if( RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET )
  { UART_SendString(STD_IO_PORT, "PWR, ");
  }

  if( RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET )
  { UART_SendString(STD_IO_PORT, "SFT, ");
  }
  
  if( RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET )
  { UART_SendString(STD_IO_PORT, "IWDG, ");
    
    dwValue = RTC_ReadBackupRegister( RTC_BKP_DR0 );
    if( ++dwValue>=10 )
    {
      // 连续10产生IWDG异常，自动变成BOOT模式
      dwValue = 0;
      FLASH_Init();   // Required.
      DNLD_SetAppMode( (U32) BOOT_MODE_FLAG_ADDR, "\xFF\xFF\xFF\xFF", 4 );
      FLASH_Final();  // Required.
    }
  }
  RTC_WriteBackupRegister( RTC_BKP_DR0, dwValue );

  if( RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET )
  { UART_SendString(STD_IO_PORT, "WWDG, ");
  }

  if( RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET )
  { UART_SendString(STD_IO_PORT, "LowPwr ");
  }
  UART_SendString(STD_IO_PORT, ") reset.\r");
#endif

  RCC_ClearFlag();
}

#if 0

void  MCU_PeriphralClockDisable( void )
{
  UART_Final( UART1 );
  UART_Final( UART2 );
  UART_Final( UART3 );
  UART_Final( UART4 );
#if 0
  ADC_DeInit( ADC1 );
#else
  //RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
#endif

  GPIO_SetTriState( GPIOA, 
                    GPIO_Pin_1  |  // DVR_IR(O)     GPIO    OUT, 有线的IR输出，用于控制行车记录仪(DVR)
                    GPIO_Pin_2  |  // TMC-RX        UART2   UART2-tx 胎压检测                   
                    GPIO_Pin_3  |  // TMC-TX        UART2   UART2-rx 胎压检测                   
                    GPIO_Pin_4  |  // SWC1          ADC1    ADC(in)方向盘上按键检测             
                    GPIO_Pin_5  |  // SWC2          ADC1    ADC(in)方向盘上按键检测             
                    GPIO_Pin_6  |  // STEL1         GPIO    GPIO(输出)控制方向盘按键的上拉电阻值
                    GPIO_Pin_7  |  // STEL2         GPIO    GPIO(输出)控制方向盘按键的上拉电阻值
                    GPIO_Pin_8  |  // CMMB-IR       GPIO    GPIO, IR输出脚（控制TV模组）        
                    GPIO_Pin_9  |  // UART1_TX      UART1   UART-tx，车辆信息检测               
                    GPIO_Pin_10 |  // UART1_RX      UART1   UART-rx，车辆信息检测               
                    GPIO_Pin_11 |  // ILL_DET       GPIO    GPIO输入，大灯检测                  
                    GPIO_Pin_12 |  // BRAKE_DET     GPIO    GPIO输入，刹车检测
                    GPIO_Pin_15    // RDSINT        EXTI15  RDS收音机的中断输入
                  );

  GPIO_SetTriState( GPIOB, 
                    GPIO_Pin_0  |  // SYS_POW       GPIO    GPIO（输出）作为8288和功放电源开关，强制关电。
                    GPIO_Pin_1  |  // ARM_POW       GPIO    GPIO（输出）                      (睡眠时设成输入，没问题)
                    GPIO_Pin_2  |  // ARM ON/OFF    GPIO    GPIO（输出）接ARM的on/off键(2秒)。(睡眠时设成输入，没问题)
                    GPIO_Pin_3  |  // ATV_SCL       I2C     SCL  I2C时钟输出线，用于控制模拟电视
                    GPIO_Pin_4  |  // ATV_SDA       I2C     SDA  I2C数据线，用于控制模拟电视
                    GPIO_Pin_5  |  // USB_SW        GPIO    GPIO输出，USB脚功能切换，高--正常模式，低--升级模式(ARM系统升级)
                    GPIO_Pin_6  |  // ANT_POW       GPIO    GPIO输出，控制收音机天线打开
                    GPIO_Pin_7  |  // MUTE          GPIO    GPIO输出
                    GPIO_Pin_8  |  // IIC-SCL1      GPIO    I2C-CLK线，与收音机、EEPROM通信
                    GPIO_Pin_9  |  // IIC-SDA1      GPIO    I2C-数据线
                    GPIO_Pin_10 |  // ARM_RES       GPIO    GPIO（输出）用于控制ARM复位 (睡眠时设成输入，没问题)
                    GPIO_Pin_11 |  // BEEP          GPIO    PWM输出（与timer关联）
                    GPIO_Pin_12 |  // LCD_ON        GPIO    GPIO（输出）用于LCD背光控制（高电平亮）
                    GPIO_Pin_13 |  // 8288T_RES     GPIO    GPIO（输出），控制8288复位
                    GPIO_Pin_14 |  // DISC-LDSW     GPIO    GPIO（输入）
                    GPIO_Pin_15    // DISC-OUTSW    GPIO    GPIO（输入）
                  );

  GPIO_SetTriState( GPIOC,
                    GPIO_Pin_0  |  // KEY1        ADC1    ADC(in)面板按键
                    GPIO_Pin_1  |  // KEY2        ADC1    ADC(in)面板按键
                    GPIO_Pin_2  |  // KNOB1       ADC1    ADC(in)旋钮输入(ENCODE1)
                    GPIO_Pin_3  |  // KNOB2       ADC1    ADC(in)旋钮输入(ENCODE2)
                    GPIO_Pin_4  |  // UART3-Rx    UART3   ARM_RX,与ARM通信
                    GPIO_Pin_5  |  // UART3-Tx    UART3   ARM-TX,与ARM通信
                    GPIO_Pin_6  |  // DISC-INSW   GPIO    GPIO（输入）                                    
                    GPIO_Pin_7  |  // DISC-LD+    GPIO    GPIO（输出）                                    
                    GPIO_Pin_8  |  // DISC-LD-    GPIO    GPIO（输出）                                    
                    GPIO_Pin_9  |  // LCD_PWM     GPIO    PWM（内与timer2相连）脉冲输出PWM(调频率和占空比)
                    GPIO_Pin_10 |  // UART4-tx    UART4   8288_RXD
                    GPIO_Pin_11 |  // UART4-rx    UART4   8288_TXD
                    GPIO_Pin_12 |  // REVER_DET   GPIO    GPIO输入，倒车检测               
                    GPIO_Pin_13 |  // IR          GPIO    INTR, 外部中断输入脚，遥控器中断
                    GPIO_Pin_14 |  // AMP_CTL     GPIO    外部功放控制
                    GPIO_Pin_15    // S_METER     GPIO    接RDS收音机的S_METER/RST
                  );

  GPIO_SetTriState( GPIOD,
                    GPIO_Pin_2     // CCD_POW       GPIO    GPIO输出，倒车后视摄像头电源控制脚
                  );

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, DISABLE );

  RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_GPIOA | // Used by ACC EXTI
                          RCC_AHBPeriph_GPIOB | // Used by PWRCTRL
                          RCC_AHBPeriph_GPIOC |
                          RCC_AHBPeriph_GPIOD
                        , DISABLE );

  RCC_APB1PeriphClockCmd( RCC_APB2Periph_TIM1 , DISABLE ); // Beep
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2 , DISABLE ); // Beep
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3 , DISABLE ); // PWM
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM6 , DISABLE );
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM7 , DISABLE );
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM14, DISABLE );
  RCC_APB1PeriphClockCmd( RCC_APB2Periph_TIM15, DISABLE );
  RCC_APB1PeriphClockCmd( RCC_APB2Periph_TIM16, DISABLE );
  RCC_APB1PeriphClockCmd( RCC_APB2Periph_TIM17, DISABLE );

  GPIO_DeInit( GPIOA );
  GPIO_DeInit( GPIOB );
  GPIO_DeInit( GPIOC );
  GPIO_DeInit( GPIOD );
}

#endif

static U8   gbAccDebounceCnt;
static U8   gbAccState = ON;

void  ACC_Process( void )
{
  // ACC debounce
  if( READ_ACC_PIN()!=gbAccState )
  {
    if( gbAccDebounceCnt==0 )
      gbAccDebounceCnt = 100;
    else 
    {
    /*
      // ACC can be off when MCU is upgrading. So this codes are not required.
      if( gbDnldStarted )
        return;
    */

      if( --gbAccDebounceCnt==0 )
      {
        gbAccState = READ_ACC_PIN();
        if( gbAccState==OFF )
          DNLD_PowerOff();
        else
          MCU_Jump( FLASH_START + 4 );
      }
    }
  }
  else
  {
    gbAccDebounceCnt = 0;
  }
}


#if (BOOT_POWER_ON_ALL)

  void  DNLD_ArmPowerOn( void )
  {
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE );
    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC, ENABLE );

    MUTE_Enable();          MUTE_PinSetOutput();    // 加电过程中要静音

    PWR_ImgL2RPinSetOutput();   // Astern Image control
    PWR_ImgL2RPinNormal();      // Left to right

    PWR_SysPowerOn();       PWR_SysPowerPinSetOutput();
    PWR_ArmStartPinHigh();  PWR_ArmStartPinSetOutput();
    PWR_ArmPowerOn();       PWR_ArmPowerPinSetOutput();

    PWR_ArmStatusPinSetInput();
    delay_ms(1);
    gbArmBootStatus = PWR_ArmStatusPinRead() ? ARM_S_STARTED : ARM_S_NOT_STARTED;
  
    gBootFlag = (gbArmBootStatus==ARM_S_STARTED);
    if( !gBootFlag )
    {
      PWR_ArmPowerOff();
      delay_ms(200);
      PWR_ArmPowerOn();
      gwArmDelayCount = 10;
    }
    PWR_LcdOn();  PWR_LcdCtrlPinSetOutput();
  }
#endif

int  main( void )
{
/*
  { // STM32F071 is little-endian.
    bool  bBigEndain;
    bBigEndain = MCU_GetEndian();
    if( bBigEndain )
      UART_SyncSend(DNLD_UART_PORT, "MCU is Big Endian\r", 18);
    else
      UART_SyncSend(DNLD_UART_PORT, "MCU is Little Endian\r", 21);
  }
  
  {
    RCC_ClocksTypeDef  gRCC_Clocks;
    RCC_GetClocksFreq( &gRCC_Clocks );
  }

  RCC_DeInit();
  SystemInit();   // RCC_Init()
*/
  WDOG_Start();
  __disable_irq();
#if 1 // def STD_IO_PORT
  UART_Init( STD_IO_PORT, STD_IO_BAUDRATE );   // Use TPM port for log.
#endif
  UART_Init( DNLD_UART_PORT, 115200 );
  RTCDRV_Init();

  MCU_GetStartUpFlag();
  UART_SendString(STD_IO_PORT, "Bootloader (2017.05.11, 12:00:00).\r");
  if( IS_APP_MODE() )
    UART_SendString(STD_IO_PORT, "APPL");
  else
    UART_SendString(STD_IO_PORT, "BOOT");
  UART_SendString(STD_IO_PORT, "-Mode.\r");

  if( IS_APP_MODE() )
  {
    __enable_irq();
    MCU_Jump( APPL_START_ADDR + 4 );  // Jump to user application
    while(1) {};
  }
  
  FLASH_Init();   // Required.
/*
  // Temp Code: Used to set APP-MODE flag.
  FLASH_Erase( BOOT_MODE_FLAG_ADDR & ~(FLASH_PAGE_SIZE-1), 1 );
  FLASH_Write( BOOT_MODE_FLAG_ADDR, "\xAA\xAA\xAA\xAA", 4 );
  DNLD_Printf(("BOOT_MODE --> APP_MODE.\rHalt.\r"));
  while(1) { WDOG_Feed(); };
*/

  RCC_AHBPeriphClockCmd(  RCC_AHBPeriph_GPIOA, ENABLE );    // ACC
  GPIO_SetInput( PIN_PORT_ACC );

#if (BOOT_POWER_ON_ALL)
  DNLD_ArmPowerOn();          // ARM 加电
#endif
  
  while(1)
  {
    if( gbAccState!=OFF )
    {
      DNLD_Process();
#if (BOOT_POWER_ON_ALL)
      ARM_ProcessArmStart();  // ARM 开机处理
#endif
    }
    ACC_Process();
  }
}


#ifdef  USE_FULL_ASSERT
  void assert_failed(uint8_t* file, uint32_t line)
  { 
    while (1) {};
  }
#endif

