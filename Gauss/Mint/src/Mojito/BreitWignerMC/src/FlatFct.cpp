// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/FlatFct.h"
#include <iostream>
using namespace std;

FlatFct::FlatFct(const DalitzCoordinate& c)
  : _coord(c)
  , _boxLimit_min(-9998)
  , _boxLimit_max(-9999)
  , _min(-9999)
  , _max(-9999)
{}

FlatFct::FlatFct(const FlatFct& other)
  : IGenFct()
  , _coord(other._coord)
  , _boxLimit_min(other._boxLimit_min)
  , _boxLimit_max(other._boxLimit_max)
  , _min(other._min)
  , _max(other._max)
{}

/*
counted_ptr<IGenFct> FlatFct::Clone() const{  
  counted_ptr<IGenFct> cptr ( (IGenFct*) new FlatFct(*this) );
  return cptr;

}
*/

void FlatFct::setLimits(double sMin, double sMax){
  bool dbThis=false;
  _coord.setMin(sMin);
  _coord.setMax(sMax);
  if(dbThis){
    cout << "set limits " << sMin << ", " << sMax 
	 << ". check: " << min() << ", " << max()
	 << endl;
  }
  redoLimits();
  if(dbThis){
    cout << "set limits " << sMin << ", " << sMax 
	 << ". check: 2 " << min() << ", " << max()
	 << endl;
  }
}
void FlatFct::setBoxLimits(double sMin, double sMax){
  bool dbThis=false;
  if(dbThis) cout << "setBoxLimits(" << sMin << ", " << sMax << ")" << endl;
  _boxLimit_min=sMin;
  _boxLimit_max=sMax;
  redoLimits();
}
void FlatFct::resetBoxLimits(){
  _boxLimit_min= -9998;
  _boxLimit_max= -9999;
  redoLimits();
}

double FlatFct::min() const{
  return _min;
}
double FlatFct::max() const{
  return _max;
}

void FlatFct::redoLimits(){
  bool dbThis=false;
  _min= _coord.min();
  if(dbThis){
    cout << "_min now " << _min << ", min() " << min() << endl;
    cout << "boxlimits: " << _boxLimit_min 
	 << ", " << _boxLimit_max << endl;
  }
  if(_boxLimit_min < _boxLimit_max && _min > _boxLimit_min){
    _min = _boxLimit_min;
  }
  _max= _coord.max();
  if(_boxLimit_min < _boxLimit_max && _max > _boxLimit_max){
    _max = _boxLimit_max;
  }
  if(dbThis){
    cout << "and _min now " << _min << endl;
    cout << "boxlimits: " << _boxLimit_min 
	 << ", " << _boxLimit_max << endl;
  }
}

double FlatFct::generate(TRandom* rnd) const{
  double val = min() + rnd->Rndm() * (max() - min());
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
  if( sij < min() || sij > max()) return 0;
  return 1;
}
double FlatFct::transformedFctValue(double sij) const{
  if( sij < min() || sij > max()) return 0;
  return 1;
}

double FlatFct::transformedFctMax()const{
  return 1;
}

double FlatFct::integral()const{
  return (max() - min());
}

DalitzCoordinate FlatFct::getCoordinate() const{
  return _coord;
}
void FlatFct::setCoordinate(const DalitzCoordinate& c){
  _coord = c;
}

//
