#include "HltBase/EFunctions.h"
#include "HltFunctions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltFunctions
//
// 2006-07-27 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------


using namespace LHCb;

double Hlt::ETCalo::operator() (const Track& track)  const
{
  const LHCb::State* state = track.stateAt(State::MidHCal);
  if ( 0 == state ) { return 1e6; }
  
  double p = state->p();
  double x = state->x();
  double y = state->y();
  double z = state->z();
  double sintheta = sqrt(x*x+y*y)/sqrt(x*x+y*y+z*z);
  double et = p*sintheta;
  // std::cout << " et " << et << " sintheta " << sintheta << " p " << p
  //          << std::endl;
  return et;
}

double Hlt::DeltaP::operator() (const Track& track1, const Track& track2)  const
{
  double p1 = track1.p();
  double p2 = track2.p();
  double delta = (p1-p2)/p2;
  return delta;
}

double Hlt::DeltaE::operator() (const Track& track)  const
{
  const LHCb::State* state = track.stateAt(LHCb::State::MidHCal) ;
  if ( 0 == state ) { return 1.e6 ; }
  double p = track.p();
  double e = state->p();
  double de = e*(sqrt( 0.60*0.60 + 0.70*0.70/e ));
  double delta = (e-p)/de;
  return delta;
}

