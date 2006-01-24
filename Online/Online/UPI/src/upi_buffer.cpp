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

  if (upibufferlog == -1)
  {
    char *buffer_log_name;
    buffer_log_name = getenv ("UPIBUFER_LOG_FILE");
    if (buffer_log_name != NULL)
    {
      upibufferlog = 1;
      buffer_log_file = fopen (buffer_log_name, "w+");
      if (buffer_log_file == NULL)
      {
        upibufferlog = 0;
      }
    }
    else
    {
      upibufferlog = 0;
    }
  }
  This = (UpiBuffer) malloc (sizeof (UpiBufferRec));
  UpiBufferInit (This, 0, 0);
  return (This);
}

/*--------------------------------------------------------------------------*/
void UpiBufferInit (UpiBuffer buffer, char* data, int dataBytes)
/*--------------------------------------------------------------------------*/
{

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferInit: ");
    fprintf (buffer_log_file,"Buffer: %x, Address: %x, Bytes %d\n",
      buffer, data, dataBytes);
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
void UpiBufferReset (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferReset: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
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
UpiBufferInfo UpiBufferCheckProtocol (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  int* ptr;
  int length;
  int pos;
  int startPos;
  int available;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferCheckProtocol: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  if (!buffer) return (UpiBufferNoBuffer);

  if (!buffer->data) return (UpiBufferNoData);
  if (!buffer->allocated) return (UpiBufferNoData);

  pos = buffer->current - buffer->data;
  startPos = buffer->start - buffer->data;

  if (pos < startPos)
  {
    pos = startPos;
    buffer->current = buffer->start;
  }

  if (startPos == buffer->allocated)
  {
    return (UpiBufferNoData);
  }

  if (startPos > buffer->allocated)
  {
    printf ("UpiBuffer> Message start beyond allocation.\n");
    return (UpiBufferNoData);
  }

  available = buffer->allocated - startPos;

  ptr = (int*) buffer->start;

  if (ptr[1] != MAGIC_WORD)
  {
    /*
    printf ("UpiBuffer> No magic word in front of the message.\n");
    */
    return (UpiBufferBadHeader);
  }

  length = ptr[0];
  if (length < (3*sizeof(int)))
  {
    printf ("UpiBuffer> Message length unsufficient for protocol.\n");
    return (UpiBufferBadLength);
  }

  if (length > available)
  {
    printf ("UpiBuffer> Message length oversizing allocation.\n");
    return (UpiBufferBadLength);
  }

  ptr = (int*) (buffer->start + length);
  ptr--;
  if (*ptr != MAGIC_WORD)
  {
    printf ("UpiBuffer> No magic word at the message tail.\n");
    return (UpiBufferBadTail);
  }

  if (pos > startPos)
  {
    int status;

    buffer->start += length;
    status = UpiBufferCheckProtocol (buffer);
    return UpiBufferInfo(status);
  }
  else
  {
    UpiBufferSkipHeader (buffer);
    return (UpiBufferOk);
  }
}

/*--------------------------------------------------------------------------*/
char* UpiBufferGetInfoText (UpiBufferInfo info)
/*--------------------------------------------------------------------------*/
{
  static char* text[] = {
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
void UpiBufferSetUpdating (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSetUpdating: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  if (!buffer) return;

  buffer->updating = 1;
}

/*--------------------------------------------------------------------------*/
void UpiBufferResetUpdating (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferResetUpdating: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  if (!buffer) return;

  buffer->updating = 0;
}

/*--------------------------------------------------------------------------*/
void UpiBufferSetNoSend (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSetNoSend: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  if (!buffer) return;

  buffer->noSend = 1;
}

/*--------------------------------------------------------------------------*/
void UpiBufferResetNoSend (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferResetNoSend: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  if (!buffer) return;

  buffer->noSend = 0;
}

/*--------------------------------------------------------------------------*/
char* UpiBufferExtend (UpiBuffer buffer, int bytes)
/*--------------------------------------------------------------------------*/
{
  int pos;
  char* current;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferExtend: ");
    fprintf (buffer_log_file,"Buffer: %x, Bytes %d\n",
      buffer, bytes);
  }
  if (!buffer) return (0);

  if (!buffer->data)
  {
    buffer->allocated = FRAME_SIZE;
    buffer->data = (char*) malloc (buffer->allocated);
    buffer->current = buffer->data;
    buffer->start = buffer->data;

    UpiBufferPutHeader (buffer);
  }

  pos = buffer->current - buffer->data;
  if ((pos + bytes) > buffer->allocated)
  {
    int frames;
    int startPos;

    startPos = buffer->start - buffer->data;
    frames = ((pos + bytes) / FRAME_SIZE) + 1;
    buffer->allocated = frames * FRAME_SIZE;
    buffer->data = (char*) realloc (buffer->data, buffer->allocated);
    buffer->current = buffer->data + pos;
    buffer->start = buffer->data + startPos;
  }
  current = buffer->current;
  buffer->current += bytes;
  return (current);
}

/*--------------------------------------------------------------------------*/
char* UpiBufferMove (UpiBuffer buffer, int bytes)
/*--------------------------------------------------------------------------*/
{
  int pos;
  char* current;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferMove: ");
    fprintf (buffer_log_file,"Buffer: %x, Bytes %d\n",
      buffer, bytes);
  }
  if (!buffer) return (0);

  pos = buffer->current - buffer->data;

  if ((pos + bytes) > (buffer->allocated + 1))
  {
    printf ("UpiBufferMove> Trying to read beyond data space\n");
    bytes = buffer->allocated - pos + 1;
  }

  current = buffer->current;
  buffer->current += bytes;

  return (current);
}

/*--------------------------------------------------------------------------*/
int UpiBufferOffset (UpiBuffer buffer, char* address)
/*--------------------------------------------------------------------------*/
{
  if (!buffer) return (0);
  return (address - (buffer->data));
}

/*--------------------------------------------------------------------------*/
char* UpiBufferAddress (UpiBuffer buffer, int offset)
/*--------------------------------------------------------------------------*/
{
  if (!buffer) return (0);
  return ((buffer->data) + offset);
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutInt (UpiBuffer buffer, int value)
/*--------------------------------------------------------------------------*/
{
  int* p;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutInt: ");
    fprintf (buffer_log_file,"Buffer: %x, Value %d\n",
      buffer, value);
  }
  p = (int*) UpiBufferExtend (buffer, sizeof(int));
  *p = value;
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetInt (UpiBuffer buffer, int* address)
/*--------------------------------------------------------------------------*/
{
  int* p;

  p = (int*) UpiBufferMove (buffer, sizeof(int));
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetInt: ");
    fprintf (buffer_log_file,"Buffer: %x, Value %d\n",
      buffer, *p);
  }
  *address = *p;
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutDouble (UpiBuffer buffer, double value)
/*--------------------------------------------------------------------------*/
{
  double* p;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutDouble: ");
    fprintf (buffer_log_file,"Buffer: %x, Value %f\n",
      buffer, value);
  }
  p = (double*) UpiBufferExtend (buffer, sizeof(double));
  *p = value;
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetDouble (UpiBuffer buffer, double* address)
/*--------------------------------------------------------------------------*/
{
  double* p;

  p = (double*) UpiBufferMove (buffer, sizeof(double));
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetDouble: ");
    fprintf (buffer_log_file,"Buffer: %x, Value %f\n",
      buffer, *p);
  }
  *address = *p;
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutText (UpiBuffer buffer, char* value)
/*--------------------------------------------------------------------------*/
{
  int len;
  char* p;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutText: ");
    fprintf (buffer_log_file,"Buffer: %x, Value %s\n",
      buffer, value);
  }
  len = strlen (value);
  UpiBufferPutInt (buffer, len);
  len++;                         /* For the null termination of the string. */
  p = UpiBufferExtend (buffer, len);
  strcpy (p, value);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetText (UpiBuffer buffer, char** address)
/*--------------------------------------------------------------------------*/
{
  int len;
  char* p;

  UpiBufferGetInt (buffer, &len);
  p = UpiBufferMove (buffer, len + 1);
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetText: ");
    fprintf (buffer_log_file,"Buffer: %x, Value %s\n",
      buffer, p);
  }
  *address = p;
  /*
  The received string must be null-terminated.
  */
  if (p[len] != 0)
  {
    printf ("UpiBufferGetText> String not terminated\n!!!");
    p[len] = 0;
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutList (UpiBuffer buffer, int type, void* list, int size)
/*--------------------------------------------------------------------------*/
{
  int i;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutList: ");
    fprintf (buffer_log_file,"Buffer: %x, Bytes %d\n",
      buffer, size);
  }
  switch (type)
  {
  case ASC_FMT : {
    char** s = (char**)list;
    for (i=0; i<size; i++,s++) UpiBufferPutText (buffer, *s);
                 }
                 break;
  case DEC_FMT :
  case HEX_FMT :
  case OCT_FMT :
  case BIN_FMT :
  case LOG_FMT :
    {
      int* p;
      int bytes;
      int* s = (int*)list;

      bytes = size * sizeof(int);
      p = (int*) UpiBufferExtend (buffer, bytes);
      for (i=0; i<size; i++,p++,s++) *p = *s;
    }
    break;
  case REAL_FMT :
    {
      double* s = (double*)list;
      int bytes = size * sizeof(double);
      double *p = (double*) UpiBufferExtend (buffer, bytes);
      for (i=0; i<size; i++,p++,s++) *p = *s;
    }
    break;
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetList (UpiBuffer buffer, int type, void** list, int size)
/*--------------------------------------------------------------------------*/
{
  int i;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetList: ");
    fprintf (buffer_log_file,"Buffer: %x, Bytes %d\n",
      buffer, size);
  }
  switch (type)
  {
  case ASC_FMT :
    {
      char** ptr;

      ptr = (char**) list_malloc (size * sizeof(char**));
      *list = ptr;
      for (i=0; i<size; i++, ptr++) UpiBufferGetText (buffer, ptr);
    }
    break;
  case DEC_FMT :
  case HEX_FMT :
  case OCT_FMT :
  case BIN_FMT :
  case LOG_FMT :
    {
      int* p;

      p = (int*) UpiBufferMove (buffer, size * sizeof(int));
      *list = p;
    }
    break;
  case REAL_FMT :
    {
      double* p;

      p = (double*) UpiBufferMove (buffer, size * sizeof(double));
      *list = p;
    }
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

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutMenu: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
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
  while (page)
  {
    item = page->item.first;
    while (item)
    {
      if (item->id != -1)
      {
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
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
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
  Item* newItem;
  Param* param;
  Param* newParam;
  int offset;
  int paramOffset;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutItem: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  newItem = (Item*) UpiBufferExtend (buffer, sizeof(Item));
  offset = UpiBufferOffset (buffer, (char*) newItem);
  *newItem = *item;

  UpiBufferPutText (buffer, item->string);
  UpiBufferPutText (buffer, item->help);

  newItem = (Item*) UpiBufferAddress (buffer, offset);
  listr_init ((Linked_list*) &newItem->param);

  param = item->param.first;
  while (param)
  {
    paramOffset = UpiBufferPutParam (buffer, param);

    newItem = (Item*) UpiBufferAddress (buffer, offset);
    newParam = (Param*) UpiBufferAddress (buffer, paramOffset);
    listr_connect_entry ((Link*) newParam, (Linked_list*) &newItem->param);

    param = param->next;
  }
  return (offset);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetItem (UpiBuffer buffer, Item** itemPtr)
/*--------------------------------------------------------------------------*/
{
  Item* item;
  Param* param;
  Param* paramFromBuffer;

  item = (Item*) UpiBufferMove (buffer, sizeof (Item));

  *itemPtr = item;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetItem: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  UpiBufferGetText (buffer, &item->string);
  UpiBufferGetText (buffer, &item->help);

  listr_convert ((Linked_list*) &item->param);

  param = item->param.first;
  while (param)
  {
    UpiBufferGetParam (buffer, &paramFromBuffer);
    if (param != paramFromBuffer)
    {
      printf ("UpiBufferGetMenu> Desynchronized param structures\n");
    }
    param = param->next;
  }
}

/*--------------------------------------------------------------------------*/
int UpiBufferPutParam (UpiBuffer buffer, Param* param)
/*--------------------------------------------------------------------------*/
{
  Param* newParam;
  int offset;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutParam: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  newParam = (Param*) UpiBufferExtend (buffer, sizeof(Param));
  offset = UpiBufferOffset (buffer, (char*) newParam);
  *newParam = *param;

  UpiBufferPutText (buffer, param->buf);
  if (param->list_size)
  {
    UpiBufferPutList (buffer, param->type, param->list, param->list_size);
  }
  return (offset);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetParam (UpiBuffer buffer, Param** paramPtr)
/*--------------------------------------------------------------------------*/
{
  Param* param;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetParam: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  param = (Param*) UpiBufferMove (buffer, sizeof(Param));

  *paramPtr = param;

  UpiBufferGetText (buffer, &param->buf);

  if (param->list_size)
  {
    UpiBufferGetList (buffer, param->type, (void**)&param->list, param->list_size);
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutBytes (UpiBuffer buffer, char* bytes, int length)
/*--------------------------------------------------------------------------*/
{
  char* p;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutBytes: ");
    fprintf (buffer_log_file,"Buffer: %x, Bytes %d\n",
      buffer, length);
  }
  UpiBufferPutInt (buffer, length);
  p = UpiBufferExtend (buffer, length);
  memcpy (p, bytes, length);
  UpiBufferPutInt (buffer, MAGIC_WORD);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetBytes (UpiBuffer buffer, char** bytes, int neededLength)
/*--------------------------------------------------------------------------*/
{
  int length;
  int magic;
  char* p;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetBytes: ");
    fprintf (buffer_log_file,"Buffer: %x, Bytes %d\n",
      buffer, neededLength);
  }
  UpiBufferGetInt (buffer, &length);
  if (neededLength && (length != neededLength))
  {
    printf ("UpiBufferGetBytes> Mismatch in block length (%d instead of %d)\n",
      length, neededLength);
  }
  p = UpiBufferMove (buffer, length);
  *bytes = p;
  UpiBufferGetInt (buffer, &magic);
  if (magic != MAGIC_WORD)
  {
    printf ("UpiBufferGetBytes> Bad block trailer\n");
  }
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutHisto (UpiBuffer buffer, Histo* histo)
/*--------------------------------------------------------------------------*/
{
  Histo* newHisto;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutHisto: ");
    fprintf (buffer_log_file,"Buffer: %x, \n",
      buffer);
  }
  newHisto = (Histo*) UpiBufferExtend (buffer, sizeof(Histo));
  *newHisto = *histo;

  UpiBufferPutText (buffer, histo->text);
  UpiBufferPutBytes (buffer, (char*) histo->first, 
    histo->bins * sizeof(double));
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetHisto (UpiBuffer buffer, Histo** histoPtr)
/*--------------------------------------------------------------------------*/
{
  Histo* histo;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetHisto: ");
    fprintf (buffer_log_file,"Buffer: %x \n",
      buffer);
  }
  histo = (Histo*) UpiBufferMove (buffer, sizeof(Histo));
  *histoPtr = histo;

  UpiBufferGetText (buffer, &histo->text);
  UpiBufferGetBytes (buffer, (char**) &histo->first, 
    histo->bins * sizeof(double));
}

/*--------------------------------------------------------------------------*/
int UpiBufferSendAlwaysToName (UpiBuffer buffer, char* name)
/*--------------------------------------------------------------------------*/
{
  int noSend;
  int status;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSendAlwaysToName: ");
    fprintf (buffer_log_file,"Buffer: %x, Name %s\n",
      buffer, name);
  }
  noSend = buffer->noSend;
  buffer->noSend = 0;
  status = UpiBufferSendToName (buffer, name);
  buffer->noSend = noSend;
  return (status);
}

/*--------------------------------------------------------------------------*/
int UpiBufferSendToName (UpiBuffer buffer, char* name)
/*--------------------------------------------------------------------------*/
{
  int status = 0;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSendToName: ");
    fprintf (buffer_log_file,"Buffer: %x, Name %s\n",
      buffer, name);
  }
  if (!buffer || !buffer->data) return (0);

  UpiBufferPutTrailer (buffer);

  if (buffer->updating)
  {
    buffer->start = buffer->current;
  }
  else
  {
    int bytes;

    if (!buffer->noSend)
    {
      if (name)
      {
        bytes = buffer->current - buffer->data;
        if (bytes > UpiBufferProtocolSize)
        {
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
int UpiBufferSendAlways (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  int noSend;
  int status;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSendAlways: ");
    fprintf (buffer_log_file,"Buffer: %x, \n",
      buffer);
  }
  noSend = buffer->noSend;
  buffer->noSend = 0;
  status = UpiBufferSend (buffer);
  buffer->noSend = noSend;
  return (status);
}

/*--------------------------------------------------------------------------*/
int UpiBufferSend (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  int status = 0;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSend: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  if (!buffer || !buffer->data) return (0);

  UpiBufferPutTrailer (buffer);

  if (buffer->updating)
  {
    buffer->start = buffer->current;
  }
  else
  {
    int bytes;
    UpiConnect connect;

    if (!buffer->noSend)
    {
      bytes = buffer->current - buffer->data;
      connect = UpiConnects.first;
      while (connect)
      {
        if (upibufferlog == 1)
        {
          fprintf (buffer_log_file,"#Bytes %d to name %s\n",
            bytes,connect->name);
        }
        status = upic_net_send_to_name (buffer->data, bytes, connect->name);
        if (! (status & 1))
        {
          UpiConnect c;
          c	= connect;
          connect = connect->next;
          UpiConnectDelete(c);
        }
        else
        {
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
void UpiBufferPutHeader (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutHeader: ");
    fprintf (buffer_log_file,"Buffer: %x, \n",
      buffer);
  }
  UpiBufferPutInt (buffer, 0);
  UpiBufferPutInt (buffer, MAGIC_WORD);
}

/*--------------------------------------------------------------------------*/
void UpiBufferPutTrailer (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  int bytes;
  int* p;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferPutTrailer: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  UpiBufferPutInt (buffer, MAGIC_WORD);

  bytes = buffer->current - buffer->start;
  p = (int*) buffer->start;
  *p = bytes;
}

/*--------------------------------------------------------------------------*/
void UpiBufferSkipHeader (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  int length;
  int magic;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferSkipHeader: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  UpiBufferGetInt (buffer, &length);
  UpiBufferGetInt (buffer, &magic);
}

/*--------------------------------------------------------------------------*/
void UpiBufferGetTrailer (UpiBuffer buffer)
/*--------------------------------------------------------------------------*/
{
  int magic;

  if (upibufferlog == 1)
  {
    fprintf (buffer_log_file, "UpiBufferGetTrailer: ");
    fprintf (buffer_log_file,"Buffer: %x\n",
      buffer);
  }
  UpiBufferGetInt (buffer, &magic);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  UpiConnect management.                                                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
UpiConnect UpiConnectNew (char* name)
/*--------------------------------------------------------------------------*/
{
  UpiConnect This;

  This = UpiConnectFind (name);
  if (!This)
  {
    This = (UpiConnect) list_add_entry ((Linked_list*) &UpiConnects, 
      sizeof(UpiConnectRec));
    This->name = (char*) malloc (strlen(name) + 1);
    strcpy (This->name, name);
    UpiConnects.number++;
  }
  return (This);
}

/*--------------------------------------------------------------------------*/
void UpiConnectDelete (UpiConnect This)
/*--------------------------------------------------------------------------*/
{
  if (!This) return;

  if (This->name) free (This->name);
  list_remove_entry ((Link*) This);
  UpiConnects.number--;
}

/*--------------------------------------------------------------------------*/
char* UpiConnectName (UpiConnect This)
/*--------------------------------------------------------------------------*/
{
  if (!This) return (0);

  return (This->name);
}

/*--------------------------------------------------------------------------*/
UpiConnect UpiConnectFind (char* name)    {
  for(UpiConnect This = UpiConnects.first; This; This=This->next)  {
    if (!strcmp(This->name, name)) return (This);
  }
  return (0);
}

/*--------------------------------------------------------------------------*/
int UpiConnectNumber ()
/*--------------------------------------------------------------------------*/
{
  return (UpiConnects.number);
}

