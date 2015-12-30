// $Id: IPVSeeding.h,v 1.1 2008-06-11 19:28:24 witekma Exp $
#ifndef NEWTOOL_IPVSEEDING_H
#define NEWTOOL_IPVSEEDING_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/Point3DTypes.h"
// From Event
#include "Event/Track.h"


/** @class IPVSeeding IPVSeeding.h newtool/IPVSeeding.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2008-05-19
 */
class IPVSeeding : public extend_interfaces<IAlgTool> {
public:
  DeclareInterfaceID( IPVSeeding, 2, 0 );

  virtual 
  std::vector<Gaudi::XYZPoint> 
  getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
           const Gaudi::XYZPoint& beamspot) const =0;



};
#endif // NEWTOOL_IPVSEEDING_H
