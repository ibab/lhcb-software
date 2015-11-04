#ifndef VPTRACKMONI_H 
#define VPTRACKMONI_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/SystemOfUnits.h"

//from Event
#include "Event/MCHit.h"
#include "VPDet/DeVPSensor.h"
#include "VPDet/DeVP.h"
#include "Event/Track.h"
#include "Event/VPCluster.h"
#include "Linker/LinkerTool.h"
#include "Linker/AllLinks.h"
#include "Event/FitNode.h"

#include <boost/foreach.hpp>
/** @class VPTrackMoni VPTrackMoni.h
 *  
 *
 *  @author Christoph Hombach
 *  @date   2015-01-08
 */
class VPTrackMoni : public GaudiTupleAlg {
public: 
  /// Standard constructor
  VPTrackMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VPTrackMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  Gaudi::XYZPoint getResidual(const LHCb::Track*, const DeVPSensor*, Gaudi::XYZPoint);
  
protected:

private:
  //TES locations
  std::string m_mchitlocation;
  std::string m_tracklocation;
  DeVP* m_veloDet_VP;
  
};
#endif // VPTRACKMONI_H
