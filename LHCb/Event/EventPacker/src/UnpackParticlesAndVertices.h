// $Id: $
#ifndef UNPACKPARTICLESANDVERTICES_H 
#define UNPACKPARTICLESANDVERTICES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/StandardPacker.h"
#include "Relations/Relation1D.h"
#include "Event/Particle.h"
#include "Event/PackedParticle.h"
#include "Event/Vertex.h"
#include "Event/PackedVertex.h"
#include "Event/PackedRelations.h"

/** @class UnpackParticlesAndVertices UnpackParticlesAndVertices.h
 *   
 *
 *  @author Olivier Callot
 *  @date   2012-01-23
 */
class UnpackParticlesAndVertices : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackParticlesAndVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackParticlesAndVertices( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

  typedef LHCb::Relation1D<LHCb::Particle, LHCb::VertexBase> RELATION;

protected:

private:
  std::string m_inputStream;
  std::string m_postFix;
  StandardPacker m_pack;
};
#endif // UNPACKPARTICLESANDVERTICES_H
