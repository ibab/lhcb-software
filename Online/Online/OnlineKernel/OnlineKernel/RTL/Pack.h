#ifdef _WIN32
#pragma pack(push, datamember_aligment, 1)
#define PACK_DATA(x) x
#define PACKED_DATA 
#else
#define PACK_DATA(x) x __attribute__((__packed__))
#define PACKED_DATA __attribute__((__packed__))
#endif
