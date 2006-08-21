#include <iostream>
#include <stdlib.h>

// ROOT stuff
#include <TROOT.h>

// class steering the GUI
#include "HPDMonitorApp.h"


#ifdef WIN32
namespace win {
#include <windows.h>
}
#endif


// -------------------------------------------------------------------------------------


// =============================================================================
// =============================================================================
int main (int argc, char **argv) {
  
  if (gROOT->IsBatch()) {
    std::cerr << " cannot run in batch mode, exit" << std::endl;    
    return 1;    
  } // if isBatch

  std::cout << "HPD Monitor starts " << std::endl;
  
  HPDMonitorApp *mApp = new HPDMonitorApp;
  mApp->Start();
  

  std::cout << "HPD Monitor ends " << std::endl;
  return (0);  
} // int main

