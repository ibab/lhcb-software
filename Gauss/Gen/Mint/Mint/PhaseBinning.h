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

class PhaseBinning : public ScpBinning {

  FitAmpSum* _fas;

  int _nbins;
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

  virtual void fillData(IDalitzEventList* data);
  virtual void fillDataCC(IDalitzEventList* data);
 
  int createBinning(IDalitzEventList* events );

  virtual double setEvents(IDalitzEventList* data
			 , IDalitzEventList* mc
			 );

};

std::ostream& operator<<(std::ostream& os, const ScpBinning& c2b);

#endif
//
