
#include "Mint/Eff4piSymmetric.h"
#include "Mint/CLHEPSystemOfUnits.h"

using namespace std;
using namespace MINT;

Eff4piSymmetric::Eff4piSymmetric(int order
				 , const DalitzEventPattern& pat
				 , MINT::MinuitParameterSet* pset)
  : _order(order)
  , _pat(pat)
  , _pset(pset)
{
  init();
}

bool Eff4piSymmetric::init(){
  bool success = true;
  success &= setCentreAndTypicalVal();
  success &= makeTerms();
  success &= makeSijTijVectors();
  return success;
}
bool Eff4piSymmetric::makeTerms(){
  //  cout << "Hello from Eff4piSymmetric::init()" << endl;
  for(int i=0; i <= _order; i++){
    _tTerms.push_back(symMultiPolyTerm(2,i));
    _sTerms.push_back(symMultiPolyTerm(3,i));
  }

  double initVal = 1; // make 1st one 1, all others 0
  for(int thisOrder = 0; thisOrder <= _order; thisOrder++){
    for(int tord = 0; tord <= thisOrder; tord++){
      int sord = thisOrder - tord;
      symMultiPolyTerm& tTermsTord(_tTerms[tord]);
      symMultiPolyTerm& sTermsSord(_sTerms[sord]);

      double guessError = 1.0/(10*pow(3.0, thisOrder));

      for(unsigned int ti = 0; ti < tTermsTord.size(); ti++){
	for(unsigned int si = 0; si < sTermsSord.size(); si++){
	  counted_ptr<ProdWithFitParameter>
	    ptr(new ProdWithFitParameter(tTermsTord[ti], sTermsSord[si]
					 , "a"
					 , initVal
					 , guessError
					 , _pset
					 )
		);
	  _allTerms.push_back(ptr);
	  initVal=0; // make 1st one 1, all others 0
	}
	
      }
    }
  }
  return true;
}

bool Eff4piSymmetric::setCentreAndTypicalVal(){
  _t01_ctr = (_pat.sijMax(2,3,4) + _pat.sijMin(2,3,4) ) / 2.0;
  _s12_ctr = (_pat.sijMax(1,2)   + _pat.sijMin(1,2)   ) / 2.0;
  _s23_ctr = (_pat.sijMax(2,3)   + _pat.sijMin(2,3)   ) / 2.0;
  _s34_ctr = (_pat.sijMax(3,4)   + _pat.sijMin(3,4)   ) / 2.0;
  _t40_ctr = (_pat.sijMax(1,2,3) + _pat.sijMin(1,2,3) ) / 2.0;
  
  _t01_del = fabs(_pat.sijMax(2,3,4) - _pat.sijMin(2,3,4) ) / 2.0;
  _s12_del = fabs(_pat.sijMax(1,2)   - _pat.sijMin(1,2)   ) / 2.0;
  _s23_del = fabs(_pat.sijMax(2,3)   - _pat.sijMin(2,3)   ) / 2.0;
  _s34_del = fabs(_pat.sijMax(3,4)   - _pat.sijMin(3,4)   ) / 2.0;
  _t40_del = fabs(_pat.sijMax(1,2,3) - _pat.sijMin(1,2,3) ) / 2.0;
  
  printCentreAndTypicalVal();
  return true;
}

void Eff4piSymmetric::printCentreAndTypicalVal(std::ostream& os) const{
  os << "Eff4piSymmetric: Centre: " 
     << _t01_ctr << ", " << _s12_ctr 
     << ", " << _s23_ctr << ", " << _s34_ctr 
     << ", " << _t40_ctr
     << endl;
  os << "... and widths: "
     << _t01_del << ", " << _s12_del 
     << ", " << _s23_del << ", " << _s34_del 
     << ", " << _t40_del
     << endl;
  
}

bool Eff4piSymmetric::makeSijTijVectors(){
  _tijVec.push_back(&_t01);
  _tijVec.push_back(&_t40);
  
  _sijVec.push_back(&_s12);
  _sijVec.push_back(&_s23);
  _sijVec.push_back(&_s34);

  return true;
}

void Eff4piSymmetric::print(std::ostream& os) const{
  for(unsigned int i=0; i < _allTerms.size(); i++){
    if(0 != i) os << "\n\t + ";
    os << *(_allTerms[i]);
  }
}

double Eff4piSymmetric::getVal( double t01, double s12, double s23
				, double s34, double t40) const{
  _t01 = (t01 - _t01_ctr)/_t01_del;
  _s12 = (s12 - _s12_ctr)/_s12_del;
  _s23 = (s23 - _s23_ctr)/_s23_del;
  _s34 = (s34 - _s34_ctr)/_s34_del;
  _t40 = (t40 - _t40_ctr)/_t40_del;

  return getValFromSavedCoordinates();
}
double Eff4piSymmetric::getVal(const IDalitzEvent& evt) const{

  _t01 = (evt.t(0,1) - _t01_ctr)/_t01_del;
  _s12 = (evt.s(1,2) - _s12_ctr)/_s12_del;
  _s23 = (evt.s(2,3) - _s23_ctr)/_s23_del;
  _s34 = (evt.s(3,4) - _s34_ctr)/_s34_del;
  _t40 = (evt.t(4,0) - _t40_ctr)/_t40_del;

  //  cout << "extracted sij" << endl;
  return getValFromSavedCoordinates();
}

double Eff4piSymmetric::getValFromSavedCoordinates() const{
  double sum=0;
  for(unsigned int i=0; i < _allTerms.size(); i++){
    sum += (_allTerms[i])->getVal(_tijVec, _sijVec);
  }
  return sum;
}

double Eff4piSymmetric::RealVal(IDalitzEvent& evt){
  //  cout << "Hello from  Eff4piSymmetric::RealVal()" << endl;
  return this->getVal(evt);
}

void Eff4piSymmetric::fixZerothTherm(){
  if(0 != _allTerms.size()) _allTerms[0]->fp()->fix();
}

Eff4piSymmetric::
ProdWithFitParameter::ProdWithFitParameter(const symPolyTerm& p1
					   , const symPolyTerm& p2
					   , const std::string& prefix
					   , double initValue
					   , double guessError
					   , MinuitParameterSet* pset)
  : _t1(p1)
  , _t2(p2)
  , _prefix(prefix)
  , _pset(pset)
{
  makeFitParameter(initValue, guessError);
}


std::string Eff4piSymmetric::
ProdWithFitParameter::name() const{
  return _prefix + "_" + _t1.name() + "_" + _t2.name();
}

bool Eff4piSymmetric::
ProdWithFitParameter::makeFitParameter(double initValue, double guessError){
  counted_ptr<FitParameter> 
    fp(new FitParameter(name()
			, 0
			, initValue, guessError, 0, 0
			, _pset, NamedParameterBase::QUIET)
       );
  _fp = fp;
  return true;
}

void Eff4piSymmetric::
ProdWithFitParameter::print(std::ostream& os) const{
  os << "(" << _fp->name() << ")*(" << _t1 << ") * (" << _t2 << ")";
}


double Eff4piSymmetric::
ProdWithFitParameter::getVal(const std::vector<double* >& t1_coords
			     , const std::vector<double* >& t2_coords
			     ) const{

  return ((double) (*_fp)) * _t1.eval(t1_coords) * _t2.eval(t2_coords);
}


std::ostream& operator<<(std::ostream& os
			 , const Eff4piSymmetric::ProdWithFitParameter& pwfp){
  pwfp.print(os);
  return os;
}

std::ostream& operator<<(std::ostream& os
			 , const Eff4piSymmetric& e4pi){
  e4pi.print(os);
  return os;
}

