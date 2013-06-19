#include "config.h"
#include "libbeye/libbeye.h"
using namespace	usr;
#include "libbeye/osdep/__os_dep.h"
/**
 * @namespace   libbeye
 * @file        libbeye/osdep/qnxnto/os_dep.c
 * @brief       This file contains implementation of OS depended part for QNX6.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Andrew Golovnia
 * @since       2003
 * @note        Development, fixes and improvements
 * @note        Big thanks to Mike Gorchak for icongen program
 *
 * @author      Mauro Giachero
 * @since       11.2007
 * @note        Added __get_home_dir() and some optimizations
**/
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include "ph_icon.h"

#define LIBDIR "/usr/share"

#define BEYE_PULSE_CODE _PULSE_CODE_MINAVAIL

static char _ini_name[FILENAME_MAX+1];
static char _rc_dir_name[FILENAME_MAX+1];
static char _home_dir_name[FILENAME_MAX + 1];
static bool break_status;
static int chid=0;
static timer_t t=0;
static struct sigevent evp;
static struct itimerspec it;
static struct _pulse pulse;

/*
The home directory is a good place for configuration
and temporary files.
At least (strlen(progname) + 9) characters should be
available before the buffer end.
The trailing '/' is included in the returned string.
*/
char* __FASTCALL__ __get_home_dir(const char *progname)
{
	char *p;

	if (_home_dir_name[0]) return _home_dir_name; //Already computed

	if((p=getenv("HOME"))==NULL||strlen(p)>FILENAME_MAX-10)
		strcpy(_home_dir_name,"/tmp");
	else
		strcpy(_home_dir_name,p);

	strcat(_home_dir_name,"/");

	return _home_dir_name;
}

char* __FASTCALL__ __get_ini_name(const char *progname)
{
	char *p;

	if (_ini_name[0]) return _ini_name; //Already computed

	p=__get_home_dir(progname);
	strcpy(_ini_name,p);
	strcat(_ini_name,".");
	strcat(_ini_name,progname);
	strcat(_ini_name,"rc");

	return _ini_name;
}

char* __FASTCALL__ __get_rc_dir(const char *progname)
{
	if (_rc_dir_name[0]) return _rc_dir_name; //Already computed

	strcpy(_rc_dir_name,LIBDIR"/");
	strcat(_rc_dir_name,progname);
	strcat(_rc_dir_name,"/");
	return _rc_dir_name;
}

void __FASTCALL__ __OsYield()
{
	it.it_value.tv_sec=0;
	it.it_value.tv_nsec=100000;
	it.it_interval.tv_sec=0;
	it.it_interval.tv_nsec=0;
	timer_settime(t,0,&it,NULL);
	MsgReceive(chid,&pulse,sizeof(struct _pulse),NULL);
}

bool __FASTCALL__ __OsGetCBreak()
{
	return break_status;
}

void __FASTCALL__ __OsSetCBreak(bool state)
{
	break_status=state;
}

static void cleanup(int sig)
{
    __term_sys();
    std::ostringstream os;
    os<<sig;
    throw std::runtime_error(std::string("Terminated by signal ")+os.str();
}

/* static struct sigaction sa; */

void __FASTCALL__ __init_sys()
{
	umask(0077);
	signal(SIGTERM,cleanup);
	signal(SIGINT,cleanup);
	signal(SIGQUIT,cleanup);
	signal(SIGILL,cleanup);

	if(chid==0)
		chid=ChannelCreate(0);
		if(chid==-1) throw std::rutime_error("Can't create channel");

	evp.sigev_notify=SIGEV_PULSE;
	evp.sigev_coid=ConnectAttach(ND_LOCAL_NODE,0,chid,_NTO_SIDE_CHANNEL,0);
	evp.sigev_priority=getprio(0);
	evp.sigev_code=BEYE_PULSE_CODE;

	if(t==0) timer_create(CLOCK_REALTIME,&evp,&t);
	if(t==-1) throw std::runtime_error("Can't create timer");
	_ini_name[0] = '\0';
	_rc_dir_name[0] = '\0';
	_home_dir_name[0] = '\0';
}

void __FASTCALL__ __term_sys()
{
	if(t!=-1) timer_delete(t);
	ConnectDetach(evp.sigev_coid);
	ChannelDestroy(chid);
}
