#ifndef _TrackMaps_H
#define _TrackMaps_H

#include <map>
#include <string>
#include "Event/Track.h"

namespace TrackMonitorMaps{ 

  typedef std::map<std::string,LHCb::Track::Types> TypeMap; 
  const TypeMap& typeDescription() ;

  struct HistoRange{

      HistoRange(std::string id, double xMin, double xMax): 
    	fid(id), fxMin(xMin),  fxMax(xMax)
      {}
      std::string fid;
      double fxMin;
      double fxMax;
  };

  typedef std::map<LHCb::Track::AdditionalInfo, HistoRange> InfoHistMap; 
  const InfoHistMap& infoHistDescription() ;

}
                                                                                                

                                                                    
#endif
