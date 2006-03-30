
//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.h
 *
 * Header file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoPAlg.h,v 1.14 2006-03-30 14:09:22 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_CHARGEDPROTOPALG_H
#define GLOBALRECO_CHARGEDPROTOPALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"

// Event
#include "Event/Track.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoPAlg ChargedProtoPAlg.h
 *
 *  Algorithm to build charge ProtoParticles from the results of the reconstruction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 29/03/2006
 */
//-----------------------------------------------------------------------------

class ChargedProtoPAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoPAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoPAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Access to the ProtoParticle container
  inline LHCb::ProtoParticles * protos() { return m_protos; }

  /// Load the RichPIDs and build reverse mappings
  StatusCode getRichData();

  /// Load the MuonPIDs and build reverse mappings
  StatusCode getMuonData();

  /// Add Rich information to the given ProtoParticle
  bool addRich( LHCb::ProtoParticle * proto );

  /// Add Muon information to the given ProtoParticle
  bool addMuon( LHCb::ProtoParticle * proto );

private: // data

  std::string m_tracksPath;   ///< Location in TES of input Tracks
  std::string m_richPath;     ///< Location in TES of input RichPIDs
  std::string m_muonPath;     ///< Location in TES of input MuonPIDs

  std::string m_protoPath;    ///< Location in TES of output ProtoParticles
  
  LHCb::ProtoParticles * m_protos; ///< Pointer to current ProtoParticle container

  /// Track selector tool
  ITrackSelector * m_trSel;

  /// mapping type from Track to RichPID data objects
  typedef std::map<const LHCb::Track *, const LHCb::RichPID *> TrackToRichPID;
  /// mapping from Track to RichPID data objects
  TrackToRichPID m_richMap;

  /// mapping type from Track to MuonPID data objects
  typedef std::map<const LHCb::Track *, const LHCb::MuonPID *> TrackToMuonPID;
  /// mapping from Track to RichPID data objects
  TrackToMuonPID m_muonMap;

};

#endif // GLOBALRECO_CHARGEDPROTOPALG_H
