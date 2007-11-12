/*
upi_net.c
Created           : 15-DEC-1989 by 
*/
#include "WT/wt_facilities.h"
#include "WT/wtdef.h"
#include "AMS/amsdef.h"
#include "UPI/upidef.h"
#include "RTL/rtl.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static int  Opened = 0;
static char My_node[80];
static char Source_name[AMS_NAME_LENGTH];

#define DUMMYLENGTH  32
struct MessageLog {
  MessageLog Link_items;

  int isSpy;
  int status;
  int total;
  char sourceName[AMS_NAME_LENGTH];
  char buffer[DUMMYLENGTH];
};

static struct {
  MessageLog Linked_list_items;
  int logs;
} MessageLogs = {0, 0, 0};

static int MessageLogActive = false;
static int MessageLogMaxLogs = 100;

//----------------------------------------------------------------------------
int upic_net_init (const char* name, char** server, WtRoutine handler, WtRoutine broadcast)  {
  if (Opened) upic_net_close();
  Opened = 1;
  int status = wtc_init();
  wtc_subscribe (WT_FACILITY_AMS,handler,broadcast);
  status = (name) ? amsc_init (name) : amsc_init (0);
  if ( server )  {
    *server = 0;
    char srv[AMS_NAME_LENGTH];
    if ( upic_net_server_name(srv) )  {
      *server = (char*)::malloc(::strlen(srv)+1);
      ::strcpy(*server,srv);
    }
  }
  amsc_get_node(My_node); 
  strcat(My_node,"::");
  return (status);
}

//----------------------------------------------------------------------------
void upic_net_close ()   {
  amsc_close();
  Opened = 0;
}

//----------------------------------------------------------------------------
void upic_net_discard_server ()  {
}

//----------------------------------------------------------------------------
int upic_net_server_name (char* name)  {
  const char* source = (char*)getenv("TERMINAL_SERVER");
  if ( source )   {
    strcpy (name, source);
    return 1;
  }
  return 0;
}

//----------------------------------------------------------------------------
int upic_net_set_server_name (const char* /*name*/)  {
  return 0;  
}

//----------------------------------------------------------------------------
int upic_net_read (char** buffer, size_t* bytes, char** source) {
  int status;
  unsigned int f;
  size_t fill=1, total=0;
  char blank[32];
  status = amsc_spy_next_message (blank, &fill, Source_name, &f, &total);
  if ( AMS_SUCCESS == status )  {
    *buffer = (char*) malloc (total + 1);
    *bytes  = total+1;
    status = amsc_get_message (*buffer, bytes, Source_name, 0, 0, &f, 0, 0);
    fill = strlen(My_node) + strlen(Source_name) + 1;
    *source = (char*)list_malloc (fill);
    amsc_full_name(*source,Source_name,fill,DECNET_STYLE);
  }
  else  {
    fill = sizeof(blank);
    ::amsc_read_message(blank,&fill,Source_name,&f,0);
    *buffer = 0;
    *bytes = 0;
    *source = 0;
  }
  return status;
}

//----------------------------------------------------------------------------
int upic_net_spy (char** source)  {
  unsigned int f;
  size_t total, fill = 1;
  char blank[32];
  int status = amsc_spy_next_message (blank, &fill, Source_name, &f, &total);
  fill = strlen(My_node) + strlen(Source_name) + 1;
  *source = (char*)list_malloc (fill);
  amsc_full_name(*source,Source_name,fill,DECNET_STYLE);
  return status;
}

//----------------------------------------------------------------------------
int upic_net_send_to_name (const char* buffer, int bytes, const char* dest)  {
  return amsc_send_message (buffer, bytes, dest, WT_FACILITY_AMS,0);
}

//----------------------------------------------------------------------------
int upic_net_who_am_i (char* name)   {
  char n[64];
  ::lib_rtl_get_process_name(n,sizeof(n));
  strcpy(name,n);
  return 1;
}

/*-------------------------------------------------------------------------*/
void upic_net_stop_log ()   {
  MessageLogActive = 0;
}

/*-------------------------------------------------------------------------*/
void upic_net_start_log ()   {
  MessageLog* log;
  const char* text = (char*)getenv ("UPIMSG_MAX_LOGS");
  if (text)  {
    int logs = 0;
    sscanf (text, "%d", &logs);
    if (logs > 0) 
      MessageLogMaxLogs = logs;
    else   {
      upic_net_stop_log ();
      return;
    }
  }
  while ((log = MessageLogs.first)) list_remove_entry ((Link*) log);
  MessageLogs.logs = 0;
  MessageLogActive = 1;
}
/*-------------------------------------------------------------------------*/
void upic_net_log_spy (int status, int bytes)   {
  MessageLog* log = 0;
  if (!MessageLogActive) return;
  if (MessageLogs.logs > MessageLogMaxLogs)  {
    log = MessageLogs.first;
    list_remove_entry ((Link*) log);
  }
  else 
    MessageLogs.logs++;
  log = (MessageLog*) list_add_entry ((Linked_list*)&MessageLogs,sizeof (MessageLog));
  log->isSpy = 1;
  log->status = status;
  log->total = bytes;
  strcpy (log->sourceName, Source_name);
}
/*-------------------------------------------------------------------------*/
void upic_net_log_last_message (int status, int bytes, const char* buffer)  {
  MessageLog* log = 0;
  if (!MessageLogActive) return;
  if (MessageLogs.logs > MessageLogMaxLogs)  {
    log = MessageLogs.first;
    list_remove_entry ((Link*) log);
  }
  else 
    MessageLogs.logs++;
  log = (MessageLog*) list_add_entry ((Linked_list*) &MessageLogs,sizeof (MessageLog));
  log->status = status;
  log->total = bytes;
  strcpy (log->sourceName, Source_name);
  if (bytes >= DUMMYLENGTH+1) bytes = DUMMYLENGTH+1;
  strncpy (log->buffer, buffer, bytes);
}
/*-------------------------------------------------------------------------*/
void upic_net_flush_log ()  {
  const char* fileName = (char*) getenv ("UPIMSG_LOG_FILE");
  if (!fileName) return;

  FILE* f = fopen (fileName, "w+");
  if (!f) return;

  MessageLog* log = MessageLogs.first;
  while (log)  {
    if (log->isSpy) fprintf (f, "Spy ");
    else            fprintf (f, "    ");
    fprintf (f, "stat 0x%x ", log->status);
    fprintf (f, "from [%s] ", log->sourceName);
    fprintf (f, "bytes %d : ", log->total);
    if (!log->isSpy)    {
      int bytes = log->total;
      if (bytes > DUMMYLENGTH+1) bytes = DUMMYLENGTH;
      for (int byte = 0; byte < bytes; byte++)      {
        unsigned char c = log->buffer[byte];
        fprintf (f, "%2.2x ", c);
      }
      for (int byte = 0; byte < bytes; byte++)      {
        unsigned char c = log->buffer[byte];
        c &= 0x7f;
        if (c < 0x20) c = '.';
        fprintf (f, "%c", c);
      }
    }
    fprintf (f, "\n");
    log = log->next;
  }
  fclose (f);
}
