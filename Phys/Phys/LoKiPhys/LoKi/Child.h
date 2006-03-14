// $Id: Child.h,v 1.3 2006-03-14 19:06:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/18 18:09:14  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/02/17 19:17:26  ibelyaev
// New package: "Phys"-part of restructurized LoKi project  
// 
// ============================================================================
#ifndef LOKI_CHILD_H 
#define LOKI_CHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-10 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace Child Child.h LoKi/Child.h
   *
   *  Namespace with collection of accessr to children  
   *  @see LHCb::Particle
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-11
   */
  namespace Child
  {
    
    /** @fn child 
     *  Trivial accessor to the dautgher particles for the given particle.
     *  It is not very useful in a"stand-alone-mode", but it coudl be very useful 
     *  for non-trivial access:
     *
     *  @code 
     *
     *  for( Loop phi = loop( "kaon+ kaon-" , "pho(1020)" ) ; phi ; ++phi )
     *   {
     *    if( ..... ) { phi->save("phi") ; }
     *   }
     *  
     *  for( Loop Bs = loop( "phi gamma" ) ; Bs ; ++Bs )
     *   {
     *       const Particle* kaon_plus  = child( Bs(1) , 1 ) ; 
     *       const Particle* kaon_minus = child( Bs(1) , 2 ) ; 
     *   }
     *
     *  @endcode 
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index    index   index of the child particle 
     *  @return daughter particle with given index 
     */
    LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const size_t           index    ) ;
    
    inline LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const size_t           index1   , 
      const size_t           index2   )
    { return child ( child ( particle , index1 ) , index2 ) ; }
    
    inline LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const size_t           index1   , 
      const size_t           index2   ,
      const size_t           index3   )
    { return child ( child ( particle , index1 ) , index2 , index3 ) ; }
    
    inline LHCb::Particle* child
    ( const LHCb::Particle*  particle , 
      const size_t           index1   , 
      const size_t           index2   ,
      const size_t           index3   ,
      const size_t           index4   )
    { return child ( child ( particle , index1 ) , index2 , index3 , index4 ) ; }
    
  } ; // end of the namespace LoKi:Child 
} ; // end of the namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CHILD_H
// ============================================================================

