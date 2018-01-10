#ifndef __DRV_UART_H__
#define __DRV_UART_H__

#include "typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
  //--------------------------------------------------
  UART1:    PIN42   USART1_TX         PA9     (AF1)
            PIN43   USART1_RX         PA10    (AF1)

  //--------------------------------------------------
  UART2:    PIN16   USART2_TX         PA2     (AF1)
            PIN17   USART2_RX         PA3     (AF1)

  //--------------------------------------------------
  UART3:    PIN24   USART3_TX         PC4     (AF1)
            PIN25   USART3_RX         PC5     (AF1)

  //--------------------------------------------------
  UART4     PIN51   USART4_TX         PC10    (AF0)
            PIN52   USART4_RX         PC11    (AF0)
*/

  //---------------------------------------------------------------------------
  // UART port definition
  //---------------------------------------------------------------------------
  typedef enum
  {
    UART1  =  0,  // CAN communication
    UART2  =  1,  // TMC or Printf
    UART3  =  2,  // ARM communication
    UART4  =  3,  // DVD coomunication
    
    UART_MAX   ,
    
  } UART_PORT;

  //---------------------------------------------------------------------------
  // Special UART port
  //---------------------------------------------------------------------------
#if 1
  #define STD_IO_PORT       UART2
#endif

#if 1
  // Use ARMCOM port to upgrade MCU-APP.
  #define DNLD_UART_PORT    UART3
#else
  #if defined(STD_IO_PORT)
    #error "UART2 can not be used as both SDT_IO_PORT and DNLD_UART_PORT"
  #endif
  // Use TPM port to upgrade MCU-APP.
  #define DNLD_UART_PORT    UART2
#endif

  //---------------------------------------------------------------------------
  // UART driver functions
  //---------------------------------------------------------------------------
  void  UART_Init( UART_PORT port, U32 bandRate );
  void  UART_Final( UART_PORT port );
  
  int   UART_AsynPeek( UART_PORT port );
  int   UART_AsynRecv( UART_PORT port, void *pBuff, int len );
  void  UART_SyncSend( UART_PORT port, const U8 *pData, int size );
  void  UART_SyncRecv( UART_PORT port, void *pBuf, int len );

  void  UART_ClearError( UART_PORT port, U32 flag );
  void  UART_SendString( UART_PORT port, const char *pStr ); // sync

#if 0 //def STD_IO_PORT
  int   printf( const char *format, ... );
  int   getch( void );    // async
#endif

  void  delay_ms( U32 msec );
  void  delay_us( U32 usec );

#if 1
  void  UART_Test( void );
#endif

/*
©°©¤©Ð©¤©´
©¦  ©¦  ©¦
©À©¤©à©¤©È
©¦  ©¦  ©¦
©¸©¤©Ø©¤©¼
*/

#ifdef __cplusplus
}
#endif
  
#endif // __DRV_UART_H__

