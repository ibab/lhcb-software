// $Id: Photons.h,v 1.1 2009/11/28 19:29:45 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PHOTONS_H 
#define LOKI_PHOTONS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Kernel.h"
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
    // =======================================================================
    /// pi0 mass 
    const double s_Pi0Mass = 135  * Gaudi::Units::MeV ;            // pi0 mass 
    // =======================================================================
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
      if ( 0 == gamma ) { return 0 ; }                         // RETURN
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
      if ( 0 == gamma         ) { return 0 ; }                 // RETURN
      const LHCb::ProtoParticle* pp = protoP ( gamma ) ;
      if ( 0 == pp            ) { return 0 ; }                 // RETURN 
      if ( pp->calo().empty() ) { return 0 ; }                 // RETURN
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
      if ( 0 == gamma            ) { return 0 ; }                  // RETURN 
      const LHCb::CaloHypo* h = hypo( gamma ) ;    
      if ( 0 == h                ) { return 0 ; }                  // RETURN 
      if ( h->clusters().empty() ) { return 0 ; }                  // RETURN 
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
      if ( 0 == gamma  ) { return LHCb::CaloCellID() ; }            // RETURN
      const LHCb::CaloCluster* cl = cluster( gamma ) ;
      if ( 0 == cl     ) { return LHCb::CaloCellID() ; }            // RETURN
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
      if ( 0 == gamma ) { return -1 * Gaudi::Units::TeV ; }   // RETURN
      const LHCb::CaloHypo* h = hypo( gamma ) ;
      if ( 0 == h     ) { return -1 * Gaudi::Units::TeV ; }   // RETURN 
      double energy = 0 ;
      // collect energy from digits 
      typedef LHCb::CaloHypo::Digits Digits ;
      const Digits& digits = h->digits() ;
      for ( Digits::const_iterator idigit = digits.begin() ; 
            digits.end() != idigit ; ++idigit ) 
      {
        const LHCb::CaloDigit* digit = *idigit ;
        if ( 0 == digit     ) { continue ; }                // CONTINUE 
        if ( from ( digit ) ) { energy += digit->e() ; }
      }
      // collect energy from clusters 
      typedef LHCb::CaloHypo::Clusters Clusters ;
      const Clusters& clusters = h->clusters() ;
      for ( Clusters::const_iterator icluster = clusters.begin() ; 
            clusters.end() != icluster ; ++icluster ) 
      {
        const LHCb::CaloCluster* cluster = *icluster ;
        if ( 0 == cluster ) { continue ; }                  // CONTINUE 
        //
        typedef  std::vector<LHCb::CaloClusterEntry> Entries ;
        const Entries& entries = cluster->entries() ;    
        for ( Entries::const_iterator ientry = entries.begin() ; 
              entries.end() != ientry ; ++ientry )
        {
          const LHCb::CaloDigit* digit = ientry->digit() ;
          if ( 0 == digit     ) { continue ; }              // CONTINUE  
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
    GAUDI_API
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
      if ( 0 == gamma ) { return -1 * Gaudi::Units::TeV ; }     // RETURN 
      const LHCb::CaloHypo*    h = hypo    ( gamma ) ;
      if ( 0 == h     ) { return -1 * Gaudi::Units::TeV ; }     // RETURN 
      const LHCb::CaloCluster* c = cluster ( gamma ) ;
      if ( 0 == c     ) { return -1 * Gaudi::Units::TeV ; }     // RETURN 
      //
      double energy = 0 ;
      typedef LHCb::CaloHypo::Digits Digits ;
      const Digits& digits = h->digits() ;
      for ( Digits::const_iterator idigit = digits.begin() ; 
            digits.end() != idigit ; ++idigit ) 
      {
        const LHCb::CaloDigit* digit = *idigit ;
        if ( 0 == digit    ) { continue ; }                     // CONTINUE  
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
        if ( 0 == cluster ) { continue ; }                     // CONTINUE 
        //
        typedef  std::vector<LHCb::CaloClusterEntry> Entries ;
        const Entries& entries = cluster->entries() ;    
        for ( Entries::const_iterator ientry = entries.begin() ; 
              entries.end() != ientry ; ++ientry )
        {
          const LHCb::CaloDigit* digit = ientry->digit() ;
          if ( 0 == digit     ) { continue ; }                // CONTINUE 
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
    GAUDI_API
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
    GAUDI_API
    unsigned int getDigits 
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
    GAUDI_API
    unsigned int getDigits 
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
    GAUDI_API
    unsigned int getDigits 
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
    GAUDI_API
    unsigned int getDigits 
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
    GAUDI_API
    unsigned int getDigits 
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
    GAUDI_API
    unsigned int getDigits 
    ( const LHCb::Particle*    gamma   , 
      LHCb::CaloDigit::Vector& digits  ) ;
    // ========================================================================
    /** build the list of photons using \f$\pi^0\f$-veto.
     *  @param photons   (INPUT) the input list of photons 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param gamma     (OUPUT) the vector of single photons 
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return the index for the first "non-single" photon
     */
    GAUDI_API
    unsigned int pi0Veto 
    ( const LHCb::Particle::ConstVector& photons               , 
      const double                       deltamass             , 
      const double                       chi2mass              ,
      LHCb::Particle::ConstVector&       gammas                , 
      const double                       pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
    /** build the list of photons using \f$\pi^0\f$-veto.
     *  @param photons   (INPUT) the input list of photons 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param gamma     (OUPUT) the vector of single photons 
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return the index for the first "non-single" photon
     */
    GAUDI_API
    unsigned int pi0Veto 
    ( const LHCb::Particle::Range&       photons               , 
      const double                       deltamass             , 
      const double                       chi2mass              ,
      LHCb::Particle::ConstVector&       gammas                , 
      const double                       pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
    /** build the list of photons using \f$\pi^0\f$-veto.
     *  @param photons   (INPUT) the input list of photons 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return the index for the first "non-single" photon
     */
    GAUDI_API
    unsigned int pi0Veto 
    ( LHCb::Particle::ConstVector& photons               , 
      const double                 deltamass             , 
      const double                 chi2mass              ,
      const double                 pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
    /** check if the gamma forms pi0 with some of other phtoons for the list 
     *  @apram gamma   (INPUT) the gamma to be tested 
     *  @param photons (INPUT) the list of photons to be tested 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return true if no pi0s are formed 
     */
    GAUDI_API 
    bool pi0Veto
    ( const LHCb::Particle*              gamma                 , 
      const LHCb::Particle::ConstVector& photons               , 
      const double                       deltamass             , 
      const double                       chi2mass              ,
      const double                       pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
    /** check if the gamma forms pi0 with some of other phtoons for the list 
     *  @apram gamma   (INPUT) the gamma to be tested 
     *  @param photons (INPUT) the list of photons to be tested 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return true if no pi0s are formed 
     */
    GAUDI_API 
    bool pi0Veto
    ( const LHCb::Particle*              gamma                 , 
      const LHCb::Particle::Range&       photons               , 
      const double                       deltamass             , 
      const double                       chi2mass              ,
      const double                       pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
    /** check if the gamma forms pi0 with some of other phtoons for the list 
     *  @apram gamma1    (INPUT) the first  gamma to be tested 
     *  @apram gamma2    (INPUT) the second gamma to be tested 
     *  @param photons   (INPUT) the list of photons to be tested 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return true if both phtons do not form pi0s 
     */
    GAUDI_API 
    bool pi0Veto
    ( const LHCb::Particle*              gamma1                , 
      const LHCb::Particle*              gamma2                , 
      const LHCb::Particle::ConstVector& photons               , 
      const double                       deltamass             , 
      const double                       chi2mass              ,
      const double                       pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
    /** check if the gamma forms pi0 with some of other phtoons for the list 
     *  @apram gamma   (INPUT) the gamma to be tested 
     *  @param photons (INPUT) the list of photons to be tested 
     *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
     *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
     *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
     *  @return true if both phtoons do not form pi0s
     */
    GAUDI_API 
    bool pi0Veto
    ( const LHCb::Particle*              gamma1                , 
      const LHCb::Particle*              gamma2                , 
      const LHCb::Particle::Range&       photons               , 
      const double                       deltamass             , 
      const double                       chi2mass              ,
      const double                       pi0mass   = s_Pi0Mass ) ;
    // ========================================================================
  } //                                          end of namespace LoKi::Photons  
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_PHOTONS_H
// ============================================================================
