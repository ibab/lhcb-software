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

static const InterfaceID IID_IPVSeeding ( "IPVSeeding", 1, 0 );

/** @class IPVSeeding IPVSeeding.h newtool/IPVSeeding.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2008-05-19
 */
class IPVSeeding : virtual public IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPVSeeding; }
  virtual void getSeeds(std::vector<const LHCb::Track*>& inputTracks,
                        const Gaudi::XYZPoint& beamspot,
                        std::vector<Gaudi::XYZPoint>& seeds) =0;


protected:

private:

};
#endif // NEWTOOL_IPVSEEDING_H
