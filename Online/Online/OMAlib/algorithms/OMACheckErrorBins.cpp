// $Id: OMACheckErrorBins.cpp,v 1.3 2010-04-07 09:40:13 ggiacomo Exp $

#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include <sstream>

OMACheckErrorBins::OMACheckErrorBins(OMAlib* Env) : 
  OMACheckAlg("CheckErrorBins", Env) {
  m_ninput = 2;
  m_inputNames.push_back("Bin");   m_inputDefValues.push_back(-1.);
  m_inputNames.push_back("Mode");  m_inputDefValues.push_back(1.);
  m_npars = 1;
  m_parnames.push_back("Threshold");  m_parDefValues.push_back(0.);  
  m_needRef = false;
  m_doc = "Produce alarm if the content of a bin (indicating errors) is above Threshold.";
  m_doc += "Bin starts from 1, use -1 to check all histogram bins.";
  m_doc += "Threshold is the absolute bin content if Mode=1, the fraction wrt histogram area if Mode=2.";
  m_doc += "Applies to 1d histograms only";

  m_minEntries=0;
  m_minEntriesPerBin=0;
}

void OMACheckErrorBins::exec(TH1 &Histo,
                            std::vector<float> & warn_thresholds,
                            std::vector<float> & alarm_thresholds,
                            std::vector<float> & input_pars,
                            unsigned int anaID,
                            TH1* Ref) {
  if( warn_thresholds.size() <m_npars ||  alarm_thresholds.size() <m_npars )
    return;
  if (Histo.GetDimension() != 1) return;
  Ref=Ref;
  int Bin = -1;
  int Mode = (int) (m_inputDefValues[1]+.1);
  if( input_pars.size() > 0) {
    if (input_pars[0]>0.)
      Bin = (int) (input_pars[0]+.1);
  }
  if( input_pars.size() > 1) {
    Mode = (int) (input_pars[1]+.1);
  }
  if (Bin > Histo.GetNbinsX() )  return;
  int minBin = (Bin<0) ? 1 : Bin;
  int maxBin = (Bin<0) ? Histo.GetNbinsX() : Bin;
  OMAMessage::OMAMsgLevel level = OMAMessage::INFO;
  int nbad=0;
  std::string problem;
  for (int bin=minBin ; bin<=maxBin ; bin++) {
    double cont = Histo.GetBinContent(bin);
    if (Mode == 2) {
      if(Histo.GetSumOfWeights() > 0.1) {
        cont /= Histo.GetSumOfWeights();
      }
    }
    std::string binName=Histo.GetXaxis()->GetBinLabel(bin);
    if (binName.empty()) binName=Form("%d",bin);
    if (cont > alarm_thresholds[0]) {
      level=OMAMessage::ALARM;
      problem = Form("bin %s content is %.2f (alarm threshold=%.2f) ",
                     binName.c_str(), (float)cont, alarm_thresholds[0]);
      nbad++;
    }
    else if (cont > warn_thresholds[0]) {
      nbad++;
      if (level<OMAMessage::WARNING) {
        problem = Form("bin %s content is %.2f (warning threshold=%.2f) ",
                       binName.c_str(), (float)cont, warn_thresholds[0]);
      }
    }
  }
  if (nbad>1) {
    problem += Form(" and other %d bins are above threshold.",nbad-1);
  }
  if (level > OMAMessage::INFO) {
    std::string hname(Histo.GetName());
    raiseMessage(anaID, level, problem, hname);
  }
}

// disable check on statistics
bool OMACheckErrorBins::notEnoughStats(TH1* h) {
  h=h; // cheat compiler
  return false;
}

