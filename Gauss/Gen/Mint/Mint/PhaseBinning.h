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

 public:
  PhaseBinning(FitAmpSum* fas)
  {
	  _fas = fas;
  }

  void fillData(IDalitzEventList* data);
  void fillDataCC(IDalitzEventList* data);
 
  int createBinning(IDalitzEventList* events
		    , int minPerBin = 10
		    , int maxPerBin = 100
		    );

  double setEvents(IDalitzEventList* data
			 , IDalitzEventList* mc
			 );

};

std::ostream& operator<<(std::ostream& os, const ScpBinning& c2b);

#endif
//
