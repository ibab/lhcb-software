// $Id$
// ============================================================================
#ifndef LOKI_PHYSEXTRACT_H 
#define LOKI_PHYSEXTRACT_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Objects.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
// ============================================================================
namespace LHCb
{
  class RichPID ;
  class MuonPID ;  
}
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
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-06 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Extract
  {    
    // ========================================================================
    /** simple function to extract recursively all protoparticles, 
     *  which contributes to a given particle object, 
     * 
     *  @code 
     *
     *  const LHCb::Particle* particle = ... ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *  // get all charged protoparticles for 
     *  // positively charged particles 
     *  LoKi::Extract::getProtoParticles_if 
     *        ( particle , std::back_inserter( PPs ) , Q > 0 ) ;
     *
     *  @endcode 
     *
     *  @param  particle pointer to Particle object
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *  @param  cut      predicate 
     *
     *  @return the updated position of output iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT getProtoParticles_if 
    ( const LHCb::Particle* particle , 
      OUTPUT                output   , 
      const PREDICATE&      cut      ) 
    {
      // trivial check 
      if ( NULL ==  particle ) { return output ; }            // RETURN      
      if ( cut ( particle ) ) 
      { 
        // get own protoparticle (the basic action)
        const LHCb::ProtoParticle* proto = particle->proto() ;
        if ( 0 != proto ) 
        {
          // ATTENTION! const_cast  is in use !
          *output = const_cast<LHCb::ProtoParticle*>( proto ) ; 
          ++output ;                                          // ADVANCE 
        } 
      }
      // loop over the children (RECURSION!)
      typedef SmartRefVector<LHCb::Particle> Daughters ;
      const Daughters& daugs = particle->daughters() ;      
      for ( Daughters::const_iterator idau = daugs.begin() ; 
            daugs.end() != idau ; ++idau ) 
      { output = getProtoParticles_if ( *idau , output , cut ) ; } // RECURSION
      //
      return output ;                                      // RETURN
    }
    // ========================================================================
    /** simple function to extract recursively all protoparticles, 
     *  which contributes to a given particle object, 
     * 
     *  @code 
     *
     *  const LHCb::Particle* particle = ... ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *  // get all charged protoparticles for 
     *  // positively charged particles 
     *  LoKi::Extract::getProtoParticles_if 
     *        ( particle , std::back_inserter( PPs ) , Q > 0 ) ;
     *
     *  @endcode 
     *
     *  @param  particle pointer to Particle object
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *  @param  cut      predicate 
     *
     *  @return the updated position of output iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     */
    template <class OUTPUT, class PREDICATE> 
    inline OUTPUT protoParticles_if 
    ( const LHCb::Particle* particle , 
      OUTPUT                output   , 
      const PREDICATE&      cut      ) 
    { return getProtoParticles_if ( particle , output , cut ) ; } 
    // ========================================================================
    /** simple function to extract (recursively) all protoparticles, 
     *  which contributes to particles in the sequences 
     * 
     *  @code 
     *
     *  // any sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *  
     *  LoKi::Extract::getProtoParticles_if  
     *      ( particles.begin ()         ,
     *        particles.end   ()         ,
     *        std::back_inserter( PPs ) , Q > 0 ) ;
     *
     *  @endcode 
     *
     *  @param  first    begin of sequence 
     *  @param  last     end of sequence 
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *  @param  cut      predicate 
     *  @return the updated posiiton of outptu iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    template <class PARTICLE,class OUTPUT,class PREDICATE>
    inline OUTPUT getProtoParticles_if
    ( PARTICLE         first  , 
      PARTICLE         last   , 
      OUTPUT           output , 
      const PREDICATE& cut    )
    {
      for ( ; first != last ; ++first ) 
      { output = getProtoParticles_if ( *first , output , cut ) ; }
      return  output ;
    }
    // ========================================================================
    /** simple function to extract recursively all protoparticles, 
     *  which contributes to a given particle object
     * 
     *  @code 
     *
     *  const LHCb::Particle* particle = ... ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *
     *  LoKi::Extract::getProtoParticles 
     *        ( particle , std::back_inserter( PPs ) ) ;
     *
     *  @endcode 
     *
     *  @param  particle pointer to Particle object
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *
     *  @return the updated position of output iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     */
    template <class OUTPUT> 
    inline OUTPUT getProtoParticles
    ( const LHCb::Particle* particle , 
      OUTPUT                output   ) 
    {
      return  getProtoParticles_if 
        ( particle , output , LoKi::Objects::_ALL_ ) ;
    } 
    // ========================================================================
    /** simple function to extract (recursively) all protoparticles, 
     *  which contributes to particles in the sequences 
     * 
     *  @code 
     *
     *  // any sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *  
     *  LoKi::Extract::protoParticles_if  
     *      ( particles.begin ()         ,
     *        particles.end   ()         ,
     *        std::back_inserter( PPs ) , Q > 0 ) ;
     *
     *  @endcode 
     *
     *  @param  first    begin of sequence 
     *  @param  last     end of sequence 
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *  @param  cut      predicate 
     *  @return the updated posiiton of outptu iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    template <class PARTICLE,class OUTPUT,class PREDICATE>
    inline OUTPUT protoParticles_if
    ( PARTICLE         first  , 
      PARTICLE         last   , 
      OUTPUT           output , 
      const PREDICATE& cut    )
    { return getProtoParticles_if ( first , last , output , cut ) ; }
    // ========================================================================
    /** simple function to extract (recursively) all protoparticles, 
     *  which contributes to particles in the sequences 
     * 
     *  @code 
     *
     *  // any sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *  
     *  LoKi::Extract::getProtoParticles 
     *      ( particles.begin ()         ,
     *        particles.end   ()         ,
     *        std::back_inserter( PPs ) ) ;
     *
     *  @endcode 
     *
     *  @param  first    begin of sequence 
     *  @param  last     end of sequence 
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *  @return the updated posiiton of outptu iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    template <class PARTICLE,class OUTPUT>
    inline OUTPUT getProtoParticles 
    ( PARTICLE   first  , 
      PARTICLE   last   , 
      OUTPUT     output )
    {
      return  getProtoParticles_if 
        ( first , last , output , LoKi::Objects::_ALL_ ) ;
    } 
    // ========================================================================
    /** simple function to extract recursively all protoparticles, 
     *  which contributes to a given particle object
     * 
     *  @code 
     *
     *  const LHCb::Particle* particle = ... ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *
     *  LoKi::Extract::protoParticles 
     *        ( particle , std::back_inserter( PPs ) ) ;
     *
     *  @endcode 
     *
     *  @param  particle pointer to Particle object
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *
     *  @return the updated position of output iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     */
    template <class OUTPUT> 
    inline OUTPUT protoParticles
    ( const LHCb::Particle* particle , 
      OUTPUT                output   ) 
    { return getProtoParticles ( particle , output ) ; }
    // ========================================================================
    /** simple function to extract (recursively) all protoparticles, 
     *  which contributes to particles in the sequences 
     * 
     *  @code 
     *
     *  // any sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  ;
     *
     *  std::vector<LHCb::ProtoParticle*> PPs ;
     *  
     *  LoKi::Extract::protoParticles 
     *      ( particles.begin ()         ,
     *        particles.end   ()         ,
     *        std::back_inserter( PPs ) ) ;
     *
     *  @endcode 
     *
     *  @param  first    begin of sequence 
     *  @param  last     end of sequence 
     *  @param  output   output iterator to container 
     *                   of pointers to ProtoParticle
     *  @return the updated position of the output iterator 
     *
     *  @date 2003-02-07
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     */
    template <class PARTICLE,class OUTPUT>
    inline OUTPUT protoParticles 
    ( PARTICLE   first  , 
      PARTICLE   last   , 
      OUTPUT     output )
    { return getProtoParticles ( first , last , output ) ; } 
    // ========================================================================
    template <class OUTPUT,class PREDICATE> 
    inline  OUTPUT getParticles 
    ( const LHCb::Particle*   particle , 
      OUTPUT            output   , 
      const PREDICATE&  cut      ) ;
    // ========================================================================
    template <class OUTPUT,class PREDICATE> 
    inline  OUTPUT getParticles 
    ( const SmartRef<LHCb::Particle>& particle , 
      OUTPUT            output   , 
      const PREDICATE&  cut      ) 
    {
      return getParticles ( particle.target() , output , cut ) ;
    }
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  const LHCb::Particle* particle  = ... ;
     *
     *  // all daughter particles 
     *  Ps              particles       ; 
     *  LoKi::Extract::getParticles
     *     ( particle , std::back_inserter ( particles ) , PALL   ) ;
     *  
     *  // all daughter photons  
     *  Ps photons ; 
     *  LoKi::Extract::getParticles
     *    ( particle  , std::back_inserter( particles ) , "gamma" = ID ) ;
     *   
     *  @endcode 
     *
     *  @see LoKi::Cuts::PALL
     *  @see LoKi::Cuts::ID 
     * 
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @param cut      cut to used for particle extraction
     *  @return the updated position of outptu iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT,class PREDICATE> 
    inline  OUTPUT getParticles 
    ( const LHCb::Particle*   particle , 
      OUTPUT            output   , 
      const PREDICATE&  cut      )
    {
      // trivial check
      if ( NULL == particle ) { return output ; }               // RETURN
      // fill and advance the output iterator 
      if ( cut ( particle ) ) 
      {
        // ATTENTION: const_cats is in use! 
        *output  = const_cast<LHCb::Particle*> ( particle ) ; 
        ++output ;                                              // ADVANCE 
      }
      // loop over the children (RECURSION!)
      typedef SmartRefVector<LHCb::Particle> Daughters ;
      const Daughters& daugs = particle->daughters() ;      
      for ( Daughters::const_iterator idau = daugs.begin() ; 
            daugs.end() != idau ; ++idau ) 
      { output = getParticles ( *idau , output , cut ) ; }           // RECURSION
      //
      return output ;
    }
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  // sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  = ... ;
     *
     *  // all daughter photons  
     *  Ps photons ; 
     *  LoKi::Extract::getParticles
     *    ( particles.begin()  ,  
     *      particles.end  ()  , 
     *      std::back_inserter( particles ) , "gamma" = ID ) ;
     *   
     *  @endcode 
     *
     *  @see LoKi::Cuts::ID 
     * 
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @param cut      cut to used for particle extraction
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class PARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT getParticles 
    ( PARTICLE         begin  , 
      PARTICLE         end    , 
      OUTPUT           output  , 
      const PREDICATE& cut     )
    {
      for ( ; begin != end ; ++begin ) 
      { output = getParticles ( *begin , output , cut ) ; }
      return output ;
    }
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  const LHCb::Particle* particle  = ... ;
     *
     *  // all daughter particles 
     *  Ps              particles       ; 
     *  LoKi::Extract::getParticles
     *     ( particle , std::back_inserter ( particles ) ) ;
     *  
     *  @endcode 
     *
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT> 
    inline  OUTPUT getParticles 
    ( const LHCb::Particle* particle , 
      OUTPUT                output   )
    { 
      return getParticles 
        ( particle , output , LoKi::Objects::_ALL_ ) ;
    }
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  // sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  = ... ;
     *
     *  // all daughter photons  
     *  Ps photons ; 
     *  LoKi::Extract::getParticles
     *    ( particles.begin()  ,  
     *      particles.end  ()  , 
     *      std::back_inserter( particles ) , "gamma" = ID ) ;
     *   
     *  @endcode 
     *
     *  @see LoKi::Cuts::ID 
     * 
     *  @param output   output iterator to container of pointer to Particle
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class PARTICLE , class OUTPUT>
    inline OUTPUT getParticles 
    ( PARTICLE         begin  , 
      PARTICLE         end    , 
      OUTPUT           output )
    { 
      return getParticles 
        ( begin , end , output , LoKi::Objects::_ALL_ ) ;
    } 
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  const LHCb::Particle* particle  = ... ;
     *
     *  // all daughter particles 
     *  Ps              particles       ; 
     *  LoKi::Extract::particles
     *     ( particle , std::back_inserter ( particles ) , PALL   ) ;
     *  
     *  // all daughter photons  
     *  Ps photons ; 
     *  LoKi::Extract::particles
     *    ( particle  , std::back_inserter( particles ) , "gamma" = ID ) ;
     *   
     *  @endcode 
     *
     *  @see LoKi::Cuts::PALL
     *  @see LoKi::Cuts::ID 
     * 
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @param cut      cut to used for particle extraction
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT,class PREDICATE> 
    inline  OUTPUT particles 
    ( const LHCb::Particle*   particle , 
      OUTPUT                  output   , 
      const PREDICATE&        cut      )
    { return getParticles ( particle , output , cut ) ; } 
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  // sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  = ... ;
     *
     *  // all daughter photons  
     *  Ps photons ; 
     *  LoKi::Extract::particles
     *    ( particles.begin()  ,  
     *      particles.end  ()  , 
     *      std::back_inserter( particles ) , "gamma" = ID ) ;
     *   
     *  @endcode 
     *
     *  @see LoKi::Cuts::ID 
     * 
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @param cut      cut to used for particle extraction
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class PARTICLE , class OUTPUT, class PREDICATE>
    inline OUTPUT particles 
    ( PARTICLE         begin  , 
      PARTICLE         end    , 
      OUTPUT           output  , 
      const PREDICATE& cut     )
    { return getParticles ( begin , end , output , cut ) ; } 
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  const LHCb::Particle* particle  = ... ;
     *
     *  // all daughter particles 
     *  Ps              particles       ; 
     *  LoKi::Extract::particles
     *     ( particle , std::back_inserter ( particles ) ) ;
     *  
     *  @endcode 
     *
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class OUTPUT> 
    inline  OUTPUT particles 
    ( const LHCb::Particle* particle , 
      OUTPUT                output   )
    { return getParticles ( particle , output ) ; } 
    // ========================================================================
    /** Simple function to extract recursively all reconstructed 
     *  particles that make contribution to the given particle. 
     *  Indeed it is just a flat representation of the decay tree.
     * 
     *  @code 
     *  
     *  std::vector<const LHCb::Particle*> Ps ;
     *
     *  // sequence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE particles  = ... ;
     *
     *  // all daughters
     *  Ps photons ; 
     *  LoKi::Extract::particles
     *    ( particles.begin()  ,  
     *      particles.end  ()  , 
     *      std::back_inserter( particles ) ) ;
     *   
     *  @endcode 
     *
     *  @param particle input pointer to Particle object
     *  @param output   output iterator to container of pointer to Particle
     *  @return the updated position of output iterator 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@iep.ru
     *  @date 2003-02-07
     */
    template <class PARTICLE , class OUTPUT>
    inline OUTPUT particles 
    ( PARTICLE         begin  , 
      PARTICLE         end    , 
      OUTPUT           output )
    { return getParticles ( begin , end , output ) ; } 
    // ========================================================================
    /** @struct Particle2ProtoParticle
     *  Helper trivial functor with coudl be used to 
     *  "convert" Particle into ProtoParticle or 
     *  even the sequence of Particle nto the seqience of protoparticles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    struct Particle2ProtoParticle
    {
      inline LHCb::ProtoParticle* operator() 
        ( const LHCb::Particle* p ) const 
      { return NULL != p ? const_cast<LHCb::ProtoParticle*> ( p->proto() ) : NULL ; }
    } ;
    // ========================================================================
    /** @struct Particle2Track
     *  Helper trivial functor with could be used to 
     *  "convert" Particle into Track or 
     *  even the sequence of Particle into the sequence of tracks 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-10
     */
    struct Particle2Track
    {
      Particle2Track() : m_eval() { }
      inline LHCb::Track* operator() ( const LHCb::Particle* p ) const 
      {
        LHCb::ProtoParticle* pp = m_eval ( p ) ;
        return ( NULL != pp ? const_cast<LHCb::Track*>( pp->track() ) : NULL );        
      } ;
    private :
      Particle2ProtoParticle m_eval ;
    } ;
    // ========================================================================
    /** Simple funtion to get recursively all tracks, which 
     *  makes contribution for a given particle 
     *  
     *  @code
     * 
     *  const LHCb::Particle* patricle = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::getTracks_if  
     *    ( particle , std::back_inserter( tracks ) , Q > 0 ) ;
     *   
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT, class PREDICATE>
    inline OUTPUT getTracks_if  
    ( const LHCb::Particle* particle , 
      OUTPUT                output   , 
      const PREDICATE&      cut      ) 
    {
      // trivial check 
      if ( NULL == particle ) { return output ; }                // RETURN 
      // get the protoparticles 
      typedef std::vector<const LHCb::ProtoParticle*> PPS;
      PPS protos ;
      getProtoParticles_if ( particle , std::back_inserter( protos ) , cut ) ;
      // get the tracks from protoparticles:
      for ( PPS::const_iterator iproto = protos.begin() ; 
            protos.end() != iproto ; ++iproto ) 
      {
        const LHCb::ProtoParticle* proto = *iproto ;
        if ( NULL == proto ) { continue ; }                    // CONITNUE 
        const LHCb::Track* track = proto->track() ;
        if ( NULL == track ) { continue ; }                    // CONTINUE 
        // ATTENTION: const_cast is in use! 
        *output = const_cast<LHCb::Track*> ( track ) ;
        ++output ;                                             // ADVANCE 
      }
      //
      return output ;                                          // RETURN 
    }
    // ========================================================================
    /** Simple funtion to get recursively all tracks, which 
     *  makes contribution for a given particle 
     *  
     *  @code
     * 
     *  const LHCb::Particle* patricle = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::getTracks  
     *    ( particle , std::back_inserter( tracks ) ) ;
     *   
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT>
    inline OUTPUT getTracks  
    ( const LHCb::Particle* particle , 
      OUTPUT                output   ) 
    {
      return getTracks_if 
        ( particle , output , LoKi::Objects::_ALL_ ) ;  
    }
    // ========================================================================
    /** Simple funtion to get recursively all tracks, which 
     *  makes contribution for a given particle 
     *  
     *  @code
     * 
     *  const LHCb::Particle* patricle = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::tracks_if  
     *    ( particle , std::back_inserter( tracks ) , Q > 0 ) ;
     *   
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT, class PREDICATE>
    inline OUTPUT tracks_if  
    ( const LHCb::Particle* particle , 
      OUTPUT                output   , 
      const PREDICATE&      cut      ) 
    { return getTracks_if ( particle , output , cut ) ; }
    // ========================================================================
    /** Simple funtion to get recursively all tracks, which 
     *  makes contribution for a given particle 
     *
     *  @code
     * 
     *  const LHCb::Particle* patricle = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::tracks 
     *    ( particle , std::back_inserter( tracks ) ) ;
     *   
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT, class PREDICATE>
    inline OUTPUT tracks  
    ( const LHCb::Particle* particle , 
      OUTPUT                output   ) 
    { return getTracks ( particle , output ) ; }
    // ========================================================================
    /** Simple function to get recursively all tracks, which 
     *  makes contribution for sequence of particles 
     *  
     *  @code
     * 
     *  // any squence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE patricles = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::getTracks_if  
     *    ( particles.begin() , 
     *      particles.end  () , 
     *      std::back_inserter( tracks ) , Q > 0 ) ;
     *   
     *  @endcode 
     * 
     *  @param first    'begin'-iterator for the sequence 
     *  @param last     'end'-iterator for the sequence 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class PARTICLE,class OUTPUT,class PREDICATE>
    inline OUTPUT getTracks_if  
    ( PARTICLE         first    ,
      PARTICLE         last     , 
      OUTPUT           output   , 
      const PREDICATE& cut      ) 
    {
      for ( ; first != last ; ++first ) 
      { output = getTracks_if ( *first , output , cut ) ; }
      return output ;                                          // RETURN 
    }
    // ========================================================================
    /** Simple function to get recursively all tracks, which 
     *  makes contribution for sequence of particles 
     *  
     *  @code
     * 
     *  // any squence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE patricles = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::getTracks
     *    ( particles.begin() , 
     *      particles.end  () , 
     *      std::back_inserter( tracks ) ) ;
     *   
     *  @endcode 
     * 
     *  @param first    'begin'-iterator for the sequence 
     *  @param last     'end'-iterator for the sequence 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class PARTICLE,class OUTPUT>
    inline OUTPUT getTracks  
    ( PARTICLE         first    ,
      PARTICLE         last     , 
      OUTPUT           output   ) 
    {
      return getTracks_if 
        ( first , last , output , LoKi::Objects::_ALL_ ) ;
    }
    // ========================================================================
    /** Simple function to get recursively all tracks, which 
     *  makes contribution for sequence of particles 
     *  
     *  @code
     * 
     *  // any squence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE patricles = ... ;
     *  
     *  std::vector<LHCb::Track*> tracks ;
     *
     *  LoKi::Extract::tracks_if  
     *    ( particles.begin() , 
     *      particles.end  () , 
     *      std::back_inserter( tracks ) , Q > 0 ) ;
     *   
     *  @endcode 
     * 
     *  @param first    'begin'-iterator for the sequence 
     *  @param last     'end'-iterator for the sequence 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class PARTICLE,class OUTPUT,class PREDICATE>
    inline OUTPUT tracks_if  
    ( PARTICLE         first    ,
      PARTICLE         last     , 
      OUTPUT           output   , 
      const PREDICATE& cut ) 
    { return getTracks_if ( first , last , output , cut ) ; }
    // ========================================================================
    /** Simple funtion to get recursively all tracks, which 
     *  makes contribution for a given particle 
     *  
     *  @code
     * 
     *  const LHCb::Particle* patricle = ... ;
     *  
     *  std::vetcor<LHCb::Track*> tr ;
     *
     *  LoKi::Extract::tracks 
     *    ( particle , std::back_inserter( tr ) ) ;
     *   
     *  @endcode 
     * 
     *  @param particle pointer to the particle 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class OUTPUT>
    inline OUTPUT tracks 
    ( const LHCb::Particle* particle , 
      OUTPUT                output   ) 
    { return getTracks ( particle , output ) ; }
    // ========================================================================
    /** simple function to get recursively all tracks, which 
     *  makes contribution for sequence of particles 
     *  
     *  @code
     * 
     *  // any squence of objects, convertible to "const LHCb::Particle*"
     *  SEQUENCE patricles = ... ;
     *  
     *  std::vector<LHCb::Track*> tr ;
     *
     *  LoKi::Extract::tracks 
     *    ( particles.begin() , 
     *      particles.end  () , 
     *      std::back_inserter( tr ) ) ;
     *   
     *  @endcode 
     * 
     *  @param first    'begin'-iterator for the sequence 
     *  @param last     'end'-iterator for the sequence 
     *  @param output   the output iterator 
     *  @return the updated position of the output iterator 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    template <class PARTICLE,class OUTPUT>
    inline OUTPUT tracks 
    ( PARTICLE first    ,
      PARTICLE last     , 
      OUTPUT   output   ) 
    { return getTracks ( first , last , output ) ; } 
    // ========================================================================
    /** The most trivial function to extract LHCb::MuonPID object from 
     *  the basic particle
     *  @see LHCb::MuonPID 
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    const LHCb::MuonPID* getMuon 
    ( const LHCb::Particle* particle ) ;
    // ========================================================================
    /** The most trivial function to extract LHCb::MuonPID object from 
     *  the basic particle
     *  @see LoKi::Extract::getMuon
     *  @see LHCb::MuonPID 
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    inline 
    const LHCb::MuonPID* muon ( const LHCb::Particle* particle ) 
    { return getMuon( particle ) ; }
    // ========================================================================
    /** The most trivial function to extract LHCb::RichPID object from 
     *  the basic particle
     *  @see LHCb::RichPID 
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    const LHCb::RichPID* getRich 
    ( const LHCb::Particle* particle ) ;
    // ========================================================================
    /** The most trivial function to extract LHCb::MuonPID object from 
     *  the basic particle
     *  @see LoKi::Extract::getRich
     *  @see LHCb::RichPID 
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    inline 
    const LHCb::RichPID* rich 
    ( const LHCb::Particle* particle ) 
    { return getRich( particle ) ; }
    // ========================================================================
    /** The simple function to extract (Recursively) all 
     *  LHCb::MuonPID objects from the tree 
     *
     *  Get all Muon PID information for muons from decay tree:
     *  @code 
     *  
     *  const LHCb::Particle Bs = ... ;
     *  std::vector<LHCb::MuonPID*> muonPIDs ;
     *  LoKi::Extract::getMuons
     *    ( Bs , std::back_inserter( muonPIDs ) , "mu+" == ABSID ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Extract::getProtoParticles_if 
     *  @see LoKi::Cuts::ABSID  
     *  @see LHCb::MuonPID 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *
     *  @param particle pointer to LHCb::Particle object
     *  @param output   the output iterator 
     *  @param predicate the predicate 
     *  @return the advanced position of output iterator 
     * 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    template <class OUTPUT,class PREDICATE>
    inline OUTPUT getMuons 
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    , 
      const PREDICATE&      predicate ) 
    {
      if ( NULL == particle ) { return output ; }            // RETURN 
      typedef std::vector<const LHCb::ProtoParticle*> PPS ;
      PPS pps;
      getProtoParticles_if 
        ( particle , std::back_inserter( pps ) , predicate ) ;
      for ( PPS::const_iterator ip = pps.begin() ; pps.end() != ip ; ++ip ) 
      {
        const LHCb::ProtoParticle* pp = *ip ;
        if ( NULL == pp   ) { continue ; }                   // CONTINUE 
        const LHCb::MuonPID* muon = pp->muonPID() ;
        if ( NULL == muon ) { continue ; }                   // CONTINUE 
        // ATTENTION: const_cast is in use! 
        *output = const_cast<LHCb::MuonPID*> ( muon ) ;
        ++output ;                                           // ADVANCE 
      } ;
      return output ;
    }
    // ========================================================================
    template <class OUTPUT>
    inline OUTPUT getMuons 
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    ) 
    { return getMuons ( particle , output , LoKi::Objects::_ALL_ ) ; }
    // ========================================================================
    /** The simple function to extract (Recursively) all 
     *  LHCb::MuonPID objects from the tree 
     *
     *  Get all Muon PID information for muons from decay tree:
     *  @code 
     *  
     *  const LHCb::Particle Bs = ... ;
     *  std::vector<LHCb::MuonPID*> muonPIDs ;
     *  LoKi::Extract::muons
     *    ( Bs , std::back_inserter( muonPIDs ) , "mu+" == ABSID ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Extract::getMuons
     *  @see LoKi::Cuts::ABSID  
     *  @see LHCb::MuonPID 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *
     *  @param particle pointer to LHCb::Particle object
     *  @param output   the output iterator 
     *  @param predicate the predicate 
     *  @return the advanced position of output iterator 
     * 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    template <class OUTPUT,class PREDICATE>
    inline OUTPUT muons 
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    , 
      const PREDICATE&      predicate ) 
    { return getMuons ( particle , output , predicate ) ; }
    // ========================================================================
    template <class OUTPUT>
    inline OUTPUT muons 
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    ) 
    { return getMuons ( particle , output ) ; }
    // ========================================================================
    /** The simple function to extract (Recursively) all 
     *  LHCb::RichPID objectd from the tree 
     *
     *  Get all Rich PID information for kaons from decay tree:
     *  @code 
     *  
     *  const LHCb::Particle Bs = ... ;
     *  std::vector<LHCb::RichPID*> muonPIDs ;
     *  LoKi::Extract::getRichs
     *    ( Bs , std::back_inserter( richPIDs ) , "K+" == ABSID ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Extract::getProtoParticles_if 
     *  @see LoKi::Cuts::ABSID  
     *  @see LHCb::RichPID 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *
     *  @param particle pointer to LHCb::Particle object
     *  @param output   the output iterator 
     *  @param predicate the predicate 
     *  @return the advanced position of output iterator 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    template <class OUTPUT,class PREDICATE>
    inline OUTPUT getRichs  
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    , 
      const PREDICATE&      predicate ) 
    {
      if ( NULL == particle ) { return output ; }            // RETURN 
      typedef std::vector<const LHCb::ProtoParticle*> PPS ;
      PPS pps;
      getProtoParticles_if 
        ( particle , std::back_inserter( pps ) , predicate ) ;
      for ( PPS::const_iterator ip = pps.begin() ; pps.end() != ip ; ++ip ) 
      {
        const LHCb::ProtoParticle* pp = *ip ;
        if ( NULL == pp   ) { continue ; }                   // CONTINUE 
        const LHCb::RichPID* rich = pp->richPID() ;
        if ( NULL == rich ) { continue ; }                   // CONTINUE 
        // ATTENTION: const_cast is in use! 
        *output = const_cast<LHCb::RichPID*> ( rich ) ;
        ++output ;                                           // ADVANCE 
      } ;
      return output ;
    }
    // ========================================================================
    template <class OUTPUT>
    inline OUTPUT getRichs  
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    ) 
    { return getRichs ( particle , output , LoKi::Objects::_ALL_ ) ; }
    // ========================================================================
    /** The simple function to extract (Recursively) all 
     *  LHCb::RichPID objectd from the tree 
     *
     *  Get all Rich PID information for kaons from decay tree:
     *  @code 
     *  
     *  const LHCb::Particle Bs = ... ;
     *  std::vector<LHCb::RichPID*> muonPIDs ;
     *  LoKi::Extract::richs
     *    ( Bs , std::back_inserter( richPIDs ) , "K+" == ABSID ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Extract::getRichs
     *  @see LoKi::Cuts::ABSID  
     *  @see LHCb::RichPID 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *
     *  @param particle pointer to LHCb::Particle object
     *  @param output   the output iterator 
     *  @param predicate the predicate 
     *  @return the advanced position of output iterator 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06 
     */
    template <class OUTPUT,class PREDICATE>
    inline OUTPUT richs  
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    , 
      const PREDICATE&      predicate ) 
    { return getRichs ( particle , output , predicate ) ; }
    // ========================================================================
    template <class OUTPUT>
    inline OUTPUT richs  
    ( const LHCb::Particle* particle  , 
      OUTPUT                output    ) 
    { return getRichs ( particle , output ) ; }
    // ========================================================================
  } // end of namespace LoKi::Extract
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSEXTRACT_H
// ============================================================================
