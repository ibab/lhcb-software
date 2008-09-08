// $Id: PVReFitterAlg.h,v 1.4 2008-09-08 16:09:10 jpalac Exp $
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
class IRelatedPVFinder;
class IOnOffline;

/** @class PVReFitterAlg PVReFitterAlg.h
 *  
 * Simple DVAlgorithm that takes some particles, their related primary 
 * vertices, and refits the vertices excluding the tracks coming from the 
 * particle's decay. It also creates a relations table associating each 
 * particle to its corresponding new vertex.
 *
 * <b>Properties</b>:
 *
 * IPVOfflineTool  : Implemenmtation of IPVOfflineTool. Default: PVOfflineTool
 *
 * IPVReFitter     : Implemenmtation of IPVRefitter. Default: AdaptivePVReFitter
 *
 * ILifetimeFitter : Implemenmtation of ILifetimeFitter. Default: PropertimeFitter
 *
 * <b>ParticleInputLocation</b>: TES location of the particles who's related primary vertices will be re-fitted.
 *
 * <b>P2VRelationsInputLocation</b> : TES location of the relations table 
 * relating the particles in  ParticleInputLocation to the "best" primary 
 * vertices.
 *
 * <b>P2VRelationsOutputLocation</b> : TES location of the relations table 
 * relating the particles in ParticleInputLocation to the re-fitted 
 * primary vertices.
 *
 * <b>VertexOutputLocation</b> : TES location of the 
 * LHCb::RecVertex::Container containing the re-fitted vertices.
 *
 * The algorithm iterates over the LHCb::Particles in ParticleInputLocation,
 * obtains the related LHCb::RecVertices from the table in 
 * P2VRelationsInputLocation, then re-fits clones of these excluding any 
 * tracks that originate from the particle, and creates a relations table
 * connecting the particle to the re-fitted vertices. It places the re-fitted 
 * vertices in VertexOutputLocation and the relations table in 
 * P2VRelationsOutputLocation. The re-fitting itself is a sequence of
 * IPVOfflineTool::reDoSinglePV, IPVReFitter::remove.
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
  
  const LHCb::RecVertex* getRelatedVertex(const LHCb::Particle* p);
  

  void getTracks(const LHCb::Particle* p,
                 LHCb::Track::ConstVector& tracks) const;

private:

  IOnOffline* m_onOfflineTool;
  IPVOfflineTool* m_pvOfflineTool;
  IPVReFitter* m_pvReFitter;
  ILifetimeFitter* m_lifetimeFitter;
  IRelatedPVFinder* m_relatedPVFinder;

  std::string m_pvOfflinetoolType;
  std::string m_pvReFitterType;
  std::string m_lifetimeFitterType;
  std::string m_relatedPVFinderType;
  std::string m_particleInputLocation;
  std::string m_particle2VertexRelationsInputLocation;
  std::string m_particle2VertexRelationsOutputLocation;
  std::string m_vertexOutputLocation;
  
  LHCb::RecVertex::Container* m_vertexContainer;
  Particle2Vertex::Table m_p2VtxTable;
  
};
#endif // PVREFITTERALG_H
