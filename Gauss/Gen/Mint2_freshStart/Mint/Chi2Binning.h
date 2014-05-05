#ifndef MINTDALITZ_CHI2_BINNING_HH
#define MINTDALITZ_CHI2_BINNING_HH

#include "Mint/Chi2Box.h"
#include "Mint/Chi2BoxSet.h"
#include "Mint/IFastAmplitudeIntegrable.h"

#include "Mint/IEventList.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEvent.h"
#include "Mint/IReturnRealForEvent.h"
#include "Mint/IDalitzPdf.h"

#include "Mint/DalitzHistoStackSet.h"
#include "TH1D.h"

#include <vector>
#include <iostream>

class Chi2Binning{
  std::vector<Chi2BoxSet> _boxSets;
  int _nData;
  int _nMC;
  double _totalMCWeight;

  static int* __colourPalette;
  static int __Ncol;
  static void makeColourPaletteBlueGrey();
  static void makeColourPaletteBlueWhite();
  static void makeColourPaletteRGB();
  static void makeColourPalette();
  static int* getColourPalette();

  void setHistoColours();

  Chi2BoxSet splitBoxes(MINT::IEventList<DalitzEvent>* events // for backward
			, int maxPerBin          // compatibility
			) const;  // will be removed in future.
  Chi2BoxSet splitBoxes(MINT::IEventList<DalitzEvent>& events
			 , int maxPerBin
			 ) const;
  int mergeBoxes(Chi2BoxSet& boxes, int minPerBin);

  void resetEventCounts();
  void fillData(MINT::IEventList<DalitzEvent>& data);
  void fillData(MINT::IEventList<DalitzEvent>* dataPtr){ 
      // for backward compatibility
    if(0 != dataPtr) fillData(*dataPtr);// will be removed in future
  }
  void fillMC(MINT::IEventList<DalitzEvent>& mc, IDalitzPdf* pdf);
  void fillMC(MINT::IEventList<DalitzEvent>* mcPtr, IDalitzPdf* pdf){
    if(0 != mcPtr) fillMC(*mcPtr, pdf);
  }
  double normFactor() const;
  void setBoxesNormFactors();
  void sortByChi2();
 public:
  Chi2Binning();
  int createBinning(MINT::IEventList<DalitzEvent>* events
		    , int minPerBin = 10
		    , int maxPerBin = 100
		    );

  double setEventsAndPdf(MINT::IEventList<DalitzEvent>* data
			 , MINT::IEventList<DalitzEvent>* mc
			 , IDalitzPdf* pdf
			 , IFastAmplitudeIntegrable* fas=0// usually FitAmpSum*
			 );


  void setFas(IFastAmplitudeIntegrable* fas);

  int numBins() const;

  double getChi2_perBin() const;  
  double chi2_ofBin(unsigned int i) const;
  double getMaxChi2() const;
  void print(std::ostream& os = std::cout) const;

  DalitzHistoStackSet getDataHistoStack();
  DalitzHistoStackSet getMCHistoStack();
  MINT::counted_ptr<TH1D> getChi2Distribution() const;
  void drawChi2Distribution(const std::string& fname = "chi2Distribution.eps")const;

};

class lessByChi2BoxData{
 public:
  bool operator()(const Chi2Box& a, const Chi2Box& b) const;
};

class lessByChi2BoxSetChi2{
 public:
  bool operator()(const Chi2BoxSet& a, const Chi2BoxSet& b) const;
};

std::ostream& operator<<(std::ostream& os, const Chi2Binning& c2b);

#endif
//
