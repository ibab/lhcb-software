// $Id: Algo.h,v 1.4 2006-04-09 16:39:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_ALGO_H 
#define LOKI_ALGO_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/AlgoTypes.h"
#include "LoKi/Loop.h"
#include "LoKi/LoopObj.h"
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
 *  @date 2006-03-14 
 */
// ============================================================================

namespace LoKi 
{
  class LoopObj ;
  class Loop    ;  
  /** @class Algo Algo.h LoKi/Algo.h
   *  
   *
   *  @author Vanya BELYAEV
   *  @date   2006-03-14
   */
  class Algo 
    : public DVAlgorithm
  {
  public:
    
    /** 'Select' the particles to be used in local storage
     *  
     *  - The Particles are selected from the desktop
     *
     *  @code
     *
     *  Range kaons = select( "Kaons" , ABSID == "K+" && P > 5 * GeV );
     *
     *  @endcode
     *
     *  - The example illustrate the 'selection'/'filtering from
     *  desktop the particles, which are @c K+ or @c K- and have a
     *  momentum in excess of 5GeV/c
     *  - The selected particles
     *  are stored inside local LoKi storage under the tag @c "Kaons"
     *  and returned as a sequence of particles @c kaons .
     *
     *  @see LoKi::Types::Range 
     *  @see LoKi::Types::Cuts 
     *  @see LoKi::Cuts::ABSID 
     *  @see LoKi::Cuts::P
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select    
    ( const std::string&        name ,
      const LoKi::Types::Cuts&  cut  ) ;
    
    /** 'Select' the particles to be used in local storage
     * 
     *  - Particles are selected from the "range"
     *
     *  @code
     *
     *  Range kaons  = select( "Kaons" , ABSID == "K+" && P > 5 * GeV );
     *  Range kplus  = select( "K+" , kaons , Q >  0.5 );
     *  Range kminus = select( "K-" , kaons , Q < -0.5 );
     *
     *  @endcode
     *
     *  - The example illustrates the 'selection'/'filtering from
     *  desktop the particles, which are @c K+ or @c K- and have a
     *  momentum in excess of 5GeV/c
     *  - The selected particles
     *  are stored inside local LoKi storage under the tag @c "Kaons"
     *  and returned as a sequence of particles @c kaons .
     *  - Then from selected all kaons one selects positively charged
     *  kaons and saves them into local LoKi storage under the tag @c "K+".
     *  - The method returns the selected sequence into range @c kplus.
     *  - Analogously negatively charged kaons are saved under the tag @c "K-" 
     *
     *  @see LoKi::Types::Range 
     *  @see LoKi::Types::Cuts 
     *  @see LoKi::Cuts::ABSID
     *  @see LoKi::Cuts::P 
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range  
    select    
    ( const std::string&        name  ,
      const LoKi::Types::Range& range ,
      const LoKi::Types::Cuts&  cut   ) 
    { 
      return select ( name , range.begin() , range.end() , cut ) ; 
    } ;
    
    /** 'Select' the particles to be used in local storage
     * 
     *  - particles are selected from the container 
     *
     *  @code
     *
     *  const LHCb::Particle::Vector particles = ... ;
     *  Range positive  = select( "positive" , particles , Q >  0.5 );
     *  Range negative  = select( "negative" , particles , Q < -0.5 );
     *
     *  @endcode
     *
     *  - The example illustrates the 'selection'/'filtering from
     *  container <tt>particles</tt> positive and negative particles 
     *  - The selected particles
     *  are stored inside local LoKi storage under the tags @c "positive"
     *  and @c "negative" and returned as a sequence of particles 
     *  @c positive and @negative
     *
     *  @see LHCb::Particle::Vector
     *  @see LoKi::Types::Range 
     *  @see LoKi::Types::Cuts 
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cont input container of particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select   
    ( const std::string&            name  , 
      const LHCb::Particle::Vector& cont  , 
      const LoKi::Types::Cuts&      cuts  ) 
    { 
      return select ( name , cont.begin() , cont.end() , cuts ) ; 
    } ;
    
    /** 'Select' the particles to be used in local storage
     * 
     *  - particles are selected from the container 
     *
     *  @code
     *
     *  const LHCb::Particle::Vector particles = ... ;
     *  Range positive  = select( "positive" , particles , Q >  0.5 );
     *  Range negative  = select( "negative" , particles , Q < -0.5 );
     *
     *  @endcode
     *
     *  - The example illustrates the 'selection'/'filtering from
     *  container <tt>particles</tt> positive and negative particles 
     *  - The selected particles
     *  are stored inside local LoKi storage under the tags @c "positive"
     *  and @c "negative" and returned as a sequence of particles 
     *  @c positive and @negative
     *
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cont input container of particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select   
    ( const std::string&                 name  , 
      const LHCb::Particle::ConstVector& cont  , 
      const LoKi::Types::Cuts&           cuts  ) 
    { 
      return select ( name , cont.begin() , cont.end() , cuts ) ; 
    } ;
    
    /** 'Select' the particles to be used in local storage
     * 
     *  - particles are selected from the container 
     *
     *  @code
     *
     *  const LHCb::Particles* particles = ... ;
     *  Range positive  = select( "positive" , particles , Q >  0.5 );
     *  Range negative  = select( "negative" , particles , Q < -0.5 );
     *
     *  @endcode
     *
     *  - The example illustrates the 'selection'/'filtering from
     *  container <tt>particles</tt> positive and negative particles 
     *  - The selected particles
     *  are stored inside local LoKi storage under the tags @c "positive"
     *  and @c "negative" and returned as a sequence of particles 
     *  @c positive and @negative
     *
     *  @see LHCb::Particle
     *  @see LHCb::Particles
     *  @see LoKi::Types::Cuts
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cont input container of particles 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select   
    ( const std::string&        name  , 
      const LHCb::Particles*    cont  , 
      const LoKi::Types::Cuts&  cuts  )
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::Particles* points to null, return empty range!" ) ;
        return LoKi::Types::Range() ;
      }
      return select ( name , cont->begin() , cont->end() , cuts ) ;
    } ;
    
    /** 'Select' the particles to be used in local storage
     * 
     *  - particles are selected from arbitrary sequence of particles 
     *
     *  @code
     *
     *  /// arbitrary sequence of objects, 
     *  /// convertible to type <tt>Particle*</tt>
     *  SEQUENCE particles = ... ;
     *  Range positive  = select( "positive"          ,   // "tag"
     *                            particles.begin ()  ,   // begin of sequence
     *                            particles.end   ()  ,   // end of sequence
     *                            Q >  0.5            ) ; // cut 
     *
     *  @endcode
     *
     *  - The example illustrates the 'selection'/'filtering from
     *  container <tt>particles</tt> positive particles 
     *  - The selected particles
     *  are stored inside local LoKi storage under the tag @c "positive"
     *  and returned as a sequence of particles 
     *  @c positive and @negative
     *
     *  @see LoKi::Types::Range
     *  @see LoKi::Types::Cuts
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param first begin of sequence of particls 
     *  @param last  last  of sequence of particls 
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    template <class PARTICLE, class CUT>
    LoKi::Types::Range          
    select    
    ( const std::string& name   , 
      PARTICLE           first  , 
      PARTICLE           last   , 
      const CUT&         cut    ) 
    {
      return m_selected.add ( name , first , last , cut ) ;
    } ;
    
    /** 'Select' the vertices to be used in local storage
     *  - Vertices are selected from desktop
     *  @param name name/tag assigned to the selected vertices
     *  @param cut  cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange         
    vselect   
    ( const std::string&         name  ,
      const LoKi::Types::VCuts&  cut   ) ;
    
    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "range")
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::VRange         
    vselect   
    ( const std::string&         name  ,
      const LoKi::Types::VRange& range ,
      const LoKi::Types::VCuts&  cut   ) 
    {
      return vselect ( name , range.begin() , range.end() , cut ) ;
    } ;

    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "cont")
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&               name ,
      const LHCb::Vertex::Vector&      cont ,
      const LoKi::Types::VCuts&        cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } ;
    
    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "cont")
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&               name ,
      const LHCb::Vertex::ConstVector& cont ,
      const LoKi::Types::VCuts&        cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } ;

    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "cont")
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&              name ,
      const LHCb::PrimVertex::Vector& cont ,
      const LoKi::Types::VCuts&       cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } ;
    
    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "cont")
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&                   name ,
      const LHCb::PrimVertex::ConstVector& cont ,
      const LoKi::Types::VCuts&            cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } ;
    
    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "cont" )
     *  @param name name/tag assigned to the selected particles
     *  @param cut  cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&        name ,
      const LHCb::Vertices*     cont ,
      const LoKi::Types::VCuts& cuts ) 
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::Vertices* points to null, return empty range!" ) ;
        return LoKi::Types::VRange() ;
      }
      return vselect ( name , cont->begin() , cont->end() , cuts ) ;
    } ;
    
    /** 'Select' the vertices to be used in local storage
     * 
     *  - vertices are selected from arbitrary sequence of particles 
     *
     *  @code
     *
     *  /// arbitrary sequence of objects, 
     *  /// convertible to type <tt>Vertex*</tt>
     *  SEQUENCE vertices = ... ;
     *  VRange primaries  = 
     *        vselect( "primaries"               ,   // "tag"
     *                  vertices.begin ()        ,   // begin of sequence
     *                  vertices.end   ()        ,   // end of sequence
     *                  Vertex::Primary == VTYPE ) ; // cut 
     *
     *  @endcode
     *
     *  - The example illustrates the 'selection'/'filtering from
     *  container <tt>vertices</tt> primary verticese
     *  - The selected vertices
     *  are stored inside local LoKi storage under the tag @c "primaries"
     *  and returned as a sequence of vertices 
     *  @c positive and @negative
     *
     *  @see LoKi::Cuts
     *  @param name name/tag assigned to the selected vertices
     *  @param first begin of sequence of vertices
     *  @param last  last  of sequence of vertices 
     *  @param cut  cut to be applied
     *  @return selected range of vertices
     */
    template <class VERTEX,class CUTS>
    LoKi::Types::VRange        
    vselect    
    ( const std::string& name  , 
      const VERTEX&      first , 
      const VERTEX&      last  , 
      const CUTS&        cut   ) 
    { 
      return m_vselected.add ( name , first , last , cut ) ; 
    } ;
    
    /** Extract the selected particles from local LoKi storage  
     *   by their name/tag
     *  
     *  @code
     *
     *  Range kaons = selected( "Kaons");
     *
     *  @endcode
     *
     *  @see LoKi::Types::Range 
     *  @param name name/tag assigned to the selected particles
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    selected 
    ( const std::string& name ) const 
    {
      return m_selected ( name ) ; 
    } ;
    
    /** Extract the selected vertices from local LoKi storage  
     *   by their name/tag
     *  
     *  @code
     *
     *  VRange primaries = vselected( "Primaries");
     *
     *  @endcode
     *
     *  @param name name/tag assigned to the selected vertices
     *  @return selected range of particles
     */
    LoKi::Types::VRange 
    vselected 
    ( const std::string& name ) const 
    { 
      return m_vselected ( name ) ;
    } ;

  public:    
    /** Create loop object 
     *
     *  @code
     * 
     *  Loop Bs = loop ( "Ds pi+" , "B_s0" ) ;
     *
     *  @endcode
     *
     *  @param formula  looping formula 
     *  @param pid      effective particle ID
     *  @param combiner the combiner tool to be used for creation of the particle
     */
    LoKi::Loop loop
    ( const std::string&            formula      , 
      const std::string&            pid          , 
      const IParticleCombiner*      combiner = 0 ) ;
    /** Create loop object 
     *
     *  @code
     * 
     *  LHCb::ParticleID BS  = ... ;
     *  Loop Bs = loop ( "Ds pi+" , BS ) ;
     *
     *  @endcode
     *
     *  @param formula  looping formula 
     *  @param pid      effective particle ID
     *  @param combiner the combiner tool to be used for creation of the particle
     */
    LoKi::Loop loop 
    ( const std::string&            formula      , 
      const LHCb::ParticleID&       pid          , 
      const IParticleCombiner*      combiner = 0 ) ;    
    /** Create loop object 
     *
     *  @code
     * 
     *  const ParticleProperty* BS = ... ;
     *  Loop Bs = loop ( "Ds pi+" , BS ) ;
     *
     *  @endcode
     *
     *  @param formula  looping formula 
     *  @param pid      effective particle ID
     *  @param combiner the combiner tool to be used for creation of the particle
     */
    LoKi::Loop loop
    ( const std::string&            formula      , 
      const ParticleProperty*       pid      = 0 , 
      const IParticleCombiner*      combiner = 0 ) ;
    /** Create loop object 
     *
     *  @code
     * 
     *  Range Ds     = ... ;
     *  Range piplus = ... ;
     *
     *  Loop Bs = loop ( Ds + piplus , "B_s0" ) ;
     *
     *  @endcode
     *
     *  @param formula  looping formula 
     *  @param pid      effective particle ID
     *  @param combiner the combiner tool to be used for creation of the particle
     */    
    LoKi::Loop loop 
    ( const LoKi::Types::RangeList& formula      , 
      const std::string&            pid          , 
      const IParticleCombiner*      combiner = 0 ) ;    
    /** Create loop object 
     *
     *  @code
     * 
     *  Range Ds     = ... ;
     *  Range piplus = ... ;
     *
     *  Loop Bs = loop ( Ds + piplus , 531 ) ;
     *
     *  @endcode
     *
     *  @param formula  looping formula 
     *  @param pid      effective particle ID
     *  @param combiner the combiner tool to be used for creation of the particle
     */    
    LoKi::Loop loop 
    ( const LoKi::Types::RangeList& formula      , 
      const LHCb::ParticleID&       pid          , 
      const IParticleCombiner*      combiner = 0 ) ;
    /** Create loop object 
     *
     *  @code
     * 
     *  Range Ds     = ... ;
     *  Range piplus = ... ;
     *
     *  const ParticleProperty* BS = ... ; 
     *  Loop Bs = loop ( Ds + piplus , BS ) ;
     *
     *  @endcode
     *
     *  @param formula  looping formula 
     *  @param pid      effective particle ID
     *  @param combiner the combiner tool to be used for creation of the particle
     */    
    LoKi::Loop loop 
    ( const LoKi::Types::RangeList& formula      , 
      const ParticleProperty*       pid      = 0 , 
      const IParticleCombiner*      combiner = 0 ) ;
  public:
    
    /**
     *{
     *  loop->backup()  ;
     *  for ( ; loop ; ++loop ) 
     *   {
     *     if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
     *     if ( !vcut ( loop ) ) { continue ; }
     *     if ( ! cut ( loop ) ) { continue ; }
     *     loop->save ( tag ) ;
     *   }
     *  loop->restore() ;
     *  return selected ( tag ) ;
     * } ;
     */
    LoKi::Types::Range pattern
    ( const std::string&        tag    , 
      const LoKi::Loop&         loop   ,
      const LoKi::Types::Cuts&  cut    , 
      const LoKi::Types::VCuts& vcut   , 
      const IParticleReFitter*  fitter ) ;
    
    /**
     * { 
     *  loop->backup()  ;
     *  for ( ; loop ; ++loop ) 
     *   { 
     *     if ( !vcut1 ( loop ) ) { continue ; }
     *     if ( ! cut1 ( loop ) ) { continue ; }
     *     if ( loop->reFit( fitter ).isFailure() ) { continue ; }
     *     if ( !vcut2 ( loop ) ) { continue ; }
     *     if ( ! cut2 ( loop ) ) { continue ; }
     *     loop->save ( tag ) ;
     *   }
     *  loop->restore() ;
     *  return selected ( tag ) ;
     * }
     */
    LoKi::Types::Range pattern
    ( const std::string&        tag    , 
      const LoKi::Loop&         loop   ,
      const LoKi::Types::Cuts&  cut1   , 
      const LoKi::Types::VCuts& vcut1  , 
      const IParticleReFitter*  fitter , 
      const LoKi::Types::Cuts&  cut2   , 
      const LoKi::Types::VCuts& vcut2  ) ;
    
    /** shortcut for following "standard" pattern:
     *  
     *  @code
     *  
     *  const std::string tag     = ... ; // "particle name 
     *  const std::string formula = ... ; // combiner formula
     *  const std::string pid     = ... ; // Particle ID 
     *  const Cut         cut     = ... ; // particle cuts 
     *  const Vcut        vcut    = ... ; // vertex   cuts 
     *  const IParticleCombiner* combiner = ... ; // use this combiner (if valid)
     *  const IParticleReFitter* fitter   = ... ; // use this refitter (if valid)
     *
     *  for ( Loop L = loop( formula , pid , combiner ) ; L ; ++L )
     *   {
     *     if ( 0 != fitter && L->reFit( fitter  ).isFailure() ) { continue ; }
     *     if ( !vcut ( L )  ) { continue ; }     
     *     if ( !cut  ( L )  ) { continue ; }     
     *     L->save ( tag ) ;
     *   }
     *  Range result = selected( tag ) ;
     *  @endcode 
     * 
     *  These lines are equivalent to one call of "pattern":
     *
     *  @code 
     *
     *  Range result = pattern ( tag , formula, pid , cut , vcut , combiner , fitter ) ;
     
     *  @endcode 
     *
     */
    LoKi::Types::Range pattern 
    ( const std::string&        tag          , 
      const std::string&        formula      ,
      const std::string&        pid          , 
      const LoKi::Types::Cuts&  cut          , 
      const LoKi::Types::VCuts& vcut         , 
      const IParticleCombiner*  combiner = 0 , 
      const IParticleReFitter*  fitter   = 0 ) 
    {
      return pattern ( tag , loop ( formula , pid , combiner ) , 
                       cut , vcut , fitter ) ;
    } ;

    /** shortcut for following "standard" pattern:
     *  
     *  @code
     *  
     *  const std::string tag     = ... ; // "particle name 
     *  const std::string formula = ... ; // combiner formula
     *  const std::string pid     = ... ; // Particle ID 
     *  const Cut         cut1    = ... ; // particle cuts 
     *  const Vcut        vcut1   = ... ; // vertex   cuts 
     *  const IParticleReFitter* fitter   = ... ; // use this refitter (if valid)
     *  const Cut         cut2    = ... ; // particle cuts 
     *  const Vcut        vcut2   = ... ; // vertex   cuts 
     *  const IParticleCombiner* combiner = ... ; // use this combiner (if valid)
     *
     *  for ( Loop L = loop( formula , pid , combiner ) ; L ; ++L )
     *   {
     *     if ( !vcut1 ( L )  ) { continue ; }     
     *     if ( !cut1  ( L )  ) { continue ; }     
     *     if ( L->reFit( fitter  ).isFailure() ) { continue ; }
     *     if ( !vcut2 ( L )  ) { continue ; }     
     *     if ( !cut2  ( L )  ) { continue ; }     
     *     L->save ( tag ) ;
     *   }
     *  Range result = selected( tag ) ;
     *  @endcode 
     * 
     *  These lines are equivalent to one call of "pattern":
     *
     *  @code 
     *
     *  Range result = pattern ( tag    , formula, pid , 
     *                           cut1   , vcut1 ,
     *                           fitter , 
     *                           cut2   , vcut2 , 
     *                           combiner ) ;
     *  @endcode 
     *
     */
    LoKi::Types::Range pattern 
    ( const std::string&        tag          , 
      const std::string&        formula      ,
      const std::string&        pid          , 
      const LoKi::Types::Cuts&  cut1         , 
      const LoKi::Types::VCuts& vcut1        , 
      const IParticleReFitter*  fitter       ,
      const LoKi::Types::Cuts&  cut2         , 
      const LoKi::Types::VCuts& vcut2        , 
      const IParticleCombiner*  combiner = 0 ) 
    {
      return pattern ( tag  , loop ( formula , pid , combiner ) , 
                       cut1 , vcut1 , fitter , cut2 , vcut2 ) ;
    } ;
    
    /** shortcut for following "standard" pattern:
     *  
     *  @code
     *  
     *  const std::string tag     = ... ; // "particle name 
     *  const std::string formula = ... ; // combiner formula
     *  const std::string pid     = ... ; // Particle ID 
     *  const Cut         cut     = ... ; // particle cuts 
     *  const Vcut        vcut    = ... ; // vertex   cuts 
     *  const IParticleCombiner* combiner = ... ; // use this combiner (if valid)
     *  const IParticleReFitter* fitter   = ... ; // use this refitter (if valid)
     *
     *  for ( Loop L = loop( formula , pid , combiner ) ; L ; ++L )
     *   {
     *     if ( 0 != fitter && L->reFit( fitter  ).isFailure() ) { continue ; }
     *     if ( !vcut ( L )  ) { continue ; }     
     *     if ( !cut  ( L )  ) { continue ; }     
     *     L->save ( tag ) ;
     *   }
     *  Range result = selected( tag ) ;
     *  @endcode 
     * 
     *  These lines are equivalent to one call of "pattern":
     *
     *  @code 
     *
     *  Range result = pattern ( tag , formula, pid , cut , vcut , combiner , fitter ) ;
     
     *  @endcode 
     *
     */
    LoKi::Types::Range pattern 
    ( const std::string&            tag          , 
      const LoKi::Types::RangeList& formula      ,
      const std::string&            pid          , 
      const LoKi::Types::Cuts&      cut          , 
      const LoKi::Types::VCuts&     vcut         , 
      const IParticleCombiner*      combiner = 0 , 
      const IParticleReFitter*      fitter   = 0 ) 
    {
      return pattern ( tag , loop ( formula , pid , combiner ) , 
                       cut , vcut , fitter ) ;
    } ;

    /** shortcut for following "standard" pattern:
     *  
     *  @code
     *  
     *  const std::string tag     = ... ; // "particle name 
     *  const RangeList   formula = ... ; // combiner formula
     *  const std::string pid     = ... ; // Particle ID 
     *  const Cut         cut1    = ... ; // particle cuts 
     *  const Vcut        vcut1   = ... ; // vertex   cuts 
     *  const IParticleReFitter* fitter   = ... ; // use this refitter (if valid)
     *  const Cut         cut2    = ... ; // particle cuts 
     *  const Vcut        vcut2   = ... ; // vertex   cuts 
     *  const IParticleCombiner* combiner = ... ; // use this combiner (if valid)
     *
     *  for ( Loop L = loop( formula , pid , combiner ) ; L ; ++L )
     *   {
     *     if ( !vcut1 ( L )  ) { continue ; }     
     *     if ( !cut1  ( L )  ) { continue ; }     
     *     if ( L->reFit( fitter  ).isFailure() ) { continue ; }
     *     if ( !vcut2 ( L )  ) { continue ; }     
     *     if ( !cut2  ( L )  ) { continue ; }     
     *     L->save ( tag ) ;
     *   }
     *  Range result = selected( tag ) ;
     *  @endcode 
     * 
     *  These lines are equivalent to one call of "pattern":
     *
     *  @code 
     *
     *  Range result = pattern ( tag    , formula, pid , 
     *                           cut1   , vcut1 ,
     *                           fitter , 
     *                           cut2   , vcut2 , 
     *                           combiner ) ;
     *  @endcode 
     *
     */
    LoKi::Types::Range pattern 
    ( const std::string&            tag          , 
      const LoKi::Types::RangeList& formula      ,
      const std::string&            pid          , 
      const LoKi::Types::Cuts&      cut1         , 
      const LoKi::Types::VCuts&     vcut1        , 
      const IParticleReFitter*      fitter       ,
      const LoKi::Types::Cuts&      cut2         , 
      const LoKi::Types::VCuts&     vcut2        , 
      const IParticleCombiner*      combiner = 0 ) 
    {
      return pattern ( tag  , loop ( formula , pid , combiner ) , 
                       cut1 , vcut1 , fitter , cut2 , vcut2 ) ;
    } ;

  public:
    
    /** save the particle  into LoKi storage
     *  The particle @c p will be saved to
     *  LoKi storage with the tag/name @v tag
     *  @param  tag particle tag (only for LoKi)
     *  @param  particle particle to be saved
     *  @return status code 
     */
    StatusCode  save 
    ( const std::string& tag      , 
      LHCb::Particle*    particle ) ;
  public:
    /// clear the internal LoKi storages 
    virtual StatusCode clear() ;
  public:
    /// helper method to get a proper ParticleProperty for the given name  
    const ParticleProperty* pid ( const std::string& name ) const 
    {
      const ParticleProperty* pp = ppSvc()->find( name ) ;
      if ( 0 == pp ) 
      { Error ( "pid('" + name + "') : invalid ParticleProperty!" ) ; }
      return pp ;
    } ;
  public:
    /// get the proper error reporter 
    const LoKi::IReporter* reporter ( const std::string& name = "" ) const ;
  public:
    /// initialize the algorithm 
    virtual StatusCode initialize () ;
    /// make the execution of the algorithm 
    virtual StatusCode execute    () ;
    /// perform the real analysis 
    virtual StatusCode analyse    () ;
    /// finalize the algorithm 
    virtual StatusCode finalize   () ;
  protected:
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    Algo
    ( const std::string& name , 
      ISvcLocator*       pSvc ) ;
    /// virtual and protected destructor 
    virtual ~Algo() ;
  private:
    // the default constructor is disabled 
    Algo () ;
    // copy constructor is disabled 
    Algo           ( const Algo& ) ;
    // the assignement is disabled 
    Algo& operator=( const Algo& ) ;
  private:
    // the actual storage of selected particles
    LoKi::PhysTypes::Selected  m_selected  ; ///< the actual storage of particles
    // the actual storage of selected vertices 
    LoKi::PhysTypes::VSelected m_vselected ; ///< the actual storage of vertices
  private:
    /// collection of "error reporters"
    mutable GaudiUtils::HashMap<std::string,
                                const LoKi::IReporter*> m_reporters ;
  } ;  
} ; // end of namespace LoKi
// ============================================================================
/** @def LOKI_ALGORITHM_BODY 
 *
 *  Simple macros to avoid the typing of "standard" header file for 
 *  arbitrary LoKi-based alrorithm: 
 * 
 *  @code 
 *  
 *  LOKI_ALGORITHM_BODY( BdJPsiPhiAlg );
 *
 *  @endcode 
 *  
 *  "Standard" LoKi-based algorithm contains only one essential 
 *   <tt> virtual StatusCode analyse() </tt> method
 * 
 *  This macros could be used directly in implementation (*.cpp) file 
 *  of the algorithm, there is no nesessity to put only 1 line with macro 
 *  into the separate header file 
 *
 *  One need to implement factory, constructor & destructor of algorithm
 *  and "analyse" method.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-18
 */
// ============================================================================
#define LOKI_ALGORITHM_BODY( ALGNAME )                                         \
class ALGNAME : public LoKi::Algo                                              \
{                                                                              \
  /** friend factory for instantiation      */                                 \
  friend class AlgFactory<ALGNAME>        ;                                    \
public:                                                                        \
  /** standard method for event analysis    */                                 \
  virtual StatusCode analyse  ()          ;                                    \
protected:                                                                     \
  /** standard constructor                  */                                 \
  ALGNAME( const std::string& name ,                                           \
             ISvcLocator*       svc  )    ;                                    \
  /** virtual destructor                    */                                 \
  virtual ~ALGNAME ()                   ;                                      \
private:                                                                       \
  /** default constructor  is private       */                                 \
  ALGNAME             ()                  ;                                    \
  /** copy constructor     is private       */                                 \
  ALGNAME             ( const ALGNAME & ) ;                                    \
  /** assignement operator is private       */                                 \
  ALGNAME & operator= ( const ALGNAME & ) ;                                    \
};
// ============================================================================
/** @def LOKI_ALGORITHM_IMPLEMENT 
 *
 *  Simple macros to avoid the typing of "standard" implementation 
 *  of mandatory algorithm methods (constructors, destructors and factories) 
 *  for arbitrary LoKi-based alrorithm: 
 * 
 *  @code 
 *  #include "LoKi/Macros.h"
 *  
 *  LOKI_ALGORITHM_BODY_IMPLEMENT( BdJPsiPhiAlg );
 *
 *  /// standard LoKi method for event analysis
 *  StatusCode BdJPsiPhiAlg::analyse() {
 *    return Print("analyse() method is invoked ", StatusCode::SUCCESS ); 
 *  };
 *
 *  @endcode 
 *
 *  One need to implement only "analyse" method.
 * 
 *  This macro could be easily combined with LOKI_ALGORITHM_BODY macro
 *
 *  @code 
 *  #include "LoKi/Macros.h"
 *  
 *  LOKI_ALGORITHM_BODY           ( BdJPsiPhiAlg ) ;
 *  LOKI_ALGORITHM_IMPLEMENTATION ( BdJPsiPhiAlg ) ;
 *
 *  /// standard LoKi method for event analysis
 *  StatusCode BdJPsiPhiAlg::analyse() {
 *    return Print("analyse() method is invoked ", StatusCode::SUCCESS ); 
 *  };
 *
 *  @endcode 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-18
 */
#define LOKI_ALGORITHM_IMPLEMENT( ALGNAME )                                    \
/* ======================================================================== */ \
/** Declaration of the Algorithm Factory                                    */ \
/* ======================================================================== */ \
DECLARE_ALGORITHM_FACTORY( ALGNAME ) ;                                         \
/* ======================================================================== */ \
/** Standard constructor                                                    */ \
/* ======================================================================== */ \
ALGNAME::ALGNAME ( const std::string& name  ,                             \
                   ISvcLocator*       svc   )                             \
: LoKi::Algo( name , svc ) {} ; /* constructor for base class */               \
/* ======================================================================== */ \
/** destructor (empty)                                                      */ \
/* ======================================================================== */ \
ALGNAME ::~ALGNAME () {};
// ============================================================================
/** @def LOKI_ALGORITHM_FULLIMPLEMENT 
 *
 *  Simple macros to avoid the typing of "standard" implementation 
 *  of mandatory algorithm methods (constructors, destructors and factories) 
 *  for arbitrary LoKi-based alrorithm: 
 * 
 *  @code 
 *
 *  #include "LoKi/Macros.h"
 *  
 *  LOKI_ALGORITHM_FULLIMPLEMENT( BdJPsiPhiAlg );
 *
 *  /// standard LoKi method for event analysis
 *  StatusCode BdJPsiPhiAlg::analyse() {
 *    return Print("analyse() method is invoked ", StatusCode::SUCCESS ); 
 *  };
 *
 *  @endcode 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-18
 */
// ============================================================================
#define LOKI_ALGORITHM_FULLIMPLEMENT(   ALGNAME   )                            \
        LOKI_ALGORITHM_BODY         (   ALGNAME   ) ;                          \
        LOKI_ALGORITHM_IMPLEMENT    (   ALGNAME   ) ;
// ============================================================================
/** @def LOKI_ALGORITHM
 * 
 *  The most advanced macro to avoid the typeing of "standard" implementation
 *  of all mandatory but tediouse and totoriouse lines and methods 
 *  (algorithsm bidy, constructors, destructors factories etc)
 *  
 *  @code 
 *  
 *  #include "Loki/Macros.h"
 *  
 *  LOKI_ALGORITHM( MyAlg )
 *  {
 *    using namespace LoKi                ;  
 *    using namespace LoKi::Cuts          ;  
 *    using namespace LoKi::Fits          ;
 *  
 *    /// some implementation of algorithm
 * 
 *   return StatusCode::SUCCESS ;
 *  };
 *
 *  @endcode 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-03-11
 *  
 */
// ============================================================================
#define LOKI_ALGORITHM(   ALGNAME   )              \
        LOKI_ALGORITHM_FULLIMPLEMENT ( ALGNAME ) ; \
        StatusCode ALGNAME::analyse()                      
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGO_H
// ============================================================================
