#ifndef MINT_EFF_FOUR_PI_SYMMETRIC_HH
#define MINT_EFF_FOUR_PI_SYMMETRIC_HH

#include "Mint/IDalitzEvent.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/IReturnRealForEvent.h"

#include "Mint/FitParameter.h"
#include "Mint/MinuitParameterSet.h"
#include "Mint/counted_ptr.h"

#include "Mint/symMultiPolyTerm.h"

#include <vector>
#include <iostream>

class Eff4piSymmetric : virtual public MINT::IReturnRealForEvent<IDalitzEvent>
{
 public:
  class ProdWithFitParameter{
    MINT::counted_ptr<MINT::FitParameter> _fp;
    const symPolyTerm& _t1;
    const symPolyTerm& _t2;
    std::string _prefix;
    MINT::MinuitParameterSet* _pset;
    
    bool makeFitParameter(double initVal, double guessError);
    
  public:
    std::string name() const;
    ProdWithFitParameter(const symPolyTerm& p1
			 , const symPolyTerm& p2
			 , const std::string& prefix
			 , double initValue
			 , double guessError
			 , MINT::MinuitParameterSet* pset=0);

    double getVal(const std::vector<double* >& t1_coords
		  , const std::vector<double* >& t2_coords
		  ) const;
    void print(std::ostream& os = std::cout) const;

    MINT::counted_ptr<MINT::FitParameter> fp(){return _fp;}
  };
  
 protected:
  int _order;

  DalitzEventPattern _pat;
  MINT::MinuitParameterSet* _pset;

  // can swap all three s terms amongst themselves.
  // can swap the two t-terms (that's the bit
  // that assumes charge symmetry)
  mutable double _t01, _s12, _s23, _s34, _t40;
  double _t01_ctr, _s12_ctr, _s23_ctr, _s34_ctr, _t40_ctr;
  double _t01_del, _s12_del, _s23_del, _s34_del, _t40_del;
  
  mutable std::vector<double* > _tijVec, _sijVec;

  std::vector<symMultiPolyTerm> _tTerms, _sTerms;
  std::vector< MINT::counted_ptr< ProdWithFitParameter > > _allTerms;

  bool init();
  bool makeTerms();
  bool makeSijTijVectors();
  bool setCentreAndTypicalVal();

  virtual double getValFromSavedCoordinates() const;


 public:
  
  Eff4piSymmetric(int order
		  , const DalitzEventPattern& pat
		  , MINT::MinuitParameterSet* pset=0);
  virtual double getVal( double t01, double s12, double s23
			 , double s34, double t40) const;
  virtual double getVal(const IDalitzEvent& evt) const;
  virtual double getVal(const IDalitzEvent* evt) const{
    // for backwards compatibility only, deprecated.
    if(0 == evt) return 0;
    return getVal(*evt);
  }

  virtual double RealVal(IDalitzEvent& evt);
  
  void print(std::ostream& os = std::cout) const;
  void printCentreAndTypicalVal(std::ostream& os = std::cout) const;
  void fixZerothTherm();

  virtual ~Eff4piSymmetric(){}

};

std::ostream& operator<<(std::ostream& os
			 , const Eff4piSymmetric::ProdWithFitParameter& pwfp);

std::ostream& operator<<(std::ostream& os
			 , const Eff4piSymmetric& e4pi);


#endif
//
