// $Id: UnpackTrack.h,v 1.2 2009-11-06 18:34:34 jonrob Exp $
#ifndef UNPACKTRACK_H 
#define UNPACKTRACK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/Track.h"
#include "Event/PackedTrack.h"

/** @class UnpackTrack UnpackTrack.h
 *
 *  Unpack the PackedTrack
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class UnpackTrack : public GaudiAlgorithm
{

public: 

  /// Standard constructor
  UnpackTrack( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackTrack( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;  ///< Input location
  std::string m_outputName; ///< Output location
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing
  std::string m_ancestorFor;  ///< Container name for which ancestors shoudl be built
  std::string m_ancestorSource; ///< Container where ancestors are.

};

#endif // UNPACKTRACK_H
