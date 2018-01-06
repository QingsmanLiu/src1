//#include "../src/car_config.h"
#include <sys/cdefs.h>
/*
#ifdef WIN32
  #include <winsock2.h>
  #include <windows.h>
#else
  #include <sys/socket.h>
  #include "../src/sockutil.h"
  #include "../src/timer.h"
  #include "../src/car_config.h"
#endif
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <memory.h>
#include <time.h>
#include <sys/timeb.h>
#include <errno.h>
#include <fcntl.h>
//#include <pthread.h>
#include <unistd.h>


int  ioctl( int fd, int ctrl, ... )
{
  return 0;
}

/*
int  usleep( unsigned long usec )
{
  Sleep( usec / 1000 );
  return 0;
}
*/

int  clock_gettime( clockid_t clk_id, struct timespec *tsp )
{
  struct timeb  ft;

  _ftime( &ft );
  tsp->tv_sec  = ft.time;
  tsp->tv_nsec = ft.millitm * 1000;

  return 0;
}


int  clock_settime( clockid_t clk_id, const struct timespec *pts )
{
  SYSTEMTIME  st;
  time_t      tm;
  struct tm  *pt;

  memset( &st, 0, sizeof(st) );
  tm = pts->tv_sec;
  if( clk_id == CLOCK_REALTIME )
  {
    pt = localtime( &tm );

    st.wYear   = pt->tm_year + 1900;
    st.wMonth  = pt->tm_mon + 1;
    st.wDay    = pt->tm_mday;
    st.wHour   = pt->tm_hour;
    st.wMinute = pt->tm_min;
    st.wSecond = pt->tm_sec;
    
    SetLocalTime( &st );
  }

  return 0;
}


int  gettimeofday( struct timeval *tv, struct timezone *tz ) 
{
  if( tv )
  {
    SYSTEMTIME  tm;
  
    GetLocalTime( &tm );
    tv->tv_sec  = ( tm.wHour * 60 + tm.wMinute ) * 60 + tm.wSecond;
    tv->tv_usec = tm.wMilliseconds * 1000;
  }
  
  if( tz )
  {
    /*
    _daylight : 1 -- ÏÄÁîÊ±(daylight-saving-time), 0 -- Standard time.
    _timezone : Difference in seconds between UTC and local time.
    _tzname[0]: Time-zone name from TZ environmental variable.
    _tzname[1]: Time-zone name for daylight-saving-time.
    */
    tz->tz_dsttime     = 0;
    tz->tz_minuteswest = _timezone / 60;
  }
  return 0;
}

/*
int XLOGD( const char *fmt, ... )
{
  va_list   ap;
  int       n;

  va_start( ap, fmt );
//Printf( "[car-daemon]: " );
  n = vprintf( fmt, ap );
  va_end( ap );
  return n;
}


int XLOGE( const char *fmt, ... )
{
  va_list   ap;
  int       n;

  va_start( ap, fmt );
//Printf( "[car-daemon]: " );
  n = vprintf( fmt, ap );
  va_end( ap );
  return n;
}

*/

int reboot( int reboot_type ) { return 0; }
int __reboot( int magic1, int magic2, int cmd, void *str ) { return 0; }



