// $Id: CaloMCTools1.h,v 1.3 2007-03-30 06:55:54 cattanem Exp $
// ============================================================================
#ifndef EVENT_CALOMCTOOLS1_H 
#define EVENT_CALOMCTOOLS1_H 1
// include 
// from CaloEvent 
#include "GaudiKernel/HashMap.h"
#include "GaudiKernel/Hash.h"

/** @file CaloMCTools1.h
 *  few more utilities for extraction of MC information from 
 *  Calorimeter Event Object
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

namespace GaudiUtils 
{
  /** hashing function for LHCb::MCParticle object 
   *  it uses <tt>key</tt> method 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   */
  template <>
  struct Hash<const LHCb::MCParticle*>
    : public std::unary_function<const LHCb::MCParticle*,size_t>
  {
    // Needed to behave like VC++'s hash_compare
    enum { // parameters for hash table
      bucket_size = 4,    // 0 < bucket_size
      min_buckets = 8};   // min_buckets = 2 ^^ N, 0 < N
  
    size_t operator() ( const LHCb::MCParticle* p ) const
    { return 0 == p ? size_t ( 0 ) : size_t( p -> key() ) ; }

    inline bool operator() ( const LHCb::MCParticle* p1 , 
                             const LHCb::MCParticle* p2 ) const 
    { return m_less ( p1 , p2 ) ; }
    private:
      std::less<const LHCb::MCParticle*> m_less;
  };
} ;


namespace CaloMCTools 
{ 
  
  /** @class AllEnergyFromMCParticle CaloMCTools1.h Event/CaloMCTools1.h 
   *  
   * generic class/functor  to extract 
   * the ALL (active) energy deposition from given LHCb::MCParticle
   * 
   * For "general class" it returns  negative energy of 1 TeV 
   * only the "template specializations" produce 
   * reasonable results
   *
   * The specializations exist for
   *
   *   - class LHCb::MCCaloHit
   *   - class LHCb::MCCaloDigit
   *   - class MCCaloSensPlaneHit
   *   - class   CaloDigit
   *   - class   LHCb::CaloCluster
   *
   *  @see LHCb::MCParticle
   *  @see LHCb::MCCaloHit
   *  @see LHCb::MCCaloDigit
   *  @see MCCaloSensPlaneHit
   *  @see   CaloDigit
   *  @see   LHCb::CaloCluster
   *  @see   CaloHypo
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   */  
  template<class TYPE>
  struct AllEnergyFromMCParticle:
    public std::binary_function<const TYPE*,const LHCb::MCParticle*, double>
  {
    ///  the only one essential (and absolutely useless! :-)) ) method
    double operator() ( const TYPE*       /* object   */  , 
                        const LHCb::MCParticle* /* particle */  ) const 
    { return   -1. * Gaudi::Units::TeV ; };
  };
  // ==========================================================================
  
  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class LHCb::MCCaloHit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see LHCb::MCParticle
   *  @see LHCb::MCCaloHit
   *  @param hit      pointer to LHCb::MCCaloHit   object
   *  @param particle pointer to LHCb::MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<LHCb::MCCaloHit>::operator() 
    ( const LHCb::MCCaloHit*  hit      ,
      const LHCb::MCParticle* particle ) const 
  {
    // invalid arguments 
    if( 0 == hit || 0 == particle   ) { return        0   ; }
    // True particle!
    if( particle == hit->particle() ) { return hit -> activeE() ; }
    // avoid long names 
    typedef SmartRefVector<LHCb::MCVertex>   Vertices ;
    typedef SmartRefVector<LHCb::MCParticle> Products ;
    double energy = 0 ;
    const Vertices& vertices = particle->endVertices();
    // loop over all daugter particles 
    for( Vertices::const_iterator vertex = vertices.begin() ; 
         vertices.end() != vertex ; ++vertex )
      {
        const LHCb::MCVertex* mcv = *vertex ;
        if( 0 == mcv ) { continue ; }
        const Products& products = mcv->products();
        for( Products::const_iterator product = products.begin() ;
             products.end() != product ; ++product )
          {
            const LHCb::MCParticle* mcp = *product ;
            if( 0 == mcp  ) { continue ; }
            energy += (*this) ( hit , mcp );
          }
      }
    ///
    return energy ;
  };
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class LHCb::MCCaloDigit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see LHCb::MCParticle
   *  @see LHCb::MCCaloDigit
   *  @param digit    pointer to LHCb::MCCaloDigit object
   *  @param particle pointer to LHCb::MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<LHCb::MCCaloDigit>::operator() 
    ( const LHCb::MCCaloDigit* digit    ,
      const LHCb::MCParticle*  particle ) const 
  {
    // invalid arguments 
    if( 0 == digit || 0 == particle ) { return        0   ; }
    // create evaluator 
    AllEnergyFromMCParticle<LHCb::MCCaloHit> evaluator;
    double energy = 0.0 ;
    for( LHCb::MCCaloDigit::Hits::const_iterator hit = digit->hits().begin();
         digit->hits().end() != hit ; ++hit ) 
      {
	    const LHCb::MCCaloHit* mchit = *hit ;
        if( 0 == mchit ) { continue ; }
        // accumulate 
        energy += evaluator( mchit , particle );
      }
    // 
    return energy ;
  };
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class CaloDigit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see MCParticle
   *  @see   CaloDigit
   *  @param digit    pointer to CaloDigit object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<LHCb::CaloDigit>::operator() 
    ( const LHCb::CaloDigit*   digit    ,
      const LHCb::MCParticle*  particle ) const 
  {
    // invalid arguments 
    if( 0 == digit || 0 == particle ) { return 0 ; }
    // get MCTruth information 
    // temporary plug 
    if( 0 == digit->parent()        ) { return 0 ; }
    // get MC truth information
    const LHCb::MCCaloDigit* mcdigit = mcTruth<LHCb::MCCaloDigit>( digit );
    // truth is available?
    if( 0 == mcdigit                ) { return 0 ; }
    // create the evaluator 
    AllEnergyFromMCParticle<LHCb::MCCaloDigit> evaluator;
    // use evaluator 
    return evaluator( mcdigit , particle );
  };
  // ==========================================================================
  
  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class LHCb::CaloCluster
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see LHCb::MCParticle
   *  @see   LHCb::CaloCluster
   *  @param cluster  pointer to LHCb::CaloCluster object
   *  @param particle pointer to LHCb::MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<LHCb::CaloCluster>::operator() 
    ( const LHCb::CaloCluster* cluster  ,
      const LHCb::MCParticle*  particle ) const 
  {
    // invalid arguments 
    if( 0 == cluster || 0 == particle ) { return 0 ; }
    // empty clusters 
    if( cluster->entries().empty()    ) { return 0 ; }
    // create evaluator 
    AllEnergyFromMCParticle<LHCb::CaloDigit> evaluator;
    const LHCb::CaloCluster::Entries& entries = cluster->entries();
    double energy = 0.0 ;
    for( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ; 
         entries.end() != entry ; ++entry )
      {
        const LHCb::CaloDigit* digit = entry->digit() ;
        if( 0 == digit ) { continue ; }
        // use evaluatoir: accumulate energy 
        energy += evaluator( digit , particle ) ;  
      }
    return energy ;
  };
  // ==========================================================================
  
  /** the auxilalry type to keep all MC history for calo objects 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  typedef GaudiUtils::HashMap<const LHCb::MCParticle*,double> CaloMCMap ;

  /** The most efficient generic structure for extraction 
   *  of MC truth information for Calorimeter Event objects 
   *
   * For "general class" it returns  error code 
   * Only the "template specializations" produce 
   * reasonable results
   *
   * The specializations exist for
   *
   *   - class LHCb::MCCaloHit
   *   - class LHCb::MCCaloDigit
   *   - class   CaloDigit
   *   - class   LHCb::CaloCluster
   *
   *  @see MCParticle 
   *  @see LHCb::MCCaloHit 
   *  @see LHCb::MCCaloDigit
   *  @see   CaloDigit 
   *  @see   LHCb::CaloCluster 
   *
   *  @see   CaloMCTools::CaloMCMap  
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  template <class TYPE>
  class MCCaloHistory : 
    public std::unary_function<const TYPE*,StatusCode>
  {
  public:
    /** constructor from pointer to CaloMCMap object
     *  @param mcmap pointer to existing CaloMC Map object
     */
    MCCaloHistory ( CaloMCMap* mcmap ) : m_mcmap( mcmap ) {};
    
    /** the oly one essential method 
     *  indeed it is absolutely useless!
     *  @return status code 900 (always!)
     */
    inline StatusCode operator() ( const TYPE* /* object */ ) const 
    { return StatusCode( 900 ) ; } ;
    
    /** Accessor to history map
     *  @return pointer to history map 
     */
    inline CaloMCMap* mcmap() const { return m_mcmap ; }
    
  private:
    MCCaloHistory () ;      
  private:
    CaloMCMap* m_mcmap ;
  };
  
  /** The specialization of the most efficient generic 
   *  structure for extraction 
   *  of MC truth information for LHCb::MCCaloHit object
   *
   *  Error codes:
   *  <ul>
   *    <li> 
   *         901   invalid CaloMCTool::CaloMCMap object
   *    </li> 
   *    <li> 
   *         902   invalid LHCb::MCCaloHit  object
   *    </li> 
   *    <li> 
   *         903   invalid link to LHCb::MCParticle object
   *    </li> 
   *   </ul>
   *
   *  @param  hit pointer to LHCb::MCCaloHit object
   *  @return status code 
   *
   *  @see LHCb::MCCaloHit 
   *  @see CaloMCTools::CaloMCMap 
   *  
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  template<>
  inline StatusCode
  MCCaloHistory<LHCb::MCCaloHit>::operator() 
    ( const LHCb::MCCaloHit* hit ) const 
  {
    if( 0 == mcmap()  ) { return StatusCode ( 901 ) ; }     // RETURN 
    if( 0 == hit      ) { return StatusCode ( 902 ) ; }     // RETURN 
    
    // get MC particle from the hit 
    const LHCb::MCParticle* particle = hit -> particle() ;
    if( 0 == particle ) { return StatusCode ( 903 ) ; }     // RETURN 
    
    // add the energy to the MC history Map 
    (*mcmap())[ particle ] += hit -> activeE() ;
    
    return StatusCode::SUCCESS ;                            // RETURN    
  };
  
  
  /** The specialization of the most efficient generic 
   *  structure for extraction 
   *  of MC truth information for LHCb::MCCaloDigit object
   *
   *  Error codes:
   *  <ul>
   *    <li> 
   *         901   invalid CaloMCTool::CaloMCMap object
   *    </li> 
   *    <li> 
   *         904   invalid LHCb::MCCaloDigit  object
   *    </li> 
   *   </ul>
   *
   *  @param  digit pointer to LHCb::MCCaloDigit object
   *  @return status code 
   *
   *  @see LHCb::MCCaloDigit 
   *  @see CaloMCTools::CaloMCMap 
   *  
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  template<>
  inline StatusCode
  MCCaloHistory<LHCb::MCCaloDigit>::operator() 
    ( const LHCb::MCCaloDigit* digit ) const 
  {
    if( 0 == mcmap()  ) { return StatusCode  ( 901 ) ; }     // RETURN 
    if( 0 == digit    ) { return StatusCode  ( 904 ) ; }     // RETURN
    
    // get all hits 
    const LHCb::MCCaloDigit::Hits& hits = digit->hits() ;
    if( hits.empty()  ) { return StatusCode::SUCCESS ; }     // RETURN 
    
    // create the evaluator 
    MCCaloHistory<LHCb::MCCaloHit> evaluator( mcmap() ) ;
    
    // use it! 
    std::for_each( hits.begin() , hits.end() , evaluator ) ;
    
    return StatusCode::SUCCESS ;                             // RETURN 
  };

  /** The specialization of the most efficient generic 
   *  structure for extraction 
   *  of MC truth information for CaloDigit object
   *
   *  Error codes:
   *  <ul>
   *    <li> 
   *         901   invalid CaloMCTool::CaloMCMap object
   *    </li> 
   *    <li> 
   *         905   invalid CaloDigit  object
   *    </li> 
   *    <li> 
   *         906   invalid parent object
   *    </li> 
   *    <li> 
   *         907   invalid LHCb::MCCaloDigit object
   *    </li> 
   *   </ul>
   *
   *  @param  digit pointer to CaloDigit object
   *  @return status code 
   *
   *  @see CaloDigit 
   *  @see CaloMCTools::CaloMCMap 
   *  
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  template<>
  inline StatusCode
  MCCaloHistory<LHCb::CaloDigit>::operator() 
    ( const LHCb::CaloDigit* digit ) const 
  {
    if( 0 == mcmap()  ) { return StatusCode  ( 901 ) ; }     // RETURN 
    if( 0 == digit    ) { return StatusCode  ( 905 ) ; }     // RETURN
    
    // is it needed ?
    if( 0 == digit->parent() ) { return StatusCode ( 906 ) ; } // RETURN
    
    // get MC truth information
    const LHCb::MCCaloDigit* mcdigit = mcTruth<LHCb::MCCaloDigit>( digit );
    
    // truth is available? 
    if ( 0 == mcdigit  ) { return StatusCode ( 907 ) ; }      // RETURN
    
    // create the evaluator 
    MCCaloHistory<LHCb::MCCaloDigit> evaluator( mcmap() ) ;
    
    // use it!
    return evaluator( mcdigit );
  };

  
  /** The specialization of the most efficient generic 
   *  structure for extraction 
   *  of MC truth information for LHCb::CaloCluster object
   *
   *  Error codes:
   *  <ul>
   *    <li> 
   *         901   invalid CaloMCTool::CaloMCMap object
   *    </li> 
   *    <li> 
   *         908   invalid LHCb::CaloCluster  object
   *    </li> 
   *   </ul>
   *
   *  @param  cluster pointer to LHCb::CaloCluster object
   *  @return status code 
   *
   *  @see LHCb::CaloCluster
   *  @see CaloMCTools::CaloMCMap 
   *  
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  template<>
  inline StatusCode
  MCCaloHistory<LHCb::CaloCluster>::operator() 
    ( const LHCb::CaloCluster* cluster ) const 
  {
    if( 0 == mcmap()    ) { return StatusCode  ( 901 ) ; }     // RETURN 
    if( 0 == cluster    ) { return StatusCode  ( 908 ) ; }     // RETURN
    
    const LHCb::CaloCluster::Entries& entries = cluster->entries() ;
    
    if( entries.empty() ) { return StatusCode::SUCCESS ; }     // RETURN
    
    // create the evaluator 
    MCCaloHistory<LHCb::CaloDigit> evaluator( mcmap() ) ;
    // and use it! 
    for( LHCb::CaloCluster::Entries::const_iterator entry = entries.begin() ;
         entries.end() != entry ; ++entry ) 
      {
        const LHCb::CaloDigit* digit = entry->digit();
        if( 0 != digit ) { evaluator( digit ) ; }
      }
    
    return StatusCode::SUCCESS ;    
  };
  
  /** Auxillary function to update the CaloMCTools::CaloMCMap object 
   *  It adds the energy deposition from the given particle to 
   *  ALL parents
   *  @param mcp    pointer to LHCb::MCParticle
   *  @param energy nergy need to be added
   *  @param mcmap  map to be updated 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2003-04-01 
   */
  inline StatusCode updateCaloMCMap
  ( const LHCb::MCParticle* mcp      , 
    const double      energy   , 
    CaloMCMap&        mcmap    )
  {
    if( 0 == mcp ) { return StatusCode::FAILURE ; }
    
    const LHCb::MCParticle* particle = mcp ;
    while( 0 != particle ) 
      {
        const LHCb::MCVertex*  vertex    = particle -> originVertex () ;
        if( 0 == vertex   ) { return StatusCode::SUCCESS ; }
        particle                   = vertex   -> mother       () ;
        if( 0 == particle ) { return StatusCode::SUCCESS ; }
        
        mcmap[ particle ] += energy ;
      }
    
    return StatusCode::SUCCESS ;
  }
  
}; // end of namespace 

// ============================================================================
// The END 
// ============================================================================
#endif // EVENT_CALOMCTOOLS1_H
// ============================================================================
