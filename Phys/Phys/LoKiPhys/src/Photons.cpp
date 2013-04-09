// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Kinematics.h"
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
#include "LoKi/PhysKinematics.h"
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
unsigned int LoKi::Photons::getDigits 
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
unsigned int LoKi::Photons::getDigits 
( const LHCb::CaloCluster* cluster , 
  LHCb::CaloDigit::Vector& digits  ) 
{
  if ( 0 == cluster){ return digits.size () ; }
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
unsigned int LoKi::Photons::getDigits 
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
unsigned int LoKi::Photons::getDigits 
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
unsigned int LoKi::Photons::getDigits 
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
unsigned int LoKi::Photons::getDigits 
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
/*  build the list of photons using \f$\pi^0\f$-veto.
 *  @param photons   (INPUT) the input list of photons 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param gamma     (OUPUT) the vector of single photons 
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 */
// ============================================================================
unsigned int LoKi::Photons::pi0Veto 
( const LHCb::Particle::ConstVector& photons   , 
  const double                       deltamass , 
  const double                       chi2mass  ,
  LHCb::Particle::ConstVector&       gammas    , 
  const double                       pi0mass   ) 
{
  LHCb::Particle::ConstVector tmp  ( photons ) ;
  gammas.clear()  ;
  unsigned int ifind = pi0Veto   ( tmp , deltamass , chi2mass , pi0mass ) ;
  gammas.insert ( gammas.end() , tmp.begin() , tmp.begin() + ifind ) ;
  return ifind ;
}
// ============================================================================
/*  build the list of photons using \f$\pi^0\f$-veto.
 *  @param photons   (INPUT) the input list of photons 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param gamma     (OUPUT) the vector of single photons 
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 */
// ============================================================================
unsigned int LoKi::Photons::pi0Veto 
( const LHCb::Particle::Range&       photons   , 
  const double                       deltamass , 
  const double                       chi2mass  ,
  LHCb::Particle::ConstVector&       gammas    , 
  const double                       pi0mass   ) 
{
  LHCb::Particle::ConstVector tmp  ( photons.begin() , photons.end() ) ;
  gammas.clear()  ;
  unsigned int ifind = pi0Veto   ( tmp , deltamass , chi2mass , pi0mass ) ;
  gammas.insert ( gammas.end() , tmp.begin() , tmp.begin() + ifind ) ;
  return ifind ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  /** check of two Photons can form pi0 
   *  @param gamma1 (INPUT) the first  photon 
   *  @param gamma2 (INPUT) the second photon 
   *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
   *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
   *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
   *  @return true if the pair forms pi0 
   */
  inline bool _pi0 
  ( const LHCb::Particle*  gamma1    , 
    const LHCb::Particle*  gamma2    , 
    const double           deltamass , 
    const double           chi2mass  ,
    const double           pi0mass   )
  {
    //
    if ( 0      == gamma1 || 
         0      == gamma2 || 
         gamma1 == gamma2 ) { return false ; }                           // RETURN 
    //
    // check for mass 
    if ( 0 < deltamass ) 
    {
      const double mass  = ( gamma1 -> momentum () + 
                             gamma2 -> momentum () ).M() ;
      if ( std::fabs ( mass - pi0mass ) > deltamass ) { return false ; } // RETURN
    }
    // check for chi2 
    if ( 0 < chi2mass ) 
    {
      const double chi2  = Gaudi::Math::chi2mass 
        ( pi0mass                                               , 
          gamma1 -> momentum     () + gamma2 -> momentum     () , 
          gamma1 -> momCovMatrix () + gamma2 -> momCovMatrix () ) ;    
      if ( chi2 > chi2mass ) { return false ; }                         // RETURN
      
    }
    //
    return true ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
/*  build the list of photons using \f$\pi^0\f$-veto.
 *  @param photons   (INPUT) the input list of photons 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 *  @return the index for the first "non-single" photon
 */
// ============================================================================
unsigned int LoKi::Photons::pi0Veto 
( LHCb::Particle::ConstVector& photons   , 
  const double                 deltamass , 
  const double                 chi2mass  ,
  const double                 pi0mass   ) 
{
  //
  typedef LHCb::Particle::ConstVector::iterator IT ;
  IT i1 = photons.begin () ;
  IT i2 = photons.end   () ;
  //
  // structure:
  //
  // begin -> i1  : "good single photons " 
  // i2    -> i2  : "not classified photons"
  // i2    -> end : "photons from pi0 "
  //
  while ( i1 != i2 )
  {
    //
    const LHCb::Particle* p1 = *i1 ;
    if ( 0 == p1 ) 
    {
      // move the  bad particle close to the end of container 
      std::iter_swap ( i1 , --i2 ) ;
      continue ;                                         // CONTINUE 
    }
    //                                          
    // loop over all non-classified photons 
    IT i3 = i1 + 1 ;
    for ( ; i3 != i2 ; ++i3 ) 
    {
      const LHCb::Particle* p3 = *i3 ;
      if ( 0 == p3 ) 
      { 
        // move the  bad particle close to the end of container 
        std::iter_swap ( i3 , --i2 ) ;
        continue ;                                       // CONTINUE   
      }
      // check if the pair forms pi0 :
      if ( _pi0 ( p1         , 
                  p3         , 
                  deltamass  , 
                  chi2mass   , 
                  pi0mass    ) ) { break ; }              // BREAK 
      //
    }
    //
    if ( i3 == i2 ) { ++i1 ; } // this is "good" photon!
    else            
    {     
      // move i1 into i2 
      std::iter_swap ( i1 , --i2 ) ;
      // move i3 into i2 
      std::iter_swap ( i3 , --i2 ) ;
    }
    //
  }
  //
  return i1 - photons.begin() ;
}
// ============================================================================
/*  check if the gamma forms pi0 with some of other phtoons for the list 
 *  @apram gamma   (INPUT) the gamma to be tested 
 *  @param photons (INPUT) the list of photons to be tested 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 *  @return true if no pi0s are formed 
 */
// ============================================================================
bool LoKi::Photons::pi0Veto
( const LHCb::Particle*              gamma      , 
  const LHCb::Particle::ConstVector& photons    , 
  const double                       deltamass  , 
  const double                       chi2mass   ,
  const double                       pi0mass    ) 
{
  //
  if ( 0 == gamma      ) { return false ; }                  // RETURN 
  if ( photons.empty() ) { return true  ; }                  // RETURN
  //
  return pi0Veto ( gamma                             , 
                   LHCb::Particle::Range ( photons ) , 
                   deltamass                         , 
                   chi2mass                          , 
                   pi0mass                           ) ;
  
}
// ============================================================================
/*  check if the gamma forms pi0 with some of other phtoons for the list 
 *  @apram gamma   (INPUT) the gamma to be tested 
 *  @param photons (INPUT) the list of photons to be tested 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 *  @return true if no pi0s are formed 
 */
// ============================================================================
bool LoKi::Photons::pi0Veto
( const LHCb::Particle*        gamma      , 
  const LHCb::Particle::Range& photons    , 
  const double                 deltamass  , 
  const double                 chi2mass   ,
  const double                 pi0mass    ) 
{
  //
  if ( 0 == gamma      ) { return false ; }                  // RETURN 
  if ( photons.empty() ) { return true  ; }                  // RETURN
  //
  for ( LHCb::Particle::Range::iterator iph = photons.begin() ; 
        photons.end() != iph ; ++iph ) 
  {
    if ( _pi0 ( gamma     , 
                *iph      ,
                deltamass , 
                chi2mass  , 
                pi0mass   ) ) { return false ; }             // RETURN
  }
  // 
  return true ;
}
// =============================================================================
/*  check if the gamma forms pi0 with some of other phtoons for the list 
 *  @apram gamma1    (INPUT) the first  gamma to be tested 
 *  @apram gamma2    (INPUT) the second gamma to be tested 
 *  @param photons   (INPUT) the list of photons to be tested 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 *  @return true if both phtons do not form pi0s 
 */
// ========================================================================
bool LoKi::Photons::pi0Veto
( const LHCb::Particle*              gamma1    , 
  const LHCb::Particle*              gamma2    , 
  const LHCb::Particle::ConstVector& photons   , 
  const double                       deltamass , 
  const double                       chi2mass  ,
  const double                       pi0mass   ) 
{
  //
  if ( 0 == gamma1 || 0 == gamma2 || gamma1 == gamma2 ) { return false ; }
  if ( photons.empty() )                                { return true  ; }
  //
  return pi0Veto ( gamma1                            ,
                   gamma2                            , 
                   LHCb::Particle::Range ( photons ) , 
                   deltamass                         , 
                   chi2mass                          , 
                   pi0mass                           ) ;    
}
// ============================================================================
/*  check if the gamma forms pi0 with some of other phtoons for the list 
 *  @apram gamma   (INPUT) the gamma to be tested 
 *  @param photons (INPUT) the list of photons to be tested 
 *  @param deltamass (INPUT) half-mass for \f$\pi^0\f$-mass window
 *  @param chi2mass  (INPUT) \f$\chi^2\f$ for \f$\pi^0\f$-mass hypothesis
 *  @param pi0mass   (INPUT) the nominal mass of \f$\pi^0\f$-meson
 *  @return true if both phtoons do not form pi0s
 */
// ============================================================================
bool LoKi::Photons::pi0Veto
( const LHCb::Particle*        gamma1    , 
  const LHCb::Particle*        gamma2    , 
  const LHCb::Particle::Range& photons   , 
  const double                 deltamass , 
  const double                 chi2mass  ,
  const double                 pi0mass   ) 
{
  //
  if ( 0 == gamma1 || 0 == gamma2 || gamma1 == gamma2 ) { return false ; }
  //
  const LHCb::ProtoParticle* pr1 = protoP ( gamma1 ) ;
  const LHCb::ProtoParticle* pr2 = protoP ( gamma2 ) ;
  if  ( 0 != pr1 && pr1 == pr2  )                       { return false ; }
  //
  if ( photons.empty() )                                { return true  ; }
  //
  for ( LHCb::Particle::Range::iterator iph = photons.begin() ; 
        photons.end() != iph ; ++iph ) 
  {
    const LHCb::Particle* gamma = *iph ;
    //
    if ( 0       == gamma     ) { continue ; }   // CONTINUE 
    if ( gamma1  == gamma     ) { continue ; }   // CONTINUE 
    if ( gamma2  == gamma     ) { continue ; }   // CONTINUE 
    //
    const LHCb::ProtoParticle* pr = protoP ( gamma ) ;
    if ( 0 != pr && pr1 == pr ) { continue ; }   // CONITNUE 
    if ( 0 != pr && pr2 == pr ) { continue ; }   // CONITNUE
    //
    if ( _pi0 ( gamma1    , 
                gamma     ,
                deltamass , 
                chi2mass  , 
                pi0mass   ) ) { return false ; }             // RETURN
    //
    if ( _pi0 ( gamma2    , 
                gamma     ,
                deltamass , 
                chi2mass  , 
                pi0mass   ) ) { return false ; }             // RETURN
  }
  // 
  return true ;
} 
// ===========================================================================


// ============================================================================
// The END 
// ============================================================================
