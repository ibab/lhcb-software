// $Id: OMACheckEntriesInRange.cpp,v 1.6 2009-06-11 15:17:31 ggiacomo Exp $
#include <TH1F.h>
#include <TF1.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include <sstream>

OMACheckEntriesInRange::OMACheckEntriesInRange(OMAlib* Env) : 
  OMACheckAlg("CheckEntriesInRange", Env) {
  m_ninput = 3;
  m_inputNames.push_back("Xmin");  m_inputDefValues.push_back(-9.E20);
  m_inputNames.push_back("Xmax");  m_inputDefValues.push_back(+9.E20);
  m_inputNames.push_back("CheckFraction"); m_inputDefValues.push_back(1.);
  m_npars = 1;
  m_parnames.push_back("MaxEntries");
  m_parDefValues.push_back(1.);
  m_doc = "Check the number of entries in the given range (Xmin to Xmax). Check is done ";
  m_doc += " on the fraction (CheckFraction=1, default) or on the absolute value (CheckFraction=0)";
  
}

void OMACheckEntriesInRange::exec(TH1 &Histo,
                             std::vector<float> & warn_thresholds,
                             std::vector<float> & alarm_thresholds,
                             std::vector<float> & input_pars,
                             unsigned int anaID,
                             TH1* Ref) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  Ref = NULL; // avoid compil. warning
  double xmin= input_pars.size() > 0 ? input_pars[0] : m_inputDefValues[0];
  double xmax= input_pars.size() > 1 ? input_pars[1] : m_inputDefValues[1];
  float fopt = input_pars.size() > 2 ? input_pars[2] : m_inputDefValues[2];
  bool FracOption = (bool) ((int) (fopt+0.1));

  int Nent=0;
  for (int ihx=1 ; ihx<= Histo.GetNbinsX() ; ihx++) {
    if ( Histo.GetBinLowEdge(ihx)+Histo.GetBinWidth(ihx) > xmin &&
         Histo.GetBinLowEdge(ihx) < xmax) {
      for (int ihy=1 ; ihy<= Histo.GetNbinsY() ; ihy++) {
        if( Histo.GetBinContent(ihx,ihy) < OMAconstants::epsilon)
          Nent++;
      }
    }
  }
  float out = (float)Nent;
  std::string ckty="# entries ";
  if(FracOption && Histo.GetEntries() >0) {
    out /= (float) Histo.GetEntries();
    ckty = "fraction of entries ";
  }

  std::stringstream message;
  std::string hname(Histo.GetName());
  message << ckty <<" in Range "<<xmin<<" - "<<xmax<<" is "<<out;
  raiseMessage( anaID,
                ( out > warn_thresholds[0] ),
                ( out > alarm_thresholds[0] ),
                message.str(),
                hname);
}
