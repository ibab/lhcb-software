// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/Phase.h"
#include <cmath>
#include "Mint/CLHEPPhysicalConstants.h"

using namespace std;
using namespace MINT;

const double Phase::_minPh =   0; // this makes it 0 to 2pi
//const double Phase::_minPh = -pi; // this makes it -pi to pi

double Phase::rangeMax(){
  return _minPh + twopi;
}
double Phase::rangeMin(){
  return _minPh;
}
void Phase::toRange(){
  while (_ph >  rangeMax()) _ph -= 2 * pi;
  while (_ph <  rangeMin()) _ph += 2 * pi;
}
Phase::Phase(const Phase& other)
  : _ph(other._ph){
  toRange();
}
Phase::Phase(double ph_in)
  : _ph(ph_in){
  toRange();
}
Phase& Phase::operator*=(double rhs){
  _ph *= rhs;
  toRange();
  return *this;
}
Phase& Phase::operator/=(double rhs){
  _ph /= rhs;
  toRange();
  return *this;
}
Phase& Phase::operator+=(const Phase& rhs){
  _ph += (double) rhs;
  toRange();
  return *this;
}
Phase Phase::operator+(const Phase& rhs) const{
  Phase newPhase(*this);
  newPhase += rhs;
  return newPhase;
}  
const Phase& Phase::operator+() const{
  return (*this);
}  
Phase& Phase::operator-=(const Phase& rhs){
  _ph -= (double) rhs;
  _ph = fmod(_ph, twopi);
  return *this;
}
Phase Phase::operator-(const Phase& rhs) const{
  Phase newPhase(*this);
  newPhase -= rhs;
  return newPhase;
}
Phase Phase::operator-() const{
  Phase newPhase(*this);
  newPhase *= -1.;
  return newPhase;
}
double Phase::inDegrees() const{
  return _ph * 180.0/pi;
}

void Phase::testPhase(std::ostream& os) const{
  os << " testing Phase object:" << endl;
  Phase testPhase(9);
  os << " testPhase(9) " << testPhase 
     << "( = " << testPhase.inDegrees() << " deg)"
     << endl;
  Phase otherPhase(6);
  os << " otherPhase(3) " <<  otherPhase
     << "( = " << otherPhase.inDegrees() << " deg)"
     << endl;

  os << " sum of the two =" 
     << (otherPhase + testPhase).inDegrees()
     << " deg \n difference: " 
     << (otherPhase - testPhase).inDegrees()
     << " deg \n negative of testPhase "
     << -testPhase
     << " = " << (-testPhase).inDegrees() 
     << " deg" << endl;
  return;
}
//
