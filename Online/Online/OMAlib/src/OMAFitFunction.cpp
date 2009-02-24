// $Id: OMAFitFunction.cpp,v 1.4 2009-02-24 20:36:07 frankb Exp $
#include <sstream>
#include <cmath>
#include <TF1.h>
#include <TH1.h>

#include "OMAlib/OMAFitFunction.h"


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
  initfun();
}

OMAFitFunction::~OMAFitFunction() {
  if(m_fitfun) delete m_fitfun;
} 

void OMAFitFunction::initfun() {
  if(!m_predef) {
    m_fitfun=new TF1(m_name.c_str(),m_funcString.c_str());
    for (Int_t ip=0; ip < (Int_t) m_parNames.size(); ip++)
      m_fitfun->SetParName(ip,(m_parNames[ip]).c_str());
  }
}


void OMAFitFunction::init(std::vector<float>* initValues, TH1* histo) {
  histo=histo; // avoid compil. warning
  if(m_fitfun) {
    if((int) initValues->size() > np() )
      initValues->resize(np());  
    for (Int_t ip=0; ip < (Int_t) initValues->size(); ip++)
      m_fitfun->SetParameter(ip, (double) initValues->at(ip));
  }
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
      init(initValues,histo);
    }
  }
  else {
    if(false == m_predef) {
      // default init
      init(NULL,histo);
    }
  }
 
  if(m_predef && noInit && m_fitfun) {
    // if predefined function is called without init. parameters , check we don't use TF1
    delete m_fitfun;
    m_fitfun=NULL;
  }

  if(m_fitfun)
    histo->Fit(m_fitfun,"Q");
  else
    histo->Fit(m_funcString.c_str(),"Q");
}

OMAFitDoubleGaus::OMAFitDoubleGaus() :
  OMAFitFunction("doublegaus") {
  m_parNames.push_back("Constant1");
  m_parNames.push_back("Mean1");
  m_parNames.push_back("Sigma1");
  m_parNames.push_back("Constant2");
  m_parNames.push_back("Mean2");
  m_parNames.push_back("Sigma2");
  m_mustInit=true;
  m_doc="sum of two gaussian functions";
  m_funcString="[0]*exp(-((x-[1])**2/(2*[2]**2)))+[3]*exp(-((x-[4])**2/(2*[5]**2)))";
  m_predef=false;
  initfun();
}


void OMAFitDoubleGaus::init(std::vector<float>* initValues, TH1* histo) {
  if (!histo) {
    OMAFitFunction::init(initValues);
    return ;
  }
  if((int) initValues->size() >= np()) {
    // compute normalization
    double integ=sqrt(2*acos(-1E0))*
      (initValues->at(0)*initValues->at(2) + initValues->at(3)*initValues->at(5));
    if (integ > 0) {
      double k = histo->Integral("width")/integ;  
      initValues->at(0) *= k;
      initValues->at(3) *= k;
    }
    for (Int_t ip=0; ip < np() ; ip++)
      m_fitfun->SetParameter(ip, (double) initValues->at(ip));
  }
}

OMAFitGausPlusBkg::OMAFitGausPlusBkg(unsigned int degree) :
  OMAFitFunction("gaus+pol"), m_degree(degree) {
  unsigned int deg;
  std::stringstream cname;
  cname << m_name << m_degree;
  m_name = cname.str();
  m_parNames.push_back("Constant");
  m_parNames.push_back("Mean");
  m_parNames.push_back("Sigma");
  for (deg=0 ; deg<=m_degree; deg++) {
    std::stringstream pname;
    pname << "P" << deg;
    m_parNames.push_back(pname.str());
  }
  m_mustInit=false;
  m_doc="gaussian plus polynomium for background";
  std::stringstream fstr;
  fstr << "[0]*exp(-((x-[1])**2/(2*[2]**2)))";
  int iP=3;
  for (deg=0 ; deg<=m_degree; deg++) {
    fstr << "+["<< iP++ <<"]";
    if (deg>0)
      fstr << "*x**"<<deg;
  }
  m_funcString = fstr.str();
  m_predef=false;
  initfun();
}

void OMAFitGausPlusBkg::init(std::vector<float>* initValues, TH1* histo) {
  if (!histo) {
    OMAFitFunction::init(initValues);
    return ;
  }
  unsigned int deg;
  if((int) initValues->size() >= np()) {
     // init values available: check normalization
    double Xmin=histo->GetXaxis()->GetXmin();
    double Xmax=histo->GetXaxis()->GetXmax();
    double integ=sqrt(2*acos(-1E0))*initValues->at(0)*initValues->at(2) + 
      initValues->at(3)*(Xmax-Xmin);
    for (deg=1 ; deg <= m_degree; deg++) {
      integ += ( (initValues->at(3+deg)) * (pow(Xmax,deg+1E0)-pow(Xmin,deg+1E0))/((double)deg) );
    }
    if (integ > 0) {
      double k = histo->Integral("width")/integ;  
      initValues->at(0) *= k;
      for (deg=3 ; deg <= 3+m_degree; deg++) {
        initValues->at(deg) *= k;
      }
    }
    for (Int_t ip=0; ip < np() ; ip++)
      m_fitfun->SetParameter(ip, (double) initValues->at(ip));
  }
  else { // default init
    // estimate background from sides
    double Yi=histo->GetBinContent(1);
    double Ye=histo->GetBinContent(histo->GetNbinsX());
    double bkg=(histo->GetXaxis()->GetXmax() - histo->GetXaxis()->GetXmin()) *
      (Ye+Yi)/2.;
    double rms=histo->GetRMS();
    double gConst = rms >0 ? (histo->Integral("width")-bkg)/(sqrt(2*acos(-1E0))*rms ) : 0.;
    m_fitfun->SetParameter(0, gConst);
    m_fitfun->SetParameter(1, histo->GetMean() );
    m_fitfun->SetParameter(2, rms);
    if (0 == m_degree) {
      m_fitfun->SetParameter(3, (Ye+Yi)/2.);
    }
    else {
      double slope =  (Ye-Yi)/(histo->GetXaxis()->GetXmax() - histo->GetXaxis()->GetXmin());
      m_fitfun->SetParameter(3, Yi - slope* histo->GetXaxis()->GetXmin());
      m_fitfun->SetParameter(4, slope);
      for (deg=2 ; deg <= m_degree; deg++) {
        m_fitfun->SetParameter(3+deg, 0.);
      }
    }
  }
}
