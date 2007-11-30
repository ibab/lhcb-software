/* 
        upi_buffer.h
         Created           : 14-MAY-1993 by 
*/

#ifndef upi_buffer_h
#define upi_buffer_h

typedef enum {
  UpiBufferNoBuffer = 0,
  UpiBufferOk,
  UpiBufferNoData,
  UpiBufferBadLength,
  UpiBufferBadHeader,
  UpiBufferBadTail
} UpiBufferInfo;
  

typedef struct _UpiBufferRec* UpiBuffer;
typedef struct _UpiConnectRec* UpiConnect;

UpiBuffer UpiBufferNew ();
void UpiBufferInit (UpiBuffer buffer, char* data, int dataBytes);
void UpiBufferReset (UpiBuffer buffer);
UpiBufferInfo UpiBufferCheckProtocol (UpiBuffer buffer);
const char* UpiBufferGetInfoText (UpiBufferInfo info);
void UpiBufferSetUpdating (UpiBuffer buffer);
void UpiBufferResetUpdating (UpiBuffer buffer);
void UpiBufferSetNoSend  (UpiBuffer buffer);
void UpiBufferResetNoSend(UpiBuffer buffer);
char* UpiBufferExtend    (UpiBuffer buffer, int bytes);
char* UpiBufferMove      (UpiBuffer buffer, int bytes);
int UpiBufferOffset      (UpiBuffer buffer, char* address);
char* UpiBufferAddress   (UpiBuffer buffer, int offset);
void UpiBufferPutInt     (UpiBuffer buffer, int value);
void UpiBufferGetInt     (UpiBuffer buffer, int* address);
void UpiBufferPutLong    (UpiBuffer buffer, long value);
void UpiBufferGetLong    (UpiBuffer buffer, long* address);
void UpiBufferPutDouble  (UpiBuffer buffer, double value);
void UpiBufferGetDouble  (UpiBuffer buffer, double* address);
void UpiBufferPutText    (UpiBuffer buffer, const char* value);
void UpiBufferGetText    (UpiBuffer buffer, char** address);
void UpiBufferPutList    (UpiBuffer buffer, int type, void* list, int size);
void UpiBufferGetList    (UpiBuffer buffer, int type, void** list, int size);
void UpiBufferPutMenu    (UpiBuffer buffer, Menu* menu);
void UpiBufferGetMenu    (UpiBuffer buffer, Menu** menuPtr);
int UpiBufferPutItem     (UpiBuffer buffer, Item* item);
void UpiBufferGetItem    (UpiBuffer buffer, Item** itemPtr);
int UpiBufferPutParam    (UpiBuffer buffer, Param* param);
void UpiBufferGetParam   (UpiBuffer buffer, Param** paramPtr);
void UpiBufferPutBytes   (UpiBuffer buffer, const char* bytes, int length);
void UpiBufferGetBytes   (UpiBuffer buffer, char** bytes, int neededLength);
void UpiBufferPutHisto   (UpiBuffer buffer, Histo* histo);
void UpiBufferGetHisto   (UpiBuffer buffer, Histo** histoPtr);
int UpiBufferSendAlwaysToName (UpiBuffer buffer, const char* name);
int UpiBufferSendToName  (UpiBuffer buffer, const char* name);
int UpiBufferSendAlways  (UpiBuffer buffer);
int UpiBufferSend        (UpiBuffer buffer);
void UpiBufferPutHeader  (UpiBuffer buffer);
void UpiBufferPutTrailer (UpiBuffer buffer);
void UpiBufferSkipHeader (UpiBuffer buffer);
void UpiBufferGetTrailer (UpiBuffer buffer);

UpiConnect UpiConnectNew (const char* name);
void UpiConnectDelete (UpiConnect This);
char* UpiConnectName (UpiConnect This);
UpiConnect UpiConnectFind (const char* name);
int UpiConnectNumber ();

#endif

