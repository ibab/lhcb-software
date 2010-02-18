// $Id: KaliUtils.h,v 1.2 2010-02-18 20:07:08 ibelyaev Exp $
// ============================================================================
#ifndef KALI_UTILS_H 
#define KALI_UTILS_H 1
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// anonymous namespace to hide local functions 
// ============================================================================
namespace 
{
  // ==========================================================================
  inline const LHCb::ProtoParticle* protoP ( const LHCb::Particle* gamma ) 
  {
    if ( 0 == gamma ) { return 0 ; }
    return gamma->proto () ;
  }
  // ==========================================================================
  inline const LHCb::CaloHypo*  hypo( const LHCb::Particle* gamma ) 
  {
    if ( 0 == gamma         ) { return 0 ; }
    const LHCb::ProtoParticle* pp = protoP ( gamma ) ;
    if ( 0 == pp            ) { return 0 ; }
    if ( pp->calo().empty() ) { return 0 ; }
    return (*(pp->calo().begin())) ;
  }
  // ==========================================================================
  inline const LHCb::CaloCluster* cluster ( const LHCb::Particle* gamma ) 
  {
    if ( 0 == gamma            ) { return 0 ; }
    const LHCb::CaloHypo* h = hypo( gamma ) ;
    if ( 0 == h                ) { return 0 ; }
    if ( h->clusters().empty() ) { return 0 ; }
    return (*(h->clusters().begin()));
  }
  // ==========================================================================
  inline LHCb::CaloCellID cellID ( const LHCb::Particle* gamma ) 
  {
    if ( 0 == gamma  ) { return LHCb::CaloCellID() ; }
    const LHCb::CaloCluster* cl = cluster( gamma ) ;
    if ( 0 == cl     ) { return LHCb::CaloCellID() ; }
    return cl->seed() ;  
  }
  // ==========================================================================
  template <class FROM> 
  inline double energyFrom 
  ( const LHCb::Particle* gamma , 
    const FROM&           from  ) 
  {
    if ( 0 == gamma ) { return -1 * Gaudi::Units::TeV ; }
    const LHCb::CaloHypo* h = hypo ( gamma ) ;
    if ( 0 == h     ) { return -1 * Gaudi::Units::TeV ; }
    double energy = 0 ;
    typedef LHCb::CaloHypo::Digits Digits ;
    const Digits& digits = h->digits() ;
    for ( Digits::const_iterator idigit = digits.begin() ; 
          digits.end() != idigit ; ++idigit ) 
    {
      const LHCb::CaloDigit* digit = *idigit ;
      if ( 0 == digit    ) { continue ; }
      if ( from( digit ) ) { energy += digit->e() ; }
    }
    return energy ;
  }
  // ==========================================================================  
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
    return energy ;
  }
  // ==========================================================================
  inline size_t getDigits 
  ( const LHCb::CaloCluster* cluster , 
    LHCb::CaloDigit::Set&    digits  ) 
  {
    if ( 0 == cluster ) { return digits.size() ; }
    typedef  std::vector<LHCb::CaloClusterEntry> Entries ;
    const Entries& entries = cluster->entries() ;    
    for ( Entries::const_iterator ientry = entries.begin() ; 
          entries.end() != ientry ; ++ientry )
    {
      const LHCb::CaloDigit* digit = ientry->digit() ;
      if ( 0 == digit ) { continue ; }
      digits.insert ( digit ) ;
    }
    return digits.size() ;
  }
  // ==========================================================================
  inline size_t getDigits 
  ( const LHCb::CaloHypo*             hypo    , 
    std::set<const LHCb::CaloDigit*>& digits  ) 
  {
    if ( 0 == hypo ){ return digits.size () ; }
    //
    typedef LHCb::CaloHypo::Digits Digits ;
    const Digits& digs = hypo->digits() ;
    for ( Digits::const_iterator idigit = digs.begin() ; 
          digs.end() != idigit ; ++idigit ) 
    {
      const LHCb::CaloDigit* digit = *idigit ;
      if ( 0 == digit    ) { continue ; }
      digits.insert ( digit ) ;
    }
    //
    if ( hypo->clusters().empty() ) { return digits.size() ; }
    return getDigits ( *(hypo->clusters().begin()) , digits ) ;  
    //
  }
  // ==========================================================================
  inline size_t getDigits 
  ( const LHCb::Particle*             gamma   , 
    std::set<const LHCb::CaloDigit*>& digits  ) 
  {
    if ( 0 == gamma         ) { return digits.size () ; }
    const LHCb::ProtoParticle* pp = gamma->proto() ;
    if ( 0 ==  pp           ) { return digits.size () ; }
    if ( pp->calo().empty() ) { return digits.size () ; }
    return getDigits( *(pp->calo().begin()), digits ) ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // KALI_UTILS_H
// ============================================================================
