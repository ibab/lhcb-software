// $Id: SimplePVSeedTool.h,v 1.1 2009-12-16 11:51:52 witekma Exp $
#ifndef PATPV_SIMPLEPVSEEDTOOL_H
#define PATPV_SIMPLEPVSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "IPVSeeding.h"            // Interface
#include "Event/Track.h"


/** @class SimplePVSeedTool SimplePVSeedTool.h tmp/SimplePVSeedTool.h
 *
 *
 *  @author Mariusz Witek
 *  @date   2005-11-19
 */
class SimplePVSeedTool : public GaudiTool, virtual public IPVSeeding {
public:

  /// Standard constructor
  SimplePVSeedTool( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  virtual ~SimplePVSeedTool( ); ///< Destructor

  void getSeeds(std::vector<const LHCb::Track*>& inputTracks,
		const Gaudi::XYZPoint& beamspot,
		std::vector<Gaudi::XYZPoint>& seeds);

protected:

private:

};
#endif // PATPV_SIMPLEPVSEEDTOOL_H
