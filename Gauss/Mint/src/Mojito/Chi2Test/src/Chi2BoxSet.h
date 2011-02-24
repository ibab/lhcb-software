#ifndef MINT_DALITZ_CHI_SQUARED_BOX_SET_HH
#define MINT_DALITZ_CHI_SQUARED_BOX_SET_HH

#include "Chi2Box.h"
#include "IIntegrationCalculator.h"
#include "counted_ptr.h"

#include <iostream>

#include <vector>

class IDalitzEvent;

class Chi2BoxSet : public std::vector<Chi2Box>{
  MINT::counted_ptr<IIntegrationCalculator> _integCalc;

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

  int nData() const;

  int nMC() const;  
  double weightedMC() const;
  double weightedMC2() const;

  double rmsMC(int Ntotal) const;

};
#endif
//
