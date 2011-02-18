
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleMaker.h
 *
 * Header file for algorithm ChargedProtoParticleMaker
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleMaker.h,v 1.2 2010-03-08 01:46:40 odescham Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleMaker_H
#define GLOBALRECO_ChargedProtoParticleMaker_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/HashMap.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"

// Event
#include "Event/Track.h"
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoParticleMaker ChargedProtoParticleMaker.h
 *
 *  Algorithm to build charged ProtoParticles from charged Tracks.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */
//-----------------------------------------------------------------------------

class ChargedProtoParticleMaker : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoParticleMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private: // data

  /// Location of the ProtoParticles in the TES
  std::string m_protoPath_;

  /// Location of the ProtoParticles in the TES
  std::string m_protoPath;

  /// Locations in TES of input Tracks
  std::vector<std::string> m_tracksPath;

  /// Locations in TES of input Tracks
  std::vector<std::string> m_tracksPath_;


  /// Track selector type
  std::string m_trSelType;

  /// Track selector tool
  ITrackSelector * m_trSel;

};

#endif // GLOBALRECO_ChargedProtoParticleMaker_H
