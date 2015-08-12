// $Id$
#ifndef BESTPVALG_H 
#define BESTPVALG_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IRelatedPVFinder.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"
#include "Relations/Get.h"
#include "Kernel/DaVinciStringUtils.h"
#include "Kernel/DefaultDVToolTypes.h"

/** @class BestPVAlg BestPVAlg.h
 *
 *  A simple GaudiAlgorithm that takes as input the TES location of some
 *  LHCb::Particles, plus the TES location of some LHCb::RecVertices <b>or</b>
 *  the TES location of an LHCb::Particle->LHCb::VertexBase relations 
 *  table(Particle2Vertex::Table), and uses an IRelatedPVFinder to create a sorted 
 *  Particle->PV relations table relating each particle to the "best" PV only.
 *
 *  The best PV is found according to the logic of the implementation of the
 *  IRelatedPVFinder.
 *
 *  <b>Properties</b>
 *
 *  <b>ParticleInputLocations</b> : TES locations of input LHCb::Particles. 
 *  Default: none. Must be set by user unless <b>P2PVRelationsInputLocations<b> is set
 *
 *  <b>PrimaryVertexInputLocation</b> : TES location of input LHCb::RecVertices. 
 *  Default: LHCb::RecVertexLocation::Primary
 *
 *  <b>P2PVRelationsInputLocations</b> : TES locations of input Particle2Vertex::Tables. 
 *  Default: None.
 *
 *  Besides the properties that <b>must</b> be set, there are two extra 
 *  conditions: neither <b>ParticleInputLocations</b> nor <b>PrimaryVertexInputLocations<b> 
 *  can be set if <b>P2PVRelationsInputLocations<b> is set. This will result in 
 *  the initalize() method returning a StatusCode::FAILURE.
 *
 *  <b>Example</b>: Create a Particle2Vertex::Table relating LHCb::Particles from
 *  "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles" to default primary vertices. 
 *  Create another Particle2Vertex::Table relating LHCb::Particles to re-fitted PVs 
 *  starting from a Particle2Vertex::Table in "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitPVRelations".
 *
 *  @code
 *  from Configurables import BestPVAlg
 *  # Default PVs. Table written to "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2StandardVertices_P2PV".
 *  p2PV = BestPVAlg("P2StandardVertices")
 *  p2PV.ParticleInputLocations = ["Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particles"]
 *  # re-fitted PVs. Talbe written to "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitVertices_P2PV".
 *  p2RefitPV = BestPVAlg("P2ReFitVertices")
 *  p2RefitPV.P2PVRelationsInputLocations = ["Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/P2ReFitPVRelations"]
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
class BestPVAlg : public GaudiAlgorithm
{

public: 

  /// Standard constructor
  BestPVAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BestPVAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  template <typename T>
  inline T* 
  i_get(const std::string& location) const
  {
    return ( getIfExists<T>( location ) );
  }

  void tables() const;
  
  void tablesFromTables() const;

  std::string tableLocation(const std::string& location) const;
  
  void checkTable(const Particle2Vertex::Table* table,
                  const std::string& tableLoc) const;

private:

  std::string m_PVInputLocation;
  std::vector<std::string> m_particleInputLocations;
  std::vector<std::string> m_P2PVInputLocations;
  bool m_useTables;
  std::string m_pvRelatorName;    ///< The name of the PV relator to use
  IRelatedPVFinder* m_pvRelator ; ///< Tool that relates the Particle to a PV


};
#endif // PVRELATORALG_H
