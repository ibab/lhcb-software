// $Id: Photons.h,v 1.1 2009-11-28 19:29:45 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHOTONS_H 
#define LOKI_PHOTONS_H 1
// ============================================================================
// Include files
// ============================================================================
// DigiEvent 
// ============================================================================
#include "Event/CaloDigit.h"
// ============================================================================
// RecEvent 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
// ============================================================================
// PhysEvent 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Photons
   *  Helper namespace with few utilities to deal with photons
   *  The functions have been originally developef for Kali : 
   *  Calorimeter Calibration framework 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-11-28
   */
  namespace Photons
  {
    // ========================================================================
    /** get the proto particle from  gamma
     *  @param gamma  (INPUT) the particle 
     *  @return protoparticle
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */
    inline const LHCb::ProtoParticle* protoP ( const LHCb::Particle* gamma ) 
    {
      if ( 0 == gamma ) { return 0 ; }
      return gamma->proto () ;
    }
    // ==========================================================================
    /** get the calo hypo from  gamma
     *  @param gamma  (INPUT) the particle 
     *  @returh hypo 
     *  @see LHCb::Particle
     *  @see LHCb::CaloHypo
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */
    inline const LHCb::CaloHypo*  hypo( const LHCb::Particle* gamma ) 
    {
      if ( 0 == gamma         ) { return 0 ; }
      const LHCb::ProtoParticle* pp = protoP ( gamma ) ;
      if ( 0 == pp            ) { return 0 ; }
      if ( pp->calo().empty() ) { return 0 ; }
      return (*(pp->calo().begin())) ;
    }
    // ========================================================================
    /** get the calo cluster from  gamma
    *  @param gamma  (INPUT) the particle 
     *  @returh cluster 
     *  @see LHCb::Particle
     *  @see LHCb::CaloCluster
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    inline const LHCb::CaloCluster* cluster ( const LHCb::Particle* gamma ) 
    {
      if ( 0 == gamma            ) { return 0 ; }
      const LHCb::CaloHypo* h = hypo( gamma ) ;
      if ( 0 == h                ) { return 0 ; }
      if ( h->clusters().empty() ) { return 0 ; }
      return (*(h->clusters().begin()));
    }
    // ========================================================================
    /** get the seed cell from  gamma
     *  @param gamma  (INPUT) the particle 
     *  @returh seed cell
     *  @see LHCb::Particle
     *  @see LHCb::CaloCellID
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    inline LHCb::CaloCellID cellID ( const LHCb::Particle* gamma ) 
    {
      if ( 0 == gamma  ) { return LHCb::CaloCellID() ; }
      const LHCb::CaloCluster* cl = cluster( gamma ) ;
      if ( 0 == cl     ) { return LHCb::CaloCellID() ; }
      return cl->seed() ;  
    }
    // ========================================================================
    /** get the energy from the given selector 
     *  @param gamma  (INPUT) the particle 
     *  @param from   (INPUT) digit selector 
     *  @return the energy
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    template <class FROM> 
    inline double energyFrom 
    ( const LHCb::Particle* gamma , 
      const FROM&           from  ) 
    {
      if ( 0 == gamma ) { return -1 * Gaudi::Units::TeV ; }
      const LHCb::CaloHypo* h = hypo( gamma ) ;
      if ( 0 == h     ) { return -1 * Gaudi::Units::TeV ; }
      double energy = 0 ;
      // collect energy from digits 
      typedef LHCb::CaloHypo::Digits Digits ;
      const Digits& digits = h->digits() ;
      for ( Digits::const_iterator idigit = digits.begin() ; 
            digits.end() != idigit ; ++idigit ) 
      {
        const LHCb::CaloDigit* digit = *idigit ;
        if ( 0 == digit     ) { continue ; }
        if ( from ( digit ) ) { energy += digit->e() ; }
      }
      // collect energy from clusters 
      typedef LHCb::CaloHypo::Clusters Clusters ;
      const Clusters& clusters = h->clusters() ;
      for ( Clusters::const_iterator icluster = clusters.begin() ; 
            clusters.end() != icluster ; ++icluster ) 
      {
        const LHCb::CaloCluster* cluster = *icluster ;
        if ( 0 == cluster ) { continue ; }
        //
        typedef  std::vector<LHCb::CaloClusterEntry> Entries ;
        const Entries& entries = cluster->entries() ;    
        for ( Entries::const_iterator ientry = entries.begin() ; 
              entries.end() != ientry ; ++ientry )
        {
          const LHCb::CaloDigit* digit = ientry->digit() ;
          if ( 0 == digit     ) { continue ; }
          if ( from ( digit ) ) { energy += digit->e() ; }
        }
      }
      //
      return energy ;
    }
    // ========================================================================
    /** get the energy from the given selector 
     *  @param gamma  (INPUT) the particle 
     *  @param calo   (INPUT) the calorimeetr name  
     *  @return the energy
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    double energyFrom 
    ( const LHCb::Particle* gamma , 
      const std::string&    calo  ) ;
    // ========================================================================
    /** get the seed energy from the given selector 
     *  @param gamma  (INPUT) the particle 
     *  @param from   (INPUT) digit selector 
     *  @return the energy
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    template <class FROM> 
    inline double seedEnergyFrom 
    ( const LHCb::Particle* gamma , 
      const FROM&           from  ) 
    {
      if ( 0 == gamma ) { return -1 * Gaudi::Units::TeV ; }
      const LHCb::CaloHypo*    h = hypo    ( gamma ) ;
      if ( 0 == h     ) { return -1 * Gaudi::Units::TeV ; }
      const LHCb::CaloCluster* c = cluster ( gamma ) ;
      if ( 0 == c     ) { return -1 * Gaudi::Units::TeV ; }
      //
      double energy = 0 ;
      typedef LHCb::CaloHypo::Digits Digits ;
      const Digits& digits = h->digits() ;
      for ( Digits::const_iterator idigit = digits.begin() ; 
            digits.end() != idigit ; ++idigit ) 
      {
        const LHCb::CaloDigit* digit = *idigit ;
        if ( 0 == digit    ) { continue ; }
        if ( c->seed().index() != digit->cellID().index() ) { continue ; }
        if ( from( digit ) ) { energy += digit->e() ; }
      }
      // collect energy from clusters 
      typedef LHCb::CaloHypo::Clusters Clusters ;
      const Clusters& clusters = h->clusters() ;
      for ( Clusters::const_iterator icluster = clusters.begin() ; 
            clusters.end() != icluster ; ++icluster ) 
      {
        const LHCb::CaloCluster* cluster = *icluster ;
        if ( 0 == cluster ) { continue ; }
        //
        typedef  std::vector<LHCb::CaloClusterEntry> Entries ;
        const Entries& entries = cluster->entries() ;    
        for ( Entries::const_iterator ientry = entries.begin() ; 
              entries.end() != ientry ; ++ientry )
        {
          const LHCb::CaloDigit* digit = ientry->digit() ;
          if ( 0 == digit     ) { continue ; }
          if ( c->seed().index() != digit->cellID().index() ) { continue ; }
          if ( from ( digit ) ) { energy += digit->e() ; }
        }
      }
      //
      return energy ;
    }
    // ========================================================================
    /** get the seed energy from the given selector 
     *  @param gamma  (INPUT) the particle 
     *  @param calo   (INPUT) the calorimeetr name  
     *  @return the energy
     *  @see LHCb::Particle
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    double seedEnergyFrom 
    ( const LHCb::Particle* gamma , 
      const std::string&    calo  ) ;
    // ========================================================================
    /** get all digits from the given cluster 
     *  @param cluster (INPUT)  the cluster 
     *  @param digits  (OUTPUT) the digits 
     *  @return number of added digits 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    size_t getDigits 
    ( const LHCb::CaloCluster* cluster , 
      LHCb::CaloDigit::Set&    digits  ) ;
    // ========================================================================
    /** get all digits from the given cluster 
     *  @param cluster (INPUT)  the cluster 
     *  @param digits  (OUTPUT) the digits 
     *  @return number of added digits 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    size_t getDigits 
    ( const LHCb::CaloCluster* cluster , 
      LHCb::CaloDigit::Vector& digits  ) ;    
    // ========================================================================
    /** get all digits from the given hypo
     *  @param hypo (INPUT)  the hypo
     *  @param digits  (OUTPUT) the digits 
     *  @return number of added digits 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    size_t getDigits 
    ( const LHCb::CaloHypo* hypo    , 
      LHCb::CaloDigit::Set& digits  ) ;
    // ========================================================================
    /** get all digits from the given hypo
     *  @param hypo (INPUT)  the hypo
     *  @param digits  (OUTPUT) the digits 
     *  @return number of added digits 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    size_t getDigits 
    ( const LHCb::CaloHypo*    hypo    , 
      LHCb::CaloDigit::Vector& digits  ) ;
    // ========================================================================
    /** get all digits from the given gamma
     *  @param hypo (INPUT)  gamma
     *  @param digits  (OUTPUT) the digits 
     *  @return number of added digits 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    size_t getDigits 
    ( const LHCb::Particle* gamma   , 
      LHCb::CaloDigit::Set& digits  ) ;
    // ========================================================================
    /** get all digits from the given gamma
     *  @param hypo (INPUT)  gamma
     *  @param digits  (OUTPUT) the digits 
     *  @return number of added digits 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-28  
     */    
    size_t getDigits 
    ( const LHCb::Particle*    gamma   , 
      LHCb::CaloDigit::Vector& digits  ) ;
    // ========================================================================
   } //                                          end of namespace LoKi::Photons  
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_PHOTONS_H
// ============================================================================
