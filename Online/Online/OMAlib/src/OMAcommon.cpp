// $Id: OMAcommon.cpp,v 1.5 2008-10-21 16:27:27 ggiacomo Exp $

#include "OMAlib/OMAcommon.h"
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Implementation file for class : OMAcommon
//
// 2008-08-11 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

// sets the default path of reference histogram files
void OMAcommon::setDefRefRoot() {
#ifdef _WIN32
  m_RefRoot = "H:";
#else
  m_RefRoot = "/hist/";
#endif
  char * envHistDir = getenv ("HISTDIR");
  if (envHistDir !=NULL) {
    m_RefRoot = envHistDir;
    m_RefRoot += "/";
  }
  if(m_histDB) {
    m_RefRoot += m_histDB->refRoot();
  }
  else {
    char * envRefRoot = getenv ("HISTREFPATH");
    if (envRefRoot !=NULL)
      m_RefRoot += envRefRoot;
    else
      m_RefRoot += OnlineHistDBEnv_constants::StdRefRoot;
  }
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
        TH1* ref = (TH1*) f->Get((h->algorithm()+"/"+h->hname()).c_str());
        if(!ref) { // try w/o algorithm name
           ref = (TH1*) f->Get(h->hname().c_str());
        }
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
