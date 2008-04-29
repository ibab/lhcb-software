#ifndef TRACKTOOLS_HLTV0UPGRADE_H 
#define TRACKTOOLS_HLTV0UPGRADE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from TrackInterfaces
#include "TrackInterfaces/IHltV0Upgrade.h"

#include "GaudiKernel/IMagneticFieldSvc.h"
#include "Kernel/ITrajPoca.h"
#include "Event/TwoProngVertex.h"
#include "Event/Track.h"
#include "Event/TrackTraj.h"

/** @class HltV0Upgrade.h TrackTools/HltV0Upgrade.h
 *  
 *  @author Jaap Panman
 *  @date   2008-03-04
 *  based on example made by Wouter Hulsbergen
 */

class HltV0Upgrade : public GaudiTool, virtual public IHltV0Upgrade{
public: 
  /// Standard construct
  HltV0Upgrade( const std::string& type, 
			const std::string& name,
			const IInterface* parent);

  virtual ~HltV0Upgrade( ); ///< Destructor

  StatusCode initialize();

  StatusCode process( LHCb::TwoProngVertex& vertex ) const ;
  
protected:
  StatusCode fittrack(LHCb::Track& track) const ;

private:
  ITrackFitter* m_trackfitter ;
  ITrajPoca*    m_pocatool ;
  IMagneticFieldSvc* m_magfieldsvc ; 

};
#endif // TRACKTOOLS_HLTV0UPGRADE_H
