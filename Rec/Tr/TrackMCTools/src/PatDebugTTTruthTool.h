// $Id: PatDebugTTTruthTool.h,v 1.1 2007-10-22 15:52:36 ocallot Exp $
#ifndef PATDEBUGTTTRUTHTOOL_H 
#define PATDEBUGTTTRUTHTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackMCInterfaces/IPatDebugTTTruthTool.h"            // Interface


/** @class PatDebugTTTruthTool PatDebugTTTruthTool.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2007-10-22
 */
class PatDebugTTTruthTool : public GaudiTool, virtual public IPatDebugTTTruthTool {
public: 
  /// Standard constructor
  PatDebugTTTruthTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~PatDebugTTTruthTool( ); ///< Destructor

  virtual void printTrueTTClusterOnTrack( const LHCb::Track* track, 
                                          const PatTTHits::const_iterator beginCoord,
                                          const PatTTHits::const_iterator endCoord   );
  
  virtual void printTTClusterTruth( MsgStream& msg, const PatTTHit* hit );

protected:

private:

};
#endif // PATDEBUGTTTRUTHTOOL_H
