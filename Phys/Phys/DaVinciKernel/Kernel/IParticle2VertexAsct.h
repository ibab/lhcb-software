// $Id: IParticle2VertexAsct.h,v 1.4 2006-06-20 05:55:52 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H
#define DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// from LHCb
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/PrimVertex.h"
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

  typedef LHCb::RelationWeighted1D<LHCb::Particle,
                                   LHCb::PrimVertex,double> TablePV;
  typedef LHCb::RelationWeighted1D<LHCb::Particle,
                                   LHCb::Vertex,double> TableV;

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticle2VertexAsct; }

  // Return the relation table linking Particles to PrimVertices
  virtual TablePV table(const LHCb::Particle::Container& particles,
                        const LHCb::PrimVertex::Container& vertices,
                        const IGeomDispCalculator* pIPTool) const = 0;

  virtual TablePV table(const LHCb::Particle::ConstVector& particles,
                        const LHCb::PrimVertex::ConstVector& vertices,
                        const IGeomDispCalculator* pIPTool) const = 0;


  virtual TablePV table(const LHCb::Particle::Container::const_iterator pBegin,
                        const LHCb::Particle::Container::const_iterator pEnv,
                        const LHCb::PrimVertex::Container::const_iterator vBegin,
                        const LHCb::PrimVertex::Container::const_iterator vEnd,
                        const IGeomDispCalculator* pIPTool) const = 0;

  virtual TablePV table(const LHCb::Particle::ConstVector::const_iterator pBegin,
                        const LHCb::Particle::ConstVector::const_iterator pEnv,
                        const LHCb::PrimVertex::ConstVector::const_iterator vBegin,
                        const LHCb::PrimVertex::ConstVector::const_iterator vEnd,
                        const IGeomDispCalculator* pIPTool) const = 0;

  // Return the relation table linking Particles to Vertices
  virtual TableV table(const LHCb::Particle::Container& particles,
                       const LHCb::Vertex::Container& vertices,
                       const IGeomDispCalculator* pIPTool) const = 0;

  virtual TableV table(const LHCb::Particle::ConstVector& particles,
                       const LHCb::Vertex::ConstVector& vertices,
                       const IGeomDispCalculator* pIPTool) const = 0;


  virtual TableV table(const LHCb::Particle::Container::const_iterator pBegin,
                       const LHCb::Particle::Container::const_iterator pEnv,
                       const LHCb::Vertex::Container::const_iterator   vBegin,
                       const LHCb::Vertex::Container::const_iterator   vEnd,
                       const IGeomDispCalculator* pIPTool) const = 0;

  virtual TableV table(const LHCb::Particle::ConstVector::const_iterator pBegin,
                       const LHCb::Particle::ConstVector::const_iterator pEnv,
                       const LHCb::Vertex::ConstVector::const_iterator   vBegin,
                       const LHCb::Vertex::ConstVector::const_iterator   vEnd,
                       const IGeomDispCalculator* pIPTool) const = 0;


};
#endif // DAVINCIKERNEL_IPARTICLE2VERTEXASCT_H
