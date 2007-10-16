// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/src/OMAAlgorithms.cpp,v 1.1.1.1 2007-10-16 13:03:03 ggiacomo Exp $
#include <TH1F.h>
#include <TH2F.h>
#include "OMAlib/OMAAlgorithms.h"


OMAEfficiency::OMAEfficiency() : OMAHcreatorAlg("Efficiency") {
  m_ninput = 2;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Ratio of two histograms with binomial errors";
}

TH1* OMAEfficiency::exec( std::vector<TH1*> *sources,
			  std::vector<float> *params,
			  std::string outName,
			  std::string outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  params=params; // avoid compil. warning
  if (sources->size() >= 2)
    out = exec( sources->at(0), sources->at(1), outName, outTitle, existingHisto);
  return out;
}

TH1* OMAEfficiency::exec(TH1* okH,
			 TH1* allH,
			 std::string outName,
			 std::string outTitle,
			 TH1* existingHisto) {  
  TH1* outHist=0;
  if (existingHisto) {
    outHist= dynamic_cast<TH1F*>(existingHisto);
    if (!outHist)
      outHist= dynamic_cast<TH2F*>(existingHisto);
  }
  if (!outHist) {
    if (dynamic_cast<TH1F*>(okH)) 
      outHist = (TH1*) new TH1F (outName.data(), outTitle.data(), 
			  okH->GetNbinsX(), 
			  okH->GetXaxis()->GetXmin(),
			  okH->GetXaxis()->GetXmax());  
    else if(dynamic_cast<TH2F*>(okH))
      outHist = (TH1*) new TH2F (outName.data(), outTitle.data(), 
			  okH->GetNbinsX(), 
			  okH->GetXaxis()->GetXmin(),
			  okH->GetXaxis()->GetXmax(),
			  okH->GetNbinsY(), 
			  okH->GetYaxis()->GetXmin(),
			  okH->GetYaxis()->GetXmax()); 
  }
  if(outHist)
    outHist->Divide(okH, allH, 1., 1., "B");
  return outHist;
}

//----------------------------------------------------------------------//

OMACheckXMinMax::OMACheckXMinMax() : OMACheckAlg("CheckXMinMax") {
  m_npars = 2;
  m_parnames.push_back("Min");
  m_parnames.push_back("Max");
  m_doc = "Check that all Histogram X entries are within Min and Max";
}

void OMACheckXMinMax::exec(TH1 &Histo,
			   std::vector<float> & warn_thresholds,
			   std::vector<float> & alarm_thresholds,
			   std::ostream &outstream) {
  exec(Histo,warn_thresholds[0],warn_thresholds[1],
       alarm_thresholds[0],alarm_thresholds[1],outstream);
}

void OMACheckXMinMax::exec(TH1 &Histo,
		       float warn_min,
		       float warn_max,
		       float alarm_min,
		       float alarm_max,
		       std::ostream &outstream) {
  bool ok=true;
  outstream << "Running algorithm CheckXMinMax on histogram "<<Histo.GetTitle()
	    <<" ... ";
  if (false == check(Histo,alarm_min,alarm_max) ) { // alarm on
    outstream << endl <<
      "  ALARM from Histogram "<<Histo.GetTitle()<< " : entries out of range"<<endl;
    ok = false;
  }
  else {
    if (false == check(Histo,warn_min,warn_max) ) { // warning on
      outstream << endl << 
	"  WARNING from Histogram "<<Histo.GetTitle()<< " : entries out of range"<<endl;
      ok = false;
    }
  }
  if (ok) outstream << " OK" <<endl;
}

bool OMACheckXMinMax::check(TH1 &Histo,
			    float min,
			    float max) {
  bool out = true;
  static const double contTh = 0.01;
  if( Histo.Integral(0,Histo.FindBin(min)) > contTh) out=false;
  if(out)
    if( Histo.Integral(Histo.FindBin(max),Histo.GetNbinsX()+1) > contTh) out=false;
  return out;
}
