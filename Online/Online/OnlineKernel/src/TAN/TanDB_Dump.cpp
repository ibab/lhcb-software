#include <cstdio>
#include "TAN/tandb.h"
extern "C" int tandb_dump ( int argc, char* argv[])  {
    printf("+------------------------------------------------------------------------------+\n");
    printf("||||||||||||           T A N   D A T A B A S E   D U M P            ||||||||||||\n");
    printf("+------------------------------------------------------------------------------+\n");
    TanDataBase::Instance().Dump( stdout );
    printf("--------------------------------------------------------------------------------\n");
    return 0x1;
}
