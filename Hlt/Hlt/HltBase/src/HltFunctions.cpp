#include "HltBase/EFunctions.h"
#include "HltBase/HltFunctions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltFunctions
//
// 2006-07-27 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------


using namespace LHCb;

double Hlt::DeltaP::operator() (const Track& track1, const Track& track2)  const
{
  double p1 = track1.p();
  double p2 = track2.p();
  double delta = (p1-p2)/p2;
  std::cout << " Delta E p1 " << p1 << " p2 " << p2
            << " delta " << delta << std::endl;
  return delta;
}

double Hlt::DeltaE::operator() (const Track& track)  const
{
  if (!track.hasStateAt(State::MidHCal)) return 1e6;
  double p = track.p();
  double e = track.stateAt(State::MidHCal).p();
  double de = e*(sqrt( 0.60*0.60 + 0.70*0.70/e ));
  double et = track.stateAt(State::MidHCal).pt();
  double delta = (e-p)/de;
  std::cout << " Delta E p " << p << " e " << e << " et " << et
            << " error " << de << " delta " << delta << std::endl;
  return delta;
}
void Hlt::VertexCreator::operator() 
  (const LHCb::Track& track1, const LHCb::Track& track2,
   LHCb::RecVertex& ver) const {
  EPoint pos = HltUtils::closestPoint(track1,track2);
  // EVector dis = HltUtils::closestDistance(track1,track2);
  ver.setPosition(pos);
  ver.addToTracks((Track*) &track1);
  ver.addToTracks((Track*) &track2);
  // std::cout << " vertex position " << pos << std::endl;
}

bool Hlt::SortTrackByPt::operator() (const Track* track0, 
                                     const Track* track1 ) const {
  double pt0 = track0->pt()+0.00001*track0->key();
  double pt1 = track1->pt()+0.00001*track1->key();
  return (pt0 > pt1);
}

