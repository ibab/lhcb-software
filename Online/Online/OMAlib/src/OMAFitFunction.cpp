// $Id: OMAFitFunction.cpp,v 1.9 2010-10-21 10:17:33 ggiacomo Exp $
#include <sstream>
#include <cmath>
#include <TH1.h>

#include "OMAlib/OMAFitFunction.h"


//-----------------------------------------------------------------------------
// Implementation file for class : OMAFitFunction
//
// 2009-02-10 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------
//=============================================================================
// constructor for derived class (implementing custom fit function)
OMAFitFunction::OMAFitFunction( std::string Name ) : TF1(),
                                                     m_name(Name), m_mustInit(true), m_doc(""), 
                                                     m_funcString(""), m_predef(false), m_useTF1(false){}

// cunstructor for simple fit functions defined by funcString
OMAFitFunction::OMAFitFunction(std::string Name,
                               std::string FuncString,
                               std::vector<std::string> &ParNames,
                               bool MustInit,
                               std::string Doc,
                               bool predefined) : TF1(),
  m_name(Name), m_parNames(ParNames), m_mustInit(MustInit), m_doc(Doc), 
  m_funcString(FuncString), m_predef(predefined), m_useTF1(false) {
  initfun();
}

OMAFitFunction::~OMAFitFunction() {
} 


void OMAFitFunction::checkDefValues() {
  // pad undefined default values for input and fit parameters
  unsigned int i;
  for (i=m_parDefValues.size(); i < m_parNames.size(); i++) {
    m_parDefValues.push_back(-999999.);
  }
  for (i=m_inputDefValues.size(); i < m_inputNames.size(); i++) {
    m_inputDefValues.push_back(0.);
  }
}

void OMAFitFunction::initfun() {
  TF1* fitfun;
  if(!m_predef) {
    fitfun=new TF1(m_name.c_str(),m_funcString.c_str());
    for (Int_t ip=0; ip < (Int_t) m_parNames.size(); ip++)
      fitfun->SetParName(ip,(m_parNames[ip]).c_str());
    m_useTF1 = true;
  }
  else { // predefined function: we could need TF1 anyway to init parameters
    std::string myname="OMA"+m_name;
    fitfun=new TF1(myname.c_str(),m_funcString.c_str());
  }
  TF1& thisfunction = *this;
  thisfunction = *fitfun;
  delete fitfun;
}


void OMAFitFunction::init(std::vector<float>* initValues, TH1*) {
  if((int) initValues->size() > np() )
    initValues->resize(np());  
  for (Int_t ip=0; ip < (Int_t) initValues->size(); ip++)
    SetParameter(ip, (double) initValues->at(ip));
}


void OMAFitFunction::fit(TH1* histo, std::vector<float>* initValues, std::vector<float>* fitRange)
{
  bool noInit=true;
  double xmin=0., xmax=0.;
  if(!histo) return;
  if(initValues) {
    if(! initValues->empty() ) {
      noInit=false;
      // in case init. parameters are passed to a predefined function we need to use the TF1
      m_useTF1=true;      
      init(initValues,histo);
    }
  }
  if(fitRange) {
    if(fitRange->size()>1) {
      xmin=fitRange->at(0);
      xmax=fitRange->at(1);
    }
  }
  if(noInit) {
    if(false == m_predef) {
      init(NULL,histo);       // default init
    }
    else {
      // if predefined function is called without init. parameters , check we don't use TF1
      m_useTF1=false;
    }
  }
 
  if(m_useTF1) {
    histo->Fit((TF1*) this,"0Q","",xmin,xmax);
    TF1 * fitf = histo -> GetFunction( this -> GetName() ) ;
    if ( fitf ) {fitf -> Draw( "LSAME" ) ; fitf->SetLineColor(4); }
  } else {
    histo->Fit(m_funcString.c_str(),"0Q","",xmin,xmax);
    TF1 * fitf = histo -> GetFunction( m_funcString.c_str() ) ;
    if ( fitf ) {fitf -> Draw( "LSAME" ) ; fitf->SetLineColor(4); }
  }
}

OMAFitDoubleGaus::OMAFitDoubleGaus() :
  OMAFitFunction("doublegaus") {
  m_parNames.push_back("Constant1");    m_parDefValues.push_back(0.5);
  m_parNames.push_back("Mean1");        m_parDefValues.push_back(0.);
  m_parNames.push_back("Sigma1");       m_parDefValues.push_back(1);
  m_parNames.push_back("Constant2");    m_parDefValues.push_back(0.5);
  m_parNames.push_back("Mean2");        m_parDefValues.push_back(0.);
  m_parNames.push_back("Sigma2");       m_parDefValues.push_back(10);
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
      initValues->at(0) *= (float) k;
      initValues->at(3) *= (float) k;
    }
    for (Int_t ip=0; ip < np() ; ip++)
      SetParameter(ip, (double) initValues->at(ip));
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
      initValues->at(0) *= (float) k;
      for (deg=3 ; deg <= 3+m_degree; deg++) {
        initValues->at(deg) *= (float) k;
      }
    }
    for (Int_t ip=0; ip < np() ; ip++)
      SetParameter(ip, (double) initValues->at(ip));
  }
  else { // default init
    // estimate background from sides
    double Yi=histo->GetBinContent(1);
    double Ye=histo->GetBinContent(histo->GetNbinsX());
    double bkg=(histo->GetXaxis()->GetXmax() - histo->GetXaxis()->GetXmin()) *
      (Ye+Yi)/2.;
    double rms=histo->GetRMS();
    double gConst = rms >0 ? (histo->Integral("width")-bkg)/(sqrt(2*acos(-1E0))*rms ) : 0.;
    SetParameter(0, gConst);
    SetParameter(1, histo->GetMean() );
    SetParameter(2, rms);
    if (0 == m_degree) {
      SetParameter(3, (Ye+Yi)/2.);
    }
    else {
      double slope =  (Ye-Yi)/(histo->GetXaxis()->GetXmax() - histo->GetXaxis()->GetXmin());
      SetParameter(3, Yi - slope* histo->GetXaxis()->GetXmin());
      SetParameter(4, slope);
      for (deg=2 ; deg <= m_degree; deg++) {
        SetParameter(3+deg, 0.);
      }
    }
  }
}
