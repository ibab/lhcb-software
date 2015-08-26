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
#include <vector>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
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

  ~ChargedProtoParticleMoni( ) override = default; ///< Destructor

  StatusCode execute   () override;    ///< Algorithm execution
  StatusCode finalize  () override;    ///< Algorithm finalization

private: // data

  /// Location of the ProtoParticles in the TES
  std::string m_protoPath;

  /// Location in TES of input Tracks
  std::string m_tracksPath;

private: // classes

  /// Simple utility tally class
  struct TrackTally
  {
    /// Default constructor
    TrackTally() = default;
    unsigned long totTracks = 0;      ///< Number of considered tracks
    unsigned long selTracks = 0;      ///< Number of tracks selected to creaste a ProtoParticle from
    unsigned long ecalTracks = 0;     ///< Number of ProtoParticles created with CALO ECAL info
    unsigned long bremTracks = 0;     ///< Number of ProtoParticles created with CALO BREM info
    unsigned long spdTracks = 0;      ///< Number of ProtoParticles created with CALO SPD info
    unsigned long prsTracks = 0;      ///< Number of ProtoParticles created with CALO PRS info
    unsigned long hcalTracks = 0;     ///< Number of ProtoParticles created with CALO HCAL info
    unsigned long richTracks = 0;     ///< Number of ProtoParticles created with RICH info
    unsigned long muonTracks = 0;     ///< Number of ProtoParticles created with MUON info
    unsigned long velodEdxTracks = 0; ///< Number of ProtoParticles created with VELO dE/dx info
  };

private:

  /// Print statistics
  void printStats( const MSG::Level level = MSG::INFO ) const;

  /// Find the ProtoParticle created from a given Track
  const LHCb::ProtoParticle * getProto( const LHCb::ProtoParticles * protos,
                                        const LHCb::Track * track ) const;

private:

  /// Event count
  unsigned long long m_nEvts = 0;

  /// Map type containing tally for various track types
  /// Total number of tracks considered and selected
  std::vector<std::pair<LHCb::Track::Types, TrackTally>> m_nTracks;

  static const constexpr struct compare_t {
      template <typename Key, typename Value>
      bool operator()(const std::pair<Key,Value>& kv, const Key& k)
      { return kv.first < k; }
  } s_cmp {};

  //WARNING: reference is invalidated as soon as an insert happens
  TrackTally& trackTally( LHCb::Track::Types key ) {
      auto i = std::lower_bound(std::begin(m_nTracks),std::end(m_nTracks),key,s_cmp);
      if ( i == std::end(m_nTracks) || i->first != key )  {
         i = m_nTracks.insert( i, { key, TrackTally{} } );
      }
      return i->second;
  }

};

#endif // GLOBALRECO_ChargedProtoParticleMoni_H
