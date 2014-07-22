// author: Nunya Business (ya.mum@bristol.ac.uk)
// status:  Mon 22 Jul 2014 19:17:59 GMT
#include <iostream>

#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include "Mint/NonRes.h"

using namespace std;
using namespace MINT;

NonRes::NonRes( const AssociatedDecayTree& tree,
		IDalitzEventAccess* events,
		const std::string& type )
  : BW_BW(tree, events)
  , _type(type)
  , _alpha("NonRes::alpha", 1.0)
{
}

std::complex<double> NonRes::getVal() {
  resetInternals();

  const double m2hh = (BW_BW::daughterP4(0)+BW_BW::daughterP4(1)).M2()/GeV/GeV;

  double non_res;
  if( _type == "Exp" )
    non_res = exp(-_alpha*m2hh);
  else if( _type == "Pow" )
    non_res = pow(m2hh, -_alpha);
  else
    non_res = 1.0;

  return Fr_PDG_BL()*non_res;
}

void NonRes::print( std::ostream& out ) const {
  out << name();
}

void NonRes::print( std::ostream& out ) {
  out << name();
}
    
double NonRes::GetAlpha() const {
  return _alpha;
}

std::ostream& operator<<( std::ostream& out, const NonRes& amp ) {
  amp.print(out);

  return out;
}
