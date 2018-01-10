#include "../src/appl.h"


#ifdef WIN32

  void USART_ClearFlag(USART_TypeDef* USARTx, uint32_t USART_FLAG)
  {
  }

  void __set_MSP(uint32_t topOfMainStack)
  {
  }

  uint32_t __get_LR( void )
  {
    return 0;
  }
#endif
