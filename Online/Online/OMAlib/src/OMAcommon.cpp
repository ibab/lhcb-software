// $Id: OMAcommon.cpp,v 1.8 2009-02-26 12:37:49 ggiacomo Exp $

#include "OMAlib/OMAcommon.h"
#include "OMAlib/OMAFitFunction.h"
#include <stdlib.h>
#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : OMAcommon
//
// 2008-08-11 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

OMAcommon::~OMAcommon() {
  std::map<std::string, OMAFitFunction*>::iterator i;
  for (i=m_fitfunctions.begin(); i != m_fitfunctions.end(); ++i)
    delete (*i).second ;
}


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

  ParNames.clear();
  ParNames.push_back("Constant"); ParNames.push_back("Slope");
  m_fitfunctions["expo"] = new
    OMAFitFunction("expo",
                   "expo",
                   ParNames,
                   false,
                   "exponential function",
                   true);
  ParNames.clear();
  ParNames.push_back("Constant"); ParNames.push_back("Slope");
  m_fitfunctions["expo"] = new
    OMAFitFunction("expo",
                   "expo",
                   ParNames,
                   false,
                   "exponential function",
                   true);

  ParNames.clear();
  ParNames.push_back("Constant"); ParNames.push_back("MPV");ParNames.push_back("Sigma");
  m_fitfunctions["landau"] = new
    OMAFitFunction("landau",
                   "landau",
                   ParNames,
                   false,
                   "landau function",
                   true);

  // polynomials
  ParNames.clear();
  for (unsigned int degree=0 ; degree<10; degree++) {
    std::stringstream pname,fname,fdesc;
    pname << "P" << degree;
    fname << "pol" << degree;
    fdesc << "polynomium of degree "<< degree;
    ParNames.push_back(pname.str());
    m_fitfunctions[fname.str()] = new
      OMAFitFunction(fname.str(),
                     fname.str(),
                     ParNames,
                     false,
                     fdesc.str(),
                     true);
  }
  OMAFitFunction * newff;
  // gauss + poly background
  for (unsigned int degree=0 ; degree<3; degree++) {
    newff = new OMAFitGausPlusBkg(degree);
    m_fitfunctions[newff->name()] = newff;
  }
  // double gaussian
  newff = new OMAFitDoubleGaus();
  m_fitfunctions[newff->name()] = newff;

  // custom functions
  OMAFitFunction * richfit = new OMAFitTH2withSinCosC();
  m_fitfunctions[richfit->name()] = richfit;

}
