// $Id: Pi0Calibr.cpp,v 1.4 2009-09-28 19:53:55 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revison:$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
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
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
/// anonymous namespace to hide local functions 
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
    const LHCb::CaloHypo* h = hypo( gamma ) ;
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
  ( const LHCb::CaloCluster*          cluster , 
    std::set<const LHCb::CaloDigit*>& digits  ) 
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
}
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  /** @class Pi0  
   *  Simple algorithm for Calorimeter Cailbration using pi0 peak
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   */
  class Pi0 : public LoKi::Algo
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<Kali::Pi0> ;    // friend factory for instantiation 
    // ========================================================================
  public:
    // ========================================================================
    /// the proper initialization 
    virtual StatusCode initialize () ;          //    the proper initialization 
    /// the main 'execution' method 
    virtual StatusCode analyse    () ;          //  the main 'execution' method 
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param name (INPUT) the algorithm instance name 
     *  @param pSvc (INPUT) the pointer to Service Locator 
     */
    Pi0 ( const std::string& name ,            //    the algorithm instance name 
          ISvcLocator*       pSvc )            // the pointer to Service Locator 
      : LoKi::Algo ( name , pSvc ) 
      , m_destroy  ( true ) 
    {
      declareProperty
        ( "Destroy" ,
          m_destroy , 
          "Destroy the content of TES containers" ) ->
        declareUpdateHandler(&Pi0::updateDestroy, this ) ;
    }
    /// virtual & protected destructor 
    virtual ~Pi0() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Pi0 () ;                            //  the default constructor is disabled      
    /// the copy  constructor is disabled 
    Pi0 ( const Pi0& ) ;                //     the copy constructor is disabled 
    /// the assignement operator is disabled 
    Pi0& operator=( const Pi0& ) ;      // the assignement operator is disabled 
    // ========================================================================
  public:
    // ========================================================================
    /// Update handler for the property
    void updateDestroy ( Property& /* p */ ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// Destroy TES containers ?
    bool m_destroy ;                                // Destroy TES containers ?
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namesapce Kali
// ============================================================================
// the proper initialization 
// ============================================================================
StatusCode Kali::Pi0::initialize ()             //    the proper initialization 
{
  StatusCode  sc = LoKi::Algo::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                        // RETURN 
  //
  if ( m_destroy )
  { Warning ( "TES containers will be destroyed!" , StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "TES containers will be preserved!" , StatusCode::SUCCESS ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// Update handler for the property
// ============================================================================
void Kali::Pi0::updateDestroy ( Property& /* p */ ) 
{
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ) { return ; }   // RETURN 
  // 
  if ( m_destroy )
  { Warning ( "TES containers will be destroyed!" , StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "TES containers will be preserved!" , StatusCode::SUCCESS ) ; }
  //
}
// ============================================================================
// the only one essential method 
// ============================================================================
StatusCode Kali::Pi0::analyse    ()            // the only one essential method 
{
  using namespace LoKi         ;
  using namespace LoKi::Types  ;
  using namespace LoKi::Cuts   ;
  using namespace Gaudi::Units ;
  
  LHCb::CaloDataFunctor::DigitFromCalo spd ( "Spd" ) ;
  LHCb::CaloDataFunctor::DigitFromCalo prs ( "Prs" ) ;
  
  // get all photons with 
  Range gamma = select ( "g" , ( "gamma" == ID ) && ( PT > 300 * MeV ) ) ;
  
  counter("#gamma") += gamma.size() ;

  Tuple tuple = nTuple ( " PI0 tuple " ) ;
  
  typedef std::set<const LHCb::CaloDigit*> SET ;
  SET digits ;
  
  AIDA::IHistogram1D* h1 = book ( "mpi0-0"       , 0 , 250 , 250 ) ;
  AIDA::IHistogram1D* h2 = book ( "mpi0-1-pt"    , 0 , 250 , 250 ) ;
  AIDA::IHistogram1D* h3 = book ( "mpi0-2-spd-1" , 0 , 250 , 250 ) ;
  AIDA::IHistogram1D* h4 = book ( "mpi0-3-spd-2" , 0 , 250 , 250 ) ;
  
  for ( Loop pi0 = loop( "g g" , "pi0" ) ; pi0 ; ++pi0 ) 
  {
    
    const double m12 = pi0->mass ( 1 , 2 ) ;
    
    if ( m12 > 250 * MeV ) { continue ; }  // CONTINUE
    
    const LHCb::Particle* g1 = pi0(1) ;
    if ( 0 == g1         ) { continue ; }  // CONTINUE 
    
    const LHCb::Particle* g2 = pi0(2) ;
    if ( 0 == g2         ) { continue ; }  // CONTINUE
    
    if ( 0 != h1 ) { h1 -> fill ( m12 ) ; }
    
    LoKi::LorentzVector p12 = pi0->p(1,2) ;
    if ( 800 * Gaudi::Units::MeV > p12.Pt() ) { continue ; }
    
    if ( 0 != h2 ) { h2 -> fill ( m12 ) ; }
    
    //double spd1e = energyFrom ( g1 , spd ) / GeV ;
    double spd1e = seedEnergyFrom ( g1 , spd ) / GeV ;
    
    if ( 0 < spd1e ) { continue ; }
    if ( 0 != h3 ) { h3 -> fill ( m12 ) ; }
    
    double spd2e = seedEnergyFrom ( g2 , spd ) / GeV ;
    if ( 0 < spd2e ) { continue ; }
    if ( 0 != h4 ) { h4 -> fill ( m12 ) ; }
    
    double prs1e = energyFrom ( g1 , prs ) / GeV ;
    double prs2e = energyFrom ( g2 , prs ) / GeV ;
    
    if ( prs1e > prs2e ) 
    {
      std::swap ( g1    , g2 ) ;
      std::swap ( spd1e , spd2e ) ;
      std::swap ( prs1e , prs2e ) ; 
    }
    
    // fill N-tuple
    
    tuple -> column ( "spd2" , spd2e ) ;
    
    tuple -> column ( "m12"  , m12             / GeV ) ;
    
    tuple -> column ( "p0"   , p12             / GeV ) ;
    tuple -> column ( "g1"   , g1->momentum()  / GeV ) ;
    tuple -> column ( "g2"   , g2->momentum()  / GeV ) ;
    
    tuple -> column ( "pt"   , p12.Pt()        / GeV ) ;
    tuple -> column ( "pt1"  , PT ( g1 )       / GeV ) ;
    tuple -> column ( "pt2"  , PT ( g2 )       / GeV ) ;
    
    tuple -> column ( "prs1" , prs1e ) ;
    tuple -> column ( "prs2" , prs2e ) ;
    
    tuple -> column ( "spd1" , spd1e ) ;
    tuple -> column ( "spd2" , spd2e ) ;
    
    const LHCb::CaloCellID cel1 = cellID( g1 ) ;
    const LHCb::CaloCellID cel2 = cellID( g2 ) ;
    
    tuple -> column ( "ind1" , cel1.index()    ) ;
    tuple -> column ( "ind2" , cel2.index()    ) ;
    
    tuple -> write () ; 
    
    getDigits ( g1 , digits ) ;
    getDigits ( g2 , digits ) ;
    
  }
  
  counter ( "#digits" ) += digits.size  () ;
  counter ( "#empty"  ) += digits.empty () ;
  
  setFilterPassed ( !digits.empty() ) ;

  if  ( !m_destroy ) { return StatusCode::SUCCESS ; }    // RETURN 
  
  // ==========================================================================
  // ATTENTION: Here we'll destroy containers in TES! 
  // ==========================================================================
  
  typedef LHCb::CaloDigit::Container Digits ;
  
  std::vector<Digits*> digs ;
  
  Digits* d1 = get<Digits> ( LHCb::CaloDigitLocation::Spd  ) ;
  digs.push_back ( d1 ) ;
  Digits* d2 = get<Digits> ( LHCb::CaloDigitLocation::Prs  ) ;
  digs.push_back ( d2 ) ;
  Digits* d3 = get<Digits> ( LHCb::CaloDigitLocation::Ecal ) ;
  digs.push_back ( d3 ) ;
  Digits* d4 = get<Digits> ( LHCb::CaloDigitLocation::Hcal ) ;
  digs.push_back ( d4 ) ;
  
  size_t size1 = 0 ;
  size_t size2 = 0 ;
  
  for ( std::vector<Digits*>::iterator ic = digs.begin() ; 
        digs.end() != ic ; ++ic ) 
  {
    Digits* d = *ic ;
    if ( 0 == d ) { continue ; }
    //
    size1 += d->size() ;
    //
    Digits::iterator ifind = d->begin() ;
    while ( ifind != d->end() ) 
    {
      const LHCb::CaloDigit* digit = *ifind ;
      if ( 0 == digit ) { continue ; }
      //
      SET::const_iterator iset = digits.find ( digit ) ;
      if ( digits.end() != iset ) { ++ifind ; continue ; }
      //
      d->erase ( *ifind ) ;
      //
      ifind = d->begin() ;
    } 
    size2 += d->size() ;
  }
  
  counter ( "#size1"        ) +=               size1 ;
  counter ( "#size2"        ) +=               size2 ;
  counter ( "#size2/size1"  ) += double(size2)/size1 ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The factory:
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,Pi0)
// ============================================================================
// The END 
// ============================================================================
