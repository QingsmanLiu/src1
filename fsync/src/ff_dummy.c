/*
  在WIN32上仿真Linux的:
      stat(), 
      opendir(), 
      readdir(), 
      closedir(), 
      utimensat()
*/

#include "fsync.h"


// Data type of Windows 
typedef unsigned int _dev_t;
typedef unsigned short _ino_t;
typedef long _off_t;

struct _stat 
{
  _dev_t st_dev;
  _ino_t st_ino;
  unsigned short st_mode;
  short st_nlink;
  short st_uid;
  short st_gid;
  _dev_t st_rdev;
  _off_t st_size;
  time_t st_atime;
  time_t st_mtime;
  time_t st_ctime;
};

// COnstants for windows
#define _S_IFMT         0170000         /* file type mask */
#define _S_IFDIR        0040000         /* directory */
#define _S_IFCHR        0020000         /* character special */
#define _S_IFIFO        0010000         /* pipe */
#define _S_IFREG        0100000         /* regular */
#define _S_IREAD        0000400         /* read permission, owner */
#define _S_IWRITE       0000200         /* write permission, owner */
#define _S_IEXEC        0000100         /* execute/search permission, owner */


// _stat() on windows
extern int __cdecl _stat(const char *, struct _stat *);


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

int  stat( const char *path, struct stat *stbuf )   // Get status information on a file.
{
  struct _stat  stb;

  memset( &stb, 0, sizeof(stb) );
  memset( stbuf, 0, sizeof(*stbuf) );

  if( _stat( path, &stb ) )
    return -1;  // failure
  
  stbuf->st_size = stb.st_size;
  stbuf->st_atime      = stb.st_atime; // access time
  stbuf->st_mtime      = stb.st_mtime; // modification time
  stbuf->st_ctime      = stb.st_ctime; // create time

  // 文件权限和文件类型信息, See S_IFDIR, S FLNK, S_ISDIR(), S_ISLNK()
  if( stb.st_mode & _S_IFDIR )
    stbuf->st_mode |= S_IFDIR;
  if( stb.st_mode & _S_IFREG )
    stbuf->st_mode |= S_IFREG;
  if( stb.st_mode & _S_IREAD )
    stbuf->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
  if( stb.st_mode & _S_IWRITE )
    stbuf->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
  if( stb.st_mode & _S_IEXEC )
    stbuf->st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;

  return 0; // success
}


// Dummy
DIR * opendir(const char *dirpath)
{
  DIR  d, *pd;
  char drive[6];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];
  char wildpath[_MAX_PATH];

  strcpy( wildpath, dirpath );
  try_append_slash( wildpath );
  strcat( wildpath, "*.*" );

  d.h = _findfirst( wildpath, &d.ffd );
  if( d.h==-1 )
    return NULL;

  pd = (DIR *)malloc( sizeof(DIR) );
  *pd = d;
  return pd;
}

struct dirent * readdir(DIR* dirp)
{
  struct dirent de, *pde;

  if( _findnext( dirp->h, &dirp->ffd )!=0 )
    return NULL;

  memset( &de, 0, sizeof(de) );

  if( dirp->ffd.attrib & FA_SUBDIR )
    de.d_type |= S_IFDIR;
  if( dirp->ffd.attrib & S_IFREG )
    de.d_type |= S_IFREG;
  de.d_type |= S_IRWXU | S_IRWXG | S_IRWXO;

  strcpy( de.d_name, dirp->ffd.name );
  
  pde = (struct dirent *) malloc( sizeof(*pde) );
  *pde = de;
  return pde;
}

int  closedir( DIR *dirp )
{
  _findclose( dirp->h );
  free( dirp );
  return 0;
}

int utimensat( int fd, const char *path, const struct timespec ts[2], int flags )
{
  struct utimbuf times;

  times.actime   = ts[0].tv_sec;
  times.modtime  = ts[1].tv_sec;
  return _utime( path, &times );
}



//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
// Others
//---------------------------------------------------------------------------------
/*
#define WILD_LIST_SEPARATOR   ';'


void    normalize_slash( char *path );
void    try_append_slash( char *path );

void    splitpath( const char *path, char *dir, char *fname, char *ext );

const  char *get_file_name( const char *path );
const  char *get_file_ext( const char *path );
extern void  str_trim( char *pStr, char c );
bool    is_wild_path( const char *path );
int     scan_wild_count( const char *wild_str );
int     normalize_wild_string( char *wild_str1 );
bool    is_similar_wild_string( const char *wild_str1, const char *wild_str2 );
bool    is_valid_dir_name( const char *dir );       // not a path, just a dir-name only
bool    is_valid_file_name( const char *fname );    // not a path, just file-name only
bool    is_valid_path( const char *path );;         // path
bool    is_full_path( const char *path );           // "c:\"
bool    is_path_from_root( const char *path );      // "\user\src", "c:\user\src"
bool    is_dir_exist_by_dir( const char *dir );
bool    is_dir_exist_by_path( const char *path );
bool    create_directory( const char *dir );
bool    create_dir_on_path( const char *path );
void    make_asb_path( char *pFull, const char *pBase, const char *pRel );
bool    make_relative_path( const char *pPath, const char *pBase, char *pRel );
int     open_with_path_create( const char *path, int mode, int share );
FILE   *fopen_with_path_create( const char *path, const char *mode );

bool    enum_files_by_wildlist( const char *szDirSrc, const char *szDirdst, const char *wild_list, 
                                unsigned depth, TEnumFileCB pCallback, void *pData );
bool    match_wildlist( const char *str, char *pWildList );
bool    wild_match_ext( const char *str, const char *wildcards, char *matched_strs );
void    translate_wild_string( const char *matched_strs, const char *wild_str, char *str );
*/


/*
const char *get_file_name( const char *path )
{
  const char *ptr = path + strlen( path );

  while( --ptr >= path )
  {
    if( *ptr == STD_SLASH[0] || *ptr == EXT_SLASH[0] )
      break;
  }
  return ++ptr;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
const char *get_file_ext( const char *path )
{
  const char *ptr1 = path + strlen( path );
  const char *ptr  = ptr1;

  while( --ptr >= path )
  {
    if( *ptr == '.' )
      return ptr;

    if( *ptr == STD_SLASH[0] || *ptr == EXT_SLASH[0] )
      break;
  }
  return ptr;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
void  str_trim( char *pStr, char c )
{
  char  *p;

  for( p = pStr + strlen(pStr) - 1; p >= pStr && *p == c; p-- )
     *p = EOS;

  for( p = pStr; *p != EOS && *p == c; p++ ) { }

  if( p != pStr )
    memmove( pStr, p, strlen(p)+1 );
}
*/


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  is_full_path( const char *path )
{
  return ( path[0]==STD_SLASH[0] );
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  is_path_from_root( const char *path )
{
  return ( path[0]==STD_SLASH[0] );
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  is_wild_path( const char *path )
{
    char    *ptr;
    char    dir[_MAX_DIR];
    char    fname[_MAX_FNAME];
    char    ext[_MAX_EXT];

    if( strlen( path ) == 0 )
        return false;
    
    splitpath( path, dir, fname, ext );
    strcat( fname, ext );

    ptr = fname;
    while( *ptr != EOS )
    {
      if( *ptr == '*' || *ptr == '?' )
        return true;
      ++ptr;
    }
    return false;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  is_valid_dir_name( const char *dir )
{
  const char    *p = dir;
  int           c;

  while( c = *p++, c != EOS )
  {
    if( c==EXT_SLASH[0] )
      return false;

    if( c=='?' || c=='*' )
      return false;
      
    if( c == '<' || c == '>' )
      return false;

  //if( c == ':' || c == ';' )
  //    return false;

    if( c == '|' || c == '\"')
      return false;
  }
  
  return true;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  is_valid_file_name( const char *fname )
{
  const char    *p = fname;
  int           c;

  while( c = *p++, c != EOS )
  {
    if( c == '\\' || c == '/' )
      return false;
    
    if( c == '?' || c == '*' )
      return false;

    if( c == '<' || c == '>' )
      return false;

  //if( c == ':' || c == ';' )
  //    return false;

    if( c == '|' || c == '\"')
      return false;
  }
  
  return true;  
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  is_valid_path( const char *path )
{
  char    dir[_MAX_DIR];
  char    fname[_MAX_FNAME];
  char    ext[_MAX_EXT];

  splitpath( path, dir, fname, ext );

  // check directory
  if( ! is_valid_dir_name( dir ) )
    return false;

  strcat( fname, ext );
  return is_valid_file_name( fname );
}
*/

//---------------------------------------------------------------------------------
// Note: "c:\src\debug" and "c:\src\debug\" is a same directory.
//---------------------------------------------------------------------------------
/*
bool  is_dir_exist_by_dir( const char *dir )
{
  return (access(dir, 0)==0 );
}
*/

//---------------------------------------------------------------------------------
// path = "c:\src\debug"  ==> check "c:\src\" only
// path = "c:\src\debug\" ==> check "c:\src\debug\"
//---------------------------------------------------------------------------------
/*
bool  is_dir_exist_by_path( const char *path )
{
  return (access(path, 0)==0 );
}
*/

//---------------------------------------------------------------------------------
// Create the directory one by one according to the directory-path.
//---------------------------------------------------------------------------------
/*
bool  create_directory( const char *dir )
{
  char  path[_MAX_PATH];
  char  *p1, *p2;
  int   ch;

  if( ! is_path_from_root( dir ) )
    make_asb_path( path, NULL, dir );
  else
    strcpy( path, dir );
  try_append_slash( path );

  p1 = &path[1];
  while( *p1 != EOS )
  {
    p2 = p1;
    while( *p2 != EOS && *p2 != '/' && *p2 != '\\' ) p2++;
    
    ch  = *p2;  // save the character at p2.
    *p2 = EOS;

  #ifdef WIN32
    if( mkdir( path ) )
  #else
    if( mkdir( path, 0 ) )
  #endif
    {
      if( ! is_dir_exist_by_dir( path ) )
        return false;
    }
    *p2 = ch;   // restore the character at p2.

    if( ch == EOS )
      break;

    p1 = p2 + 1;
  }

  return true;
}
*/

//---------------------------------------------------------------------------------
// path = "c:\src\debug"  ==> create "c:\src\"
// path = "c:\src\debug\" ==> create "c:\src\debug\"
//---------------------------------------------------------------------------------
/*
bool  create_dir_on_path( const char *path )
{
  char    buff[_MAX_PATH];
  char    dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];

  splitpath( path, dir, fname, ext  );
  makepath(  buff, dir, "", "" );
  return create_directory( buff );
}
*/                   


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
void  make_asb_path( char *full, const char *base, const char *relative )
{
  #define  MIN_PATH_LEN	  2
  int	  i, j, n;
  char  base_dir[_MAX_PATH];

  if( base == NULL || base[0] == EOS )
    getcwd( base_dir, sizeof( base_dir ) );
  else
    make_asb_path( base_dir, NULL, base );
  try_append_slash( base_dir );

  if( relative == NULL || relative[0] == EOS )
  {
    strcpy( full, base_dir );
    strcat( full, STD_SLASH "*" );
    return;
  }

  // make the full path
  if( relative[1] == ':' )
  {
    // path with drive
    if( relative[2] == '/' || relative[2] == '\\' )
    { // with drive and started from root.
      strcpy( full, relative );
    }
    else
    { // Drive is specified. Path is not started from root directory.
      int  drive = tolower(relative[0]) - 'a' + 1;
      getdcwd( drive, base_dir, sizeof(base_dir) );
      try_append_slash( base_dir );

      strcpy( full, base_dir );
      strcat( full, &relative[2] );
    }
  }
  else if( relative[0] == '/' || relative[0] == '\\' )
  {
    // suppose the drive name is the same as the base.
    strncpy( full, base_dir, 2 );
    strcpy( full + 2, relative );
  }
  else
  { // relative path
    strcpy( full, base_dir );
    strcat( full, relative );
  }

  // simplify the double dots in the fullpath
  i = 0;
  n = strlen( full );
  while( i < n && full[i] != '.' ) i++;
  j = i;

  while( i < n )
  {
    if( full[ i ] != '.' )
    {
      full[ j++ ] = full[ i++ ];
      continue;
    }
    
    if( i + 1 >= n )
	  break;
      
    if( full[ i + 1 ] == '.' )
    {
      j--;
      while( (j-1) > MIN_PATH_LEN && full[j-1] != '\\' && full[j-1] != '/' ) j--;
      if( (i+2) < n && ( full[i+2] == '\\' || full[i+2] == '/' ) )
	i += 3;
      else
	i += 2;
    }
    else if( full[i+1] == '\\' || full[i+1] == '/' )
    {
      i += 2;
    }
    else
    {
      full[ j++ ] = full[ i++ ];
    }
  }

  full[ j ] = 0;
}
*/

//---------------------------------------------------------------------------------
// Get the relative path of pPath[] which is relative to the pBaseDir[].
// Both pPath[] and pBaseDir[] should be full path with the same drive.
//---------------------------------------------------------------------------------
/*
bool  make_relative_path( const char *pPath, const char *pBaseDir, char *pRelPath )
{
    char    full[_MAX_PATH];
    char    base[_MAX_PATH];
    int     c1, c2;
    char    *pa1, *pa2, *pb1, *pb2;
    int     result, slash_cnt;

    if( ! is_path_from_root( pPath ) )
      return false;

    if( ! is_path_from_root( pBaseDir ) )
      return false;

    if( tolower( pPath[0] ) != tolower( pBaseDir[0] ) )
      return false;

    strcpy( full, pPath );
    strcpy( base, pBaseDir );
    try_append_slash( base );

    // skip the same part 
    pa1 = full + 3;
    pb1 = base + 3;
    while( *pa1 != EOS && *pb1 != EOS )
    {
      pa2 = pa1 + 1;
      pb2 = pb1 + 1;
      
      while( c1 = *pa2, c1 != EOS && c1 != '/' && c1 != '\\' ) pa2++;
      while( c2 = *pb2, c2 != EOS && c2 != '/' && c2 != '\\' ) pb2++;
      *pa2 = EOS;
      *pb2 = EOS;
      result = strcmp( pa1, pb1 );
      *pa2 = c1;
      *pb2 = c2;

      if( result != 0 )
        break;
      
      pa1 = pa2 + 1;
      pb1 = pb2 + 1;
    }

    // depath of difference
    pRelPath[0] = EOS;
    slash_cnt = 0;
    while( *pb2 != EOS )
    {
      c2 = *pb2++;
      if( c2 == '\\' || c2 == '/' )
      {
        if( slash_cnt )
          strcat( pRelPath, ".." STD_SLASH );
        slash_cnt++;
      }
    }

    strcat( pRelPath, pa1 );
    return true;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
int  open_with_path_create( const char *szFile, int mode, int share )
{
  if( ! create_dir_on_path( szFile ) )
    return (-1);
  
  return open( szFile, mode, share ); 
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
FILE * fopen_with_path_create( const char *szFile, const char *mode )
{
  if( ! create_dir_on_path( szFile ) )
    return NULL;
  
  return fopen( szFile, mode ); 
}
*/



//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
int  normalize_wild_string( char *wild_str1 )
{ 
  // Returns total number of wildcards in the string.
  // Normalization: "?*?*...*?*?" ===> "?*?...*??"
  char    w1;
  char    *ptr;
  int     wild_flag1;
  int     wild_cnt = 0;

  ptr = wild_str1;
  while( w1 = *wild_str1, w1 != EOS )
  {
    switch( w1 )
    {
      case  '?' :  
            ++wild_cnt;
            *ptr++ = w1;
            wild_flag1 |= WILD_QUEST;
            break;

      case  '*' :  
            if( ( wild_flag1 | WILD_STAR ) != 0 )
            { ++wild_cnt;
              *ptr++ = w1;
              wild_flag1 |= WILD_STAR;
            }
            break;

      default   :  
            *ptr++ = w1;
            wild_flag1 = WILD_NONE;
            break;
    }
    
    ++wild_str1;
  }

  return wild_cnt;
}
*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
bool  is_similar_wild_string( const char *wild_str1, const char *wild_str2 )
{
  // Note: The two wild string must be normalized.
  char    w1, w2;
  
  while( 1 )
  {
    for( ; w1 = *wild_str1, w1 != EOS && w1 != '?' && w1 != '*'; wild_str1++ ) { };
    for( ; w2 = *wild_str2, w2 != EOS && w2 != '?' && w2 != '*'; wild_str2++ ) { };
    if( w1 != w2 )
      return false;

    if( w1 == EOS ) // && w2 == EOS
      break;
    
    wild_str1++;
    wild_str2++;
  }
  return true;
}
*/

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/*
void  translate_wild_string( const char *matched_strs, const char *wild_str, char *out_str )
{
  // matched_strs is a string gotten from calling wild_match_ext(). It's format is:
  // each string in it being terminated with '\0' and the last string being terminated
  // with double '\0'.
  int     len;
  char    w;

  for( ; *wild_str != EOS; wild_str++ )
  {
    w = *wild_str;
    switch( w )
    {
      case '*' :
      case '?' :  len = strlen( matched_strs );
                  if( len > 0 )
                  { strcpy( out_str, matched_strs );
                    out_str += len;
                    matched_strs += len + 1; 
                  }
                  break;

      default  :  *out_str++ = w;
                  break;
    }
  }

  *out_str = EOS;
}
*/ 

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
char *strndup( const char *str, size_t nBuyes )
{
  char  *ptr = malloc( nBuyes + 1 );
  memcpy( ptr, str, nBuyes );
  ptr[ nBuyes ] = EOS;
  return ptr;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
int  scan_wild_count( const char *wild_str )
{
  int   count = 0;
  
  for( ; *wild_str != EOS; wild_str++ )
  {
    if( *wild_str == '?' || *wild_str == '*' )
      ++count;
  }
  return count;
}
*/


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  wild_match_ext( const char *str, const char *wildcards, char *szMatchString )
{
  // To get the matched sub-strings correctly, it is required that 
  //   match_wildcard() is called first and it's result is true.
  const char  *pDotInWild = NULL;
  const char  *pStr1;
  const char  *pStr2;
  const char  *pWild1;
  const char  *pWild2;
  const char  *pSubStr, *pSubKey, *ptr;
  int         nKeyLen, nStrLen, least_match_len;
  char        w, c;
  int         i, forward_cnt, backward_cnt;
  int         wildcard_cnt; 
  char      **pMatchStrs;
  char        szTmp[4];
  char        *pStr;

  if( str[0] == EOS || wildcards[0] == EOS ) 
    return false;

  wildcard_cnt = scan_wild_count( wildcards );
  if( wildcard_cnt == 0 )
    pMatchStrs = NULL;
  else
  {
    pMatchStrs = calloc( wildcard_cnt, sizeof(char *) );
    for( i = 0; i < wildcard_cnt; i++ )
      pMatchStrs[i] = NULL;
  }
  forward_cnt  = 0;
  backward_cnt = 0;

  pStr1  = str;
  pStr2  = pStr1 + strlen( str );
  pWild1 = wildcards;
  pWild2 = pWild1 + strlen( wildcards );
  
  for( pDotInWild = pWild2 - 1; pDotInWild >= pWild1; pDotInWild-- )
  {
    if( *pDotInWild == '.' )
      break;
  }
  if( pDotInWild < pWild1 )
    pDotInWild = NULL;

  while( true )
  {
    // try to match at the end. 
    if( pWild2[-1] != '*' )
    { 
      w = tolower( pWild2[-1] );
      c = tolower( pStr2[-1] );
      if( w != '?' && w != c )
        break; // failure
      
      if( w == '?' && pMatchStrs )
      { szTmp[0] = c;
        szTmp[1] = EOS;
        pMatchStrs[ wildcard_cnt - 1 - backward_cnt++ ] = strdup( szTmp );
      }
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
        break; // failure
      
      if( w == '?' && pMatchStrs )
      { szTmp[0] = c;
        szTmp[1] = EOS;
        pMatchStrs[ forward_cnt++ ] = strdup( szTmp );
      }
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
        if( ptr == pDotInWild )
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
      if( ! pSubStr || nStrLen < least_match_len )
        break;  // failure

      if( pMatchStrs )
      {
        int   index  = wildcard_cnt - 1 - backward_cnt++;
        char  *p_str = nKeyLen ? ( (char*)pSubStr + nStrLen ) : (char*)pSubStr; 
        int   slen   = nKeyLen ? ( pStr2 - p_str ) : nStrLen;
        p_str = strndup( p_str, slen );
        pMatchStrs[index] = p_str; 
        for( i = least_match_len - 1; i >= 0 ; i-- )
        {
          szTmp[0] = p_str[0];
          szTmp[1] = EOS;
          strcpy( p_str + i, p_str + i + 1 );
          pMatchStrs[--index] = strdup( szTmp );
          backward_cnt++;
        }
      }
      pStr2  = pSubStr;
      pWild2 = pSubKey;
    }
    if( pStr1 >= pStr2 || pWild1 >= pWild2 )
      break;
    
   #if 0
    // code for dictionary
    if( wild_mode != FOR_FILE_NAME )
    {
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
          if( *ptr == '*' || *ptr == '?' )
            break;
        }
        pSubKey = pWild1;
        nKeyLen = ptr - pSubKey;

        nStrLen = nKeyLen;
        pSubStr = forward_locate_substr( pSubKey, &nStrLen, pStr1, pStr2 );
        if( ! pSubStr || nStrLen < least_match_len )
          break; // failure
      }

      if( pMatchStrs )
      {
        char  *p_str = nKeyLen ? (char*)pStr1 : (char*)pSubStr; 
        int   slen   = nKeyLen ? ( pSubStr - pStr1 ) : nStrLen;
        p_str = strndup( p_str, slen );
        pMatchStrs[forward_cnt++] = p_str; 
        for( i = 0; i < least_match_len; i++ )
        {
          szTmp[0] = p_str[ slen - least_match_len ];
          szTmp[1] = EOS;
          strcpy( p_str + slen - least_match_len, p_str + slen - least_match_len + 1 );
          pMatchStrs[forward_cnt++] = strdup( szTmp );
        }
      }
      pStr1  = pSubStr + nStrLen;
      pWild1 = pSubKey + nKeyLen;
    
      if( pStr1 >= pStr2 || pWild1 >= pWild2 )
        break;
    }
  #endif
  }
  
  // Output the matched sub-strings. Each string is terminated with zero. The last
  // string is terminated with doubled zeros.
  szMatchString[0] = EOS;
  pStr = szMatchString;
  if( ( forward_cnt + backward_cnt ) == wildcard_cnt && pStr1 == pStr2 && pWild1 == pWild2 ) 
  {
    for( i = 0; i < wildcard_cnt; i++ )
    {
      strcpy( pStr, pMatchStrs[i] );
      pStr += strlen( pStr ) + 1; 
    }
  }
  *pStr = EOS;

  // free memory
  for( i = 0; i < wildcard_cnt; i++ )
  {
    if( pMatchStrs[i] )
      free( pMatchStrs[i] );
  }

  if( wildcard_cnt )
     free( pMatchStrs );

  return true;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
bool  match_wildlist( const char *szFileName, char *pWildList )
{
  char    *p1, *p2;
  int     c;

  p1 = pWildList;
  while( *p1 != EOS )
  {
    for( p2 = p1 + 1; *p2 != EOS && *p2 != WILD_LIST_SEPARATOR; p2++ ) { };

    c   = *p2 ;
    *p2 = EOS;

    if( match_wildcard( szFileName, p1 ) )
      return true;

    *p2 = c;
    p1  = p2 + 1;
  }

  return false;
}
*/

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
/*
typedef struct tagTEnumWildListData
{
  TEnumFileCB  pCallback;
  void        *pData;
  char        *pWildList;

} TEnumWildListData;


static  bool  enum_wild_list_callback( const char *szDirSrc, const char *szDirdst, TFileFindData *pfi, void *pUserData )
{
  TEnumWildListData  *pData = pUserData;
  
  if( match_wildlist( pfi->m_fitem.d_name, pData->pWildList ) )
    return pData->pCallback( szDirSrc, szDirdst, pfi, pData->pData );
  
  return true; // continue finding;
}

  
bool  enum_files_by_wildlist( const char *szDirSrc, const char *szDirDst, const char *szWildList, 
                              unsigned depth, TEnumFileCB pCallback, void *pUserData )
{
  bool                result;
  TEnumWildListData   wild_list_data;

  wild_list_data.pCallback = pCallback;
  wild_list_data.pData     = pUserData;
  wild_list_data.pWildList = strdup( szWildList );
  if( ! wild_list_data.pWildList )
    return false;

  result = enum_files_by_wildcard( szDirSrc, szDirDst, "*", depth,
                   enum_wild_list_callback, &wild_list_data );
  return result; // true -- totally enumerated. false -- stopped by user.
}
*/

//---------------------------------------------------------------------------------
// The following code is to implement findfirst(), findnext(), splitpath()
// makepath() for Linux Platform.
//---------------------------------------------------------------------------------
/*
void splitpath( const char *path, char *dir, char *fname, char *ext )
{
  const char  *pSrc, *ptr;
  char        *pDst;
  char        c;
  int         cnt;

  pSrc = path;

  // scan directory
  pDst = dir;
  while( 1 )
  {
    ptr = pSrc;
    while( c = *ptr++, c!=EOS && c!=STD_SLASH[0] ) { };
    if( c == EOS )
      break;
    cnt = ptr - pSrc;
    
    if( pDst )
    { memmove( pDst, pSrc, cnt );
      pDst += cnt;
    }
    pSrc = ptr;
  }
  *pDst = EOS;

  // scan file name
  pDst = fname;
  ptr  = pSrc;
  while( c = *ptr, c != EOS && c != '.' )
  {
    if( pDst )
      *pDst++ = c;
    ptr++;
  }

  // scan file extension
  if( ext )
    strcpy( ext, ptr );
}


void makepath( char *path, const char *dir, const char *fname, const char *ext )
{
  path[0] = 0; // EOS;

  if( dir )
    strcat( path, dir );
  if( fname )
    strcat( path, fname );
  if( ext )
    strcat( path, ext );
}
*/



