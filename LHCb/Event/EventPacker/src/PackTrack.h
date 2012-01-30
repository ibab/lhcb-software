// $Id: PackTrack.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
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
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  bool m_deleteInput;       ///< Flag to turn on the removal of the input data after packing
};
#endif // PACKTRACK_H
