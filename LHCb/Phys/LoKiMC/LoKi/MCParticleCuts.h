// $Id: MCParticleCuts.h,v 1.9 2007-06-03 20:41:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.9 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef LOKI_MCPARTICLECUTS_H 
#define LOKI_MCPARTICLECUTS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCParticles.h"
#include "LoKi/MCPIDOperators.h"
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
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  namespace Cuts
  {   
    /** Minimum from 2 functions 
     *  
     *  @code 
     *  
     *  MCFun f1 = ... ;
     *  MCFun f2 = ... ;
     *  
     *  MCFun f = MCMIN( f1 , f2 ) ;
     *  
     *  @endcode 
     *  
     *  @see LoKi::Min
     */
    typedef LoKi::Min<const LHCb::MCParticle*>                        MCMIN;

    /** Maxumum from 2 functions 
     *  
     *  @code 
     *  
     *  MCFun f1 = ... ;
     *  MCFun f2 = ... ;
     *  
     *  MCFun f = MCMAX( f1 , f2 ) ;
     *  
     *  @endcode 
     *  
     *  @see LoKi::Max
     */
    typedef LoKi::Max<const LHCb::MCParticle*> MCMAX ;
    
    /** Simple "switch"
     *  The function evaluated the predicate and returns 
     *  one of the predefined values, depending on result on 
     *  predicate evaluation.
     *  In some sense it is a conversion of "predicate" to "function"
     * 
     *  @code
     *
     *   const MCParticle* mcp = ...  ;
     *
     *   MCFun fun = MCSSWITCH( MCMOTHCUT( MCVALID )  , 1 , -1 ) 
     *
     *   const double value = fun( p ) ;
     *
     *  @endcode 
     *
     *  For this example function returns 1 if MC Particle has valid 
     *  mother particle and -1 otherwise 
     *
     *  @see LoKi::SipleSwitch 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     */
    typedef LoKi::SimpleSwitch<const LHCb::MCParticle*> MCSSWITCH ;
    
    /** switch"
     *  The function evaluates the predicate and returns 
     *  the values of one of the predefined functions, 
     *  depending on result on predicate evaluation.
     * 
     *  @code
     *
     *   const MCParticle* mcp = ...  ;
     *
     *   MCFun fun = MCSWITCH( MCMOTHCUT( MCVALID )  ,
     *                          MCMOTHER ( MCPT )     ,
     *                         -1 * GeV               ) 
     *
     *   const double value = fun( p ) ;
     *
     *  @endcode 
     *
     *  For this example function returns the value of 
     *  PT of mother particle for MC Particle with valid mother
     *  and -1 * GeV otherwise 
     *
     *  @see LoKi::Switch 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     */
    typedef LoKi::Switch<const LHCb::MCParticle*>                    MCSWITCH;

    /** @var MCVALID 
     *
     *  The trivial checker of "the validity" of the pointer to 
     *   LHCb::MCParticle objects
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const bool valid =   = MCVALID (  p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Valid
     *  @authot Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-01-21
     */
    const LoKi::Valid<const LHCb::MCParticle*>                      MCVALID ;
    
    /** @var MCP 
     *  MCParticle momentum
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      m  = MCP  (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Momentum
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::Momentum                      MCP  ;

    /** @var MCE 
     *  MCParticle energy    
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      e  = MCE  (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Energy   
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::Energy                        MCE  ;    

    /** @var OSCILLATED
     *
     *  It evaluated to 'true' for scilalted particles 
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const bool oscillated = OSCILLATED (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Oscillated
     *  
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2006-02-05
     */
    const LoKi::MCParticles::Oscillated            OSCILLATED ;

    /** @var MCOSCILLATED
     *
     *  It evaluated to 'true' for scilalted particles 
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const bool oscillated = MCOSCILLATED (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Oscillated
     *  
     *  @author Vanya Belyaev ibelyaev@physics.syr.edu
     *  @date   2006-02-05
     */
    const LoKi::MCParticles::Oscillated            MCOSCILLATED ;

    /** @var MCPT
     *  MCParticle transverse momentum 
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      v  = MCPT (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::TransverseMomentum
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::TransverseMomentum            MCPT ;
    
    /** @var MCPX
     *  MCParticle x-momentum               
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      v  = MCPX (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::MomentumX           
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::MomentumX                     MCPX ;    

    /** @var MCPY
     *  MCParticle y-momentum               
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      v  = MCPY (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::MomentumY           
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::MomentumY                     MCPY ;

    /** @var MCPZ
     *  MCParticle z-momentum               
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      v  = MCPZ (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::MomentumZ           
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::MomentumZ                     MCPZ ;

    /** @var MCETA
     *  MCParticle pseudorapidity 
     * 
     *  @code
     * 
     *   const MCParticle* p    = ...         ;
     *   const double      eta  = MCETA(  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::PseudoRapidity           
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::PseudoRapidity              MCETA ;

    /** @var MCPHI
     *  MCParticle asymuthal angle 
     * 
     *  @code
     * 
     *   const MCParticle* p    = ...         ;
     *   const double      phi  = MCPHI(  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Phi
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::Phi                         MCPHI ;
    
    /** @var MCTHETA
     *  MCParticle polar angle 
     * 
     *  @code
     * 
     *   const MCParticle* p    = ...         ;
     *   const double      theta  = MCTHETA(  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Theta
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-17
     */
    const LoKi::MCParticles::Theta                         MCTHETA ;

    /** @var MCMASS 
     *  MCParticle mass                     
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      v  = MCMASS ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Mass          
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::Mass                        MCMASS ;

    /** @var MCID 
     *  MCParticle ID 
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      id = MCID (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::Identifier 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::Identifier                    MCID ;

    /** @var MCABSID 
     *  abs( MCParticle ID )  
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...            ;
     *   const double      id = MCABSID (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::AbsIdentifier 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::AbsIdentifier                MCABSID ;

    /** @var MC3Q 
     *  MCParticle three charge 
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      q  = MC3Q (  p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::ThreeCharge
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::ThreeCharge                   MC3Q ;

    /** @var MCTIME
     *  MCParticle proper lifetime (in c*tau units)
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      t  = MCTIME ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::ProperLifeTime
     *  @see LoKi::Cuts::MCCTAU
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::ProperLifeTime              MCTIME ;

    /** @var MCCTAU
     *  MCParticle proper lifetime (in c*tau units)
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   const double      t  = MCCTAU ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::ProperLifeTime
     *  @see LoKi::Cuts::MCTIME
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const LoKi::MCParticles::ProperLifeTime              MCCTAU ;

    /** quark content 
     * 
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *   MCCut cut = MCQUARK( ParticleID::bottom )
     * 
     *   const bool beauty  = cut ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::HasQuark
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    typedef LoKi::MCParticles::HasQuark             MCQUARK ;

    /** @var BEAUTY
     *  Check for bottom quark 
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool beauty  = BEAUTY ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::HasQuark
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::HasQuark  BEAUTY( LHCb::ParticleID::bottom ) ;    

    /** @var CHARM
     *  Check for charm quark 
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool charm  = CHARM ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::HasQuark
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::HasQuark  CHARM ( LHCb::ParticleID::charm  ) ;
    
    /** @var STRANGE
     *  Check for strange quark 
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool strange = STRANGE ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::HasQuark
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::HasQuark  STRANGE ( LHCb::ParticleID::strange ) ;

    /** @var TOP
     *  Check for TOP  quark 
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool top  = TOP( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::HasQuark
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const  LoKi::MCParticles::HasQuark  TOP     ( LHCb::ParticleID::top     ) ;

    /** @var CHARGED
     *  Check for particle charge 
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool charged = CHARGED ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsCharged
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsCharged                            CHARGED ;

    /** @var NEUTRAL
     *  Check for particle charge 
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool neutral = NEUTRAL ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsNeutral
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsNeutral                            NEUTRAL ;

    /** @var LEPTON 
     *  Check for particle type   
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool lepton  = LEPTON  ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsLepton 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsLepton                             LEPTON  ;

    /** @var MESON  
     *  Check for particle type   
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool meson   = MESON   ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsMeson  
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsMeson                              MESON   ;

    /** @var BARYON 
     *  Check for particle type   
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool baryon  = BARYON  ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsBaryon 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsBaryon                             BARYON  ;

    /** @var HADRON 
     *  Check for particle type   
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool hadron  = HADRON  ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsHadron 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsHadron                             HADRON  ;

    /** @var NUCLEUS
     *  Check for particle type   
     *
     *  @code
     * 
     *   const MCParticle* p  = ...         ;
     *
     *   const bool nucleus = NUCLEUS ( p ) ;
     *
     *  @endcode 
     *
     *  @see MCParticle
     *  @see LoKi::MCParticles::IsNucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2004-03-01
     */
    const  LoKi::MCParticles::IsNucleus                            NUCLEUS ;


    /** siple predicate wich evaluates if MC particle comes from decay tree of 
     *  (an)other MC particle(s)
     * 
     * @code 
     *
     * const MCParticle* p = .... ;
     * const MCParticle* parent = ... ;
     * 
     *  MCFun fun = FROMMCTREE( parent ) ;
     * 
     *  const bool fromTree = fun( p ) ;
     *
     * @endcode 
     *  
     * @see MCParticle
     * @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     * @date 2004-07-07
     */
    typedef LoKi::MCParticles::FromMCDecayTree                    FROMMCTREE ;
    
    /** simple adapter function which counts teh number 
     *  particle in MC decay tree which satisfy a certain criteria
     *  
     *  @code 
     *
     *   const MCParticle* p = ... ;
     * 
     *   // number of pions 
     *   MCFun fun = NINMCDOWN( "pi+" == MCABSID ) ;
     *  
     *   double numPI = fun( p ) ;
     *
     *  @endcode 
     * 
     */
    typedef LoKi::MCParticles::NinMCdownTree         NINMCDOWN ;

    /** simple adapter function whcih delegates the 
     *  evaluation of 'main' function to the mother particle
     *  
     *  @code 
     *
     *   const MCParticle* p = ... ;
     *
     *   // mc ID for mother particle 
     *   MCFun fun = MCMOTHER( MCABSID , -1000 ) ;
     *  
     *   double mothID = fun( p ) ;
     *
     *  @endcode 
     * 
     */
    typedef LoKi::MCParticles::MCMotherFunction         MCMOTHER ;
    
    /** simple adapter function which delegates the 
     *  evaluation of 'main' predicate to the mother 
     *  particle
     *  
     *  @code 
     *
     *   const MCParticle* p = ... ;
     *
     *   // check if mother is beauty particle 
     *   MCCut cut = MCMOHCUT( BEAUTY , false ) ;
     *  
     *   bool fromBeauty  = cut( p ) ;
     *
     *  @endcode 
     * 
     */
    typedef LoKi::MCParticles::MCMotherPredicate         MCMOTHCUT ;
    
    /** Trivial predicate to check the identity of a particle 
     *  and ContainedObject
     *
     *  @see LoKi::MCParticles::IsContainedObject
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-02
     */
    typedef LoKi::MCParticles::IsContainedObject           ISMCCO  ;
    
    
    /** Trivial predicate to check the identity of a particle 
     *  and another particle 
     *
     *  @see LoKi::MCParticles::IsParticle
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-02-02
     */
    typedef LoKi::MCParticles::IsParticle                    ISMCPART ;
     
    /** @var MCTRUE
     *  primitive predicate, "always true"
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const LHCb::MCParticle*> MCTRUE  ( true  ) ;
    
    /** @var MCFALSE
     *  primitive predicate, "always false"
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const LHCb::MCParticle*> MCFALSE ( false ) ;
    
    /** @var MCALL
     *  primitive predicate, "always true"
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const LHCb::MCParticle*> MCALL  = MCTRUE   ;
    
    /** @var MCNONE
     *  primitive predicate, "always false"
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::BooleanConstant<const LHCb::MCParticle*> MCNONE = MCFALSE  ;
    
    /** @var MCONE
     *  primitive function, "always 1"
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::Constant       <const LHCb::MCParticle*> MCONE   ( 1.0   ) ;
    
    /** @var MCZERO
     *  primitive function, "always 0"
     *  @see LoKi::Constant
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
     *  @date 2005-03-27
     */
    const LoKi::Constant       <const LHCb::MCParticle*> MCZERO  ( 0.0   ) ;
    
    /** the evaluator of euclidian distance with 
     *  respect to the given 4-momentum, useful 
     *  e.g. for trivial "kinematical matching". 
     *  One can find 
     *  the particle with "closest" 4-momentum 
     *  to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            particles = ... ;
     *  LoKi::LorentzVector vct         = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( particles.begin () , 
     *                      particles.end   () , 
     *                      MCMOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::LorentzVector 
     *  @see LoKi::MCParticles::MomentumDistance 
     *  
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    typedef LoKi::MCParticles::MomentumDistance      MCMOMDIST ;
    
    /** primitive evaluator of particle's transverse momentum
     *  relative to some direction
     *
     *  @code 
     * 
     *  const MCParticle*           p = ... ;
     *  const LoKi::ThreeVector& vct = ... ; 
     *
     *  MCFun ptRel = MCPTDIR( vct ) ;
     * 
     *  const double pt = ptRel( p ) ;
     *  
     *  @endcode 
     * 
     *  @see LoKi::LorentzVector 
     *  @see LoKi::MCParticles::TransverseMomentumRel
     *  
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */     
    typedef LoKi::MCParticles::TransverseMomentumRel MCPTDIR   ;
    
    /** @var MCOVALID 
     *  trivial predicate which evaluated to true for 
     *  MC particlres with valid "origin" vertex 
     *  @see LoKi::MCParticles::VaildOrigin
     *  @author Vanya BELYAEV Ivanb.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    const   LoKi::MCParticles::ValidOrigin           MCOVALID;
    
    /** simple adapter, which evaluated the actual function 
     *  computation to "origin" vertex for MC particle
     *  
     *  @code 
     *   
     *   const LoKi::Point3D& point = ... ; 
     *
     *   MCFun dist = MCVFASPF( MCVDIST( point ) ) ;
     * 
     *  @endcode
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    typedef LoKi::MCParticles::MCVertexFunAdapter    MCVFASPF;

    
    /** Simple predicate to test certain decay pattern, using
     *  nice tool IMCDecayFinder, developed by Olivier DORMOND
     *
     *  The idea of this predicate belongs to Luis FERNANDEZ
     *
     *  @code 
     *
     *   // create the predicate which evaluated to true 
     *   // for B0 -> + pi- and all its ancestors 
     *   MCCut b2pp = MCDECAY ( "B0 -> pi+ pi-"   , 
     *                          mcDecayFinder () ) ;
     * 
     *   // use this predicate :  
     *  MCRange B0 = mcselect ( "B0" , b2pp && "B0" == MCID ) ;
     *
     *  @endcode 
     * 
     *  @attention The "decay" string of IMCDecayFinder tool is redefined!
     *
     *  @see IMCDecayFinder
     *  @see LoKi::MCParticles::MCDecayPattern
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    typedef LoKi::MCParticles::MCDecayPattern        MCDECAY ;
    
    /** @typedef MCDPHI
     *  simple evaluator of "delta phi" of the particle momenta
     * 
     *  @code 
     *
     *   double phi = ... ;
     * 
     *   // create the function: 
     *  MCFun dphi = GMCDPHI(phi) ;
     * 
     *  const LHCb::MCParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = dphi ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCParticless::DeltaPhi 
     *  @see LoKi::MCParticless::Phi
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::MCParticles::DeltaPhi                    MCDPHI ;

    /** @typedef MCDETA
     *  simple evaluator of "delta eta" of the particle momenta
     * 
     *  @code 
     *
     *   double eta = ... ;
     * 
     *   // create the function: 
     *  MCFun deta = MCDETA(eta) ;
     * 
     *  const LHCb::MCParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = deta ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCParticless::DeltaEta 
     *  @see LoKi::MCParticless::Eta
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::MCParticles::DeltaEta                    MCDETA ;

    /** @typedef GDR2 
     *  simple evaluator of "delta eta" of the particle momenta
     * 
     *  @code 
     *
     *   double et
     * 
     *   // create the function: 
     *  MCFun r2 = MCDR2( v ) ;
     * 
     *  const LHCb::MCParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = r2 ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCParticless::DeltaR2  
     *  @see LoKi::MCParticless::DeltaEta
     *  @see LoKi::MCParticless::DeltaPhi
     *  @see LoKi::Cuts::MCDELTAR2 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::MCParticles::DeltaR2                     MCDR2     ;
    
    /** @typedef MCDELTAR2 
     *  simple evaluator of "delta eta" of the particle momenta
     * 
     *  @code 
     *
     *   double et
     * 
     *   // create the function: 
     *  MCFun r2 = MCDELTAR2( v ) ;
     * 
     *  const LHCb::MCParticle* p = ... ;
     *
     *  // use the function 
     *  const double result = r2 ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::MCParticless::DeltaR2  
     *  @see LoKi::MCParticless::DeltaEta
     *  @see LoKi::MCParticless::DeltaPhi
     *  @see LoKi::Cuts::MCDR2 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03 
     */
    typedef LoKi::MCParticles::DeltaR2                     MCDELTAR2 ;
    
    /** @typedef MCREC 
     *  the simple function(wrapper for IMCRecontructible tool)
     *  which return the recontruction category for the given MC-particle
     *
     *  @code
     *  
     *  const LHCb::MCParticle*  kaon = ... ;
     *  const IMCRecontructible* tool = ... ;
     *
     *  // create the function to recontruction category
     *
     *  MCFun rec = MCREC(tool) ;
     *
     *  // use the function:
     *
     *  const bool recAsLong = rec ( kaon ) == IMCRecontructible::ChargedLong ;
     *
     *  @endcode 
     *
     *  @see IMCRecontructible
     *  @see IMCRecontructible::RecCategory
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     */
    typedef LoKi::MCParticles::MCReconstructible           MCREC ;
    
    /** @typedef MCRECAS
     *  the simple predicate(wrapper for IMCRecontructible tool)
     *  which checks the reocntruction category for the given MC-particle
     *
     *  @code
     *  
     *  const LHCb::MCParticle*  kaon = ... ;
     *  const IMCRecontructible* tool = ... ;
     *
     *  // create the cut to check if it is recontructibleas long track:
     *
     *  MCCut asLong = MCRECAS ( tool , IMCRecontructible::ChargedLong ) ;
     *
     *  // use the predicate
     *
     *  const bool recAsLong = asLong ( kaon )
     *
     *  @endcode 
     *
     *  @see IMCRecontructible
     *  @see IMCRecontructible::RecCategory
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     */
    typedef LoKi::MCParticles::MCReconstructibleAs         MCRECAS ;
    /** @typedef MCCHILD 
     *  Simple function which delegates the evaluation of 
     *  another function to a daughter "decay" MC-particle 
     * 
     *  @code 
     *  
     *   const LHCb::MCParticle* phi = ... ;
     * 
     *   // construct the fuction, which evaluates to PT of the first decy daughter:
     *   MCFun pt1 = MCCHILD ( MCPT , 1 ) ;
     * 
     *   // evaluate the PT of th efirst decay daughter:
     *   
     *   const double value = pt1 ( phi ) ;
     *
     *  @endcode 
     * 
     *  @attention index starts form 1, 
     *   null-value corresponds to the particle itself
     *
     *  @see LoKi::Cuts::MCPT 
     *  @see LoKi::Child::child  
     *  @see LoKi::MCParticles::ChildFunction
     *  @see LoKi::Cuts::MCCHILDFUN 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::ChildFunction              MCCHILD    ;
    /** @typedef MCCHILDFUN
     *  Simple function which delegates the evaluation of 
     *  another function to a daughter "decay" MC-particle 
     * 
     *  @code 
     *  
     *   const LHCb::MCParticle* phi = ... ;
     * 
     *   // construct the fuction, which evaluates to PT of the first decy daughter:
     *   MCFun pt1 = MCCHILDFUN ( MCPT , 1 ) ;
     * 
     *   // evaluate the PT of th efirst decay daughter:
     *   
     *   const double value = pt1 ( phi ) ;
     *
     *  @endcode 
     * 
     *  @attention index starts form 1, 
     *   null-value corresponds to the particle itself
     *
     *  @see LoKi::Cuts::MCPT 
     *  @see LoKi::Child::child  
     *  @see LoKi::MCParticles::ChildFunction
     *  @see LoKi::Cuts::MCCHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::ChildFunction              MCCHILDFUN ;    
    /** @typedef MCCHILDCUT
     *  Simple predicate which delegates the evaluation of 
     *  another function to a daughter "decay" MC-particle 
     * 
     *  @code 
     *  
     *   const LHCb::MCParticle* phi = ... ;
     * 
     *   // construct the function, which checks PT of the first child 
     *   MCCut pt1 = MCCHILDFUN ( MCPT > 1 * GeV  , 1 ) ;
     * 
     *   // check pt of th efirst daughter particle:
     *   
     *   const bool large  = pt1 ( phi ) ;
     *
     *  @endcode 
     * 
     *  @attention index starts form 1, 
     *   null-value corresponds to the particle itself
     *
     *  @see LoKi::Cuts::MCPT 
     *  @see LoKi::Child::child  
     *  @see LoKi::MCParticles::ChildPredicate
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::ChildPredicate              MCCHILDCUT ;    
    /** @typedef MCINTREE
     *  Simple predicate to check the presence of a certain 
     *  particle in the decay tree
     *  @see LoKi::MCParticles::InTree
     *  @see LoKi::MCAlgs::found 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::InTree                     MCINTREE ;
    /** @typedef MCNINTREE
     *  Simple function to count a certain particles in the decay tree
     *  @see LoKi::MCParticles::NinTree
     *  @see LoKi::MCAlgs::count_if 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::NinTree                    MCNINTREE ;
    /** @typedef MCSUMTREE
     *  Simple function to accumulate certain  values in the decay tree
     *  @see LoKi::MCParticles::SumTree
     *  @see LoKi::MCAlgs::accumulate 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::SumTree                    MCSUMTREE ;
    /** @typedef MCMULTTREE
     *  Simple function to accumulate certain  values in the decay tree
     *  @see LoKi::MCParticles::MultTree
     *  @see LoKi::MCAlgs::accumulate 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::MultTree                   MCMULTTREE ;
    /** @typedef MCMINTREE
     *  Simple function to find a minimum value in the decay tree
     *  @see LoKi::MCParticles::MinTree
     *  @see LoKi::MCAlgs::min_value
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::MinTree                    MCMINTREE ;    
    /** @typedef MCMAXTREE
     *  Simple function to find a maximum value in the decay tree
     *  @see LoKi::MCParticles::MaxTree
     *  @see LoKi::MCAlgs::min_value
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-02
     */
    typedef LoKi::MCParticles::MaxTree                    MCMAXTREE ;
  } // end of namespace LoKi::Cuts
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCPARTICLECUTS_H
// ============================================================================
