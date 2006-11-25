// $Id: GenVertexCuts.h,v 1.4 2006-11-25 19:14:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/05/02 14:30:27  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_GENVERTEXCUTS_H 
#define LOKI_GENVERTEXCUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// HepMC
// ============================================================================
#include "HepMC/GenVertex.h"
// ============================================================================
// LoKiGen 
// ============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/GenVertices.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Collection of useful functions/predicates to deal with 
 *  HepMC generator information
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
 *  @date 2006-02-08 
 */
// ============================================================================

namespace LoKi 
{
  namespace Cuts 
  {
    /** @var GVTRUE
     *  trivial predicate which always returns "true"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::BooleanConstant<const HepMC::GenVertex*>   GVTRUE   ( true  ) ;

    /** @var GVFALSE
     *  trivial predicate which always returns "false"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::BooleanConstant<const HepMC::GenVertex*>   GVFALSE  ( false ) ;

    /** @var GVALL
     *  trivial predicate which always returns "true"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::BooleanConstant<const HepMC::GenVertex*>   GVALL    ( true  ) ;

    /** @var GVNONE
     *  trivial predicate which always returns "false"
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::BooleanConstant<const HepMC::GenVertex*>   GVNONE   ( false ) ;

    /** @var GVONE
     *  trivial function which always returns 1
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::Constant<const HepMC::GenVertex*>          GVONE    ( 1     ) ;

    /** @var GVNULL
     *  trivial function which always returns 0
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::Constant<const HepMC::GenVertex*>          GVNULL   ( 0     ) ;

    /** @var GVZERO
     *  trivial function which always returns 0
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const LoKi::Constant<const HepMC::GenVertex*>          GVZERO   ( 0     ) ;

    /** Minimum from 2 functions 
     *  
     *  @code 
     *  
     *  GVFun f1 = ... ;
     *  GVFun f2 = ... ;
     *  
     *  GVFun f = GVMIN( f1 , f2 ) ;
     *  
     *  @endcode 
     *  
     *  @see LoKi::Min
     */
    typedef LoKi::Min<const HepMC::GenVertex*>             GVMIN ;

    /** Maximum from 2 functions 
     *  
     *  @code 
     *  
     *  GVFun f1 = ... ;
     *  GVFun f2 = ... ;
     *  
     *  GVFun f = GVMAX( f1 , f2 ) ;
     *  
     *  @endcode 
     *  
     *  @see LoKi::MAX
     */
    typedef LoKi::Min<const HepMC::GenVertex*>             GVMAX ;

    /** @var GVBAR
     *  trivial function which returns the "bar-code" for 
     *  HepMC::GenVertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const   LoKi::GenVertices::BarCode                     GVBAR     ;

    /** @var GVBARCODE
     *  trivial function which returns the "bar-code" for 
     *  HepMC::GenVertex
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    const   LoKi::GenVertices::BarCode                     GVBARCODE ;
    
    /** @var GVX 
     *  trivial function which returns the X-position of 
     *  HepMC::GenVertex 
     *  @see HepMC::GenVertex 
     *  @see LoKi::GenVertices::PositionX
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    const   LoKi::GenVertices::PositionX                   GVX    ;

    /** @var GVY
     *  trivial function which returns the X-position of 
     *  HepMC::GenVertex 
     *  @see HepMC::GenVertex 
     *  @see LoKi::GenVertices::PositionY
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    const   LoKi::GenVertices::PositionY                   GVY    ;

    /** @var GVZ 
     *  trivial function which returns the Z-position of 
     *  HepMC::GenVertex 
     *  @see HepMC::GenVertex 
     *  @see LoKi::GenVertices::PositionZ
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    const   LoKi::GenVertices::PositionZ                   GVZ    ;

    /** @var GVT
     *  trivial function which returns the T-position of 
     *  HepMC::GenVertex 
     *  @see HepMC::GenVertex 
     *  @see LoKi::GenVertices::PositionT
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    const   LoKi::GenVertices::PositionT                   GVT    ;

    /** @var GVTIME
     *  trivial function which returns the T-position of 
     *  HepMC::GenVertex 
     *  @see HepMC::GenVertex 
     *  @see LoKi::GenVertices::PositionT
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    const   LoKi::GenVertices::PositionT                   GVTIME ;
    
    /** the adapter function which counts number of particles 
     *  which satisfy certain criteria within the specified range 
     *
     *  E.g. find the decay H -> b b~ X :
     *
     *  @code
     *
     *  // number of b-quarks withing children particles 
     *  GVFun num1 =  GVCOUNT( "b"  == GID , HepMC::children ) ; 
     *
     *  // number of b~-quarks withing children particles 
     *  GVFun num2 =  GVCOUNT( "b~" == GID , HepMC::children ) ; 
     *
     *  // use adapter function to End vertex 
     *  GFun num = GFAEVX ( num1 + num2 , 0 ) ;
     * 
     *  // find the decay H -> b b :
     *  
     *  const LHCB::HepMCEvents* events = ... ;
     *
     *  GCut decay = ( 2 == num ) && ( "H_10" == GID ) ;  
     *
     *  const bool good = LoKi::GenAlgs::found ( events , decay ) ;
     *
     *  @endcode 
     *  
     *  @see HepMC::GenVertex 
     *  @see HepMC::GenParticle 
     *  @see LoKi::GenVertices::CountIF
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    typedef LoKi::GenVertices::CountIF                     GVCOUNT ;

    /** the adapter function which accumulated the value of 
     *  given function over the particles which satisfy 
     *  certain criteria within the specified range 
     *
     *  Total energy of all stable daughter charged leptons:
     *
     *  @code
     *
     *  GVFun e1 = GVSUM( GE , HepMC::descendants , 
     *                           GCHARGED && GLEPTON ) ;
     *   
     *  GFun eLep = GFAEVX( e1 ) 
     *
     *  const HepMC::GenParticle* p = ... ;
     * 
     *  const double eTot = eLep( p ) ;
     *
     *  @endcode 
     *  
     *  @see HepMC::GenVertex 
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GFAEVX 
     *  @see LoKi::GenVertices::SumIF
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */     
    typedef LoKi::GenVertices::SumIF                     GVSUM ;

  } ; // end of namespace LoKi::Cuts
} ; // end of namespace LoKi

// ============================================================================
// The END
// ============================================================================
#endif // LOKI_GENVERTEXCUTS_H
// ============================================================================




