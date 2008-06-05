#ifdef _WIN32
#pragma pack(push, datamember_aligment, 1)
#define PACK_DATA(x) x
#else
#define PACK_DATA(x) x __attribute__((__packed__))
#endif
