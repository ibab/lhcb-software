// $Id$
#ifndef KERNEL_PARTICLE2VERTEX_H 
#define KERNEL_PARTICLE2VERTEX_H 1

// Include files
#include "Event/VertexBase.h"
#include "Event/Particle.h"
#include "Relations/RelationWeighted.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/Relation.h"
#include "Relations/Relation1D.h"
#include "GaudiKernel/HashMap.h"
/** @namespace Particle2Vertex Particle2Vertex.h Kernel/Particle2Vertex.h
 *  
 * Namespace containing types corresponding to the Particle -> VertexBase 
 * uni-directional weighted relationship.
 *
 *  @author Juan PALACIOS
 *  @date   2006-08-17
 */
namespace Particle2Vertex {
  typedef double                                          WeightType ;
  typedef LHCb::RelationWeighted1D<LHCb::Particle, 
                                   LHCb::VertexBase,
                                   WeightType>            WTable      ;

  typedef Relations::RelationWeighted<LHCb::Particle, 
                                      LHCb::VertexBase,
                                      WeightType>         LightWTable ;

  typedef WTable::IBase::TypeTraits::Entry                 WRelation   ;
  typedef WTable::IBase::TypeTraits::Entries               WRelations  ;

  typedef LHCb::Relation1D<LHCb::Particle, 
                           LHCb::VertexBase>              Table      ;

  typedef Relations::Relation<LHCb::Particle, 
                              LHCb::VertexBase>           LightTable ;
  typedef Table::IBase::TypeTraits::Entry                 Relation   ;
  typedef Table::IBase::TypeTraits::Entries               Relations  ;

  typedef GaudiUtils::HashMap<const LHCb::Particle*, const LHCb::VertexBase*> Map;

}
#endif // KERNEL_PARTICLE2VERTEX_H
