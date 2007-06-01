// $Id: PhysRangeTypes.h,v 1.7 2007-06-01 12:07:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2007/04/16 16:16:26  pkoppenb
// removed polemic comment
//
// Revision 1.5  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.4  2006/08/15 15:13:25  ibelyaev
//  update for new inheritance scheme Vertex<--VertexBase-->RecVertex
//
// ============================================================================
#ifndef LOKI_PHYSRANGETYPES_H 
#define LOKI_PHYSRANGETYPES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/NamedRange.h"
#include "LoKi/Selected.h"
#include "LoKi/RangeList.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
namespace LoKi
{
  /** @namespace LoKi::PhysTypes LoKi/PhysRangeTypes.h
   *  useful "physics types"
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */   
  namespace PhysTypes
  {
    /// the actual type of the container of particles  
    typedef LHCb::Particle::ConstVector          Container ;
    /// the actual storage  of containers of particles  
    typedef LoKi::Selected_<Container>            Selected ;
    /// the actual type of range of particles 
    typedef Selected::Range   Range ;
    /// the actual type of the container of vertices 
    typedef LHCb::VertexBase::ConstVector         VContainer ;
    /// the actual storagf  of containers of particles  
    typedef LoKi::Selected_<VContainer>           VSelected  ;
    /// the actual type of range of particles 
    typedef VSelected::Range                         VRange  ;
  } // end of namespace LoKi::PhysTypes  
  namespace Types
  {
    /// the actual range of particles 
    typedef LoKi::PhysTypes::Range                  Range   ;
    /// the actual range of particles 
    typedef LoKi::PhysTypes::VRange                 VRange  ;
    /// the actual range of range list particles 
    typedef LoKi::RangeList_<Range>                 RangeL  ;
    /// the actual range of range list particles 
    typedef LoKi::RangeList_<VRange>                VRangeL ;
  } // end of namespace LoKi::Types  
  /** Trivial conversion function 
   *  @param vct vector of noon-const particles 
   *  @param return vector of const particles 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */
  inline 
  LHCb::Particle::ConstVector 
  convert ( LHCb::Particle::Vector&      vct ) 
  { return LHCb::Particle::ConstVector ( vct.begin() , vct.end() ) ; }
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSRANGETYPES_H
// ============================================================================
