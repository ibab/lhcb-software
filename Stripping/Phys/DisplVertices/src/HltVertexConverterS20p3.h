#ifndef DISPLVERTICES_HLTVERTEXCONVERTERS20p3_H
#define DISPLVERTICES_HLTVERTEXCONVERTERS20p3_H 1

#include "Kernel/DaVinciAlgorithm.h"

// fwd
namespace LHCb {
  class HltObjectSummary;
}

/** @class HltVertexConverterS20p3 HltVertexConverterS20p3.h DisplVertices/HltVertexConverterS20p3.h
 *
 * Revive Hlt2 candidate vertices
 * @see HltTrackConverter
 *
 * @author Pieter David
 * @date   2013-12-29
 */
class HltVertexConverterS20p3 : public DaVinciAlgorithm {
public:
  /// Standard constructor
  HltVertexConverterS20p3( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltVertexConverterS20p3();           ///< Destructor

  virtual StatusCode initialize();              ///< Algorithm initialization
  virtual StatusCode execute   ();              ///< Algorithm execution
  virtual StatusCode finalize  ();              ///< Algorithm finalization
private:
  std::string m_HltSelReportsLocation;
  std::vector<std::string> m_HltLineNames;

  bool m_recursive;
  bool m_matchTracksToOffline;
  Gaudi::SymMatrix3x3 m_defaultVertexCovMatrix;
  LHCb::Particle* reviveParticle( const LHCb::HltObjectSummary* summary, bool recursive );
  LHCb::ProtoParticle* reviveProto( const LHCb::HltObjectSummary* summary );
  LHCb::Track* reviveTrack( const LHCb::HltObjectSummary* summary );

  LHCb::ProtoParticles* m_protos;
  LHCb::Tracks* m_tracks;
  LHCb::ProtoParticles* protoParticles();
  LHCb::Tracks* tracks();
};
#endif // DISPLVERTICES_HLTVERTEXCONVERTERS20p3_H
