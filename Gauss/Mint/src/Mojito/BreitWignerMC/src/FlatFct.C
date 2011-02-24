// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "FlatFct.h"

FlatFct::FlatFct(const DalitzCoordinate& c)
  : _coord(c)
{}

FlatFct::FlatFct(const FlatFct& other)
  : IGenFct()
  , _coord(other._coord)
{}

/*
counted_ptr<IGenFct> FlatFct::Clone() const{  
  counted_ptr<IGenFct> cptr ( (IGenFct*) new FlatFct(*this) );
  return cptr;

}
*/

void FlatFct::setLimits(double sMin, double sMax){
  _coord.setMin(sMin);
  _coord.setMax(sMax);
}

double FlatFct::generate(TRandom* rnd) const{
  double val = _coord.min() + rnd->Rndm() * (_coord.max() - _coord.min());
  return val;
}

double FlatFct::generateRho(TRandom* rnd) const{
  return generate(rnd);
}

double FlatFct::generatingPDFValue(double sij) const{
  double num = generatingFctValue(sij);
  if(num <=0) return 0;
  double norm = integral();
  if(norm <= 0) return 0;
  return num/norm;
}

double FlatFct::generatingFctValue(double sij) const{
  if( sij < _coord.min() || sij > _coord.max()) return 0;
  return 1;
}
double FlatFct::transformedFctValue(double sij) const{
  if( sij < _coord.min() || sij > _coord.max()) return 0;
  return 1;
}

double FlatFct::transformedFctMax()const{
  return 1;
}

double FlatFct::integral()const{
  return (_coord.max() - _coord.min());
}

DalitzCoordinate FlatFct::getCoordinate() const{
  return _coord;
}
void FlatFct::setCoordinate(const DalitzCoordinate& c){
  _coord = c;
}

//
