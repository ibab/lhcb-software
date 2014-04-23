#ifndef MINT_DALITZ_CHI_SQUARED_BOX_SET_HH
#define MINT_DALITZ_CHI_SQUARED_BOX_SET_HH

#include "Mint/Chi2Box.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/counted_ptr.h"
#include "Mint/DalitzHistoSet.h"

#include "TColor.h"
#include "Rtypes.h"

#include <iostream>
#include <vector>

class IDalitzEvent;

class Chi2BoxSet : public std::vector<Chi2Box>{
  MINT::counted_ptr<IIntegrationCalculator> _integCalc;

  DalitzHistoSet _histoData, _histoMC;

  double _normFactor;

 public:
  Chi2BoxSet();
  Chi2BoxSet(const DalitzEventPattern& pat
	     , const MINT::counted_ptr<IIntegrationCalculator>& integPtr=
	     (MINT::counted_ptr<IIntegrationCalculator>)0);
  Chi2BoxSet(const DalitzArea& area
	     , const MINT::counted_ptr<IIntegrationCalculator>& integPtr=
	     (MINT::counted_ptr<IIntegrationCalculator>)0);
  Chi2BoxSet(const std::vector<Chi2Box>& other);
  Chi2BoxSet(const Chi2BoxSet& other);


  void setIIntegrationCalculator(MINT::counted_ptr<IIntegrationCalculator> fap){
    _integCalc=fap;
  }

  void add(const Chi2Box& box);
  void add(const Chi2BoxSet& boxSet);

  void resetEventCounts();

  bool addData(const IDalitzEvent& evt);
  bool addData(const IDalitzEvent* evt);
  bool addMC(IDalitzEvent& evt, double weight);
  bool addMC(IDalitzEvent* evt, double weight);

  void printBoxInfo(std::ostream& os = std::cout)const;
  void print(std::ostream& os = std::cout)const{ printBoxInfo(os); }

  int nData() const;

  int nMC() const;  
  double weightedMC() const;
  double weightedMC2() const;

  double rmsMC(int Ntotal) const;

  void setNormFactor(double nf){_normFactor = nf;}
  double normFactor()const{return _normFactor;}

  void setHistoColour(Color_t fcolor);
  void setFillColour(Color_t fcolor);
  void setLineColour(Color_t fcolor);
  DalitzHistoSet& histoData();
  const DalitzHistoSet& histoData()const;
  DalitzHistoSet& histoMC();
  const DalitzHistoSet& histoMC()const;

  double chi2(double normFactorPassed=-1) const;
};

std::ostream& operator<<(std::ostream& os, const Chi2BoxSet& c2bs);

#endif
//
