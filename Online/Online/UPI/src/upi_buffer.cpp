/*
upi_buffer.c
Created           : 14-MAY-1993 by Christian Arnault
*/

/*--------------------------------------------------------------------------*/
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "SCR/lists.h"
#include "UPI/upidef.h"
#include "UPI/upirem.h"
#include "UPI/upi_buffer.h"
#include "WT/wt_facilities.h"
#include "WT/wtdef.h"

#define FRAME_SIZE 1500
#define MAGIC_WORD 0xDEADDEAD

typedef struct _UpiBufferHeaderRec* UpiBufferHeader;
typedef struct _UpiBufferHeaderRec {
  int length;
  int magic;
} UpiBufferHeaderRec;

typedef struct _UpiBufferTailRec* UpiBufferTail;
typedef struct _UpiBufferTailRec {
  int magic;
} UpiBufferTailRec;

typedef struct _UpiBufferRec {
  char* data;
  int allocated;
  char* start;
  char* current;
  int updating;
  int noSend;
} UpiBufferRec;

typedef struct _UpiConnectRec UpiConnectRec;
struct _UpiConnectRec {
  UpiConnectRec Link_items;

  char* name;
};

static struct {
  UpiConnectRec Linked_list_items;
  int number;
} UpiConnects = {0, 0, 0};

static int UpiBufferProtocolSize = sizeof (UpiBufferHeaderRec) +
sizeof (UpiBufferTailRec);

static int upibufferlog = -1;
FILE *buffer_log_file;

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  UpiBuffer management.                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
UpiBuffer UpiBufferNew ()
/*--------------------------------------------------------------------------*/
{
  UpiBuffer This;

  if (upibufferlog == -1)  {
    char *buffer_log_name;
    buffer_log_name = getenv ("UPIBUFER_LOG_FILE");
    if (buffer_log_name != NULL)    {
      upibufferlog = 1;
      buffer_log_file = fopen (buffer_log_name, "w+");
      if (buffer_log_file == NULL)      {
        upibufferlog = 0;
      }
    }
    else    {
      upibufferlog = 0;
    }
  }
  This = (UpiBuffer) malloc (sizeof (UpiBufferRec));
  UpiBufferInit (This, 0, 0);
  return (This);
}

/*--------------------------------------------------------------------------*/
void UpiBufferInit (UpiBuffer buffer, char* data, int dataBytes)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferInit: ");
    fprintf (buffer_log_file,"Buffer: %p, Address: %p, Bytes %d\n",
      (void*)buffer, data, dataBytes);
  }
  if (!buffer) return;
  buffer->data = data;
  buffer->allocated = dataBytes;
  buffer->start = data;
  buffer->current = data;
  buffer->updating = 0;
  buffer->noSend = 0;
}

/*--------------------------------------------------------------------------*/
void UpiBufferReset (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferReset: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  if (!buffer) return;

  if (buffer->data) free (buffer->data);
  buffer->data = 0;
  buffer->allocated = 0;
  buffer->start = 0;
  buffer->current = 0;
  buffer->updating = 0;
  buffer->noSend = 0;
}

/*--------------------------------------------------------------------------*/
UpiBufferInfo UpiBufferCheckProtocol (UpiBuffer buffer) {
  int* ptr;
  size_t length;
  int pos;
  int startPos;
  size_t available;

  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferCheckProtocol: ");
    fprintf (buffer_log_file,"Buffer: %p\n", (void*)buffer);
  }
  if (!buffer) return (UpiBufferNoBuffer);

  if (!buffer->data) return (UpiBufferNoData);
  if (!buffer->allocated) return (UpiBufferNoData);
  pos = buffer->current - buffer->data;
  startPos = buffer->start - buffer->data;
  if (pos < startPos)  {
    pos = startPos;
    buffer->current = buffer->start;
  }

  if (startPos == buffer->allocated)  {
    return (UpiBufferNoData);
  }

  if (startPos > buffer->allocated)  {
    printf ("UpiBuffer> Message start beyond allocation.\n");
    return (UpiBufferNoData);
  }

  available = buffer->allocated - startPos;

  ptr = (int*) buffer->start;

  if (ptr[1] != int(MAGIC_WORD) )  {
    /*
    printf ("UpiBuffer> No magic word in front of the message.\n");
    */
    return (UpiBufferBadHeader);
  }

  length = ptr[0];
  if (length < int(3*sizeof(int)))  {
    printf ("UpiBuffer> Message length unsufficient for protocol.\n");
    return (UpiBufferBadLength);
  }

  if (length > available)  {
    printf ("UpiBuffer> Message length oversizing allocation.\n");
    return (UpiBufferBadLength);
  }

  ptr = (int*) (buffer->start + length);
  ptr--;
  if (*ptr != int(MAGIC_WORD) )  {
    printf ("UpiBuffer> No magic word at the message tail.\n");
    return (UpiBufferBadTail);
  }

  if (pos > startPos)  {
    buffer->start += length;
    return UpiBufferInfo(UpiBufferCheckProtocol (buffer));
  }
  else {
    UpiBufferSkipHeader (buffer);
    return (UpiBufferOk);
  }
}

/*--------------------------------------------------------------------------*/
const char* UpiBufferGetInfoText (UpiBufferInfo info)
/*--------------------------------------------------------------------------*/
{
  static const char* text[] = {
    "UpiBuffer> NoBuffer",
    "UpiBuffer> Ok",
    "UpiBuffer> NoData",
    "UpiBuffer> BadLength",
    "UpiBuffer> BadHeader",
    "UpiBuffer> BadTail"
  };
  return (text[info]);
}

/*--------------------------------------------------------------------------*/
void UpiBufferSetUpdating (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSetUpdating: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*) buffer);
  }
  if (!buffer) return;
  buffer->updating = 1;
}

/*--------------------------------------------------------------------------*/
void UpiBufferResetUpdating (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferResetUpdating: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*) buffer);
  }
  if (!buffer) return;
  buffer->updating = 0;
}

/*--------------------------------------------------------------------------*/
void UpiBufferSetNoSend (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSetNoSend: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*) buffer);
  }
  if (!buffer) return;
  buffer->noSend = 1;
}

/*--------------------------------------------------------------------------*/
void UpiBufferResetNoSend (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferResetNoSend: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*) buffer);
  }
  if (!buffer) return;
  buffer->noSend = 0;
}

/*--------------------------------------------------------------------------*/
char* UpiBufferExtend (UpiBuffer buffer, int bytes)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferExtend: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*) buffer);
  }
  if (!buffer) return (0);

  if (!buffer->data)  {
    buffer->allocated = FRAME_SIZE;
    buffer->data = (char*) malloc (buffer->allocated);
    buffer->current = buffer->data;
    buffer->start = buffer->data;
    UpiBufferPutHeader (buffer);
  }

  int pos = buffer->current - buffer->data;
  if ((pos + bytes) > buffer->allocated)  {
    int startPos = buffer->start - buffer->data;
    int frames = ((pos + bytes) / FRAME_SIZE) + 1;
    buffer->allocated = frames * FRAME_SIZE;
    buffer->data = (char*) realloc (buffer->data, buffer->allocated);
    buffer->current = buffer->data + pos;
    buffer->start = buffer->data + startPos;
  }
  char* current = buffer->current;
  buffer->current += bytes;
  return (current);
}

/*--------------------------------------------------------------------------*/
char* UpiBufferMove (UpiBuffer buffer, int bytes) {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferMove: ");
    fprintf (buffer_log_file,"Buffer: %p, Bytes %d\n",(void*)buffer, bytes);
  }
  if (!buffer) return (0);
  int pos = buffer->current - buffer->data;
  if ((pos + bytes) > (buffer->allocated + 1))  {
    printf ("UpiBufferMove> Trying to read beyond data space\n");
    bytes = buffer->allocated - pos + 1;
  }
  char* current = buffer->current;
  buffer->current += bytes;
  return (current);
}

/*--------------------------------------------------------------------------*/
int UpiBufferOffset (UpiBuffer buffer, char* address) {
  if (!buffer) return (0);
  return (address - (buffer->data));
}

/*--------------------------------------------------------------------------*/
char* UpiBufferAddress (UpiBuffer buffer, int offset) {
  if (!buffer) return (0);
  return ((buffer->data) + offset);
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutInt (UpiBuffer buffer, int value)  {
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file,"UpiBufferPutInt: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %d\n",(void*)buffer, value);
  }
  int* p = (int*)UpiBufferExtend(buffer,sizeof(value));
  *p = value;
}
/*--------------------------------------------------------------------------*/
void UpiBufferGetInt (UpiBuffer buffer, int* address) {
  int* p = (int*) UpiBufferMove (buffer, sizeof(int));
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferGetInt: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %d\n",(void*)buffer, *p);
  }
  *address = *p;
}
/*--------------------------------------------------------------------------*/
void UpiBufferPutLong (UpiBuffer buffer, long value)  {
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file,"UpiBufferPutLong: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %ld\n",(void*)buffer, value);
  }
  long* p = (long*)UpiBufferExtend(buffer,sizeof(value));
  *p = value;
}
/*--------------------------------------------------------------------------*/
void UpiBufferGetLong(UpiBuffer buffer,long* address) {
  long* p = (long*) UpiBufferMove (buffer, sizeof(long));
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferGetInt: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %ld\n",(void*)buffer, *p);
  }
  *address = *p;
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutDouble (UpiBuffer buffer, double value)    {
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferPutDouble: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %f\n",(void*)buffer, value);
  }
  double* p = (double*) UpiBufferExtend (buffer, sizeof(double));
  *p = value;
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetDouble (UpiBuffer buffer, double* address) {
  double* p= (double*) UpiBufferMove (buffer, sizeof(double));
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferGetDouble: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %f\n",(void*)buffer, *p);
  }
  *address = *p;
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutText (UpiBuffer buffer, const char* value) {
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferPutText: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %s\n",(void*)buffer, value);
  }
  size_t len = strlen (value);
  UpiBufferPutInt (buffer, len);
  len++;                         /* For the null termination of the string. */
  char* p = UpiBufferExtend (buffer, len);
  strcpy (p, value);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetText (UpiBuffer buffer, char** address)  {
  int len;
  UpiBufferGetInt (buffer, &len);
  char* p = UpiBufferMove (buffer, len + 1);
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferGetText: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Value %s\n",(void*)buffer, p);
  }
  *address = p;
  /*
  The received string must be null-terminated.
  */
  if (p[len] != 0)  {
    printf ("UpiBufferGetText> String not terminated\n!!!");
    p[len] = 0;
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutList (UpiBuffer buffer, int type, void* list, int size)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutList: ");
    fprintf (buffer_log_file,"Buffer: %p, Bytes %d\n",(void*)buffer, size);
  }
  switch (type)  {
  case UPI_ASC_FMT :     {
    char** s = (char**)list;
    for (int i=0; i<size; i++,s++) UpiBufferPutText (buffer, *s);
    break;
  }
  case UPI_DEC_FMT :
  case UPI_HEX_FMT :
  case UPI_OCT_FMT :
  case UPI_BIN_FMT :
  case UPI_LOG_FMT :    {
    int* s = (int*)list;
    int bytes = size * sizeof(int);
    int* p = (int*) UpiBufferExtend (buffer, bytes);
    for (int i=0; i<size; i++,p++,s++) *p = *s;
    break;
  }
  case UPI_REAL_FMT :    {
    double* s = (double*)list;
    int bytes = size * sizeof(double);
    double *p = (double*) UpiBufferExtend (buffer, bytes);
    for (int i=0; i<size; i++,p++,s++) *p = *s;
    break;
  }
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetList (UpiBuffer buffer, int type, void** list, int size) {
  if (upibufferlog == 1)  {
    ::fprintf (buffer_log_file, "UpiBufferGetList: ");
    ::fprintf (buffer_log_file,"Buffer: %p, Bytes %d\n",(void*)buffer, size);
  }
  switch (type)  {
  case UPI_ASC_FMT :
    {
      char** ptr = (char**) list_malloc (size * sizeof(char**));
      *list = ptr;
      for (int i=0; i<size; i++, ptr++) UpiBufferGetText (buffer, ptr);
    }
    break;
  case UPI_DEC_FMT :
  case UPI_HEX_FMT :
  case UPI_OCT_FMT :
  case UPI_BIN_FMT :
  case UPI_LOG_FMT :
    *list = UpiBufferMove (buffer, size * sizeof(int));
    break;
  case UPI_REAL_FMT :
    *list = UpiBufferMove (buffer, size * sizeof(double));
    break;
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutMenu (UpiBuffer buffer, Menu* menu)
/*--------------------------------------------------------------------------*/
{
  Menu* newMenu;
  Page* page;
  Page* newPage;
  Item* item;
  Item* newItem;
  int menuOffset;
  int pageOffset;
  int itemOffset;

  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutMenu: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  newMenu = (Menu*) UpiBufferExtend (buffer, sizeof(Menu));
  menuOffset = UpiBufferOffset (buffer, (char*) newMenu);
  *newMenu = *menu;

  UpiBufferPutText (buffer, menu->mn_title);
  UpiBufferPutText (buffer, menu->up_title);
  UpiBufferPutText (buffer, menu->bt_title);

  newPage = (Page*) UpiBufferExtend (buffer, sizeof(Page));
  pageOffset = UpiBufferOffset (buffer, (char*) newPage);
  *newPage = *(menu->page.first);

  newMenu = (Menu*) UpiBufferAddress (buffer, menuOffset);
  listr_init ((Linked_list*) &newMenu->page);
  listr_connect_entry ((Link*) newPage, (Linked_list*) &newMenu->page);
  listr_init ((Linked_list*) &newPage->item);

  page = menu->page.first;
  while (page)  {
    item = page->item.first;
    while (item)  {
      if (item->id != -1)  {
        itemOffset = UpiBufferPutItem (buffer, item);
        newPage = (Page*) UpiBufferAddress (buffer, pageOffset);
        newItem = (Item*) UpiBufferAddress (buffer, itemOffset);
        listr_connect_entry ((Link*) newItem, (Linked_list*) &newPage->item);
      }
      item = item->next;
    }
    page = page->next;
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetMenu (UpiBuffer buffer, Menu** menuPtr)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferGetMenu: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  Menu* menu = (Menu*) UpiBufferMove (buffer, sizeof(Menu));
  *menuPtr = menu;

  UpiBufferGetText (buffer, &menu->mn_title);
  UpiBufferGetText (buffer, &menu->up_title);
  UpiBufferGetText (buffer, &menu->bt_title);

  listr_convert ((Linked_list*) &menu->page);
  Page* page = (Page*) UpiBufferMove (buffer, sizeof(Page));
  if (menu->page.first != page)  {
    printf ("UpiBufferGetMenu> Desynchronized page structures\n");
  }
  listr_convert ((Linked_list*) &page->item);
  Item *itemFromBuffer, *item = page->item.first;
  while (item)  {
    UpiBufferGetItem (buffer, &itemFromBuffer);
    if (item != itemFromBuffer)    {
      printf ("UpiBufferGetMenu> Desynchronized item structures\n");
    }
    item = item->next;
  }
}

/*--------------------------------------------------------------------------*/
int UpiBufferPutItem (UpiBuffer buffer, Item* item)   {
  Param* newParam;
  int offset;
  int paramOffset;
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutItem: ");
    fprintf (buffer_log_file,"Buffer: %p\n", (void*)buffer);
  }
  Item* newItem = (Item*) UpiBufferExtend (buffer, sizeof(Item));
  offset = UpiBufferOffset (buffer, (char*) newItem);
  *newItem = *item;

  UpiBufferPutText (buffer, item->string);
  UpiBufferPutText (buffer, item->help);

  newItem = (Item*) UpiBufferAddress (buffer, offset);
  listr_init ((Linked_list*) &newItem->param);

  for(Param* param = item->param.first; param; param = param->next )  {
    paramOffset = UpiBufferPutParam (buffer, param);
    newItem = (Item*) UpiBufferAddress (buffer, offset);
    newParam = (Param*) UpiBufferAddress (buffer, paramOffset);
    listr_connect_entry ((Link*) newParam, (Linked_list*) &newItem->param);
  }
  return (offset);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetItem (UpiBuffer buffer, Item** itemPtr)  {
  Param* paramFromBuffer;
  Item* item = (Item*) UpiBufferMove (buffer, sizeof (Item));

  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferGetItem: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  *itemPtr = item;
  UpiBufferGetText (buffer, &item->string);
  UpiBufferGetText (buffer, &item->help);
  listr_convert ((Linked_list*) &item->param);
  for(Param* param = item->param.first; param; param = param->next )  {
    UpiBufferGetParam (buffer, &paramFromBuffer);
    if (param != paramFromBuffer)    {
      printf ("UpiBufferGetMenu> Desynchronized param structures\n");
    }
  }
}

/*--------------------------------------------------------------------------*/
int UpiBufferPutParam (UpiBuffer buffer, Param* param)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutParam: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  Param* newParam = (Param*) UpiBufferExtend (buffer, sizeof(Param));
  int offset = UpiBufferOffset (buffer, (char*) newParam);
  *newParam = *param;

  UpiBufferPutText (buffer, param->buf);
  if (param->list_size)  {
    UpiBufferPutList (buffer, param->type, param->list, param->list_size);
  }
  return (offset);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetParam (UpiBuffer buffer, Param** paramPtr)   {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferGetParam: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  Param* param = (Param*) UpiBufferMove (buffer, sizeof(Param));
  *paramPtr = param;
  UpiBufferGetText (buffer, &param->buf);
  if (param->list_size)  {
    UpiBufferGetList (buffer, param->type, (void**)&param->list, param->list_size);
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutBytes (UpiBuffer buffer, const char* bytes, int length)    {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutBytes: ");
    fprintf (buffer_log_file,"Buffer: %p, Bytes %d\n",(void*)buffer, length);
  }
  UpiBufferPutInt (buffer, length);
  char* p = UpiBufferExtend (buffer, length);
  memcpy (p, bytes, length);
  UpiBufferPutInt (buffer, MAGIC_WORD);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetBytes (UpiBuffer buffer, char** bytes, int neededLength)   {
  int length, magic;
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferGetBytes: ");
    fprintf (buffer_log_file,"Buffer: %p, Bytes %d\n",(void*)buffer, neededLength);
  }
  UpiBufferGetInt (buffer, &length);
  if (neededLength && (length != neededLength))  {
    printf ("UpiBufferGetBytes> Mismatch in block length (%d instead of %d)\n",
      length, neededLength);
  }
  *bytes = UpiBufferMove (buffer, length);
  UpiBufferGetInt (buffer, &magic);
  if (magic != int(MAGIC_WORD)) { 
    printf ("UpiBufferGetBytes> Bad block trailer\n");
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutHisto (UpiBuffer buffer, Histo* histo)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutHisto: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  Histo* newHisto = (Histo*) UpiBufferExtend (buffer, sizeof(Histo));
  *newHisto = *histo;

  UpiBufferPutText (buffer, histo->text);
  UpiBufferPutBytes (buffer, (char*)histo->first, histo->bins * sizeof(double));
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetHisto (UpiBuffer buffer, Histo** histoPtr)   {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferGetHisto: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  Histo* histo = (Histo*) UpiBufferMove (buffer, sizeof(Histo));
  *histoPtr = histo;

  UpiBufferGetText (buffer, &histo->text);
  UpiBufferGetBytes (buffer, (char**) &histo->first, 
    histo->bins * sizeof(double));
}

/*--------------------------------------------------------------------------*/
int UpiBufferSendAlwaysToName (UpiBuffer buffer, const char* name)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSendAlwaysToName: ");
    fprintf (buffer_log_file,"Buffer: %p, Name %s\n",(void*)buffer, name);
  }
  int noSend = buffer->noSend;
  buffer->noSend = 0;
  int status = UpiBufferSendToName (buffer, name);
  buffer->noSend = noSend;
  return (status);
}

/*--------------------------------------------------------------------------*/
int UpiBufferSendToName (UpiBuffer buffer, const char* name)  {
  int status = 0;
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSendToName: ");
    fprintf (buffer_log_file,"Buffer: %p, Name %s\n",(void*)buffer, name);
  }
  if (!buffer || !buffer->data) return (0);

  UpiBufferPutTrailer (buffer);

  if (buffer->updating)  {
    buffer->start = buffer->current;
  }
  else  {
    if (!buffer->noSend)    {
      if (name)   {
        int bytes = buffer->current - buffer->data;
        if (bytes > UpiBufferProtocolSize)   {
          status = upic_net_send_to_name (buffer->data, bytes, name);
        }
      }
    }
    buffer->start = buffer->data;
    buffer->current = buffer->data;
  }

  UpiBufferPutHeader (buffer);
  return (status);
}

/*--------------------------------------------------------------------------*/
int UpiBufferSendAlways (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSendAlways: ");
    fprintf (buffer_log_file,"Buffer: %p \n",(void*)buffer);
  }
  int noSend = buffer->noSend;
  buffer->noSend = 0;
  int status = UpiBufferSend (buffer);
  buffer->noSend = noSend;
  return (status);
}

/*--------------------------------------------------------------------------*/
int UpiBufferSend (UpiBuffer buffer)  {
  int status = 0;
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSend: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*) buffer);
  }
  if (!buffer || !buffer->data) return (0);

  UpiBufferPutTrailer (buffer);

  if (buffer->updating)  {
    buffer->start = buffer->current;
  }
  else  {
    if (!buffer->noSend)    {
      int bytes = buffer->current - buffer->data;
      UpiConnect c, connect = UpiConnects.first;
      while (connect)       {
        if (upibufferlog == 1)   {
          fprintf (buffer_log_file,"#Bytes %d to name %s\n",
            bytes,connect->name);
        }
        status = upic_net_send_to_name (buffer->data, bytes, connect->name);
        if (! (status & 1))  {
          c	= connect;
          connect = connect->next;
          UpiConnectDelete(c);
        }
        else {
          connect = connect->next;
        }
      }
    }
    buffer->start = buffer->data;
    buffer->current = buffer->data;
  }

  UpiBufferPutHeader (buffer);
  return (status);
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutHeader (UpiBuffer buffer)  {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutHeader: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  UpiBufferPutInt (buffer, 0);
  UpiBufferPutInt (buffer, MAGIC_WORD);
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutTrailer (UpiBuffer buffer) {
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferPutTrailer: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  UpiBufferPutInt (buffer, MAGIC_WORD);
  int bytes = buffer->current - buffer->start;
  int* p = (int*) buffer->start;
  *p = bytes;
}

/*--------------------------------------------------------------------------*/
void UpiBufferSkipHeader (UpiBuffer buffer) {
  int length, magic;
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferSkipHeader: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  UpiBufferGetInt (buffer, &length);
  UpiBufferGetInt (buffer, &magic);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetTrailer (UpiBuffer buffer) {
  int magic;
  if (upibufferlog == 1)  {
    fprintf (buffer_log_file, "UpiBufferGetTrailer: ");
    fprintf (buffer_log_file,"Buffer: %p\n",(void*)buffer);
  }
  UpiBufferGetInt (buffer, &magic);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  UpiConnect management.                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
UpiConnect UpiConnectNew (const char* name)  {
  UpiConnect c = UpiConnectFind (name);
  if (!c)  {
    c = (UpiConnect) list_add_entry ((Linked_list*) &UpiConnects, sizeof(UpiConnectRec));
    c->name = (char*) malloc (strlen(name) + 1);
    strcpy (c->name, name);
    UpiConnects.number++;
  }
  return c;
}

/*--------------------------------------------------------------------------*/
void UpiConnectDelete (UpiConnect c)  {
  if (!c) return;
  if (c->name) free (c->name);
  list_remove_entry ((Link*) c);
  UpiConnects.number--;
}

/*--------------------------------------------------------------------------*/
char* UpiConnectName (UpiConnect c)  {
  if (!c) return (0);
  return (c->name);
}

/*--------------------------------------------------------------------------*/
UpiConnect UpiConnectFind (const char* name)    {
  for(UpiConnect c = UpiConnects.first; c; c=c->next)  {
    if (!strcmp(c->name, name)) return (c);
  }
  return (0);
}

/*--------------------------------------------------------------------------*/
int UpiConnectNumber ()  {
  return UpiConnects.number;
}

