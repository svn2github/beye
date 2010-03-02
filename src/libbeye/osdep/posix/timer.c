/**
 * @namespace   libbeye
 * @file        libbeye/osdep/posix/timer.c
 * @brief       This file contains implementation of timer depended part for POSIX.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Nickols_K
 * @since       1999
 * @note        Development, fixes and improvements
**/
#include <sys/time.h>
#include <signal.h>
#include <stddef.h>

#include "libbeye/libbeye.h"

static timer_callback *user_func = NULL;
static struct itimerval otimer;
static void (*old_alrm)(int) = SIG_DFL;

static void my_alarm_handler( int signo )
{
  if(user_func) (*user_func)();
  UNUSED(signo);
}

unsigned  __FASTCALL__ __OsSetTimerCallBack(unsigned ms,timer_callback func)
{
   unsigned ret;
   struct itimerval itimer;
   user_func = func;
   getitimer(ITIMER_REAL,&otimer);
   old_alrm = signal(SIGALRM,my_alarm_handler);
   signal(SIGALRM,my_alarm_handler);
   itimer.it_interval.tv_sec = 0;
   itimer.it_interval.tv_usec = ms*1000;
   itimer.it_value.tv_sec = 0;
   itimer.it_value.tv_usec = ms*1000;
   setitimer(ITIMER_REAL,&itimer,NULL);
   getitimer(ITIMER_REAL,&itimer);
   ret = itimer.it_interval.tv_sec*1000 + itimer.it_interval.tv_usec/1000;
   if(!ret) __OsRestoreTimer();
   return ret;
}

                             /* Restore time callback function to original
                                state */
void  __FASTCALL__ __OsRestoreTimer(void)
{
  signal(SIGALRM,old_alrm);
  setitimer(ITIMER_REAL,&otimer,NULL);
}
