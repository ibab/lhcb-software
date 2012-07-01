// $Id$
// ============================================================================
#ifndef LOKI_ALGO_H 
#define LOKI_ALGO_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciTupleAlgorithm.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Keeper.h"
#include "LoKi/UniqueKeeper.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/IDecay.h"
#include "LoKi/ImpParBase.h"
#include "LoKi/Interface.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/AlgoTypes.h"
#include "LoKi/Loop.h"
#include "LoKi/LoopObj.h"
// ============================================================================
namespace Decays { class Decay ; }
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
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-14 
 */
// ============================================================================
namespace LoKi 
{
  // ===========================================================================
  class LoopObj ;
  class Loop    ;
  // ==========================================================================
  /** @class Algo LoKi/Algo.h
   *  The basic "working horse" of the whole LoKi project
   *  @see DaVinciTupleAlgorithm
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-14
   */
  class GAUDI_API Algo : public DaVinciTupleAlgorithm
  {
    // ========================================================================
  public:    
    // ========================================================================
    /** 'Select' the particles to be used in local storage
     *  
     *  - The Particles are selected from the local storage
     *
     *  @code
     *
     *  Range kaons = select( "Kaons" , ABSID == "K+" && P > 5 * GeV );
     *
     *  @endcode
     *
     *  - The example illustrates a 'selection'/'filtering from
     *   the local particles, which are @c K+ or @c K- and have a
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
    // ========================================================================    
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
     *  @param name  name/tag assigned to the selected particles
     *  @param range input range from which particles should be selected
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
    } 
    // ========================================================================
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
     *  @c positive and @c negative
     *
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cont input container of particles 
     *  @param cuts cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select   
    ( const std::string&                 name  , 
      const LHCb::Particle::ConstVector& cont  , 
      const LoKi::Types::Cuts&           cuts  ) 
    { 
      return select ( name , cont.begin() , cont.end() , cuts ) ; 
    } 
    // ========================================================================    
    /** 'Select' the particles to be used in local storage
     * 
     *  - particles are selected from the container 
     *
     *  @code
     *
     *  const LHCb::Particle::Container* particles = ... ;
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
     *  @c positive and @c negative
     *
     *  @see LHCb::Particle
     *  @see LHCb::Particles
     *  @see LoKi::Types::Cuts
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cont input container of particles 
     *  @param cuts cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select   
    ( const std::string&               name  , 
      const LHCb::Particle::Container* cont  , 
      const LoKi::Types::Cuts&         cuts  )
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::Particle::Container* points to null, return empty range!" ) ;
        return LoKi::Types::Range() ;
      }
      return select ( name , cont->begin() , cont->end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the particles to be used in local storage
     * 
     *  - particles are selected from the container 
     *
     *  @code
     *
     *  const LHCb::Particle::Container* particles = ... ;
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
     *  @c positive and @c negative
     *
     *  @see LHCb::Particle
     *  @see LHCb::Particles
     *  @see LoKi::Types::Cuts
     *  @see LoKi::Cuts::Q
     *  @param name name/tag assigned to the selected particles
     *  @param cont input container of particles 
     *  @param cuts cut to be applied
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select   
    ( const std::string&               name  , 
      const LHCb::Particle::Selection* cont  , 
      const LoKi::Types::Cuts&         cuts  )
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::Particle::Selection* points to null, return empty range!" ) ;
        return LoKi::Types::Range() ;
      }
      return select ( name , cont->begin() , cont->end() , cuts ) ;
    }
    // ========================================================================
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
     *  @c positive and @c negative
     *
     *  @see LoKi::Types::Range
     *  @see LoKi::Types::Cuts
     *  @see LoKi::Cuts::Q
     *  @param name  name/tag assigned to the selected particles
     *  @param first begin of sequence of particles 
     *  @param last  last  of sequence of particles 
     *  @param cut   cut to be applied
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
    } 
    // ========================================================================
  public: // decay descriptors: 
    // ========================================================================
    /** 'Select' the particles using decay finder 
     * 
     *  @code
     *
     *  const Decays::IDecay::Finder& finder = ... ;
     *  
     *  Range d0 = select( "D0" , finder ) ;
     *
     *  @endcode
     *
     *  @see Decays::IDecay::Finder  
     *  @param name   (INPUT)  name/tag assigned to the selected particles
     *  @param finder (INPUT) the decay finder 
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select 
    ( const std::string&            tag    ,
      const Decays::IDecay::Finder& finder ) ;
    // ========================================================================
    /** 'Select' the particles using decay tree
     * 
     *  @code
     *
     *  const Decays::IDecay::iTree& tree = ... ;
     *  
     *  Range d0 = select( "D0" , tree ) ;
     *
     *  @endcode
     *
     *  @see Decays::IDecay::iTree  
     *  @param name   (INPUT)  name/tag assigned to the selected particles
     *  @param tree   (INPUT) the decay tree 
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select 
    ( const std::string&            tag    ,
      const Decays::IDecay::iTree&  tree   ) ;
    // ========================================================================
    /** 'Select' the particles using decay descriptor 
     * 
     *  @code
     *
     *  Range d0 = select( "D0" , "[ D0 -> K- pi+ ]CC") ;
     *
     *  @endcode
     *
     *  @param name       (INPUT)  name/tag assigned to the selected particles
     *  @param descriptor (INPUT) the decay descriptor 
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select 
    ( const std::string&            tag        ,
      const std::string&            descriptor ) ;
    // ========================================================================
    /** 'Sub-select' the particles using decay finder 
     * 
     *  @code
     *
     *  const Decays::IDecay::Finder& finder = ... ;
     *  const Range input = ... ;
     * 
     *  Range d0 = select( "D0" , input , finder ) ;
     *
     *  @endcode
     *
     *  @see Decays::IDecay::Finder  
     *  @param name   (INPUT) name/tag assigned to the selected particles
     *  @param input  (INPUT) the input particles 
     *  @param finder (INPUT) the decay finder 
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select 
    ( const std::string&            tag    ,
      const LoKi::Types::Range&     range  ,
      const Decays::IDecay::Finder& finder ) ;
    // ========================================================================
    /** 'Select' the particles using decay tree
     * 
     *  @code
     *
     *  const Decays::IDecay::iTree& tree = ... ;
     *  const Range  input = ... ;
     *
     *  Range d0 = select( "D0" , input , tree ) ;
     *
     *  @endcode
     *
     *  @see Decays::IDecay::iTree  
     *  @param name   (INPUT) name/tag assigned to the selected particles
     *  @param input  (INPUT) the input particles 
     *  @param tree   (INPUT) the decay tree 
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select 
    ( const std::string&            tag    ,
      const LoKi::Types::Range&     range  ,
      const Decays::IDecay::iTree&  tree   ) ;
    // ========================================================================
    /** 'Select' the particles using decay descriptor 
     * 
     *  @code
     *
     *  const Range  input = ... ;
     *  Range d0 = select( "D0" , input , "[ D0 -> K- pi+ ]CC") ;
     *
     *  @endcode
     *
     *  @param name       (INPUT)  name/tag assigned to the selected particles
     *  @param input      (INPUT) the input particles 
     *  @param descriptor (INPUT) the decay descriptor 
     *  @return selected range of particles
     */
    LoKi::Types::Range 
    select 
    ( const std::string&            tag        ,
      const LoKi::Types::Range&     range      ,
      const std::string&            descriptor ) ;
    // ========================================================================
  public: // vertices 
    // ========================================================================
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
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name  name/tag assigned to the selected vertices
     *  @param range range of vertices to be selected
     *  @param cut   cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange         
    vselect   
    ( const std::string&         name  ,
      const LoKi::Types::VRange& range ,
      const LoKi::Types::VCuts&  cut   ) 
    {
      return vselect ( name , range.begin() , range.end() , cut ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect    
    ( const std::string&                   name ,
      const LHCb::VertexBase::ConstVector& cont ,
      const LoKi::Types::VCuts&            cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&               name ,
      const LHCb::Vertex::ConstVector& cont ,
      const LoKi::Types::VCuts&        cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  (Vertices are selected from the "cont")
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&                   name ,
      const LHCb::RecVertex::ConstVector& cont ,
      const LoKi::Types::VCuts&            cuts ) 
    {
      return vselect ( name , cont.begin() , cont.end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&             name ,
      const LHCb::Vertex::Container* cont ,
      const LoKi::Types::VCuts&      cuts ) 
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::Vertex::Container* points to null, return empty range!" ) ;
        return LoKi::Types::VRange() ;
      }
      return vselect ( name , cont->begin() , cont->end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&             name ,
      const LHCb::Vertex::Selection* cont ,
      const LoKi::Types::VCuts&      cuts ) 
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::Vertex::Selection points to null, return empty range!" ) ;
        return LoKi::Types::VRange() ;
      }
      return vselect ( name , cont->begin() , cont->end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&                name ,
      const LHCb::RecVertex::Container* cont ,
      const LoKi::Types::VCuts&         cuts ) 
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::RecVertex::Container* points to null, return empty range!" ) ;
        return LoKi::Types::VRange() ;
      }
      return vselect ( name , cont->begin() , cont->end() , cuts ) ;
    } 
    // ========================================================================
    /** 'Select' the vertices to be used in local storage
     *  @param name name/tag assigned to the selected vertices
     *  @param cont container of vertices to be selected
     *  @param cuts cut to be applied
     *  @return selected range of vertices
     */
    LoKi::Types::VRange        
    vselect   
    ( const std::string&                name ,
      const LHCb::RecVertex::Selection* cont ,
      const LoKi::Types::VCuts&         cuts ) 
    {
      if ( 0 == cont ) 
      {
        Error ( "LHCb::RecVertex::Selection points to null, return empty range!" ) ;
        return LoKi::Types::VRange() ;
      }
      return vselect ( name , cont->begin() , cont->end() , cuts ) ;
    } 
    // ========================================================================
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
     *  @c positive and @c negative
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
    } 
    // ========================================================================
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
    } 
    // ========================================================================
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
    } 
    // ========================================================================
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
    // ========================================================================
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
    // ========================================================================
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
      const LHCb::ParticleProperty* pid      = 0 , 
      const IParticleCombiner*      combiner = 0 ) ;
    // ========================================================================
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
    // ========================================================================
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
    // ========================================================================
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
      const LHCb::ParticleProperty* pid      = 0 , 
      const IParticleCombiner*      combiner = 0 ) ;
    // ========================================================================
    /** Create the loop object from "decay"
     *  @see DaVinci::Decay
     *  @param decay the decay desctrptor
     *  @param combiner the combiner
     *  @return the valid looping-object
     */
    LoKi::Loop loop 
    ( const Decays::Decay&       decay        , 
      const IParticleCombiner* combiner = 0 ) ;
    // ========================================================================
  public:
    /** shortcut for the following symbolic expression:
     * 
     *  @code 
     * 
     * {
     *  loop->backup()  ;
     *  for ( ; loop ; ++loop ) 
     *   {
     *     // get the mass from the sum of LorentzVectors 
     *     const double mass = loop->mass() ;
     *     // apply a mass window 
     *     if ( low > mass || high > mass ) { continue ; } 
     *     // use the explicit refitter (if needed) 
     *     if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
     *     // apply the vertex cuts 
     *     if ( !vcut ( loop ) ) { continue ; }
     *     // apply other cuts cuts 
     *     if ( ! cut ( loop ) ) { continue ; }
     *     loop->save ( tag ) ;
     *   }
     *  loop->restore() ;
     *  return selected ( tag ) ;
     * } ;
     * 
     *  @endcode 
     *
     *  @param tag  the symbolic unique tag 
     *  @param loop the looping object itself 
     *  @param low  low edge for mass window 
     *  @param high high edge for mass window 
     *  @param cut  cut to be used for filtering 
     *  @param vcut vertex cut to be used for filtering 
     *  @param fitter refitter to be applied before cuts 
     *  @return the selected range of particles 
     */
    LoKi::Types::Range pattern
    ( const std::string&        tag        , 
      const LoKi::Loop&         loop       ,
      const double              low        , 
      const double              high       ,
      const LoKi::Types::Cuts&  cut        , 
      const LoKi::Types::VCuts& vcut       , 
      const IParticleReFitter*  fitter = 0 ) ;
    // ========================================================================
    /** shortcut for the following symbolic expression:
     * 
     *  @code 
     * 
     * {
     *  loop->backup()  ;
     *  for ( ; loop ; ++loop ) 
     *   {
     *     // get the mass from the sum of LorentzVectors 
     *     const double mass = loop->mass() ;
     *     // apply a mass window 
     *     if (  abs( mass - nominal ) > window ) { continue ; } 
     *     // use the explicit refitter (if needed) 
     *     if ( 0 != fitter && loop->reFit( fitter ).isFailure() ) { continue ; }
     *     // apply the vertex cuts 
     *     if ( !vcut ( loop ) ) { continue ; }
     *     // apply other cuts cuts 
     *     if ( ! cut ( loop ) ) { continue ; }
     *     loop->save ( tag ) ;
     *   }
     *  loop->restore() ;
     *  return selected ( tag ) ;
     * } ;
     * 
     *  @endcode 
     *
     *  @param tag  the symbolic unique tag 
     *  @param loop the looping object itself 
     *  @param window the width of the mass window 
     *  @param cut  cut to be used for filtering 
     *  @param vcut vertex cut to be used for filtering 
     *  @param fitter refitter to be applied before cuts 
     *  @return the selected range of particles 
     */
    LoKi::Types::Range pattern
    ( const std::string&        tag        , 
      const LoKi::Loop&         loop       ,
      const double              window     , 
      const LoKi::Types::Cuts&  cut        , 
      const LoKi::Types::VCuts& vcut       , 
      const IParticleReFitter*  fitter = 0 ) ;
    // ========================================================================
    /** shortcut for the following expression:
     *
     *  @code 
     *
     * { 
     *  loop->backup()  ;
     *  for ( ; loop ; ++loop ) 
     *   { 
     *     // get the mass from the sum of LorentzVectors 
     *     const double mass = loop->mass() ;
     *     // apply a mass window 
     *     if ( low > mass || high > mass ) { continue ; } 
     *     // apply vertex selection cuts  
     *     if ( !vcut1 ( loop ) ) { continue ; }
     *     // apply selection cuts 
     *     if ( ! cut1 ( loop ) ) { continue ; }
     *     // refit 
     *     if ( loop->reFit( fitter ).isFailure() ) { continue ; }
     *     // apply vertex selection cuts  
     *     if ( !vcut2 ( loop ) ) { continue ; }
     *     // apply selection cuts 
     *     if ( ! cut2 ( loop ) ) { continue ; }
     *     loop->save ( tag ) ;
     *   }
     *  loop->restore() ;
     *  return selected ( tag ) ;
     * }
     *
     *  @endcode 
     *
     *  @param tag   the symbolic unique tag 
     *  @param loop  the looping object itself 
     *  @param low   low edge for mass window 
     *  @param high  high edge for mass window 
     *  @param cut1  cut to be used for filtering before refit
     *  @param vcut1 vertex cut to be used for filtering before refitt
     *  @param fitter refitter to be applied before cuts 
     *  @param cut2  cut to be used for filtering after refit
     *  @param vcut2 vertex cut to be used for filtering after refitt
     *  @return the selected range of particles 
     */
    LoKi::Types::Range pattern
    ( const std::string&        tag    , 
      const LoKi::Loop&         loop   ,
      const double              low    , 
      const double              high   , 
      const LoKi::Types::Cuts&  cut1   , 
      const LoKi::Types::VCuts& vcut1  , 
      const IParticleReFitter*  fitter , 
      const LoKi::Types::Cuts&  cut2   , 
      const LoKi::Types::VCuts& vcut2  ) ;    
    // ========================================================================
    /** shortcut for the following expression:
     *
     *  @code 
     *
     * { 
     *  loop->backup()  ;
     *  for ( ; loop ; ++loop ) 
     *   { 
     *     // get the mass from the sum of LorentzVectors 
     *     const double mass = loop->mass() ;
     *     // apply a mass window 
     *     if ( low > mass || high > mass ) { continue ; } 
     *     // apply vertex selection cuts  
     *     if ( !vcut1 ( loop ) ) { continue ; }
     *     // apply selection cuts 
     *     if ( ! cut1 ( loop ) ) { continue ; }
     *     // refit 
     *     if ( loop->reFit( fitter ).isFailure() ) { continue ; }
     *     // apply vertex selection cuts  
     *     if ( !vcut2 ( loop ) ) { continue ; }
     *     // apply selection cuts 
     *     if ( ! cut2 ( loop ) ) { continue ; }
     *     loop->save ( tag ) ;
     *   }
     *  loop->restore() ;
     *  return selected ( tag ) ;
     * }
     *
     *  @endcode 
     *
     *  @param tag    the symbolic unique tag 
     *  @param loop   the looping object itself 
     *  @param window the width of the mass window around nominal mass 
     *  @param cut1  cut to be used for filtering before refit
     *  @param vcut1 vertex cut to be used for filtering before refitt
     *  @param fitter refitter to be applied before cuts 
     *  @param cut2  cut to be used for filtering after refit
     *  @param vcut2 vertex cut to be used for filtering after refitt
     *  @return the selected range of particles 
     */
    LoKi::Types::Range pattern
    ( const std::string&        tag    , 
      const LoKi::Loop&         loop   ,
      const double              window , 
      const LoKi::Types::Cuts&  cut1   , 
      const LoKi::Types::VCuts& vcut1  , 
      const IParticleReFitter*  fitter , 
      const LoKi::Types::Cuts&  cut2   , 
      const LoKi::Types::VCuts& vcut2  ) ;    
    // ========================================================================
  public:
    /** save the particle  into LoKi storage
     *  The particle @c p will be saved to
     *  LoKi storage with the tag/name @c tag
     *  @param  tag particle tag (only for LoKi)
     *  @param  particle particle to be saved
     *  @return status code 
     */
    StatusCode  save 
    ( const std::string& tag      , 
      LHCb::Particle*    particle ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// clear the internal LoKi storages 
    virtual StatusCode clear() ;
    // ========================================================================
  public:
    // ========================================================================
    /** get the helper "geometry" object
     *  (essentially it is a wrapper for IDistanceCalculator tool 
     *  @param  vertex (input) vertex to ve used in the configuration
     *  @param  nick nickname of IDistanceCalculatro tool 
     *  @return helper geometry object/tool 
     *  @see LoKi::Vertices::ImpParBase 
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see IDistanceCalculator
     */
    // ========================================================================
    LoKi::Vertices::ImpParBase geo 
    ( const LHCb::VertexBase* vertex = 0  , 
      const std::string&      nick   = "" ) const ;
    /** get the helper "geometry" object
     *  (essentially it is a wrapper for IDistanceCalculator tool 
     *  @param  point (input) point to be used in the configuration
     *  @param  nick nickname of IDistanceCalculatro tool 
     *  @return helper geometry object/tool 
     *  @see LoKi::Vertices::ImpParBase 
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see IDistanceCalculator
     */
    LoKi::Vertices::ImpParBase geo 
    ( const LoKi::Point3D& point      ,
      const std::string&   nick  = "" ) const ;
    // ========================================================================
    /** get the helper "geometry" object
     *  (essentially it is a wrapper for IDistanceCalculator tool 
     *  @param  vertex (input) vertex to ve used in the configuration
     *  @param  nick nickname of IDistanceCalculatro tool 
     *  @return helper geometry object/tool 
     *  @see LoKi::Vertices::ImpParBase 
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see IDistanceCalculator
     */
    inline LoKi::Vertices::ImpParBase point
    ( const LHCb::VertexBase* vertex    , 
      const std::string&      nick  = "") const 
    { return geo ( vertex , nick ) ; }
    // ========================================================================
    /** get the helper "geometry" object
     *  (essentially it is a wrapper for IDistanceCalculator tool 
     *  @param  point (input) point to be used in the configuration
     *  @param  nick nickname of IDistanceCalculatro tool 
     *  @return helper geometry object/tool 
     *  @see LoKi::Vertices::ImpParBase 
     *  @see LoKi::Vertices::ImpactParamTool
     *  @see IDistanceCalculator
     */
    inline LoKi::Vertices::ImpParBase point 
    ( const LoKi::Point3D& point       , 
      const std::string&    nick  = "" ) const 
    { return geo ( point , nick ) ; }
    // ========================================================================
  public:
    // ========================================================================
    /** get the value for cut 
     *  
     *  @code 
     * 
     *  Cut cut = P >  cutValue( "pMin") ;
     *
     *  @endcode 
     * 
     *  The cut value is  specified through the properties:
     *
     *  @code 
     *
     *   MyAlg.Cuts += { "pMin" : 5000 } ;
     *
     *  @endcode 
     *
     *  @param name the name for the cut value 
     *  @return the value of the cut 
     */
    inline double cutValue ( const std::string& name ) const 
    {
      CutValues::iterator ifind = m_cutValues.find( name ) ;
      if ( m_cutValues.end() == ifind ) 
      { Exception ( "The value is not specified for cut='" + name + "'" ) ; }
      return ifind->second ;
    } 
    // ========================================================================
    /** get the value for cut 
     *  
     *  @code 
     * 
     *  Cut cut = P >  cutValue( "pMin" , 5 * GeV ) ;
     *
     *  @endcode 
     *
     *  The cut value is  specified through the properties:
     *
     *  @code 
     *
     *   MyAlg.Cuts += { "pMin" : 5000. } ;
     *
     *  @endcode 
     *
     *  @param name the name for the cut value 
     *  @param value the defautl valeu to be used if the cut if not specified 
     *  @return the value of the cut 
     */
    inline  double cutValue ( const std::string& name  , 
                              const double       value ) const 
    {
      CutValues::iterator ifind = m_cutValues.find( name ) ;
      if ( m_cutValues.end() == ifind ) 
      {
        m_cutValues[ name ] = value ;
        debug() << " Add the cut/value pair: "
                << "'" << name << "'/" << value << endreq ;
        return value ;
      }
      return ifind->second ;
    } 
    // ========================================================================
  public:
    // ========================================================================
    /// static accessor to the current algorithm 
    static LoKi::Algo* currentAlgo    () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// static  setter  to the current algorithm 
    static LoKi::Algo* setCurrentAlgo ( LoKi::Algo* ) ;
    // ========================================================================
  protected:
    // ========================================================================
    class Lock
    {
      // ======================================================================
    public:
      // ======================================================================
      Lock ( LoKi::Algo* algo ) ;
      ~Lock() ;
      // ======================================================================
    private:
      // ======================================================================
      Lock() ;
      Lock( const Lock& ) ;
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Algo* m_old ;       
      // ======================================================================
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /// get the proper error reporter 
    const LoKi::IReporter* reporter ( const std::string& name = "" ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the algorithm 
    virtual StatusCode initialize () ;
    // ========================================================================
    /// make the execution of the algorithm 
    virtual StatusCode execute    () ;
    // ========================================================================
    /// perform the real analysis 
    virtual StatusCode analyse    () ;
    // ========================================================================
    /// finalize the algorithm 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    Algo
    ( const std::string& name , 
      ISvcLocator*       pSvc ) ;
    // ========================================================================
    /// virtual and protected destructor 
    virtual ~Algo() ;
    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled 
    Algo () ;
    // ========================================================================
    // copy constructor is disabled 
    Algo           ( const Algo& ) ;
    // ========================================================================
    // the assignement is disabled 
    Algo& operator=( const Algo& ) ;
    // ========================================================================
  private:
    // ========================================================================
    // the actual storage of selected particles
    LoKi::PhysTypes::Selected  m_selected  ; ///< the actual storage of particles
    // ========================================================================
    // the actual storage of selected vertices 
    LoKi::PhysTypes::VSelected m_vselected ; ///< the actual storage of vertices
    // ========================================================================
  private:
    // ========================================================================
    /// collection of "error reporters"
    mutable GaudiUtils::HashMap<std::string,
                                const LoKi::IReporter*> m_reporters ;
    // ========================================================================
  private:
    // ========================================================================
    /// the actual type of comtainer of cut values 
    typedef std::map<std::string,double> CutValues ;
    // ========================================================================
    /// local storage of cut values 
    mutable CutValues m_cutValues ;
    // ========================================================================
  private:
    // ========================================================================
    /// the factory for decay-trees 
    LoKi::Interface<Decays::IDecay> m_decay ; // the factory for decay-trees
    // ========================================================================
  private:
    // ========================================================================
    // the static pointer to current algorithm 
    static LoKi::Algo* s_currentAlgo ;
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                      end of namespace LoKi
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
#define LOKI_ALGORITHM_BODY( ALGNAME )                                        \
namespace LoKi                                                                \
{                                                                             \
  class ALGNAME : public LoKi::Algo                                           \
  {                                                                           \
    /** friend factory for instantiation      */                              \
    friend class AlgFactory<LoKi::ALGNAME>  ;                                 \
  public:                                                                     \
    /** standard method for event analysis    */                              \
    virtual StatusCode analyse  ()          ;                                 \
  protected:                                                                  \
    /** standard constructor                  */                              \
    ALGNAME( const std::string& name ,                                        \
               ISvcLocator*       svc  )    ;                                 \
    /** virtual destructor                    */                              \
    virtual ~ALGNAME ()                   ;                                   \
  private:                                                                    \
    /** default constructor  is private       */                              \
    ALGNAME             ()                  ;                                 \
    /** copy constructor     is private       */                              \
    ALGNAME             ( const ALGNAME & ) ;                                 \
    /** assignement operator is private       */                              \
    ALGNAME & operator= ( const ALGNAME & ) ;                                 \
  };                                                                          \
} // end of namespace LoKi 
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
 *  StatusCode LoKi::BdJPsiPhiAlg::analyse() 
 *  {
 *    return Print("analyse() method is invoked ", StatusCode::SUCCESS ); 
 *   }
 *
 *  @endcode 
 *
 *  One need to implement only "analyse" method.
 * 
 *  This macro could be easily combined with LOKI_ALGORITHM_BODY macro
 *
 *  @code 
 *
 *  #include "LoKi/Macros.h"
 *  
 *  LOKI_ALGORITHM_BODY           ( BdJPsiPhiAlg ) ;
 *  LOKI_ALGORITHM_IMPLEMENTATION ( BdJPsiPhiAlg ) ;
 *
 *  /// standard LoKi method for event analysis
 *  StatusCode LoKi::BdJPsiPhiAlg::analyse() 
 *  {
 *    return Print("analyse() method is invoked ", StatusCode::SUCCESS ); 
 *  }
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
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LoKi , ALGNAME ) ;                        \
/* ======================================================================== */ \
/** Standard constructor                                                    */ \
/* ======================================================================== */ \
LoKi::ALGNAME::ALGNAME ( const std::string& name  ,                            \
                         ISvcLocator*       svc   )                            \
: LoKi::Algo( name , svc ) {} ; /* constructor for base class */               \
/* ======================================================================== */ \
/** destructor (empty)                                                      */ \
/* ======================================================================== */ \
LoKi::ALGNAME ::~ALGNAME () {};
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
 *  StatusCode LoKi::BdJPsiPhiAlg::analyse() 
 *  {
 *    return Print("analyse() method is invoked ", StatusCode::SUCCESS ); 
 *  }
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
 *  #include "LoKi/Macros.h"
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
 *  }
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
        StatusCode LoKi::ALGNAME::analyse()                      
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGO_H
// ============================================================================
