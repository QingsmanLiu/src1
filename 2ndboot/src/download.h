#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__

#include <typedef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    U8    bStart;
    U8    bCmd;
    U8    bStatus;
    U8    bTag;
    U16   wSize;

  } PACKET;

  typedef enum tagTPackStat
  {
    STATE_IDLE      ,
    STATE_SYNCED    ,
    STATE_COMMAND   ,
    STATE_LENHIGH   ,
    STATE_LENLOW    ,
    STATE_DATA      ,
    STATE_HEADER    ,
    STATE_CHECKHIGH ,
    STATE_CHECKLOW  ,
    STATE_MATCHED   ,
    STATE_FINISHED  ,

  } TPackStat;

  typedef enum
  {
    DNLD_CMD_SHAKE_HAND       = 0x01,
    DNLD_CMD_ERASE_PAGE       = 0x02,
    DNLD_CMD_DOWNLOAD_DATA    = 0x03,
    DNLD_CMD_SET_START_MODE   = 0x04,
    
  } DNLD_CMD;

  #define CMD_MAGIC         0xA5
  #define ANS_MAGIC         0x5A

  typedef enum
  {
    ERR_OK                 = 0,
    ERR_SHAKE_HAND            ,

    ERR_MCU_CONNECT           ,
    ERR_MCU_FILE_PATH         ,
    ERR_MCU_COMMUNICATION     ,
    ERR_MCU_COMMAND           ,
    ERR_MCU_MAGIC             ,
    ERR_MCU_PAGE_SIZE         ,
    ERR_MCU_DATA_SIZE         ,
    ERR_MCU_DNLD_SIZE         ,
    ERR_MCU_CHECK_SUM         ,
    ERR_MCU_DNLD_ADDR         ,
    ERR_MCU_FLASH_WRITE       ,
    ERR_MCU_FLASH_ERASE       ,
    ERR_MCU_TIMEOUT           ,

    ERR_8288_1ST_CONNECT      ,
    ERR_8288_OPEN_RAM_BOOT    ,
    ERR_8288_OPEN_ROM_CODE    ,
    ERR_8288_CONFIG_SDRAM     ,
    ERR_8288_INIT_SDRAM       ,
    ERR_8288_DNLD_RAM_BOOT    ,
    ERR_8288_SET_ROM_CODE_SIZE,
    ERR_8288_DEVICE_RESET     ,
    ERR_8288_NEW_CONNECT      ,
    ERR_8288_ERASE_FLASH      ,
    ERR_8288_DNLD_ROM_CODE    ,
    ERR_8288_CHECK_SUM        ,

  } DNLD_ERR;

  typedef struct 
  {
    char        szDecription[32];
    U32         dwVersion;
    U32         nFileNum;
    U32         dwFileSize[1];

  } DNLD_FILE_INFO;

  typedef struct tagT_BANK_INFO
  {
    U16       wBankIndex  ; // Index of bank.                 
    U32       dwBankAddr  ; // Address of the bank.           
    U32       dwBankSize  ; // Size of the bank.              
    U32       dwPageSize  ; // Size of flash page.            
    U16       wFileIndex  ; // Index of file to be downloaded.
    U32       dwOffInFile ; // Offset of data in the file.    

  } T_BANK_INFO;
  

  char const *DNLD_GetMcuErrorText( DNLD_ERR err );
  DNLD_ERR    DNLD_DownloadMCU( const char *szUartDev, const char *szCodeFile );

#ifdef __cplusplus
}
#endif


#endif // __DOWNLOAD_H__

