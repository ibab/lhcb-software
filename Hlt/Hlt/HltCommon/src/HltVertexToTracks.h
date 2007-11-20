// $Id: HltVertexToTracks.h,v 1.1 2007-11-20 12:48:46 hernando Exp $
#ifndef HLTCOMMON_HLTVERTEXTOTRACKS_H 
#define HLTCOMMON_HLTVERTEXTOTRACKS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class L0Vertex2Tracks L0Vertex2Tracks.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-08-03
 */
class HltVertexToTracks : public HltAlgorithm {
public: 
  /// Standard constructor
  HltVertexToTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexToTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

};
#endif // L0VERTEX2TRACKS_H
