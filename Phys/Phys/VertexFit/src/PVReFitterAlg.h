// $Id: PVReFitterAlg.h,v 1.17 2010-08-04 12:59:29 jpalac Exp $
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
 * <b>IPVOfflineTool</b>  : Implemenmtation of IPVOfflineTool. Default: PVOfflineTool.
 *
 * <b>IPVReFitter</b>     : Implemenmtation of IPVRefitter. Default: AdaptivePVReFitter.
 *
 * <b>UseIPVOfflineTool</b> : bool, apply IPVOfflineTool::reDoSinglePV method. Default: false.
 *
 * <b>UseIPVReFitter</b> : bool, apply IPVReFitter::remove method. Default: true.
 *
 * <b>ParticleInputLocations</b>: Vector of TES locations of the particles who's related primary vertices will be re-fitted. Default "".
 *
 * <b>PrimaryVertexInputLocation</b>: TES location of the LHCb::RecVertices to be
 * re-fitted.
 *
 * The algorithm iterates over the LHCb::Particles in each of the ParticleInputLocations,
 * and the LHCb::RecVertices in PrimaryVertexLocation, re-fits clones of the
 * vertices excluding any
 * tracks that originate from the particle, and creates a relations table
 * connecting the particle to the re-fitted vertices. The re-fitted vertices
 * are stored  in a KeyedContainer<LHCb::RecVertex>,
 * which is placed in <location>/<instance name>_PVs. The relations table
 * is placed in <location>/<instance name>_P2PV. <location>
 * is obtained by stripping away "/Particles" from each of the <b> ParticleInputLocations</b>
 *
 * The re-fitting itself is a sequence of
 * IPVOfflineTool::reDoSinglePV (if UseIPVOfflineTool==true),
 * and IPVReFitter::remove (if UseIPVReFitter == true).
 * <b>Beware</b>: If there is more than one candidate per event, there can be
 * more than one refitted PV for each original one. Hence users are required
 * to access the re-fitted vertices on a particle by particle basis, using
 * the relations table, and not the full container of re-fitted vertices.
 *
 *
 *  @author Juan PALACIOS
 *  @date   2008-06-25
 */
class PVReFitterAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  PVReFitterAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVReFitterAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  LHCb::RecVertex* refitVertex(const LHCb::RecVertex* v,
                               const LHCb::Particle* p  ) const;

  void getTracks(const LHCb::Particle* p,
                 LHCb::Track::ConstVector& tracks) const;


  void executeForLocation(const std::string& particleLocation,
                          const LHCb::RecVertex::Range& vertices) const;

private:

  IPVOfflineTool* m_pvOfflineTool;
  IPVReFitter* m_pvReFitter;

  std::string m_pvOfflinetoolType;
  std::string m_pvReFitterType;
  bool m_useIPVOfflineTool;
  bool m_useIPVReFitter;
  std::vector<std::string> m_particleInputLocations;
  std::string m_PVInputLocation;
  std::string m_particle2VertexRelationsOutputLocation;
  std::string m_vertexOutputLocation;
  std::string m_outputLocation;

  Particle2Vertex::Table m_p2VtxTable;

};

#endif // PVREFITTERALG_H
