// $Id: OMAFit.cpp,v 1.5 2010-10-21 10:17:33 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
using namespace TMath;

// special Check algorithm: fit function is specified for each analysis by the first input parameter
// no prior knowledge of output algorithms that are taken dinamically according to requested fit function
// and are defined in dedicated OMAFitFunction objects
OMAFit::OMAFit(OMAlib* Env) : 
  OMACheckAlg("Fit", Env) {
  m_npars = 1;
  m_parnames.push_back("Min_chi2_prob"); m_parDefValues.push_back(0.);

  m_ninput = 2;
  m_inputNames.push_back("Fit_function"); m_inputDefValues.push_back(1);
  m_inputNames.push_back("confidence"); m_inputDefValues.push_back(.95);
  m_doc = "Fit histogram with a given function and check chi2 and/or output parameters";
  m_doc +=  " with a given normal confidence level (default is 0.95)";
}

void OMAFit::exec(TH1 &Histo,
                  std::vector<float> & warn_thresholds,
                  std::vector<float> & alarm_thresholds,
                  std::vector<float> & input_pars,
                  unsigned int anaID,
                  TH1* ) {
  if (input_pars.size() < 1) return;
  if (0 == (int) Histo.GetEntries()) return; //skip empty histograms

  // get requested fit function
  std::string fitfunName = 
    m_omaEnv->dbSession()->getFitFunction((int)(input_pars[0]+.1));

  OMAFitFunction* fitfun=m_omaEnv->getFitFunction(fitfunName);
  
  unsigned int NcheckPars= fitfun->np()*2+1;
  if (warn_thresholds.size() < NcheckPars ||
      alarm_thresholds.size() < NcheckPars ) return;

  // perform the fit, with or without init. parameter values
  if(input_pars.size() > m_ninput) {
    std::vector<float> fitPars;
    fitPars.insert(fitPars.end(), input_pars.begin()+m_ninput, input_pars.end());
    fitfun->fit(&Histo, &fitPars);
  }
  else {
    fitfun->fit(&Histo,NULL);
  }
  // check the result
  std::string hname(Histo.GetName());
  TF1* fit = fitfun->fittedfun();
  if (!fit)
    fit = Histo.GetFunction(fitfun->name().c_str());
  if (fit) {
    float confidence=0.95;
    if (input_pars.size() > 1)
      confidence=input_pars[1];
    
    
    double chi2prob = TMath::Prob( fit->GetChisquare(), fit->GetNDF());
    std::stringstream chimess;
    chimess << "Bad Fit with function "<<fitfun->name()<<
      ": chi2="<< fit->GetChisquare() << " with "<< fit->GetNDF() <<
      " dof";
    
    raiseMessage(anaID, 
                 chi2prob < warn_thresholds[0],
                 chi2prob < alarm_thresholds[0],
                 chimess.str(),
                 hname);
    // convert confidence to upper/lower limits
    double Nsigma = TMath::NormQuantile(1-confidence);
    double uplimit,lowlimit;
    for (int ip=0; ip<fitfun->np() ; ip++) {
      uplimit = fit->GetParameter(ip)+fit->GetParError(ip)*Nsigma;
      lowlimit= fit->GetParameter(ip)-fit->GetParError(ip)*Nsigma;
      std::stringstream mess;
      mess << "parameter "<< (fitfun->parNames())[ip] << " of fit " <<
        fitfun->name()<< " is out of range: "<< fit->GetParameter(ip) <<
        " (alarm range is "<<alarm_thresholds[1+ip*2] << " to "<<
        alarm_thresholds[2+ip*2]<<")";
      raiseMessage(anaID,
                   uplimit < warn_thresholds[1+ip*2],
                   uplimit < alarm_thresholds[1+ip*2],
                   mess.str(),
                   hname);
      raiseMessage(anaID,
                   lowlimit > warn_thresholds[2+ip*2],
                   lowlimit > alarm_thresholds[2+ip*2],
                   mess.str(),
                   hname);
      
    }
  }
}


