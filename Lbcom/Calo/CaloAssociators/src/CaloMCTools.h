// $Id: CaloMCTools.h,v 1.1 2002-06-26 19:03:57 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOMCTOOLS_H 
#define CALOMCTOOLS_H 1
// include 
// from CaloEvent 
#include "Event/CaloMCTools.h"

namespace CaloMCTools 
{ 
  
  /** @class AllEnergyFromMCParticle CaloMCTools.h Event/CaloMCTools.h 
   *  
   * generic class/functor  to extract 
   * the ALL (active) energy deposition from given MCParticle
   * 
   * For "general class" it returns  negative energy of 1 TeV 
   * only the "template specializations" produce 
   * reasonable results
   *
   * The specializations exist for
   *
   *   - class MCCaloHit
   *   - class MCCaloDigit
   *   - class MCCaloSensPlaneHit
   *   - class   CaloDigit
   *   - class   CaloCluster
   *
   *  @see MCParticle
   *  @see MCCaloHit
   *  @see MCCaloDigit
   *  @see MCCaloSensPlaneHit
   *  @see   CaloDigit
   *  @see   CaloCluster
   *  @see   CaloHypo
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   */  
  template<class TYPE>
  struct AllEnergyFromMCParticle:
    public std::binary_function<const TYPE*,const MCParticle*, double>
  {
    ///  the only one essential (and absolutely useless! :-)) ) method
    double operator() ( const TYPE*       /* object   */  , 
                        const MCParticle* /* particle */  ) const 
    { return   -1. * TeV ; };
  };
  // ==========================================================================
  
  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class MCCaloHit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see MCParticle
   *  @see MCCaloHit
   *  @param hit      pointer to MCCaloHit   object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<MCCaloHit>::operator() 
    ( const MCCaloHit*  hit      ,
      const MCParticle* particle ) const 
  {
    // invalid arguments 
    if( 0 == hit || 0 == particle   ) { return        0   ; }
    // True particle!
    if( particle == hit->particle() ) { return hit -> e() ; }
    // avoid long names 
    typedef SmartRefVector<MCVertex>   Vertices ;
    typedef SmartRefVector<MCParticle> Products ;
    double energy = 0 ;
    const Vertices& vertices = particle->endVertices();
    // loop over all daugter particles 
    for( Vertices::const_iterator vertex = vertices.begin() ; 
         vertices.end() != vertex ; ++vertex )
      {
        if( 0 == *vertex ) { continue ; }
        const Products& products = (*vertex)->products();
        for( Products::const_iterator product = products.begin() ;
             products.end() != product ; ++product )
          {
            if( 0 == *product ) { continue ; }
            energy += (*this) ( hit , *product );
          }
      }
    ///
    return energy ;
  };
  // ==========================================================================

  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class MCCaloDigit
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see MCParticle
   *  @see MCCaloDigit
   *  @param digit    pointer to MCCaloDigit object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<MCCaloDigit>::operator() 
    ( const MCCaloDigit* digit    ,
      const MCParticle*  particle ) const 
  {
    // invalid arguments 
    if( 0 == digit || 0 == particle ) { return        0   ; }
    // create evaluator 
    AllEnergyFromMCParticle<MCCaloHit> evaluator;
    double energy = 0.0 ;
    for( MCCaloDigit::Hits::const_iterator hit = digit->hits().begin();
         digit->hits().end() != hit ; ++hit ) 
      {
        if( 0 == *hit ) { continue ; }
        // accumulate 
        energy += evaluator( *hit , particle );
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
  AllEnergyFromMCParticle<CaloDigit>::operator() 
    ( const CaloDigit*   digit    ,
      const MCParticle*  particle ) const 
  {
    // invalid arguments 
    if( 0 == digit || 0 == particle ) { return 0 ; }
    // get MCTruth information 
    // temporary plug 
    if( 0 == digit->parent()        ) { return 0 ; }
    // get MC truth information
    const MCCaloDigit* mcdigit = mcTruth<MCCaloDigit>( digit );
    // truth is available?
    if( 0 == mcdigit                ) { return 0 ; }
    // create the evaluator 
    AllEnergyFromMCParticle<MCCaloDigit> evaluator;
    // use evaluator 
    return evaluator( mcdigit , particle );
  };
  // ==========================================================================
  
  // ==========================================================================
  /** The template specialization of main inline method of 
   *  templated class AllEnergyFromMCParticle for class CaloCluster
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   26/06/2002
   *
   *  @see MCParticle
   *  @see   CaloCluster
   *  @param cluster  pointer to CaloCluster object
   *  @param particle pointer to MCParticle  object 
   *  @return active energy of the hit if the 
   *          hit was  produced by this particle or its daughter 
   */
  template<> 
  inline double 
  AllEnergyFromMCParticle<CaloCluster>::operator() 
    ( const CaloCluster* cluster  ,
      const MCParticle*  particle ) const 
  {
    // invalid arguments 
    if( 0 == cluster || 0 == particle ) { return 0 ; }
    // empty clusters 
    if( cluster->entries().empty()    ) { return 0 ; }
    // create evaluator 
    AllEnergyFromMCParticle<CaloDigit> evaluator;
    const CaloCluster::Entries& entries = cluster->entries();
    double energy = 0.0 ;
    for( CaloCluster::Entries::const_iterator entry = entries.begin() ; 
         entries.end() != entry ; ++entry )
      {
        const CaloDigit* digit = entry->digit() ;
        if( 0 == digit ) { continue ; }
        // use evaluatoir: accumulate energy 
        energy += evaluator( digit , particle ) ;  
      }
    return energy ;
  };
  // ==========================================================================

}; // end of namespace 

// ============================================================================
// The END 
// ============================================================================
#endif // CALOMCTOOLS_H
// ============================================================================
