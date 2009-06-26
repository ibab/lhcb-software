// $Id: PVRelatorAlg.h,v 1.6 2009-06-26 13:04:51 jpalac Exp $
#ifndef PVRELATORALG_H 
#define PVRELATORALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IOnOffline;
class IRelatedPVFinder;

/** @class PVRelatorAlg PVRelatorAlg.h
 *  
 *
 *  A simple GaudiAlgorithm that takes as input the TES location of some
 *  LHCb::Particles, plus the TES location of some LHCb::RecVertices <b>or</b>
 *  the TES location of an LHCb::Particle->LHCb::VertexBase weighted relations 
 *  table(Particle2Vertex::Table), and uses an IRelatedPVFinder to create sorted 
 *  Particle->PV relations or to re-sort the input relations if a relations table 
 *  has given as input.
 *  The sorting is done according to the logic of the implementation of the
 *  IRelatedPVFinder, which is determined by an IOnOfflineTool.
 *
 *  <b>Properties</b>
 *
 *  <b>ParticleInputLocation</b> : TES location of input LHCb::Particles. 
 *  Default: "". Must be set by user.
 *
 *  <b>PrimaryVertexInputLocation</b> : TES location of input LHCb::RecVertices. 
 *  Default: LHCb::RecVertexLocation::Primary
 *
 *  <b>P2PVRelationsInputLocation</b> : TES location of input Particle2Vertex::Table. 
 *  Default: "".
 *
 *  <b>P2PVRelationsOutputLocation</b> : TES location of the output 
 *  Particle2Vertex::Table with the newly sorted relations. Default "".
 *  Must be set by user.
 *
 *  Besides the properties that <b>must</b> be set, there is one extra 
 *  condition: <b>PrimaryVertexInputLocation<b> and <b>P2PVRelationsInputLocation<b> 
 *  cannot both be set. This will result in the initalize() method returning a 
 *  StatusCode::FAILURE.
 *
 *  <b>Example</b>: Create a Particle2Vertex::Table relating LHCb::Particles from
 *  "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" to default primary vertices and 
 *  put it in "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2DefaultPVRelations". Create 
 *  another Particle2Vertex::Table relating LHCb::Particles to re-fitted PVs 
 *  starting from a Particle2Vertex::Table in "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitPVRelations" 
 *  and put it in "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitPVSortedRelations"
 *
 *  @code
 *  from Configurables import PVRelatorAlg
 *  # Default PVs
 *  p2PV = PVRelatorAlg("P2StandardVertices")
 *  p2PV.ParticleInputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"
 *  p2PV.P2PVRelationsOutputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2DefaultPVRelations"
 *  # re-fitted PVs
 *  p2RefitPV = PVRelatorAlg("P2ReFitVertices")
 *  p2RefitPV.ParticleInputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"
 *  p2RefitPV.P2PVRelationsInputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitPVRelations"
 *  p2RefitPV.P2PVRelationsOutputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitPVSortedRelations"
 *  # Add it to a selection sequence
 *  seq = GaudiSequencer('SeqDC06selBd2Jpsi2MuMu_Kst2KPi')
 *  seq.Members += [p2PV, P2RefitPV]
 *
 * 
 *  @endcode
 *
 *  @author Juan Palacios
 *  @date   2009-05-23
 */
class PVRelatorAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PVRelatorAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVRelatorAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  template <typename T>
  inline T* 
  i_get(const std::string& location) const
  {
    return (exist<T>( location )) ? get<T>( location ) : 0 ;
  }

  Particle2Vertex::Table* table() const;
  
  Particle2Vertex::Table* tableFromTable() const;

private:

  std::string m_particleInputLocation;
  std::string m_PVInputLocation;
  std::string m_P2PVInputLocation;
  std::string m_P2PVOutputLocation;
  bool m_useTable;
  IOnOffline* m_OnOffline ; ///< context switch tool. To be deprecated.
  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV


};
#endif // PVRELATORALG_H
