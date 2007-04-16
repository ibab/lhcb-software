// $Id: PhysCnv.h,v 1.5 2007-04-16 16:16:26 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.3  2006/03/18 12:40:18  ibelyaev
//  fix a warning for Win32
//
// Revision 1.2  2006/03/16 14:43:55  ibelyaev
//  tiny fix
//
// ============================================================================
#ifndef LOKI_PHYSCNV_H 
#define LOKI_PHYSCNV_H 1
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysRangeTypes.h"
// ============================================================================

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
  /** @namespace LoKi::Cnv PhysCnv.h LoKi/PhysCnv.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-17
   */
  namespace Cnv
  { 
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @param vct vector of non-const particles 
     *  @param return vector of const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    inline 
    LHCb::Particle::ConstVector 
    convert 
    ( const LHCb::Particle::Vector&      vct ) 
    { return LHCb::Particle::ConstVector( vct.begin() , vct.end() ) ; }
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @param vct vector of const particles 
     *  @param return vector of non-const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    LHCb::Particle::Vector 
    convert 
    ( const LHCb::Particle::ConstVector& vct ) ;
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @param vct vector of const particles 
     *  @param return vector of non-const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    inline  
    LHCb::Particle::Vector 
    convert 
    ( SmartRefVector<LHCb::Particle>& vct ) 
    { return LHCb::Particle::Vector( vct.begin() , vct.end() ) ; }
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @param vct vector of const particles 
     *  @param return vector of non-const particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    LHCb::Particle::Vector 
    convert 
    ( const SmartRefVector<LHCb::Particle>& vct ) ;
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @param vct vector of non-const vertices 
     *  @param return vector of const vertices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    inline 
    LHCb::Vertex::ConstVector 
    convert 
    ( const LHCb::Vertex::Vector&      vct ) 
    { return LHCb::Vertex::ConstVector( vct.begin() , vct.end() ) ; }
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @param vct vector of const vertices 
     *  @param return vector of non-const vertices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    LHCb::Vertex::Vector 
    convert 
    ( const LHCb::Vertex::ConstVector& vct ) ;
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    LHCb::Particle::Vector 
    convert 
    ( const LoKi::Types::Range& vct ) ;
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    inline 
    LHCb::Vertex::Vector 
    convert 
    ( SmartRefVector<LHCb::Vertex>& vct )
    { return LHCb::Vertex::Vector( vct.begin() , vct.end() ) ; }
    
    /** @fn convert 
     *  Trivial conversion function 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-16 
     */
    LHCb::Vertex::Vector 
    convert 
    ( const SmartRefVector<LHCb::Vertex>& vct ) ;
    

  } ; // end of namespace LoKi::Cnv 
} ; //end of namespace LoKi 

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSCNV_H
// ============================================================================
