#ifndef MINT_EFF_FOUR_PI_SYMMETRIC_HH
#define MINT_EFF_FOUR_PI_SYMMETRIC_HH

#include "IGetDalitzEvent.h"
#include "DalitzEventAccess.h"
#include "IDalitzEventAccess.h"
#include "IDalitzEventList.h"
#include "IDalitzEvent.h"

#include "FitParameter.h"
#include "MinuitParameterSet.h"
#include "counted_ptr.h"

#include "symMultiPolyTerm.h"

#include <vector>
#include <iostream>

class Eff4piSymmetric : virtual public IGetDalitzEvent
, public DalitzEventAccess
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
  double _typicalValue; // a typical value that s12 etc can have.

  MINT::MinuitParameterSet* _pset;

  // can swap all three s terms amongst themselves.
  // can swap the two t-terms (that's the bit
  // that assumes charge symmetry)
  mutable double _t01, _s12, _s23, _s34, _t40;
  mutable std::vector<double* > _tijVec, _sijVec;

  std::vector<symMultiPolyTerm> _tTerms, _sTerms;
  std::vector< MINT::counted_ptr< ProdWithFitParameter > > _allTerms;

  bool init();
  bool makeTerms();
  bool makeSijTijVectors();

  virtual double getValFromSavedCoordinates() const;

 public:
  
  Eff4piSymmetric(int order
		  , double typicalValue_sij
		  , IDalitzEventAccess* daddyPDF
		  , MINT::MinuitParameterSet* pset=0);
  Eff4piSymmetric(int order
		  , double typicalValue_sij
		  , IDalitzEventList* evtList
		  , MINT::MinuitParameterSet* pset=0);
  virtual double getVal( double t01, double s12, double s23
			 , double s34, double t40) const;
  virtual double getVal(const IDalitzEvent* evt) const;
  virtual double getVal() const;

  virtual double RealVal();
  
  void print(std::ostream& os = std::cout) const;

  void fixZerothTherm();

  virtual ~Eff4piSymmetric(){}

};

std::ostream& operator<<(std::ostream& os
			 , const Eff4piSymmetric::ProdWithFitParameter& pwfp);

std::ostream& operator<<(std::ostream& os
			 , const Eff4piSymmetric& e4pi);


#endif
//
