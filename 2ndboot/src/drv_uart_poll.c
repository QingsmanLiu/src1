#include <appl.h>
#include "stm32f0xx_it.h"


//------------------------------------------------------------------------------
// INternal function delarations
//------------------------------------------------------------------------------
static void  UART_GPIO_Init( UART_PORT port );
static void  UART_BAUD_Init( UART_PORT port, U32 bandRate );


//------------------------------------------------------------------------------
// Interbal data definitions
//------------------------------------------------------------------------------

static USART_TypeDef * const gapSUart[UART_MAX] =
{
  USART1,
  USART2,
  USART3,
  USART4,
};


//------------------------------------------------------------------------------
// UART internal functions
//------------------------------------------------------------------------------
static void  UART_GPIO_Init( UART_PORT port )
{
  GPIO_InitTypeDef        GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; // GPIO_PuPd_NOPULL, GPIO_PuPd_DOWN
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  
  switch( port )
  {
#ifdef ENABLE_ALL_CODE
  case  UART1 :
        // PIN42   USART1_TX         PA9     (AF1)
        // PIN43   USART1_RX         PA10    (AF1)
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA , ENABLE );    // Enable GPIOA and DMA clock 
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE );  // Enable USART1 APB clock 
      #if 0            
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
        GPIO_Init( GPIOA, &GPIO_InitStructure );
      #else
        GPIO_Init( GPIOA, GPIO_Pin_9 | GPIO_Pin_10, &GPIO_InitStructure );
      #endif  
        GPIO_PinAFConfig( GPIOA, GPIO_PinSource9 , GPIO_AF_1 );    
        GPIO_PinAFConfig( GPIOA, GPIO_PinSource10, GPIO_AF_1 ); 
        break;
#endif
        
  case  UART2 :
        // PIN16   USART2_TX         PA2     (AF1)
        // PIN17   USART2_RX         PA3     (AF1)
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA , ENABLE );    // Enable GPIOA and DMA clock 
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );  // Enable USART2 APB clock 
       
        GPIO_PinAFConfig( GPIOA, GPIO_PinSource2, GPIO_AF_1 );
        GPIO_PinAFConfig( GPIOA, GPIO_PinSource3, GPIO_AF_1 );
      #if 0    
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
        GPIO_Init( GPIOA, &GPIO_InitStructure );
      #else
        GPIO_Init( GPIOA, GPIO_Pin_2 | GPIO_Pin_3, &GPIO_InitStructure );
      #endif
        break;
        
  case  UART3 :
        // PIN24   USART3_TX         PC4     (AF1)
        // PIN25   USART3_RX         PC5     (AF1)
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC , ENABLE );    // Enable GPIOA and DMA clock 
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );  // Enable USART2 APB clock 
       
        GPIO_PinAFConfig( GPIOC, GPIO_PinSource4, GPIO_AF_1 );
        GPIO_PinAFConfig( GPIOC, GPIO_PinSource5, GPIO_AF_1 );
      #if 0   
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
        GPIO_Init( GPIOC, &GPIO_InitStructure );
      #else
        GPIO_Init( GPIOC, GPIO_Pin_4 | GPIO_Pin_5, &GPIO_InitStructure );
      #endif
        break;

#if (!CODE_FOR_MCU_BOOT)
  case  UART4 :
        // PIN51   USART4_TX         PC10    (AF0)
        // PIN52   USART4_RX         PC11    (AF0)
        RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOC , ENABLE );    // Enable GPIOA and DMA clock 
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART4, ENABLE );  // Enable USART2 APB clock 
                
        GPIO_PinAFConfig( GPIOC, GPIO_PinSource10, GPIO_AF_0 );
        GPIO_PinAFConfig( GPIOC, GPIO_PinSource11, GPIO_AF_0 );
      #if 0   
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_Init( GPIOC, &GPIO_InitStructure );
      #else
        GPIO_Init( GPIOC, GPIO_Pin_10 | GPIO_Pin_11, &GPIO_InitStructure );
      #endif
        break;
#endif
        
  default     :
        break;
  }
}


static void  UART_BAUD_Init( UART_PORT port, U32 bandRate )
{
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate   = bandRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits   = USART_StopBits_2;
  USART_InitStructure.USART_Parity     = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init( gapSUart[port], &USART_InitStructure );
}


//------------------------------------------------------------------------------
// UART APIs
//------------------------------------------------------------------------------
void  UART_Init( UART_PORT port, U32 bandRate )
{
  if( port >= UART_MAX )
    return;
  
  USART_DeInit( gapSUart[port] );

  UART_GPIO_Init( port );
  UART_BAUD_Init( port, bandRate );
  USART_Cmd( gapSUart[port], ENABLE );
}


void  UART_Final( UART_PORT port )
{
  if( port >= UART_MAX )
    return;

  USART_DeInit( gapSUart[port] );
}

/*
  USART_FLAG_WU:    Wake up flag, not available for  STM32F030 devices.
  USART_FLAG_CM:    Character match flag.
  USART_FLAG_EOB:   End of block flag, not available for  STM32F030 devices.
  USART_FLAG_RTO:   Receive time out flag.
  USART_FLAG_CTS:   CTS Change flag.
  USART_FLAG_LBD:   LIN Break detection flag, not available for  STM32F030 devices.
  USART_FLAG_TC:    Transmission Complete flag.
  USART_FLAG_IDLE:  IDLE line detected flag.
  USART_FLAG_ORE:   OverRun Error flag.
  USART_FLAG_NE:    Noise Error flag.
  USART_FLAG_FE:    Framing Error flag.
  USART_FLAG_PE:    Parity Errorflag.

  while( (gapSUart[port]->ISR & USART_ISR_TC) != USART_ISR_TC ) {};
  void  UART_ClearError( UART_PORT port, USART_FLAG_LBD );
*/

#if 0

  void  UART_ClearError( UART_PORT port, U32 flag )
  {
    gapSUart[port]->ICR = flag;
    //USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE );
  }

#endif


int   UART_AsynPeek( UART_PORT port )                           // async
{
  U8    data;
  
  if( ( gapSUart[port]->ISR & USART_ISR_RXNE ) == USART_ISR_RXNE )
  {
    data = (U8) gapSUart[port]->RDR;  // Receive data, clear RXNE flag
    return data;
  }
  else
  {
  #if 1
    gapSUart[port]->ICR = USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF | USART_ICR_PECF;
  #endif
    return -1; // EOF
  }
}

#ifdef ENABLE_ALL_CODE

  int  UART_AsynRecv( UART_PORT port, void *pBuff, int len )        // async
  {
    U8   *p = (PU8) pBuff;
    int   c, n;
  
    if( port >= UART_MAX )
      return 0;
  
    for( n = 0; n < len; n++ )
    {
      c = UART_AsynPeek( port );
      if( c < 0 )
        break;
      *p++ = c;
    }
    return n; // return the bytes that actually received
  }

#endif


void  UART_SyncSend( UART_PORT port, const U8 *pData, int nSize ) // sync
{
  for( ; nSize>0; nSize-- )
  {
    // Waiting for last Tx complete.
    while( (gapSUart[port]->ISR & USART_ISR_TC) != USART_ISR_TC ) 
    { delay_us(10);
    };
    
    // Fill TDR with a new data and clear transfer complete flag.
    gapSUart[port]->TDR = *pData++;
  }
}


void  UART_SendString( UART_PORT port, const char *pStr )   // sync
{
  while( *pStr )
  {
    // Waiting for last Tx complete.
    while( (gapSUart[port]->ISR & USART_ISR_TC) != USART_ISR_TC ) 
    { delay_us(10); 
    };
    gapSUart[port]->TDR = *pStr++;
  }
}


#ifdef ENABLE_ALL_CODE

  void  UART_SyncRecv( UART_PORT port, void *pBuf, int len )  // sync
  {
    PU8  p = (PU8)pBuf;
    int  n;
  
    while( len )
    {
      n = UART_AsynRecv( port, p, len );
      len -= n;
      p   += n;
    } 
  }

#endif


//-----------------------------------------------------------------------------
// Support STD-IO
//-----------------------------------------------------------------------------

#if 0 // def STD_IO_PORT

  #if  1
    //-----------------------------------------
    // KEIL Compiler
    //-----------------------------------------
    int fputc( int ch, FILE *f )  // Sync
    {
      U8    c = ch;
      UART_SyncSend( STD_IO_PORT, &c, 1 );
      return c;
    }

  #else
    //-----------------------------------------
    // IAR Compiler
    //-----------------------------------------
    int  printf( const char *format, ... )
    {
      char    buf[64];
      va_list ap;
      int     len;
    
      va_start( ap, format );
    
    #ifdef WIN32
      _vsnprintf( buf, sizeof(buf), format, ap );
    #else
      vsnprintf( buf, sizeof(buf), format, ap );
    #endif
      va_end( ap );
    
      len = strlen( buf );
      UART_SyncSend( STD_IO_PORT, buf, len );
      return len;
    }
  #endif

/*
  int   getch( void )             // async
  {
    U8  c;
  
    if( !UART_AsynRecv( STD_IO_PORT, &c, 1 ) )
      return -1; // EOF
    return c;
  }
*/

#endif


//-----------------------------------------------------------------------------
// UART Test Program
//-----------------------------------------------------------------------------
#if 0

  void  UART_Test( void )
  {
    U8 buf[16];
    int  n;
    
    UART_Init( UART1, 115200 );
    while( 1 )
    {
      n = UART_AsynRecv( UART1, buf, sizeof(buf) );
      if( n )
        UART_SyncSend( UART1, buf, n );
    }
  }

#endif

