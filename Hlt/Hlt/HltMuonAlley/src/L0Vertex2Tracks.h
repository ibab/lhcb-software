// $Id: L0Vertex2Tracks.h,v 1.1 2007-08-03 09:29:30 asatta Exp $
#ifndef L0VERTEX2TRACKS_H 
#define L0VERTEX2TRACKS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

#include "PatTools/PatDataStore.h"

/** @class L0Vertex2Tracks L0Vertex2Tracks.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-08-03
 */
class L0Vertex2Tracks : public HltAlgorithm {
public: 
  /// Standard constructor
  L0Vertex2Tracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0Vertex2Tracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  bool isIn(LHCb::Track* track);
protected:

private:

};
#endif // L0VERTEX2TRACKS_H
