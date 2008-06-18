// $Id: PatDebugTTTruthTool.h,v 1.3 2008-06-18 07:40:07 mneedham Exp $
#ifndef PATDEBUGTTTRUTHTOOL_H 
#define PATDEBUGTTTRUTHTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/IPatDebugTTTool.h"            // Interface

class DeSTDetector;

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

  virtual StatusCode initialize(); /// initialize

  virtual void debugTTClusterOnTrack( const LHCb::Track* track, 
                                      const PatTTHits::const_iterator beginCoord,
                                      const PatTTHits::const_iterator endCoord   );
  
  virtual void debugTTCluster( MsgStream& msg, const PatTTHit* hit );

protected:

private:

  DeSTDetector* m_tracker;

};
#endif // PATDEBUGTTTRUTHTOOL_H
