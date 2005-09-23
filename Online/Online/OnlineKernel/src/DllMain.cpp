#include <cstdio>
namespace win {
  #include <windows.h>
};

win::BOOL APIENTRY DllMain(
        win::HINSTANCE hinstDLL,  // handle to DLL module
        win::DWORD fdwReason,     // reason for calling function
        win::LPVOID lpvReserved   // reserved
)   
{
    switch( fdwReason ) {
    case DLL_PROCESS_ATTACH:    {
      printf("DLL_PROCESS_ATTACH\n");
    }
    break;
    case DLL_THREAD_ATTACH:
      printf("DLL_THREAD_ATTACH\n");
    break;
    case DLL_THREAD_DETACH:
      printf("DLL_THREAD_DETACH\n");
    break;
    case DLL_PROCESS_DETACH:    {
      printf("DLL_PROCESS_DETACH\n");
    }
    break;
    }
    return TRUE;

}
