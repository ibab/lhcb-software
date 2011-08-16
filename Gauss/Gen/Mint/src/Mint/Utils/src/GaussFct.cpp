// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/GaussFct.h"

#include "Mint/CLHEPPhysicalConstants.h"
#include <cmath>

using namespace MINT;

double GaussFct::x() const{
  return _x;}

double GaussFct::height() const{
  return _height;}

double GaussFct::mean() const{
  return _mean;}

double GaussFct::sigma() const{
  return _sigma;}


void GaussFct::set_x(double x_in) {
  _x = x_in;}

void GaussFct::set_height(double h) {
  _height=h;}

void GaussFct::set_mean(double m) {
  _mean = m;}

void GaussFct::set_sigma(double s) {
  _sigma = s;}

double GaussFct::getVal() const{
  double dxs = (x()-mean())/sigma();
  return height() * exp(-0.5*dxs*dxs);
}

double GaussFct::getIntegral() const{

  return height()*_sqrt2pi*sigma();

}

GaussFct::GaussFct(double x_in, double h, double m, double s)
  : _x(x_in)
  , _height(h)
  , _mean(m)
  , _sigma(s)
  , _sqrt2pi(sqrt(twopi)){

}

GaussFct::GaussFct()
  : _x(0)
  , _height(1./sqrt(twopi))
  , _mean(0)
  , _sigma(1)
  , _sqrt2pi(sqrt(twopi)){

}
//
