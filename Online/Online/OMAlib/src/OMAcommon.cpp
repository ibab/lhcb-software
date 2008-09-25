// $Id: OMAcommon.cpp,v 1.3 2008-09-25 15:38:30 ggiacomo Exp $

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
  if(m_histDB)
    m_RefRoot += m_histDB->refRoot();
  else
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
            << startrun << ".root";
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
