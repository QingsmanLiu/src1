#ifndef __APPL_H__
#define __APPL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define BOOT_POWER_ON_ALL               1   
#define CODE_FOR_MCU_BOOT               1

#define CLIENT_ID                       CLIENT_STD_FM_9MHZ
#define CLIENT_STD_FM_9MHZ              1
#define ENABLE_WATCH_DOG                1

#define ENABLE_ARM_STATUS_PIN_DETECT    1

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "typedef.h"
#include "drv_flash.h"
#include "drv_gpio.h"
#include "proc_pwrctrl.h"
#include "drv_wdog.h"
#include "drv_rtc.h"

#include "download.h"
#include "misc.h"

#include "drv_uart_poll.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if 0 // def STD_IO_PORT
  #define SYS_Printf(x)      printf x
#else
  #define SYS_Printf(x)
#endif


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif // #define __APPL_H__
