// $Id: PatDebugTTTruthTool.h,v 1.2 2007-10-23 14:56:07 cattanem Exp $
#ifndef PATDEBUGTTTRUTHTOOL_H 
#define PATDEBUGTTTRUTHTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPatDebugTTTool.h"            // Interface


/** @class PatDebugTTTruthTool PatDebugTTTruthTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-10-22
 */
class PatDebugTTTruthTool : public GaudiTool, virtual public IPatDebugTTTool {
public: 
  /// Standard constructor
  PatDebugTTTruthTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~PatDebugTTTruthTool( ); ///< Destructor

  virtual void debugTTClusterOnTrack( const LHCb::Track* track, 
                                      const PatTTHits::const_iterator beginCoord,
                                      const PatTTHits::const_iterator endCoord   );
  
  virtual void debugTTCluster( MsgStream& msg, const PatTTHit* hit );

protected:

private:

};
#endif // PATDEBUGTTTRUTHTOOL_H
