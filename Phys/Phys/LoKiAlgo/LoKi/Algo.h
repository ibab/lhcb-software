// $Id: Algo.h,v 1.1.1.1 2006-03-18 10:39:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_ALGO_H 
#define LOKI_ALGO_H 1
// ============================================================================
// Include files
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

  protected:
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    Algo ( const std::string& name , 
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
    
  } ;  
} ; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_ALGO_H
// ============================================================================
