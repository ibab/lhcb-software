// $Id: PackTrack.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef PACKTRACK_H 
#define PACKTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/PackedTrack.h"

/** @class PackTrack PackTrack.h
 *  Pack a track container
 *
 *  @author Olivier Callot
 *  @date   2008-11-12
 */
class PackTrack : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTrack( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  void convertState( const LHCb::State* state, LHCb::PackedTracks* out );
  
private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKTRACK_H
