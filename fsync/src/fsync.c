//=================================================================================
// File     : "dir_util.c"
// Purpose  : Directory Utility
// Author   : Qing.Liu
// 
//=================================================================================

#include "fsync.h"
#undef _MAX_DRIVE


//---------------------------------------------------------------------------------
// Constant used in this file only
//---------------------------------------------------------------------------------
#define WILD_NONE       0x00
#define WILD_QUEST      0x01
#define WILD_STAR       0x02


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void  normalize_slash( char *path )
{
  for( ; path[0] != EOS; path++ )
  {
    if( path[0]==EXT_SLASH[0] )
      path[0] = STD_SLASH[0];
  }
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
void  try_append_slash( char *path )
{
  int len = strlen( path );
  
  if( len==0 )
    return; // Empty dir means current dir, not the root.

  normalize_slash( path );

  if( path[len-1]== STD_SLASH[0] )
    return;
  strcat( path, STD_SLASH );
}


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
static const char *forward_locate_substr( const char *pSubStr, int *pSubLen, const char *pStr, const char *pEnd )
{
  int   sub_len = *pSubLen;

  if( sub_len == 0 )
  {
    *pSubLen = (int)( pEnd - pStr );
    return pStr;
  }

  while( ( pStr + sub_len ) <= pEnd )
  {
    if( memcmp( pStr, pSubStr, sub_len ) == 0 )
      return pStr;
    pStr++;
  }
  return NULL;
}


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
static const char *backward_locate_substr( const char *pSubStr, int *pSubLen, const char *pStr, const char *pEnd )
{
  int   sub_len = *pSubLen;
  
  if( sub_len == 0 )
  {
    *pSubLen = (int)( pEnd - pStr );
    return pStr;
  }

  pEnd -= sub_len;
  while( pEnd >= pStr )
  {
    if( memcmp( pEnd, pSubStr, sub_len ) == 0 )
      return pEnd;
    pEnd--;
  }
  return NULL;
}


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
bool  match_wildcard( const char *str, const char *wildcards )
{
  const char  *pStr1;
  const char  *pStr2;
  const char  *pWild1;
  const char  *pWild2;
  const char  *pSubStr, *pSubKey, *ptr;
  int         nKeyLen, nStrLen, least_match_len;
  char        w, c;

  if( str[0] == EOS || wildcards[0] == EOS )
    return false;

  pStr1  = str;
  pStr2  = pStr1 + strlen( str );
  pWild1 = wildcards;
  pWild2 = pWild1 + strlen( wildcards );
  
  while( true )
  {
    // try to match at the end. 
    if( pWild2[-1] != '*' )
    { 
      w = tolower( pWild2[-1] );
      c = tolower( pStr2[-1] );
      if( w != '?' && w != c )
          return false;
      --pStr2;
      --pWild2;
    }
    if( pStr1 >= pStr2 || pWild1 >= pWild2 )
      break;

    // try to match at the beginning. 
    if( *pWild1 != '*' )
    {
      w = tolower( *pWild1 );
      c = tolower( *pStr1 );
      if( w != '?' && w != c )
        return false;
      ++pStr1;
      ++pWild1;
    }
    if( pStr1 >= pStr2 || pWild1 >= pWild2 )
      break;

    // try to do asterisk matching at the end.
    if( pWild2[-1] == '*' )
    {
      // many asterisks just like one asterisk.
      --pWild2;
      least_match_len = 0;
      while( (pWild2-1) >= pWild1 && ( pWild2[-1] == '*' || pWild1[-1] == '?' ) ) 
      {
        if( pWild2[-1] == '?' )
          ++least_match_len;
        --pWild2;
      }

      // determine sub-key-string to search for.
      for( ptr = pWild2-1; ptr >= pWild1; ptr-- )
      {
        if( *ptr == '.' )
        { ptr--;
          break;
        }

        if( *ptr == '*' || *ptr == '?' )
          break; 
      }
      pSubKey = ptr + 1;
      nKeyLen = pWild2 - pSubKey;
      
      nStrLen = nKeyLen;
      pSubStr = backward_locate_substr( pSubKey, &nStrLen, pStr1, pStr2 );
      if( ! pSubStr )
        return false;
      if( nStrLen < least_match_len )
        return false;

      pStr2  = pSubStr;
      pWild2 = pSubKey;
    }
    if( pStr1 >= pStr2 || pWild1 >= pWild2 )
      break;

    // try to do asterisk matching at the beginning.
    if( *pWild1 == '*' )
    { 
      // many asterisks just like one asterisk.
      ++pWild1;
      least_match_len = 0;
      while( pWild1 < pWild2 && ( pWild1[0] == '*' || pWild1[0] == '?' ) ) 
      {
        if( pWild1[0] == '?' )
          ++least_match_len;
        ++pWild1;
      }

      // determine sub-key-string to search for.
      for( ptr = pWild1; ptr < pWild2; ptr++ )
      {
        if( *ptr == '.' )
        { ptr++;
          break;
        }

        if( *ptr == '*' || *ptr == '?' )
          break;
      }
      pSubKey = pWild1;
      nKeyLen = ptr - pSubKey;

      nStrLen = nKeyLen;
      pSubStr = forward_locate_substr( pSubKey, &nStrLen, pStr1, pStr2 );
      if( ! pSubStr )
        return false;
      if( nStrLen < least_match_len )
        return false;
      
      pStr1  = pSubStr + nStrLen;
      pWild1 = pSubKey + nKeyLen;
    }
    if( pStr1 >= pStr2 || pWild1 >= pWild2 )
      break;
  }
  
  return ( pStr1 == pStr2 && pWild1 == pWild2 );
}


FF_CNTX *FF_FindOpen( const char *szDir, const char *szWild )
{
  char      path[_MAX_PATH];
  FF_CNTX   *pffd;

  pffd = malloc( sizeof(*pffd) );
  if( !pffd )
    return NULL;

  strcpy( pffd->m_dir, szDir );
  strcpy( pffd->m_wild, szWild );
  pffd->m_pdir = opendir( pffd->m_dir );
  
  if( pffd->m_pdir==NULL )
  { free( pffd );
    pffd = NULL;
  }
  return pffd;
}


bool  FF_FindFile( FF_CNTX *pffd )
{
  char  path[_MAX_PATH];

  if( pffd->m_pdir==NULL )
    return false;
  
  while(1)  
  {
    // 读目录中的下一个文件入口信息
    pffd->m_fitem = readdir( pffd->m_pdir );
    if( pffd->m_fitem==NULL )
      break;

    // 构造文件的路径
    strcpy( path, pffd->m_dir );
    try_append_slash( path );
    strcat( path, pffd->m_fitem->d_name );

    // 获取文件的属性
    if( stat( path, &pffd->m_fstat ) )
      continue;

    // 文件名与通配符匹配
    if( match_wildcard( pffd->m_fitem->d_name, pffd->m_wild ) )
      return true;
  }
   
  return false;
}


bool   FF_FindClose( FF_CNTX *pffd )
{
  if( !pffd )
    return false;

  closedir( pffd->m_pdir );
  free( pffd );
  return true;
}


//-----------------------------------------------------------------------------
// User program
//-----------------------------------------------------------------------------
bool FF_EnumFile( const char *szDirSrc, const char *szWild,
                  unsigned depth, FF_CALLBACK pCallback, 
                  const char *szDirDst, void *pData )
{
  FF_CNTX *pffd;

  if( depth==0 )
    return true;  // continue enumeration

  // First, enumerate files under the directory
  pffd = FF_FindOpen( szDirSrc, szWild );
  if( pffd )
  {
    while( FF_FindFile(pffd) )
    {
      if( strcmp( pffd->m_fitem->d_name, "." )!=0  && 
          strcmp( pffd->m_fitem->d_name, ".." )!=0 &&
          S_ISREG(pffd->m_fstat.st_mode)           &&
          !S_ISDIR(pffd->m_fstat.st_mode) )
      {
        if( ! pCallback( szDirSrc, szDirDst, pffd, pData ) )
          return false;  // stop enumeration
      }
    }
  }
  FF_FindClose( pffd );

  // Second, enumerate sub-directories under the directory
  pffd = FF_FindOpen( szDirSrc, "*" );
  if( pffd )
  {
    while( FF_FindFile( pffd ) )
    {
      if( strcmp( pffd->m_fitem->d_name, "." )!=0  &&
          strcmp( pffd->m_fitem->d_name, ".." )!=0 &&
        //S_ISREG(pffd->m_stat.st_mode) &&
          S_ISDIR(pffd->m_fstat.st_mode) )
      {
        char  szSubDirSrc[_MAX_PATH];
        char *pSubDirDst = NULL; // [_MAX_PATH];
        bool  bContinue;

        strcpy( szSubDirSrc, szDirSrc );
        try_append_slash( szSubDirSrc );
        strcat( szSubDirSrc, pffd->m_fitem->d_name );

        if( szDirDst )
        {
          pSubDirDst = malloc( strlen(szDirDst) + strlen(pffd->m_fitem->d_name) + 4 );
          strcpy( pSubDirDst, szDirDst );
          try_append_slash( pSubDirDst );
          strcat( pSubDirDst, pffd->m_fitem->d_name );
        }

        // Enumeration in the sub-directory
        bContinue = FF_EnumFile( szSubDirSrc, szWild, depth-1, pCallback, pSubDirDst, pData );
         if( pSubDirDst )
          free( pSubDirDst );
         if( !bContinue )
          return false;   // stop enumeration

        // Report the sub-directory to Callback
        if( !pCallback( szDirSrc, szDirDst, pffd, pData ) )
          return false;  // stopped by user
      }
    }
  }
  FF_FindClose( pffd );

  return true;  // continue enumeration
}


#define CMP_BUF_SIZE     2048

bool file_is_same( const char *spath, const char *dpath )
{
  bool  bResult = false;
  int   sfh, dfh;
  char  buf1[CMP_BUF_SIZE];
  char  buf2[CMP_BUF_SIZE];

  sfh = open( spath, O_RDONLY );
  dfh = open( dpath, O_RDONLY );
  if( sfh<0 || dfh<0 )
    bResult = false;
  else
  {
    int  n1, n2;

    while(1)
    {
      n1 = read( sfh, buf1, CMP_BUF_SIZE );
      n2 = read( dfh, buf2, CMP_BUF_SIZE );
      if( n1!=n2 )
      { bResult = false;
        break;
      }
      else if( n1==0 )
      { bResult = true;
        break;
      }
      else if( n1<0 )
      { bResult = false;
        break;
      }
      else if( memcmp( buf1, buf2, n1 )!=0 )
      { bResult = false;
        break;
      }
      continue;
    }
  }

  if( sfh>0 )
    close( sfh );
  if( dfh>0 )
    close( dfh );
  return bResult;
}


bool enum_callback_time_sync( const char *szDirSrc, const char *szDirDst, FF_CNTX *pffd, void *pData )
{
  char    spath[_MAX_PATH]; // src path
  char    dpath[_MAX_PATH]; // dst path
  int     flags = 0;
  struct stat     d_stat;
  struct timespec mtime, times[2];

  if( !szDirSrc )
    return true;  // continue

  if( !szDirDst )
    return true;  // continue

  // 源文件的路径
  strcpy( spath, szDirSrc );
  try_append_slash( spath );
  strcat( spath, pffd->m_fitem->d_name );
  //printf( "SRC file %s\n", spath );

  // 目标文件的路径
  strcpy( dpath, szDirDst );
  try_append_slash( dpath );
  strcat( dpath, pffd->m_fitem->d_name );
  //printf( "  @@ %s\n", dpath );

  // 检查目标文件是否存在
  if( stat( dpath, &d_stat )!= 0 )
  {
    printf( "%-128s   =>   %64s \n", spath, "" );
    return true; // next one
  }


  // 检查源文件是否是实际文件
  if( !S_ISREG(pffd->m_fstat.st_mode) )
  { //printf( "  SRC %s is not regular file.\n", spath );
    return true; // next one
  }
  // 检查目标文件是否是实际文件
  if( !S_ISREG(d_stat.st_mode) )
  { //printf( "  DST %s is not regular file.\n", dpath );
    return true; // next one
  }

  // 打印目录
  if( S_ISDIR(pffd->m_fstat.st_mode) )
  { //printf( "SRC_DIR  %s \n", szDirSrc );
    return true; // next one
  }
  if( S_ISDIR(d_stat.st_mode) )
  { //printf( "DST_DIR  %s \n", szDirDst );
    return true; // next one
  }

  // 判断源、目标文件是否相同
  if( (d_stat.st_size!=pffd->m_fstat.st_size) || 
      (file_is_same( spath, dpath )==false) )
  { 
    printf( "%-128s   <>   %s \n", spath, dpath );
    return true; // next one
  }

  // 内容相同时，作时间同步
  if( pffd->m_fstat.st_mtime!=d_stat.st_mtime )
  {
    if( pffd->m_fstat.st_mtime<d_stat.st_mtime )
    {
      mtime.tv_sec  = pffd->m_fstat.st_mtime;
      mtime.tv_nsec = 0; //pffd->m_fstat.st_mtime_nsec;
      times[0] = mtime;
      times[1] = mtime;
      //flags |= AT_SYMLINK_NOFOLLOW; 
      utimensat( AT_FDCWD, dpath, times, flags );
      //printf( "  Synchronize ( %s --> %s )\n", spath, dpath );
    }
    else
    {
      mtime.tv_sec  = d_stat.st_mtime;
      mtime.tv_nsec = 0; //pffd->m_fstat.st_mtime_nsec;
      times[0] = mtime;
      times[1] = mtime;
      //flags |= AT_SYMLINK_NOFOLLOW; 
      utimensat( AT_FDCWD, spath, times, flags );
      //printf( "  Synchronize ( %s <-- %s )\n", spath, dpath );
    }
  }
  return true; // next one
}


bool enum_callback_simple_check( const char *szDirSrc, const char *szDirDst, FF_CNTX *pffd, void *pData )
{
  char    spath[_MAX_PATH]; // src path
  char    dpath[_MAX_PATH]; // dst path
  int     flags = 0;
  struct stat d_stat;
  //struct timespec mtime, times[2];

  if( !szDirSrc )
    return true;  // continue
  if( !szDirDst )
    return true;  // continue

  // 源文件的路径
  strcpy( spath, szDirSrc );
  try_append_slash( spath );
  strcat( spath, pffd->m_fitem->d_name );
  //printf( "SRC file %s\n", spath );

  // 目标文件的路径
  strcpy( dpath, szDirDst );
  try_append_slash( dpath );
  strcat( dpath, pffd->m_fitem->d_name );
  //printf( "  @@ %s\n", dpath );

  // 检查目标文件是否存在
  if( stat( dpath, &d_stat )!=0 )    // if( _access( dpath, 0 )!=0 )
  { printf( "%-128s   ---->   %64s \n", spath, "" );
    return true; // next one
  }
  
  // 两者属性不同，report
  if( S_ISLNK(pffd->m_fstat.st_mode) != S_ISLNK(d_stat.st_mode) )
  { printf( "%-128s   <-L->   %s \n", spath, dpath );
    return true; // next one
  }

  if( S_ISDIR(pffd->m_fstat.st_mode) != S_ISDIR(d_stat.st_mode) )
  { printf( "%-128s   <-D->   %s \n", spath, dpath );
    return true; // next one
  }

  if( S_ISREG(pffd->m_fstat.st_mode) )
  {
    if( pffd->m_fstat.st_mode != d_stat.st_mode )
    { printf( "%-128s   <-m->   %s \n", spath, dpath );
      return true; // next one
    }

    if( pffd->m_fstat.st_size != d_stat.st_size )
    { printf( "%-128s   <-L->   %s \n", spath, dpath );
      return true; // next one
    }
    
    if( pffd->m_fstat.st_mtime != d_stat.st_mtime )
    {
      if( file_is_same( spath, dpath )==false )
        printf( "%-128s   <-!->   %s \n", spath, dpath );
      else  
        printf( "%-128s   <=T=>   %s \n", spath, dpath );
      return true; // next one
    }
  }

  return true; // next one
}


bool enum_callback_content_check( const char *szDirSrc, const char *szDirDst, FF_CNTX *pffd, void *pData )
{
  char    spath[_MAX_PATH]; // src path
  char    dpath[_MAX_PATH]; // dst path
  int     flags = 0;
  struct stat d_stat;
  struct timespec mtime, times[2];

  if( !szDirSrc )
    return true;  // continue
  if( !szDirDst )
    return true;  // continue

  // 源文件的路径
  strcpy( spath, szDirSrc );
  try_append_slash( spath );
  strcat( spath, pffd->m_fitem->d_name );
  //printf( "SRC file %s\n", spath );

  // 目标文件的路径
  strcpy( dpath, szDirDst );
  try_append_slash( dpath );
  strcat( dpath, pffd->m_fitem->d_name );
  //printf( "  @@ %s\n", dpath );

  // 检查目标文件是否存在
  if( stat( dpath, &d_stat )!=0 )    // if( _access( dpath, 0 )!=0 )
  { printf( "%-128s   =>   %64s \n", spath, "" );
    return true; // next one
  }

  // 检查源文件是否是实际文件
  if( !S_ISREG(pffd->m_fstat.st_mode) )
  { //printf( "  SRC %s is not regular file.\n", spath );
    return true; // next one
  }
  // 检查目标文件是否是实际文件
  if( !S_ISREG(d_stat.st_mode) )
  { //printf( "  DST %s is not regular file.\n", dpath );
    return true; // next one
  }

  // 检查源文件是否是目录
  if( S_ISDIR(pffd->m_fstat.st_mode) )
  { //printf( "  SRC %s is directory.\n", spath );
    return true; // next one
  }
  // 检查目标文件是否是目录
  if( S_ISDIR(d_stat.st_mode) )
  { //printf( "  DST %s is directory.\n", dpath );
    return true; // next one
  }
  
  // 判断源、目标文件内容是否相同
  if( (d_stat.st_size!=pffd->m_fstat.st_size) ||
      (file_is_same( spath, dpath )==false) )
  {
    printf( "%-128s   <>   %s \n", spath, dpath );
    return true; // next one
  }

/*
  // 内容相同时，作时间同步
  mtime.tv_sec  = pffd->m_fstat.st_mtime;
  mtime.tv_nsec = 0; //pffd->m_fstat.st_mtime_nsec;
  times[0] = mtime;
  times[1] = mtime;
  //flags |= AT_SYMLINK_NOFOLLOW; 
  utimensat( AT_FDCWD, dpath, times, flags );
  //printf( "  Synchronize ( %s => %s )\n", spath, dpath );
*/

  return true; // next one
}

#define MODE_CHK_SIMPLE   '0'
#define MODE_CHK_CONTENT  '1'
#define MODE_SYNC         '2'


int  main( int argc, char**argv )
{
  if( argc!=4 )
  { //             argv0  argv1  argv2    argv3
    printf("Usage:  %s    mode   src_dir  dst_dir \n", argv[0]);
    printf("  mode: 0--Simple check ,\n"
           "        1--Content check,\n"
           "        2--Time stamp Sync.\n");
    printf("For example: \n"
           "  %s  0  dir1  dir2 \n", argv[0]);
    return 1;
  }

  if( access(argv[2], 0)!=0 )
  { printf("Source directory not exist.\n");
    return 1;
  }

  if( access(argv[3], 0)!=0 )
  { printf("Target directory not exist.\n");
    return 1;
  }

  switch( argv[1][0] )
  {
    case  MODE_CHK_SIMPLE :
          // if one of the files not exist, report;
          // else if file-size is different, report;
          // else if time-stamp is different, report;
          // else ignore.
          FF_EnumFile( argv[2], "*", 128, enum_callback_simple_check, argv[3], NULL );
          break;

    case  MODE_CHK_CONTENT:
          // if one of the files not exist, ignore;
          // else if file-size is different, ignore;
          // else if content is same, ignore;
          // else report.
          FF_EnumFile( argv[2], "*", 128, enum_callback_content_check, argv[3], NULL );
          break;

    case  MODE_SYNC :
          // if one of the files not exist, ignore;
          // else if file-size is different, ignore;
          // else if time-stamp is same, ignore;
          // else if content is not same, igore;
          // else synchronize.
          // That is: do synchronize only when size/time/content are same.
          FF_EnumFile( argv[2], "*", 128, enum_callback_time_sync , argv[3], NULL ); // 时间同步：src-->dst
          break;

    default :
          printf("Mode error.\n");
          break;
  }

  return 0;
}

