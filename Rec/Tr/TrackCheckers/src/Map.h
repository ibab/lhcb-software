#ifndef _TrackMaps_H
#define _TrackMaps_H

#include <map>
#include <string>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'

#include "Event/Track.h"
#include "MCInterfaces/IMCReconstructible.h"

namespace TrackMaps{ 

  typedef std::map<std::string,LHCb::Track::Types> TypeMap; 
  const TypeMap& typeDescription() ;

  typedef std::map<std::string,LHCb::Track::AdditionalInfo> InfoMap; 
  // const InfoMap& infoDescription() ;

  typedef std::map<std::string,IMCReconstructible::RecCategory> RecMap;
  const RecMap& recDescription();

  class HistoRange{
    public:

      HistoRange(std::string id, double xMin, double xMax): 
	fid(id),
        fxMin(xMin),  
        fxMax(xMax){;}
      std::string fid;
      double fxMin;
      double fxMax;
  };

  typedef std::map<LHCb::Track::AdditionalInfo, HistoRange> InfoHistMap; 
  const InfoHistMap& infoHistDescription() ;

}
                                                                                                

                                                                    
#endif
