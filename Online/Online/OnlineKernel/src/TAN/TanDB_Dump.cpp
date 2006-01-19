#include <cstdio>
#include "TAN/TanDB.h"
extern "C" int rtl_tandb_dump ( int /* argc */, char** /* argv */ )  {
    printf("+------------------------------------------------------------------------------+\n");
    printf("||||||||||||           T A N   D A T A B A S E   D U M P            ||||||||||||\n");
    printf("+------------------------------------------------------------------------------+\n");
    TanDataBase::Instance().Dump( stdout );
    printf("--------------------------------------------------------------------------------\n");
    return 0x1;
}
