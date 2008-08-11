// $Id: OMAcommon.cpp,v 1.1 2008-08-11 08:05:16 ggiacomo Exp $

#include "OMAlib/OMAcommon.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OMAcommon
//
// 2008-08-11 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

// sets the default path of reference histogram files
void OMAcommon::setDefRefRoot() {
#ifdef _WIN32
  m_RefRoot = "G:";
#else
  m_RefRoot = "/group/";
#endif
  m_RefRoot += OnlineHistDBEnv_constants::StdRefRoot;
}

TH1* OMAcommon::getReference(OnlineHistogram* h,
                             int startrun,
                             std::string DataType) {
  TH1* out=0;
  if(h) {
    std::stringstream refFile;
    refFile << refRoot() << "/"
            << h->task() << "/" << DataType << "_"
            << startrun;
    TFile* f = new TFile(refFile.str().c_str(),"READ");
    if(f) {
      if (false == f->IsZombie() ) {
        TH1* ref = (TH1*) f->Get(h->hname().c_str());
        if (ref) {
          ref->SetDirectory(0);
          out = ref;
        }
      }
      f->Close();
    }
    delete f;
  }
  return out;
}
