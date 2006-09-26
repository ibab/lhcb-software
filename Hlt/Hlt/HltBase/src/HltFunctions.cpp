#include "HltBase/EFunctions.h"
#include "HltBase/HltFunctions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltFunctions
//
// 2006-07-27 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------


using namespace LHCb;

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
  double pt0 = track0->pt();
  double pt1 = track1->pt();
  return (pt0 > pt1);
}

