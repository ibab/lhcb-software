#ifndef MINT_DALITZ_SCP_BOX_SET_HH
#define MINT_DALITZ_SCP_BOX_SET_HH

#include "Mint/ScpBox.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/counted_ptr.h"
#include "Mint/DalitzHistoSet.h"

#include "TColor.h"
#include "Rtypes.h"

#include <iostream>
#include <vector>

class IDalitzEvent;

class ScpBoxSet : public std::vector<ScpBox>{
  MINT::counted_ptr<IIntegrationCalculator> _integCalc;

  DalitzHistoSet _histoData, _histoMC;

  double _normFactor;

 public:
  ScpBoxSet();
  ScpBoxSet(const DalitzEventPattern& pat
	     , const MINT::counted_ptr<IIntegrationCalculator>& integPtr=
	     (MINT::counted_ptr<IIntegrationCalculator>)0);
  ScpBoxSet(const DalitzArea& area
	     , const MINT::counted_ptr<IIntegrationCalculator>& integPtr=
	     (MINT::counted_ptr<IIntegrationCalculator>)0);
  ScpBoxSet(const std::vector<ScpBox>& other);
  ScpBoxSet(const ScpBoxSet& other);


  void setIIntegrationCalculator(MINT::counted_ptr<IIntegrationCalculator> fap){
    _integCalc=fap;
  }

  void add(const ScpBox& box);
  void add(const ScpBoxSet& boxSet);

  void resetEventCounts();

  bool addData(const IDalitzEvent& evt, double weight = 1.0);
  bool addData(const IDalitzEvent* evt, double weight = 1.0);
  bool addData(int);

  bool addMC(IDalitzEvent& evt, double weight);
  bool addMC(IDalitzEvent* evt, double weight);
  bool addMC(int);

  bool subtractData(const IDalitzEvent& evt, double weight);
  bool subtractData(const IDalitzEvent* evt, double weight);
  bool subtractMC(IDalitzEvent& evt, double weight);
  bool subtractMC(IDalitzEvent* evt, double weight);


  void printBoxInfo(std::ostream& os = std::cout)const;
  void print(std::ostream& os = std::cout)const{ printBoxInfo(os); }

  int nData() const;
  double weightedData() const;

  int nMC() const;  
  double weightedMC() const;
  double weightedMC2() const;

  double weightedBkg() const;
  double weightedBkgCC() const;



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

  double scp(double normFactorPassed=1) const;

  bool boxSet(const IDalitzEvent* Devt);

  void save();
};

std::ostream& operator<<(std::ostream& os, const ScpBoxSet& c2bs);

#endif
//
