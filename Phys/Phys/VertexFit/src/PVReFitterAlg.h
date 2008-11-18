// $Id: PVReFitterAlg.h,v 1.9 2008-11-18 17:56:18 jpalac Exp $
#ifndef PVREFITTERALG_H 
#define PVREFITTERALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// From LHCb
#include "Kernel/Particle2Vertex.h"

class IPVReffitter;
class IPVOfflineTool;
class ILifetimeFitter;

/** @class PVReFitterAlg PVReFitterAlg.h
 *  
 * Simple GaudiAlgorithm that takes some particles, some primary 
 * vertices, and refits the vertices excluding the tracks coming from the 
 * particle's decay. It also creates a relations table associating each 
 * particle to its corresponding new vertex.
 *
 * <b>Properties</b>:
 *
 * <b>IPVOfflineTool</b>  : Implemenmtation of IPVOfflineTool. Default: PVOfflineTool
 *
 * <b>IPVReFitter</b>     : Implemenmtation of IPVRefitter. Default: AdaptivePVReFitter
 *
 * <b>ILifetimeFitter</b> : Implemenmtation of ILifetimeFitter. Default: PropertimeFitter
 *
 * <b>ParticleInputLocation</b>: TES location of the particles who's related primary vertices will be re-fitted.
 *
 * <b>PrimaryVertexInputLocation</b>: TES location of the LHCb::RecVertices to be
 * re-fitted.
 *
 * <b>P2VRelationsOutputLocation</b>: TES location of the relations table 
 * relating the particles in ParticleInputLocation to the re-fitted 
 * primary vertices.
 *
 * <b>VertexOutputLocation</b>: TES location of the 
 * KeyedContainer<LHCb::RecVertex> containing the re-fitted vertices.
 *
 * The algorithm iterates over the LHCb::Particles in ParticleInputLocation,
 * and the LHCb::RecVertices in PrimaryVertexLocation, re-fits clones of the
 * vertices excluding any 
 * tracks that originate from the particle, and creates a relations table
 * connecting the particle to the re-fitted vertices. The re-fitted vertices 
 * are stored  in a KeyedContainer<LHCb::RecVertex>,
 * which is placed in VertexOutputLocation. The relations table is placed in 
 * P2VRelationsOutputLocation. The re-fitting itself is a sequence of
 * IPVOfflineTool::reDoSinglePV, IPVReFitter::remove.
 * <b>Beware</b>: If there is more than one candidate per event, there can me 
 * more than one refitted PV for each original one. Hence users are encouraged
 * to access the re-fitted vertices on a particle by particle basis, using
 * the relations table, and not the full container of re-fitted vertices.
 *
 * @todo Test!
 *
 *  @author Juan PALACIOS
 *  @date   2008-06-25
 */
class PVReFitterAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PVReFitterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVReFitterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  LHCb::RecVertex* refitVertex(const LHCb::RecVertex* v,
                               const LHCb::Particle* p  ) const;
  
  void getTracks(const LHCb::Particle* p,
                 LHCb::Track::ConstVector& tracks) const;

private:

  IPVOfflineTool* m_pvOfflineTool;
  IPVReFitter* m_pvReFitter;

  std::string m_pvOfflinetoolType;
  std::string m_pvReFitterType;
  std::string m_relatedPVFinderType;
  std::string m_particleInputLocation;
  std::string m_PVInputLocation;
  std::string m_particle2VertexRelationsOutputLocation;
  std::string m_vertexOutputLocation;

  Particle2Vertex::Table m_p2VtxTable;
  
};
#endif // PVREFITTERALG_H
