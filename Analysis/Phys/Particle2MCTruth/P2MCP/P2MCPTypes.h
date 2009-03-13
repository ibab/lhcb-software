// $Id: P2MCPTypes.h,v 1.2 2009-03-13 18:05:06 jpalac Exp $
#ifndef P2MCP_P2MCPTYPES_H 
#define P2MCP_P2MCPTYPES_H 1

// Include files
#include "Event/MCParticle.h"
#include "Event/Particle.h"
#include "Relations/Relation.h"
#include "Relations/Relation1D.h"
#include "Kernel/MCAssociation.h"
#include <vector>
/** @namespace P2MCPTypes P2MCP/P2MCPTypes.h
 *  
 *
 *  Namespace containing types corresponding to the Particle -> MCParticle 
 *  uni-directional weighted relationship.
 *
 *  @author Juan PALACIOS
 *  @date   2009-03-10
 */
namespace P2MCP {

  namespace Types 
  {

    typedef LHCb::MCParticle::ConstVector  FlatTree;
    typedef std::vector<FlatTree>    FlatTrees;

    typedef LHCb::Relation1D<LHCb::Particle, 
                             LHCb::MCParticle  >            Table      ;

    typedef Relations::Relation<LHCb::Particle, 
                                LHCb::MCParticle >          LightTable ;
    typedef Table::Range                                    Range      ;
    typedef Table::IBase::TypeTraits::Entry                 Relation   ;
    typedef Table::IBase::TypeTraits::Entries               Relations  ;
    typedef Table::To                                       To         ;
    typedef Table::From                                     From       ;

    typedef std::vector<MCAssociation>                      ToVector;
  }
  
}
#endif // P2MCP_P2MCPTYPES_H
