#include "TROOT.h"
#include "TSystem.h"
#include "TInterpreter.h"

namespace Tests {
  void init_root() {
    static bool first = true;
    if ( first ) {
      first = false;
      gSystem->Load("libCintex");
      gROOT->ProcessLine("Cintex::Cintex::Enable()");
      gROOT->ProcessLine("#include <vector>");
      gInterpreter->EnableAutoLoading();
      gInterpreter->AutoLoad("DataObject");
      gSystem->Load("libGaudiKernelDict");
    }
  }
}
