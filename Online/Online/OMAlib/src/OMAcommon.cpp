// $Id: OMAcommon.cpp,v 1.6 2009-02-16 10:38:21 ggiacomo Exp $

#include "OMAlib/OMAcommon.h"
#include "OMAlib/OMAFitFunction.h"
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

void OMAcommon::doFitFuncList() {
  std::vector<std::string> ParNames;
  ParNames.push_back("Constant"); ParNames.push_back("Mean");ParNames.push_back("Sigma");
  m_fitfunctions["gaus"] = new
    OMAFitFunction("gaus",
                   "gaus",
                   ParNames,
                   false,
                   "gaussian function",
                   true);

  ParNames.push_back("p0"); ParNames.push_back("p1");
  m_fitfunctions["gaus+p1"] = new
    OMAFitFunction("gaus+p1",
                   "[0]*exp(-((x-[1])/[2])**2)+[3]+[4]*x",
                   ParNames,
                   true,
                   "gaussian function + flat background",
                   false);
}
