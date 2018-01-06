//=================================================================================
// File     : "dir_util.c"
// Purpose  : Directory Utility
// Author   : Qing.Liu
//=================================================================================
#ifndef  __Dir_Util_H__
#define  __Dir_Util_H__


//---------------------------------------------------------------------------------
// Include headers
//---------------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/utime.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h> 
//#include <curses.h>   // conio.h
#include <assert.h>


//---------------------------------------------------------------------------------
// Constant Definitions
//---------------------------------------------------------------------------------
#define false                 0
#define true                  1
                              
#define bool                  int
#define U64                   unsigned long long
#define S64                   signed long long
#define U32                   unsigned long
#define S32                   signed long
#define U16                   unsigned short
#define S16                   signed short
#define U8                    unsigned char
#define S8                    signed char
#define EOS                   0
#define EOL                   '\n'


#ifdef WIN32
  #define STD_SLASH           "\\"
  #define EXT_SLASH           "/"
#else
  #define STD_SLASH           "/"
  #define EXT_SLASH           "\\"
#endif

#ifdef WIN32
  #define FA_NORMAL           FILE_ATTRIBUTE_NORMAL               
  #define FA_RDONLY           FILE_ATTRIBUTE_READONLY             
  #define FA_HIDDEN           FILE_ATTRIBUTE_HIDDEN               
  #define FA_SYSTEM           FILE_ATTRIBUTE_SYSTEM               
  #define FA_SUBDIR           FILE_ATTRIBUTE_DIRECTORY            
  #define FA_ARCH             FILE_ATTRIBUTE_ARCHIVE              

//#define getdcwd(drv,buf,len)     _getdcwd(drv,buf,len)

  #define O_RDONLY            _O_RDONLY
  #define O_WRONLY            _O_WRONLY
  #define O_RDWR              _O_RDWR  
  #define O_BINARY            _O_BINARY
  #define O_TEXT              _O_TEXT  
  #define O_CREAT             _O_CREAT 
  #define O_EXCL              _O_EXCL  
  #define O_TRUNC             _O_TRUNC 
  #define O_APPEND            _O_APPEND
                              
  #define S_IREAD             _S_IREAD 
  #define S_IWRITE            _S_IWRITE
#endif

//---------------------------------------------------------------------------------
// Constant or Structure Member Definitions for Platform Compatiblity
//---------------------------------------------------------------------------------
#ifdef WIN32
  #undef  _MAX_PATH
  #undef  _MAX_DIR 
  #undef  _MAX_FNAME
  #undef  _MAX_EXT  
#endif

#define  _MAX_PATH            1024
#define  _MAX_DIR             1024
#define  _MAX_FNAME           1024
#define  _MAX_EXT             1024


//---------------------------------------------------------------------------------
// Find file context
//---------------------------------------------------------------------------------
typedef struct tagFF_CNTX
{
  // 通配符
  char           m_wild[32];

  // 当前目录
  char           m_dir[_MAX_PATH];
  DIR           *m_pdir;    // handle of directory

  // 目录中的文件
  struct stat    m_fstat;   // file stat  in the directory
  struct dirent *m_fitem;   // file entry in the directory

} FF_CNTX;


//---------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
typedef bool  (*FF_CALLBACK)( const char *szDirSrc, const char *szDirDst, FF_CNTX *pfi, void *pData );

FF_CNTX * FF_FindOpen( const char *szDir, const char *szWild );
bool      FF_FindFile ( FF_CNTX *pffd );
bool      FF_FindClose( FF_CNTX *pffd );
bool      FF_EnumFile( const char *szDirSrc, const char *wild, 
                       unsigned depth, FF_CALLBACK pCallback, 
                       const char *szDirDst, void *pData );

bool  match_wildcard( const char *str, const char *wildcards );



//=======================================================================================================
#endif //__Dir_Util_H__



/*
struct dirent
{
  unsigned short int d_reclen;
  unsigned char      d_type;
  char               d_name[256];
};

extern DIR   *opendir ( const char *name );   // NULL -- fail
extern int    closedir ( DIR *dirp );         // 0 if successful, -1 if not.
extern struct dirent   *readdir( DIR *dirp );
extern struct dirent64 *readdir64( DIR *dirp );
*/

/*

fopen
fclose
fread
fwrite
fflush
fseek
fgetc
getc
getchar
fputc
putc
putchar
fgets
gets
printf
fprintf
sprintf
scanf
fscanf
sscanf


fsetpos : 设置文件流的当前(读写)位置。
ftell   : 返回文件流当前(读写)位置的偏移值。
rewind  : 重置文件流里的读写位置。
freopen : 重新使用一个文件流。
setvbuf : 设置文件流的缓冲机制。
remove  : 相当于unlink函数，但如果它的path参数是一个目录的话，其作用就相当于rmdir函数。


#include <stdio.h>
int   fileno( FILE *fp ) ;
FILE *fdopen( int fildes, const char *mode );

#include <unistd.h>
size t write( int fildes, const void *buf, size t nbytes );
size t read( int fildes, void *buf, size t nbytes );
int    close( int fildes );
int    ioctl( int fi1des, int cmd, ... );

#include <unistd.h>
#include <sys/types.h>
off_t lseek( int fildes, off_t offset, int whence );

#include <unistd.h>
#include <sys/stat.h>
#include <sys/ types . h>
int fstat( int fildes, struct stat *buf );      // Get information about an open file.
int stat( const char *path, struct stat *buf );
int lstat( const char *path, struct stat *buf );

#include <unistd.h>
int dup( int fildes );
int dup2( int fildes, int fildes2 );

#include <linux/utime.h>
int futime( int handle, struct utimbuf *filetime );           // Sets modification time on an open file.
int utime( unsigned char *filename, struct utimbuf *times );   // Set the file modification time.
int utimensat( int fd, const char *path, const struct timespec times[2], int flags );

void touch( )
{
  int aflag = 0, mflag = 0, flags = 0;
  struct timespec atime, times[2];

  case 'l': flags |= AT_SYMLINK_NOFOLLOW; break;

  if ((mflag == 0) && (aflag == 0))
    aflag = mflag = 1;

  if (aflag)
    times[0] = atime;
  else
    times[0].tv_nsec = UTIME_OMIT;

  if (mflag)
    times[1] = atime;
  else
    times[1].tv_nsec = UTIME_OMIT;

  utimensat( AT_FDCWD, file, times, flags ); 
}
  

#include <sys/stat.h>
int chmod( const char *path, mode t mode );               // 改变文件或目录的访问权限

#include <unistd.h>
int chown( const char *path, uid_t owner, gid_t group );  // 改变一个文件的属主。

#include <unistd.h>
int unlink( const char *path );                           // 减少文件的链接数，当文件的链接数减少到零时，这个文件就会被删除。
int link( const char *pathl, const char *path2 );         // 创建一个文件的新链接。
int symlink( const char *pathl, const char *path2 );      // 创建符号链接


#include <sys/stat.h>
int mkdir( const char *path , mode_t mode );
int rmdir( const char *path );

#include <unistd.h>
int   chdir( const char *path );
char *getcwd( char *buf, size_t size );


#include <sys/types.h>
#include <dirent.h>
DIR           *opendir( const char *name );
struct dirent *readdir( DIR *dirp );
long int       telldir( DIR *dirp );
void           seekdir( DIR *dirp, long int loc );
int            closedir( DIR *dirp );


#include <stdio.h>
char *strerror( int errno );
void perror( const char *s );

#include <fcntl.h>
int fcntl( int fildes, int cmd );
int fcntl( int fildes, int cmd, long arg );

#include <sys/mman.h>
void *mmap( void *addr, size t len, int prot, int flags, int fildes , off_t off );
int   msync( void *addr, size_t len, int flags );
int   munmap( void *addr , size t len );  // un-map


  struct stat statbuf;
  rnode t modes;

  stat( "filenarne", &statbuf );
  modes = statbuf.st mode;

  if( !S ISDIR(modes) && (modes&S_IRWXU)==S IXUSR )
  {
  }

*/

