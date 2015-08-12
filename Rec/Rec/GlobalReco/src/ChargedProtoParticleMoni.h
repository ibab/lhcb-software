
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleMoni.h
 *
 * Header file for algorithm ChargedProtoParticleMoni
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 28/08/2009
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleMoni_H
#define GLOBALRECO_ChargedProtoParticleMoni_H 1

// STL
#include <sstream>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/HashMap.h"
// Event
#include "Event/Track.h"
#include "Event/ProtoParticle.h"

// boost
#include "boost/format.hpp"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoParticleMoni ChargedProtoParticleMoni.h
 *
 *  Algorithm to build charged ProtoParticles from charged Tracks.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */
//-----------------------------------------------------------------------------

class ChargedProtoParticleMoni : public GaudiHistoAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleMoni( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // data

  /// Location of the ProtoParticles in the TES
  std::string m_protoPath;

  /// Location in TES of input Tracks
  std::string m_tracksPath;

private: // classes

  /// Simple utility tally class
  class TrackTally
  {
  public:
    /// Default constructor
    TrackTally() : totTracks(0), selTracks(0),
                   ecalTracks(0), bremTracks(0), spdTracks(0), prsTracks(0), hcalTracks(0),
                   richTracks(0), muonTracks(0), velodEdxTracks(0) { }
    unsigned long totTracks;      ///< Number of considered tracks
    unsigned long selTracks;      ///< Number of tracks selected to creaste a ProtoParticle from
    unsigned long ecalTracks;     ///< Number of ProtoParticles created with CALO ECAL info
    unsigned long bremTracks;     ///< Number of ProtoParticles created with CALO BREM info
    unsigned long spdTracks;      ///< Number of ProtoParticles created with CALO SPD info
    unsigned long prsTracks;      ///< Number of ProtoParticles created with CALO PRS info
    unsigned long hcalTracks;     ///< Number of ProtoParticles created with CALO HCAL info
    unsigned long richTracks;     ///< Number of ProtoParticles created with RICH info
    unsigned long muonTracks;     ///< Number of ProtoParticles created with MUON info
    unsigned long velodEdxTracks; ///< Number of ProtoParticles created with VELO dE/dx info
  };

private: // definitions

  /// Map type containing tally for various track types
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, TrackTally > TrackMap;

private:

  /// Print statistics
  void printStats( const MSG::Level level = MSG::INFO ) const;

  /// Find the ProtoParticle created from a given Track
  const LHCb::ProtoParticle * getProto( const LHCb::ProtoParticles * protos,
                                        const LHCb::Track * track ) const;

private:

  /// Event count
  unsigned long long m_nEvts;

  /// Total number of tracks considered and selected
  TrackMap m_nTracks;

};

#endif // GLOBALRECO_ChargedProtoParticleMoni_H
