#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "RTL/que.h"
#include "WT/wtdef.h"
#include "AMS/amsdef.h"

struct amsu_fac_entry : public qentry_t {
  unsigned int     facility;
  amsuc_callback_t broadcast;
  amsuc_callback_t action;
  void*            param;
}; 

struct amsu_deadfac_entry : public qentry_t {
  char source[32];
  int  srclen;
  int  conv_fac;
}; 

static amsu_fac_entry *scan_entry = 0;
static qentry_t *fac_list = 0;
static qentry_t *deadfac_list = 0;
static int inited = 0;

#define _REMQHI(x,y)            remqhi((x),(y))
#define _REMQTI(x,y)            remqti((x),(y))
#define _INSQHI(x,y)            insqhi((x),(y))
#define _INSQTI(x,y)            insqti((x),(y))

void start_fac_scan()   {
  scan_entry = (amsu_fac_entry*)fac_list;
}

amsu_fac_entry *next_fac()    {
  amsu_fac_entry *entry = (amsu_fac_entry*)((long)scan_entry->next+(long)scan_entry);
  scan_entry = entry;
  return entry;
}

static amsu_fac_entry *find_facility(unsigned int facility) {
  amsu_fac_entry *fac;
  for (fac = (amsu_fac_entry*)((long)fac_list->next+(long)fac_list);
       fac != (amsu_fac_entry*)fac_list ; 
       fac = (amsu_fac_entry*)((long)fac->next+(long)fac))
    if( fac->facility == facility ) break;
  return fac;
}

static amsu_deadfac_entry *find_deadfacility(const char* source) {
  amsu_deadfac_entry *fac;
  int srclen = strlen(source);
  for (fac = (amsu_deadfac_entry*)((long)deadfac_list->next+(long)deadfac_list);
       fac != (amsu_deadfac_entry*)deadfac_list ; 
       fac = (amsu_deadfac_entry*)((long)fac->next+(long)fac))    {
    if (srclen==fac->srclen)      {
      if (strncmp (fac->source, source, srclen) == 0)        {
        break;
      }
    }
  }
  return fac;
}

int amsuc_init( )   {
  if ( inited ) return AMS_SUCCESS;
  fac_list = new qentry_t(0,0);
  deadfac_list = new qentry_t(0,0);
  inited = 1;
  int status = wtc_init();
  if ( status != WT_SUCCESS )  {
    return(status);
  }
  status = wtc_subscribe(WT_FACILITY_AMS,0,amsuc_dispatch);
  return status != WT_SUCCESS ? status : AMS_SUCCESS;    
}

int amsuc_subscribe(unsigned int facility, amsuc_callback_t action, amsuc_callback_t broadcast, void* param)  {
  if ( !inited ) return AMS_NOTINITED;
  amsu_fac_entry *fac = find_facility(facility);
  if ( fac == (void*)fac_list )   {
    fac = new amsu_fac_entry;
    if ( !fac ) return AMS_NOMEMORY;
    _INSQTI( fac, fac_list );        
  }
  fac->facility = facility;
  fac->action   = action;
  fac->broadcast= broadcast;
  fac->param    = param;
  return AMS_SUCCESS;    
}

int amsuc_subscribe_death(const char* source, unsigned int facility, void* /* param */)  {
  if ( !inited ) return AMS_NOTINITED;
  amsu_deadfac_entry   *fac;
  int srclen = strlen(source);
  fac = find_deadfacility(source);
  if ( fac == (void*)deadfac_list )   {
    fac = new amsu_deadfac_entry;
    if ( !fac ) return AMS_NOMEMORY;
    _INSQTI( fac, deadfac_list );        
  }
  fac->conv_fac = facility;
  ::memset(fac->source,0,sizeof(fac->source));
  ::strncpy (fac->source, source,sizeof(fac->source));
  fac->source[sizeof(fac->source)-1] = 0;
  fac->srclen	= srclen;
  return AMS_SUCCESS;
}

int amsuc_remove(unsigned int facility) {
  if ( !inited ) return AMS_NOTINITED;
  amsu_fac_entry *fac	= find_facility(facility);
  if ( fac == fac_list ) return AMS_ERROR;
  remqent( fac );
  delete fac;
  return AMS_SUCCESS;
}

int amsuc_remove_death( const char* source)   {
  if ( !inited ) return AMS_NOTINITED;
  amsu_deadfac_entry* fac = find_deadfacility(source);
  if ( fac == deadfac_list )   {
    return AMS_NODATA;
  }
  remqent(fac);
  delete fac;
  return AMS_SUCCESS;    
}

int amsuc_dispatch( unsigned int, void* )  {
  amsuc_info info;
  char dest[32];
  size_t tlen;
  info.length = sizeof(info.message);
  info.status = amsc_spy_next_message(info.message,&info.length,info.source,&info.facility,&tlen);
  if (info.status == AMS_SUCCESS)    {
    amsu_fac_entry* entry = find_facility(info.facility);
    if (entry != fac_list)    {
      if (entry->action)  {
        return (*entry->action)(&info, entry->param);
      }
    }
    return AMS_ERROR;
  }
  else if (info.status == AMS_NOPEND) {
    return AMS_SUCCESS;
  }
  else if (info.facility == 0)    {
    amsu_deadfac_entry *deadentry;
    deadentry = find_deadfacility(info.source);
    if (deadentry == (amsu_deadfac_entry*)deadfac_list)  {
      start_fac_scan();
      amsu_fac_entry* entry = next_fac();
      while (entry != fac_list)  {
        if (entry->broadcast)   {
          info.status  = (*entry->broadcast)(&info, entry->param);
        }
        entry	= next_fac();
      }
    }
    else  {
      amsu_fac_entry* entry = find_facility(deadentry->conv_fac);
      if (entry != fac_list)  {
        if (entry->broadcast)  {
          unsigned int fac = info.facility;
          info.facility = deadentry->conv_fac;
          info.status  = (*entry->broadcast)(&info,entry->param);
          info.facility = fac;
        }
      }
    }
    info.length = sizeof(info.message);
    amsc_read_message(info.message,&info.length,info.source,&info.facility,dest);
    return AMS_SUCCESS;
  }
  return info.status;
}

