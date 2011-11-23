// ============================================================================
#ifndef EVENT_CALOMCTOOLS_H 
#define EVENT_CALOMCTOOLS_H 1
// Include files
// Event 
#include "Event/MCParticle.h"
// Kernel 
#include "Event/MCTruth.h"
// GaudiKernel
#include "GaudiKernel/Map.h"
// CaloEvent/Event 
#include "Event/MCCaloHit.h"
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "CaloMCTools1.h"

/** @namespace CaloMCTools CaloMCTools.h Event/CaloMCTools.h
 *  
 *  Collection of useful methods/utilities/functors 
 *  for fast(inline) extraction of MC truth information 
 *  for Calo-object 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   31/10/2001
 */

namespace CaloMCTools
{
  // ==========================================================================
  /** check if the first particle is a "parent" of the second one  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @see MCParticle
   *  @param first   pointer to MCParticle object 
   *  @param second  pointer to MCParticle object 
   *  @return true if first particle is equivalent to the second one
   *               or it is a "parent" of the second one
   */
  inline bool isParent( const LHCb::MCParticle* first  , 
                        const LHCb::MCParticle* second )
  {
    // check arguments 
    if      ( 0 == first || 0 == second ) { return false ; }
    else if (      first      == second ) { return true  ; }
    // extract the vertex  
    const LHCb::MCVertex*   vertex = second->originVertex() ;
    // start the recursion  
    return ( 0 == vertex ) ? false : isParent( first , vertex->mother() );
  }
  // ==========================================================================

  // ==========================================================================
  /** @class EnergyFromMCParticle CaloMCTools.h Event/CaloMCTools.h 
   *  
   * generic class/functor  to extract 
   * the (active) energy deposition from given MCParticle
   * 
   * For "general class" it returns  negative energy of 1 TeV 
   * only the "template specializations" produce 
   * reasonable results
   *
   * The specializations exist for
   *
   *   - class MCCaloHit
   *   - class LHCb::MCCaloDigit
   *   - class MCCaloSensPlaneHit
   *   - class   CaloDigit
   *   - class   LHCb::CaloCluster
   *   - class   LHCb::CaloHypo
   *
   *  @see MCParticle
   *  @see MCCaloHit
   *  @see LHCb::MCCaloDigit
   *  @see MCCaloSensPlaneHit
   *  @see   CaloDigit
   *  @see   LHCb::CaloCluster
   *  @see   LHCb::CaloHypo
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   */
  template<class TYPE>
  struct EnergyFromMCParticle:
    public std::binary_function<const TYPE*,const LHCb::MCParticle*, double>
  {
    ///  the only one essential (and quite useless! :-)) ) method
    double operator() ( const TYPE*       /* object   */  , 
                        const LHCb::MCParticle* /* particle */  ) const 
    { return   -1. * Gaudi::Units::TeV ; };
  };
  // ==========================================================================

  
  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class EnergyFromMCParticle for class MCCaloHit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @see MCParticle
   *  @see MCCaloHit
   *  @param hit      pointer to MCCaloHit   object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the hit if the hit was  produced 
   *          by this particle or its ancestor   
   */
  template<> 
  inline double 
  EnergyFromMCParticle<LHCb::MCCaloHit>::operator() 
    ( const LHCb::MCCaloHit*  hit      ,
      const LHCb::MCParticle* particle ) const 
  {
    if( 0 == hit || 0 == particle ) { return 0 ; }
    return isParent( particle , hit->particle() ) ? hit->activeE() : 0.0 ; 
  }
  // ==========================================================================



  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class EnergyFromMCParticle for class LHCb::MCCaloDigit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @see MCParticle
   *  @see LHCb::MCCaloDigit
   *  @param digit    pointer to LHCb::MCCaloDigit object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the digit from teh given particle 
   *              or its dauhter particles 
   */
  template<> 
  inline double 
  EnergyFromMCParticle<LHCb::MCCaloDigit>::operator() 
    ( const LHCb::MCCaloDigit*  digit    ,
      const LHCb::MCParticle*   particle ) const 
  {
    // trivial checks 
    if( 0 == digit || 0 == particle ) { return 0 ; }
    // accumulate the energy from the hits 
    double energy = 0 ;
    EnergyFromMCParticle<LHCb::MCCaloHit> evaluator;   // the evaluator! 
    for( LHCb::MCCaloDigit::Hits::const_iterator ihit = digit->hits().begin() ;
         digit->hits().end() != ihit ; ++ihit ) 
      { energy += evaluator( *ihit ,  particle ) ; }
    ///
    return energy ;
  }
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class EnergyFromMCParticle for class CaloDigit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @see MCParticle
   *  @see CaloDigit
   *  @param digit    pointer to CaloDigit object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the digit from teh given particle 
   *              or its dauhter particles 
   */
  template<> 
  inline double 
  EnergyFromMCParticle<LHCb::CaloDigit>::operator() 
    ( const LHCb::CaloDigit*    digit    ,
      const LHCb::MCParticle*   particle ) const 
  {
    // trivial checks 
    if( 0 == digit || 0 == particle ) { return 0 ; }
    // temporary plug 
    if( 0 == digit->parent()        ) { return 0 ; }
    // get MC truth information
    const LHCb::MCCaloDigit* mcdigit = mcTruth<LHCb::MCCaloDigit>( digit );
    // truth is available?
    if( 0 == mcdigit ) { return 0 ; }
    // go to MC history! 
    EnergyFromMCParticle<LHCb::MCCaloDigit> evaluator;   // the evaluator! 
    return evaluator( mcdigit , particle );
  }
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class EnergyFromMCParticle for class LHCb::CaloCluster
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @see MCParticle
   *  @see LHCb::CaloCluster
   *  @param cluster  pointer to LHCb::CaloCluster object
   *  @param particle pointer to MCParticle  object 
   *  @return summed active energy of the digit from 
   *          the given particle or its dauhter particles 
   */
  template<> 
  inline double 
  EnergyFromMCParticle<LHCb::CaloCluster>::operator() 
    ( const LHCb::CaloCluster*  cluster  ,
      const LHCb::MCParticle*   particle ) const 
  {
    // trivial checks 
    if( 0 == cluster || 0 == particle ) { return 0 ; }
    // create the evaluator 
    EnergyFromMCParticle<LHCb::CaloDigit> evaluator;   // the evaluator! 
    // accumulate the energy 
    double energy = 0 ;
    const LHCb::CaloCluster::Entries& entries = cluster->entries();
    for( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
         entries.end() != entry ; ++entry )
      { energy += evaluator( entry->digit() , particle ); }
    //
    return energy;
  }
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class EnergyFromMCParticle for class LHCb::CaloHypo
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   * 
   *  @warning only energy from clusters are evaluated now! 
   *
   *  @see MCParticle
   *  @see LHCb::CaloHypo
   *
   *  @param hypo     pointer to LHCb::CaloHypo     object
   *  @param particle pointer to MCParticle  object 
   *  @return summed active energy of the digit from 
   *          the given particle or its dauhter particles 
   */
  template<> 
  inline double 
  EnergyFromMCParticle<LHCb::CaloHypo>::operator() 
    ( const LHCb::CaloHypo*     hypo     ,
      const LHCb::MCParticle*   particle ) const 
  {
    // trivial checks 
    if( 0 == hypo || 0 == particle ) { return 0 ; }
    // accumulate the energy 
    double energy = 0 ;
    // create the evaluator 
    EnergyFromMCParticle<LHCb::CaloCluster> evaluator1;   // the evaluator! 
    const LHCb::CaloHypo::Clusters& clusters = hypo->clusters();
    for( LHCb::CaloHypo::Clusters::const_iterator cluster = clusters.begin() ;
         clusters.end() != cluster ; ++cluster )
      { energy += evaluator1( *cluster , particle ); }
    //
    return energy;
  }
  // ==========================================================================
  
  // ==========================================================================
  /** 
   *  generic function  to extract 
   *  the (active) energy deposition from given MCParticle
   * 
   *  @see EnergyFromMCParticle
   *  @see MCParticle 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @param object   pointer to the object object
   *  @param particle pointer to MCParticle  object 
   *  @return summed active energy of the digit from 
   *          the given particle or its dauhter particles 
   */
  template<class TYPE>
  inline double 
  energyFromMCParticle( const TYPE*       object   , 
                        const LHCb::MCParticle* particle )
  {
    // create the actual evaluator 
    EnergyFromMCParticle<TYPE> evaluator;
    // use the created evaluator! 
    return evaluator( object , particle );
  }
  // ==========================================================================

  // ==========================================================================
  /** @class FromMCParticle CaloMCTools.h Event/CaloMCTools.h 
   * 
   *  simple predicate/functor to select useful objects, 
   *  which have the connection with given MCParticle 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   30/10/2001 
   */
  template <class TYPE>
  class FromMCParticle: public std::unary_function<const TYPE*,bool>
  {
  public:
    /** constructor 
     *  @param particle pointer to MCParticle
     *  @param threshold on active energy deposition 
     */
    FromMCParticle( const LHCb::MCParticle* particle     ,
                    const double      threshold = 0 )
      : m_particle  ( particle  ) 
      , m_evaluator (           ) 
      , m_threshold ( threshold )   {} ;
    /** the only essential method
     *  @param object pointer to the object
     *  @return true if energy deposition from given 
     *           MCparticle to this object exceeds 
     *           the given threshold 
     */
    inline bool operator() ( const TYPE* object ) const 
    {
      if( 0 == m_particle || 0 == object ) { return false ; }
      return m_threshold <  m_evaluator( object , m_particle );
    };
    ///
  private:
    ///
    const LHCb::MCParticle*          m_particle  ;
    EnergyFromMCParticle<TYPE> m_evaluator ;
    double                     m_threshold ;
    ///
  };
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of 
   *  templated class FromMCParticle for class LHCb::MCCaloDigit
   *
   *  @see FromMCParticle 
   *  @see EnergyFromMCParticle
   *  @see LHCb::MCCaloDigit
   *  @see MCParticle
   *
   *  simple predicate/functor to select useful objects, 
   *  which have the connection with given MCParticle 
   *  
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   30/10/2001 
   */
  template <>
  class FromMCParticle<LHCb::MCCaloDigit>: 
    public std::unary_function<const LHCb::MCCaloDigit*,bool>
  {
  public:
    /** constructor 
     *  @param particle pointer to MCParticle
     *  @param threshold on active energy deposition 
     */
    FromMCParticle( const LHCb::MCParticle* particle     ,
                    const double      threshold = 0 )
      : m_particle  ( particle  ) 
      , m_evaluator (           ) 
      , m_threshold ( threshold )   {} ;
    /** the only essential method
     *  @param digit pointer to the LHCb::MCCaloDigit
     *  @return true if energy deposition from given 
     *           MCParticle to this digit exceeds 
     *           the given threshold 
     */
    inline bool operator() ( const LHCb::MCCaloDigit* digit ) const 
    {
      // check arguments!
      if( 0 == m_particle || 0 == digit  ) { return false ; }
      // dractical speed up!!!
      if( digit->activeE() < m_threshold       ) { return false ; }
      // use general technique
      return m_threshold <  m_evaluator( digit , m_particle );
    };
    ///
  private:
    ///
    const LHCb::MCParticle*                 m_particle  ;
    EnergyFromMCParticle<LHCb::MCCaloDigit> m_evaluator ;
    double                            m_threshold ;
    ///
  };
  // ==========================================================================

  // ==========================================================================
  /** 
   *  simple function to select useful objects, 
   *  which have the connection with given MCParticle 
   *
   *  @see FromMCParticle
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   30/10/2001 
   *
   *  @param object pointer to the object
   *  @param particle pointer to MCParticle
   *  @param threshold on active energy deposition 
   *  @return true if energy deposition from given 
   *           MCparticle to this object exceeds 
   *           the given threshold 
   */
  template <class TYPE>
  inline bool 
  fromMCParticle ( const TYPE*       object         , 
                   const LHCb::MCParticle* particle       ,
                   const double      threshold  = 0 )
  {
    // create evaluator 
    FromMCParticle<TYPE> evaluator( particle , threshold );
    // use the evaluator 
    return evaluator( object );
  }
  // ==========================================================================
  
  // ==========================================================================
  /** useful type definition to return the particle found.
   *  The first element of the pair is the active energy deposition
   *  the second one is the pointer to MCParticle object
   */
  typedef std::pair<double,const LHCb::MCParticle*>  ParticlePair;
  // ==========================================================================
  
  // ==========================================================================
  /** @class LargestDeposition CaloMCTools.h Event/CaloMCTools.h
   *  
   *  Determine the largest energy deposition to teh caloriimeter object. 
   *  
   *  It returs empty pair (deposition,MCParticle) for generic classes
   *  The non-trivial specializations exist for
   *
   *    - class LHCb::MCCaloHit 
   *    - class LHCb::MCCaloDigit 
   *    - class   CaloDigit
   *    - class   LHCb::CaloCluster
   *    - class MCCaloSensPlaneHit
   *
   *    @see MCParticle 
   *    @see LHCb::MCCaloHit 
   *    @see LHCb::MCCaloDigit 
   *    @see   CaloDigit
   *    @see   LHCb::CaloCluster
   *    @see MCCaloSensPlaneHit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   */
  template <class TYPE>
  struct  LargestDeposition: 
    public std::unary_function<const TYPE*,ParticlePair>
  {
    /** the only one essential method 
     *  it is "trivial" for general class 
     */ 
    inline ParticlePair operator() ( const TYPE* /* obj */ ) const
    { return ParticlePair( -1 * Gaudi::Units::TeV, (const LHCb::MCParticle*) 0 ); };
    ///
  };
  // ==========================================================================
  

  // ==========================================================================
  /** The template specialisation of class LargestDeposition
   *  for class LHCb::MCCaloHit  
   *
   *  Determine the largest energy deposition to teh caloriimeter object. 
   *  
   *  @see MCParticle 
   *  @see LHCb::MCCaloHit 
   *  @see LargestDeposition
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *  
   *  @param hit  pointer to LHCb::MCCaloHit object 
   *  @return pair of <energy,particle> from this hit 
   */
  template<> 
  inline ParticlePair 
  LargestDeposition<LHCb::MCCaloHit>::operator() 
    ( const LHCb::MCCaloHit* hit ) const 
  {
    return ( 0 == hit ) ?  
      ParticlePair() : ParticlePair( hit->activeE() , hit->particle() ) ;
  }
  // ==========================================================================

  // ==========================================================================
  /** The template specialisation of class LargestDeposition
   *  for class LHCb::MCCaloDigit 
   *
   *  Determine the largest energy deposition to teh caloriimeter object. 
   *  
   *  @see MCParticle 
   *  @see LHCb::MCCaloHit 
   *  @see LHCb::MCCaloDigit 
   *  @see LargestDeposition
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @param  digit pointer to LHCb::MCCaloDigit object 
   *  @return pair of <energy,particle> from this hit 
   */
  template<> 
  inline ParticlePair 
  LargestDeposition<LHCb::MCCaloDigit>::operator() 
    ( const LHCb::MCCaloDigit* digit ) const 
  {
    // skip NULLs and trivial digits    
    if( 0 == digit || digit->hits().empty() ) { return ParticlePair(); }
    /// prepare the output 
    ParticlePair output( 0 , (const LHCb::MCParticle*) 0 );
    LargestDeposition<LHCb::MCCaloHit> evaluator;    
    const LHCb::MCCaloDigit::Hits& hits = digit->hits();
    for( LHCb::MCCaloDigit::Hits::const_iterator hit = hits.begin() ; 
         hits.end() != hit ; ++hit )
      {
        ParticlePair aux( evaluator( *hit ) );
        if( aux.first >= output.first ) { output = aux ; }
      }
    ///
    return output;  
  }
  
  // ==========================================================================
  /** The template specialisation of class LargestDeposition
   *  for class CaloDigit 
   *
   *  Determine the largest energy deposition to the calorimeter objects. 
   *  
   *  @see MCParticle 
   *  @see LHCb::MCCaloDigit
   *  @see CaloDigit
   *  @see LargestDeposition
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @param  digit  pointer to CaloDigit object 
   *  @return pair of <energy,particle> from this hit 
   */
  template<> 
  inline ParticlePair 
  LargestDeposition<LHCb::CaloDigit>::operator() 
    ( const LHCb::CaloDigit* digit ) const 
  {
    // skip nulls 
    if( 0 == digit           ) { return ParticlePair() ; }
    // temporary plug
    if( 0 == digit->parent() ) { return ParticlePair() ; }
    // extract MC truth information
    const LHCb::MCCaloDigit* mcdigit = mcTruth<LHCb::MCCaloDigit>( digit );
    if( 0 == mcdigit ) { return ParticlePair() ; }
    // create evaluator 
    LargestDeposition<LHCb::MCCaloDigit> evaluator;
    return evaluator( mcdigit );
  }
  // ==========================================================================
  
  // ==========================================================================
  /** The template specialisation of class LargestDeposition
   *  for class LHCb::CaloCluster 
   *
   *  Determine the largest energy deposition to the calorimeter objects. 
   *  
   *  @see MCParticle 
   *  @see LHCb::CaloCluster
   *  @see LargestDeposition
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *
   *  @param  cluster  pointer to LHCb::CaloCluster object 
   *  @return pair of <energy,particle> from this hit 
   */
  template<> 
  inline ParticlePair 
  LargestDeposition<LHCb::CaloCluster>::operator() 
    ( const LHCb::CaloCluster* cluster ) const 
  {
    // skip NULLs and trivial clusters 
    if( 0 == cluster || cluster->entries().empty() ) { return ParticlePair(); }
    // evaluator 
    LargestDeposition<LHCb::MCCaloHit> evaluator;
    // local map 
    typedef GaudiUtils::Map<const LHCb::MCParticle*,double> CaloMap ;
    CaloMap auxMap;
    // fill the local map with information 
    const LHCb::CaloCluster::Entries& entries = cluster->entries() ;
    for( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
         entries.end() != entry ; ++entry )
      {
        const LHCb::CaloDigit*     digit    = entry->digit  () ;
        if( 0 ==   digit               ) { continue ; }   //   CONTINUE !
        // temporary plug to avoid crash in mcTruth<> !
        if( 0 ==   digit->parent()     ) { continue ; }   //   CONTINUE !
        const LHCb::MCCaloDigit* mcdigit    = mcTruth<LHCb::MCCaloDigit>( digit );
        if( 0 == mcdigit )               { continue ; }   //   CONTINUE !
        // loop over all MC hits 
        const LHCb::MCCaloDigit::Hits& hits = mcdigit->hits () ;
        for( LHCb::MCCaloDigit::Hits::const_iterator hit = hits.begin() ;
             hits.end() != hit ; ++hit )
          {
            const ParticlePair auxPair( evaluator( *hit ) );
            auxMap[ auxPair.second ] += auxPair.first ;
          } // end of loop over hits in LHCb::MCCaloDigit         
      } // end of loop over Cluster entries 
    // find maximal deposition
    ParticlePair output;
    for( CaloMap::const_iterator imap = auxMap.begin() ; 
         imap != auxMap.end()  ; ++imap ) 
      { // select the maximal contribution
        if ( imap->second > output.first && 0 != imap->first ) 
          { output.first  = imap->second ; output.second = imap->first ; }
      }
    ///
    return output ;
  }
  // ==========================================================================


  // ==========================================================================
  /** 
   *  Determine the largest energy deposition to the calorimeter object. 
   *  
   *  @see LargestDeposition
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/10/2001
   *  @param  object  pointer to object 
   *  @return pair of <energy,particle> from this object 
   */
  template <class TYPE>
  inline ParticlePair 
  largestDeposition ( const TYPE* object )
  {
    // create the evaluator 
    LargestDeposition<TYPE> evaluator;
    // use it! 
    return evaluator( object );
  }
  
} // The end of namespace CaloMCTools 

// ============================================================================
#endif // EVENT_CALOMCTOOLS_H
// ============================================================================
