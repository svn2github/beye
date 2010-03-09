/**
 * @namespace   libbeye
 * @file        libbeye/osdep/os2/os_dep.c
 * @brief       This file contains implementation of OS depended part for OS/2-32.
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
 *
 * @author      Mauro Giachero
 * @since       11.2007
 * @note        Added __get_home_dir() and some optimizations
**/
#define INCL_SUB
#define INCL_DOSSIGNALS
#define INCL_DOSPROCESS
#define INCL_DOSMODULEMGR
#define INCL_DOSSEMAPHORES
#define INCL_DOSERRORS
#define INCL_DOSEXCEPTIONS
#include <os2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libbeye/libbeye.h"

static char rbuff[FILENAME_MAX+1];
static char rbuff2[FILENAME_MAX+1];
static char _home_dir_name[FILENAME_MAX + 1];

static bool __c__break = false;

HEV beyeSem;

#if !(defined( __DISABLE_MMF ) || defined( __DISABLE_LOWLEVEL_MMF))
extern ULONG __FASTCALL__ PageFaultHandler(PEXCEPTIONREPORTRECORD p1,
				           PEXCEPTIONREGISTRATIONRECORD p2,
				           PCONTEXTRECORD p3,
				           PVOID pv);

static ULONG APIENTRY MyExceptionHandler(PEXCEPTIONREPORTRECORD p1,
				         PEXCEPTIONREGISTRATIONRECORD p2,
				         PCONTEXTRECORD p3,
				         PVOID pv)
{
  switch(p1->ExceptionNum)
  {
    case XCPT_ACCESS_VIOLATION: return PageFaultHandler(p1,p2,p3,pv);
    case XCPT_SIGNAL_INTR:
    case XCPT_SIGNAL_BREAK:
                             if(__c__break) exit(EXIT_FAILURE);
                             else __c__break = true;
                             return XCPT_CONTINUE_EXECUTION;
    default: break;
  }
  return XCPT_CONTINUE_SEARCH;
}

static EXCEPTIONREGISTRATIONRECORD RegRec = { NULL, MyExceptionHandler };
#endif

void __FASTCALL__ __init_sys( void )
{
  APIRET rc;
#if !(defined( __DISABLE_MMF ) || defined( __DISABLE_LOWLEVEL_MMF))
  DosSetExceptionHandler(&RegRec); /* <- we must call it before creating any global objects */
#endif
  rc = DosCreateEventSem(NULL,&beyeSem,DC_SEM_SHARED,FALSE);
  if(rc)
  {
    fprintf(stderr,"Can not create semaphore: error = %lu\n",rc);
    exit(EXIT_FAILURE);
  }
#if !(defined( __DISABLE_MMF ) || defined( __DISABLE_LOWLEVEL_MMF))
  DosAcknowledgeSignalException(XCPT_SIGNAL_INTR);
  DosAcknowledgeSignalException(XCPT_SIGNAL_BREAK);
#endif

  rbuff[0] = '\0';
  rbuff2[0] = '\0';
  _home_dir_name[0] = '\0';
}

void __FASTCALL__ __term_sys( void )
{
  DosCloseEventSem(beyeSem);
#if !(defined( __DISABLE_MMF ) || defined( __DISABLE_LOWLEVEL_MMF))
  DosUnsetExceptionHandler(&RegRec); /* <- we must call it after destroying any global objects */
#endif
}

bool __FASTCALL__ __OsGetCBreak( void )
{
  return __c__break;
}

void  __FASTCALL__ __OsSetCBreak( bool state )
{
  __c__break = state;
}

void __FASTCALL__ __OsYield( void ) { DosSleep(1); }

extern char **ArgVector;

static void __FASTCALL__ getStartupFolder( char *to,unsigned size )
{
   PTIB ptib;
   PPIB ppib;

   DosGetInfoBlocks(&ptib,&ppib);
   DosQueryModuleName(ppib->pib_hmte,size,to);
}

char * __FASTCALL__ __get_ini_name( const char *progname )
{
   int len;

   if (rbuff[0]) return rbuff; //Already computed

   UNUSED(progname);
   getStartupFolder(rbuff,sizeof(rbuff));
   len = strlen(rbuff);
   if(stricmp(&rbuff[len-4],".exe") == 0) strcpy(&rbuff[len-4],".ini");
   else                                   strcat(rbuff,".ini");
   return rbuff;
}

char * __FASTCALL__ __get_rc_dir( const char *progname )
{
   char *p1,*p2,last;

   if (rbuff2[0]) return rbuff2; //Already computed

   UNUSED(progname);
   getStartupFolder(rbuff2,sizeof(rbuff2));
   p1 = strrchr(rbuff2,'\\');
   p2 = strrchr(rbuff2,'/');
   p1 = max(p1,p2);
   if(p1) p1[1] = '\0';
   last = p1[strlen(p1)-1];
   if(!(last == '\\' || last == '/')) strcat(rbuff2,"\\");
   return rbuff2;
}

/*
The home directory is a good place for configuration
and temporary files.
The trailing '\\' is included in the returned string.
*/
char * __FASTCALL__ __get_home_dir(const char *progname)
{
   char *p1,*p2,last;

   if (_home_dir_name[0]) return _home_dir_name; //Already computed

   UNUSED(progname);
   getStartupFolder(_home_dir_name,sizeof(_home_dir_name));
   p1 = strrchr(_home_dir_name,'\\');
   p2 = strrchr(_home_dir_name,'/');
   p1 = max(p1,p2);
   if(p1) p1[1] = '\0';
   last = p1[strlen(p1)-1];
   if(!(last == '\\' || last == '/')) strcat(_home_dir_name,"\\");
   return _home_dir_name;
}
