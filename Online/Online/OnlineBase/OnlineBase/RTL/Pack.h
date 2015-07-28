#ifdef _WIN32
#ifndef _DISABLED_WARNING_C4103
#define _DISABLED_WARNING_C4103 1
// used #pragma pack to change alignment
#pragma warning ( disable : 4103 )
#endif
#pragma pack(push, datamember_aligment, 1)
#define PACK_DATA(x) x
#define PACKED_DATA 
#else
#define PACK_DATA(x) x __attribute__((__packed__))
#define PACKED_DATA __attribute__((__packed__))
#endif
