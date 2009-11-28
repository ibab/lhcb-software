// $Id: Photons.cpp,v 1.1 2009-11-28 19:29:45 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
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
#include "Event/CaloDataFunctor.h"
// ============================================================================
// PhysEvent 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Photons.h"
// ============================================================================
/** @file 
 * Implementation file for namespace LoKi::Photons
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */
// ============================================================================
/*  get all digits from the given cluster 
 *  @param cluster (INPUT)  the cluster 
 *  @param digits  (OUTPUT) the digits 
 *  @return number of added digits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
size_t LoKi::Photons::getDigits 
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
// ============================================================================
/*  get all digits from the given cluster 
 *  @param cluster (INPUT)  the cluster 
 *  @param digits  (OUTPUT) the digits 
 *  @return number of added digits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
size_t LoKi::Photons::getDigits 
( const LHCb::CaloCluster* cluster , 
  LHCb::CaloDigit::Vector& digits  ) 
{
  if ( 0 == hypo ){ return digits.size () ; }
  LHCb::CaloDigit::Set digs ( digits.begin() , digits.end() );
  getDigits ( cluster , digs ) ;
  digits.clear() ;
  digits.insert ( digits.end() , digs.begin() , digs.end() ) ;
  return digits.size() ;  
}
// ============================================================================
/*  get all digits from the given hypos 
 *  @param hypo (INPUT)  the hypo
 *  @param digits  (OUTPUT) the digits 
 *  @return number of added digits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
size_t LoKi::Photons::getDigits 
( const LHCb::CaloHypo* hypo    , 
  LHCb::CaloDigit::Set& digits  )  
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
// ============================================================================
/*  get all digits from the given hypos 
 *  @param hypo (INPUT)  the hypo
 *  @param digits  (OUTPUT) the digits 
 *  @return number of added digits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
size_t LoKi::Photons::getDigits 
( const LHCb::CaloHypo*    hypo    , 
  LHCb::CaloDigit::Vector& digits  )  
{
  if ( 0 == hypo ){ return digits.size () ; }
  LHCb::CaloDigit::Set digs ( digits.begin() , digits.end() );
  getDigits ( hypo , digs ) ;
  digits.clear() ;
  digits.insert ( digits.end() , digs.begin() , digs.end() ) ;
  return digits.size() ;  
}
// ============================================================================
/*  get all digits from the given gamma
 *  @param hypo (INPUT)  gamma
 *  @param digits  (OUTPUT) the digits 
 *  @return number of added digits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
size_t LoKi::Photons::getDigits 
( const LHCb::Particle* gamma   , 
  LHCb::CaloDigit::Set& digits  ) 
{
  if ( 0 == gamma         ) { return digits.size () ; }
  const LHCb::ProtoParticle* pp = gamma->proto() ;
  if ( 0 ==  pp           ) { return digits.size () ; }
  if ( pp->calo().empty() ) { return digits.size () ; }
  return getDigits( *(pp->calo().begin()), digits ) ;
}
// ============================================================================
/*  get all digits from the given gamma
 *  @param hypo (INPUT)  gamma
 *  @param digits  (OUTPUT) the digits 
 *  @return number of added digits 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
size_t LoKi::Photons::getDigits 
( const LHCb::Particle*    gamma   , 
  LHCb::CaloDigit::Vector& digits  ) 
{
  if ( 0 == gamma ){ return digits.size () ; }
  LHCb::CaloDigit::Set digs ( digits.begin() , digits.end() );
  getDigits ( gamma , digs ) ;
  digits.clear() ;
  digits.insert ( digits.end() , digs.begin() , digs.end() ) ;
  return digits.size() ;  
}
// ============================================================================
/*  get the energy from the given selector 
 *  @param gamma  (INPUT) the particle 
 *  @param from   (INPUT) the calorimeetr name  
 *  @return the energy
 *  @see LHCb::Particle
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
double LoKi::Photons::energyFrom 
( const LHCb::Particle* gamma , 
  const std::string&    calo  )
{
  LHCb::CaloDataFunctor::DigitFromCalo detector ( calo ) ;
  return energyFrom ( gamma , detector ) ;
}
// ============================================================================
/*  get the seed energy from the given selector 
 *  @param gamma  (INPUT) the particle 
 *  @param calo   (INPUT) the calorimeetr name  
 *  @return the energy
 *  @see LHCb::Particle
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-11-28  
 */    
// ============================================================================
double LoKi::Photons::seedEnergyFrom 
( const LHCb::Particle* gamma , 
  const std::string&    calo  ) 
{
  LHCb::CaloDataFunctor::DigitFromCalo detector ( calo ) ;
  return seedEnergyFrom ( gamma , detector ) ;
}
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
