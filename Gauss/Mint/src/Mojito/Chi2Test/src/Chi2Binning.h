#ifndef MINTDALITZ_CHI2_BINNING_HH
#define MINTDALITZ_CHI2_BINNING_HH

#include "Chi2Box.h"
#include "Chi2BoxSet.h"
#include "IFastAmplitudeIntegrable.h"

#include "IDalitzEventList.h"
#include "IDalitzEvent.h"
#include "IGetRealEvent.h"
#include "IDalitzPdf.h"

#include <vector>

class Chi2Binning{
  std::vector<Chi2BoxSet> _boxSets;
  int _nData;
  int _nMC;
  double _totalMCWeight;

  Chi2BoxSet splitBoxes(IDalitzEventList* events
			 , int maxPerBin
			 ) const;
  int mergeBoxes(Chi2BoxSet& boxes, int minPerBin);

  void resetEventCounts();
  void fillData(IDalitzEventList* data);
  void fillMC(IDalitzEventList* mc, IDalitzPdf* pdf);

 public:
  int createBinning(IDalitzEventList* events
		    , int minPerBin = 10
		    , int maxPerBin = 100
		    );

  double setEventsAndPdf(IDalitzEventList* data
			 , IDalitzEventList* mc
			 , IDalitzPdf* pdf
			 , IFastAmplitudeIntegrable* fas=0// usually FitAmpSum*
			 );

  double getChi2_perBin() const;

  void setFas(IFastAmplitudeIntegrable* fas);

  int numBins() const;
  
};

class lessByChi2BoxData{
 public:
  bool operator()(const Chi2Box& a, const Chi2Box& b) const;
};
#endif
//
