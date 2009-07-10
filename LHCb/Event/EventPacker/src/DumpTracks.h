// $Id: DumpTracks.h,v 1.1 2009-07-10 06:15:55 cattanem Exp $
#ifndef DUMPTRACKS_H 
#define DUMPTRACKS_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include <string>

/** @class DumpTracks DumpTracks.h
 *  Dump all tracks in an event. 
 *  Amount printed depends on OutputLevel:
 *    INFO: prints size of container
 *   DEBUG: prints also first "NumberOfObjectsToPrint" Tracks and States
 *          (default is 5)
 * VERBOSE: prints all Tracks and States
 *
 *  @author Marco Cattaneo
 *  @date   2004-07-14
 */
class DumpTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  DumpTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DumpTracks( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  unsigned int m_numObjects;    ///< Number of objects to print
  std::string m_tracksLocation; ///< Location of tracks container
};

#endif // DUMPTRACKS_H
