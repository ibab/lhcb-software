#ifndef _TrackMaps_H
#define _TrackMaps_H

#include <map>
#include <string>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#include "Event/Track.h"
#include "MCInterfaces/IMCReconstructible.h"

namespace TrackIdealPR{ 

  typedef std::map<IMCReconstructible::RecCategory,LHCb::Track::Types> CatMap; 
  const CatMap& catToType() ;


}

#endif
