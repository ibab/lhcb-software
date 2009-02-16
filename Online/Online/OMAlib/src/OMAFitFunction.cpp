// $Id: OMAFitFunction.cpp,v 1.1 2009-02-16 10:38:21 ggiacomo Exp $
#include "OMAlib/OMAFitFunction.h"
#include <TF1.h>
#include <TH1.h>

//-----------------------------------------------------------------------------
// Implementation file for class : OMAFitFunction
//
// 2009-02-10 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------
//=============================================================================
OMAFitFunction::OMAFitFunction( std::string Name ) :
  m_name(Name), m_mustInit(true), m_doc(""), 
  m_funcString(""), m_fitfun(NULL) {}

OMAFitFunction::OMAFitFunction(std::string Name,
                               std::string FuncString,
                               std::vector<std::string> &ParNames,
                               bool MustInit,
                               std::string Doc,
                               bool predefined) :
  m_name(Name), m_parNames(ParNames), m_mustInit(MustInit), m_doc(Doc), 
  m_funcString(FuncString), m_fitfun(NULL), m_predef(predefined) {
  if(!predefined) {
    m_fitfun=new TF1(m_name.c_str(),FuncString.c_str());
    for (Int_t ip=0; ip < (Int_t) ParNames.size(); ip++)
      m_fitfun->SetParName(ip,(ParNames[ip]).c_str());
  }
}

OMAFitFunction::~OMAFitFunction() {
  if(m_fitfun) delete m_fitfun;
} 

void OMAFitFunction::fit(TH1* histo, std::vector<float>* initValues)
{
  bool noInit=true;
  if(!histo) return;
  if(initValues) {
    if(! initValues->empty() ) {
      noInit=false;
      if(!m_fitfun) {
        // in case init. parameters are passed to a predefined function
        std::string myname="OMA"+m_name;
        m_fitfun=new TF1(myname.c_str(),m_funcString.c_str());
      }
      if((int) initValues->size() > np() )
        initValues->resize(np());      
      for (Int_t ip=0; ip < (Int_t) initValues->size(); ip++)
        m_fitfun->SetParameter(ip, (double) initValues->at(ip));
    }
  }
 
  if(m_predef && noInit && m_fitfun) {
    // if predefined function is called without init. parameters , check we don't use TF1
    delete m_fitfun;
    m_fitfun=NULL;
  }

  if(m_fitfun)
    histo->Fit(m_fitfun);
  else
    histo->Fit(m_funcString.c_str());
}
