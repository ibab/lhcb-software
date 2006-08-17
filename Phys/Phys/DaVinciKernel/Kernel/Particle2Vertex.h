// $Id: Particle2Vertex.h,v 1.1 2006-08-17 16:41:00 jpalac Exp $
#ifndef KERNEL_PARTICLE2VERTEX_H 
#define KERNEL_PARTICLE2VERTEX_H 1

// Include files
#include "Event/VertexBase.h"
#include "Event/Particle.h"
#include "Relations/RelationWeighted1D.h"
/** @namespace Particle2Vertex Particle2Vertex.h Kernel/Particle2Vertex.h
 *  
 * Namespace containing types corresponding to the Particle -> VertexBase 
 * uni-directional weighted relationship.
 *
 *  @author Juan PALACIOS
 *  @date   2006-08-17
 */
namespace Particle2Vertex {
  typedef LHCb::RelationWeighted1D<LHCb::Particle, 
                                   LHCb::VertexBase, 
                                   double>               _P2V   ;
  typedef _P2V::To                                       To     ;
  typedef _P2V::From                                     From   ;
  typedef _P2V::Weight                                   Weight ;
  typedef _P2V::Range                                    Range  ;

}
#endif // KERNEL_PARTICLE2VERTEX_H
