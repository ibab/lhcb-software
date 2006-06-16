// $Id: IParticle2VertexAsct.h,v 1.2 2006-06-16 13:27:48 jonrob Exp $
#ifndef DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H
#define DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Relations/RelationWeighted1D.h"

class IGeomDispCalculator;

static const InterfaceID IID_IParticle2VertexAsct ( "IParticle2VertexAsct", 1, 0 );

/** @class IParticle2VertexAsct IParticle2VertexAsct.h Kernel/IParticle2VertexAsct.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2006-06-06
 */

class IParticle2VertexAsct : virtual public IAlgTool
{

public:

  typedef LHCb::RelationWeighted1D<LHCb::Particle,LHCb::Vertex,double> Table;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2VertexAsct; }

  // Return the relation table linking particles to PVs
  virtual Table table(const LHCb::Particle::Container& particles,
                      const LHCb::Vertex::Container& vertices,
                      const IGeomDispCalculator* pIPTool) const = 0;

  virtual Table table(const LHCb::Particle::ConstVector& particles,
                      const LHCb::Vertex::ConstVector& vertices,
                      const IGeomDispCalculator* pIPTool) const = 0;


  virtual Table table(const LHCb::Particle::Container::const_iterator pBegin,
                      const LHCb::Particle::Container::const_iterator pEnv,
                      const LHCb::Vertex::Container::const_iterator   vBegin,
                      const LHCb::Vertex::Container::const_iterator   vEnd,
                      const IGeomDispCalculator* pIPTool) const = 0;

  virtual Table table(const LHCb::Particle::ConstVector::const_iterator pBegin,
                      const LHCb::Particle::ConstVector::const_iterator pEnv,
                      const LHCb::Vertex::ConstVector::const_iterator   vBegin,
                      const LHCb::Vertex::ConstVector::const_iterator   vEnd,
                      const IGeomDispCalculator* pIPTool) const = 0;

};
#endif // DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H
