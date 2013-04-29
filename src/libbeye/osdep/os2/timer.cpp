#include "config.h"
#include "libbeye/libbeye.h"
using namespace beye;
/**
 * @namespace   libbeye
 * @file        libbeye/osdep/os2/timer.c
 * @brief       This file contains implementation of timer depended part for OS/2-32.
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
#define INCL_DOSSEMAPHORES
#define INCL_DOSMISC
#define INCL_DOSDATETIME
#define INCL_DOSPROCESS
#include <os2.h>
#include <stddef.h>
#include <stdlib.h>

extern HEV      beyeSem;
static HTIMER   timerID = 0;
static TID      timerThread = 0;
static          timer_callback *user_callback = NULL;

static VOID _Syscall thread_callback( ULONG threadMsg )
{
  ULONG recv;
  UNUSED(threadMsg);
  while(1)
  {
    DosResetEventSem(beyeSem,&recv);
    DosWaitEventSem(beyeSem,SEM_INDEFINITE_WAIT);
    if(user_callback) (*user_callback)();
  }
}

unsigned    __FASTCALL__  __OsSetTimerCallBack(unsigned ms,timer_callback func)
{
   ULONG min_interval,max_interval,real_interval;
   int rc;

   DosQuerySysInfo(QSV_MIN_SLICE,QSV_MIN_SLICE,&min_interval,sizeof(ULONG));
   DosQuerySysInfo(QSV_MAX_SLICE,QSV_MAX_SLICE,&max_interval,sizeof(ULONG));
   real_interval = ms / min_interval;
   real_interval *= min_interval;
   if(real_interval < ms) real_interval += min_interval - 1;
   if(real_interval > max_interval) real_interval = max_interval-1;
   user_callback = func;
   rc = DosCreateThread(&timerThread,&thread_callback,0,0,0x1000);
   if(!rc) rc = DosSetPriority(PRTYS_THREAD,PRTYC_TIMECRITICAL,PRTYD_MINIMUM,timerThread);
   if(!rc)
   {
       if(DosStartTimer(real_interval,(HSEM)beyeSem,&timerID) == 0)
       {
	  return real_interval;
       }
   }
   return 0;
}
			     /* Restore time callback function to original
				state */
void   __FASTCALL__  __OsRestoreTimer(void)
{
  if(timerID) { DosStopTimer(timerID); timerID = 0; }
  if(timerThread) { DosKillThread(timerThread); timerThread = 0; }
  user_callback = 0;
}
