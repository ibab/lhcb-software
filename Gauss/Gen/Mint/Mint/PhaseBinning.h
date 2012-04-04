#ifndef MINTDALITZ_PHASE_BINNING_HH
#define MINTDALITZ_PHASE_BINNING_HH


#include "Mint/ScpBoxSet.h"
#include "Mint/ScpBox.h"
#include "Mint/ScpBinning.h"



#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/FitAmpSum.h"

#include "Mint/IDalitzEventList.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IDalitzPdf.h"

#include "Mint/DalitzHistoStackSet.h"
#include "TH1D.h"
#include "TMath.h"

#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
class PhaseBinning : public ScpBinning {

  FitAmpSum* _fas;

  int _nbins;
  DalitzEventPattern _pdg;
 public:
  PhaseBinning()
  {
	  _nbins = 9;
  }
  PhaseBinning(FitAmpSum* fas, int nbins = 9)
  {
	  _fas = fas;
	  _nbins = nbins;
  }
  PhaseBinning(const char* filename,
		  	  DalitzEventPattern pdg,
		  	  const char* treename = "PhaseBinning");

  virtual void fillData(IDalitzEventList* data);
  virtual void fillDataCC(IDalitzEventList* data);
 
  int createBinning(IDalitzEventList* events );
  int createBinning(DalitzEventPattern pdg );

  virtual double setEvents(IDalitzEventList* data
			 , IDalitzEventList* mc
			 );

  double MaxPhase(int bin);

  double Phase(std::complex<double>);

  double MinPhase(int bin);

  void Save(const char* name = "PhaseBinning");

  PhaseBinning operator + (PhaseBinning);

};

std::ostream& operator<<(std::ostream& os, const ScpBinning& c2b);

#endif
//
