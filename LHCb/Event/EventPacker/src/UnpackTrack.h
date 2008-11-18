// $Id: UnpackTrack.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef UNPACKTRACK_H 
#define UNPACKTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/Track.h"
#include "Event/PackedTrack.h"

/** @class UnpackTrack UnpackTrack.h
 *  Unpack the PackedTrack
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class UnpackTrack : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackTrack( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

  void convertState ( const LHCb::PackedState& pSta, LHCb::Track* tra );

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKTRACK_H
