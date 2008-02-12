// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/OMAlib/OMAAlgorithms.h,v 1.3 2008-02-12 18:16:28 ggiacomo Exp $
#ifndef OMAALGORITHMS_H
#define OMAALGORITHMS_H 1

#include "OMAlib/OMAalg.h"
#include <TH1.h>

class OMAEfficiency : public OMAHcreatorAlg
{
 public:
  OMAEfficiency();
  virtual TH1* exec( const std::vector<TH1*> *sources,
                     const std::vector<float> *params,
		     std::string outName,
		     std::string outTitle,
		     TH1* existingHisto=0);  
  TH1* exec( TH1* okH,
	     TH1* allH,
	     std::string outName,
	     std::string outTitle,
	     TH1* existingHisto=0);

};

class OMACheckXMinMax : public OMACheckAlg
{
  public:
  OMACheckXMinMax();
  virtual void exec(TH1 &Histo,
		    std::vector<float> & warn_thresholds,
		    std::vector<float> & alarm_thresholds,
		    std::ostream &outstream);
  virtual void exec(TH1 &Histo,
		    float warn_min,
		    float warn_max,
		    float alarm_min,
		    float alarm_max,
		    std::ostream &outstream);
 private:
  bool check(TH1 &Histo,
	     float min,
	     float max);
};



#endif // OMAALGORITHMS_H
