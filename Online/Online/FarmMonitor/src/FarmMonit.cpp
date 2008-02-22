// $Id: FarmMonit.cpp,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
// Include files 
#include <stdarg.h>
#include "FarmMonitor.h"
#include "StoreMonitor.h"

int main( int argc, char* argv[] ) {
  bool display      = false;
  bool displayStore = false;
  std::string part = "LHCb";

  for ( int n = 1; argc > n; ++n ) {
    if ( 0 == strcmp( argv[n], "-d" )   ) display = true;
    if ( 0 == strcmp( argv[n], "-store" ) ) displayStore = true;
  }

  if ( displayStore ) {
    std::cout << "Update information on storage system";
    if ( display ) std::cout << " and display it";
    std::cout << std::endl;
    
    StoreMonitor myStore( part );
    while ( true ) {
      myStore.update();
      if ( display ) myStore.display();
      sleep( 2 );
    }
  } else {
    std::cout << "Update information on HLT farm";
    if ( display ) std::cout << " and display it";
    std::cout << std::endl;
    FarmMonitor myFarm( part );
    while ( true ) {
      sleep( 2 );
      myFarm.update();
      if ( display ) myFarm.display();
    }
  }
}
//=============================================================================
