// $Id: Pi0.cpp,v 1.8 2009-12-16 16:48:08 apuignav Exp $
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
// ICaloDigits4TrackTool
// ============================================================================
#include "CaloInterfaces/ICaloDigits4Track.h"
// ============================================================================
// Local 
// ============================================================================
#include "KaliUtils.h"
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
    /// the proper initialzation
    virtual StatusCode initialize () ;             // the proper tinitialzation
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
        //
      , m_mirror ( false )
      , m_histograms ( false )
    {
      declareProperty 
        ( "Mirror" , 
          m_mirror , 
          "Flag to activate Albert's trick with backroung estimation" )
        -> declareUpdateHandler ( &Kali::Pi0::mirrorHandler , this ) ;
      declareProperty 
        ( "Histograms" , 
          m_histograms , 
          "Flag to activate monitoring histograms creation" )
        -> declareUpdateHandler ( &Kali::Pi0::histogramsHandler , this ) ;
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
    /// fill tuple method
    void fillTuple 
    ( const Tuple& ,
      const Gaudi::LorentzVector& , 
      const Gaudi::LorentzVector& ,
      const Gaudi::LorentzVector& , 
      const double , 
      const double , 
      const double , 
      const double ,
      const LHCb::CaloCellID& , 
      const LHCb::CaloCellID& , 
      const int    ) ;
    double caloEnergy4Photon( const Gaudi::LorentzVector&  p ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// update handler for 'Mirror' property
    void mirrorHandler ( Property& p ) ; // update handler for 'Mirror' property
    /// update handler for 'Histograms' property
    void histogramsHandler ( Property& p ) ; // update handler for 'Histograms' property
    // ========================================================================
  private:
    // ========================================================================
    bool  m_mirror ;                                     // use Albert's trick? 
    bool  m_histograms ;                                 // produce moni histos
    // Tool for retrieving SPD digits info
    ICaloDigits4Track*  m_spdDigitsTool ;
    // Monitoring histograms
    AIDA::IHistogram1D* h1 ;
    AIDA::IHistogram1D* h2 ;
    AIDA::IHistogram1D* h3 ;
    AIDA::IHistogram1D* h4 ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace Kali
// ============================================================================
// update handler for 'Mirror' property
// ============================================================================
void Kali::Pi0::mirrorHandler ( Property& /* p */ ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  if ( m_mirror ) 
  { Warning ( "Albert's trick for background evaluation is   activated!", StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "Albert's trick for background evaluation is deactivated!", StatusCode::SUCCESS ) ; }  
  //
}
// ============================================================================
// update handler for 'Histos' property
// ============================================================================
void Kali::Pi0::histogramsHandler ( Property& /* p */ ) 
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  if ( m_histograms ) 
  { Warning ( "Producing monitoring histograms is   activated", StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "Producing monitoring histograms is deactivated!", StatusCode::SUCCESS ) ; }  
  //
}
// ============================================================================
// tuple fill helper function 
// ============================================================================
void Kali::Pi0::fillTuple
( const Tuple&                tuple ,
  const Gaudi::LorentzVector& p1    , 
  const Gaudi::LorentzVector& p2    ,
  const Gaudi::LorentzVector& p12   ,
  const double                prs1e , 
  const double                prs2e ,
  const double                spd1e , 
  const double                spd2e ,
  const LHCb::CaloCellID&     cell1 , 
  const LHCb::CaloCellID&     cell2 , 
  const int                   bkg   )
{
    using namespace Gaudi::Units ;
    
    // fill N-tuple
    tuple -> column ( "p0"   , p1 + p2  / MeV ) ;
    tuple -> column ( "g1"   , p1       / MeV ) ;
    tuple -> column ( "g2"   , p2       / MeV ) ;
    
    tuple -> column ( "pt"   , p12.Pt() / MeV ) ;
    tuple -> column ( "pt1"  , p1.Pt()  / MeV ) ;
    tuple -> column ( "pt2"  , p2.Pt()  / MeV ) ;
    
    tuple -> column ( "prs1" , prs1e ) ;
    tuple -> column ( "prs2" , prs2e ) ;
    
    tuple -> column ( "spd1" , (int) (spd1e/3.2) ) ;
    tuple -> column ( "spd2" , (int) (spd2e/3.2) ) ;
    
    tuple -> column ( "ind1" , cell1.index() ) ;
    tuple -> column ( "ind2" , cell2.index() ) ;
    
    tuple -> column ( "m12"  , p12.M()  / MeV ) ;
    tuple -> column ( "bkg"  , bkg , 0 , 2    ) ;
    
    Gaudi::XYZVector mom1 = p1.Vect () ;
    Gaudi::XYZVector mom2 = p2.Vect () ;
    if ( 1 == bkg ) {
      mom2.SetX ( -mom2.X () ) ;
      mom2.SetY ( -mom2.Y () ) ;
    }
    double cosPhi = mom1.Dot ( mom2 ) / ( mom1.R () * mom2.R () ) ;
    tuple -> column ( "cosPhi" , cosPhi ) ;
 
    tuple -> write () ;
}
// ============================================================================
// Photon CaloDigitEnergy
// ============================================================================
double Kali::Pi0::caloEnergy4Photon( const Gaudi::LorentzVector&  p )
{
  typedef std::set<const LHCb::CaloDigit*> SET ;
  SET digits ;
  Gaudi::XYZPoint point( 0 , 0 , 0 ) ;
  Gaudi::XYZVector vector = p.Vect() ;
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> LINE ;
  LINE line( point , vector ) ;
  m_spdDigitsTool->collect ( line , digits );
  double e = 0.0 ;
  for ( SET::const_iterator digit = digits.begin() ; digits.end() != digit ; digit++ ){
    e += (*digit)->e() ;
  } 
  return e ;
}
// ============================================================================
// the proper initialization
// ============================================================================
StatusCode Kali::Pi0::initialize  ()                // the proper initialzation
{
  StatusCode sc = LoKi::Algo::initialize();
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( m_mirror ) 
  { Warning ( "Albert's trick for background evaluation is   activated!", StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "Albert's trick for background evaluation is deactivated!", StatusCode::SUCCESS ) ; }  
  //
  if ( m_histograms ) 
  { Warning ( "Producing monitoring histograms is   activated", StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "Producing monitoring histograms is deactivated!", StatusCode::SUCCESS ) ; }  
  //
  m_spdDigitsTool = tool<ICaloDigits4Track>( "SpdEnergyForTrack" , this ) ; // Load tool for SPD
  sc = Gaudi::Utils::setProperty ( m_spdDigitsTool , "AddNeighbours" , 1 ) ;
  if ( sc.isFailure() ) { return sc ; }
  //
  return StatusCode::SUCCESS ;
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
  
  Tuple tuple = nTuple ( "Pi0-Tuple" ) ;
  
  typedef std::set<const LHCb::CaloDigit*> SET ;
  SET digits ;
  
  if ( m_histograms )
  {
    h1 = book ( "mpi0-0"       , 0 , 0.250 , 250 ) ;
    h2 = book ( "mpi0-1-pt"    , 0 , 0.250 , 250 ) ;
    h3 = book ( "mpi0-2-spd-1" , 0 , 0.250 , 250 ) ;
    h4 = book ( "mpi0-3-spd-2" , 0 , 0.250 , 250 ) ;
  }

  typedef std::set<const LHCb::Particle*> Photons ;
  Photons                                 photons ;
  
  for ( Loop pi0 = loop( "g g" , "pi0" ) ; pi0 ; ++pi0 ) 
  {
    
    const double              m12 = pi0->mass ( 1 , 2 ) ;
    const LoKi::LorentzVector p12 = pi0->p    ( 1 , 2 ) ;

    const LHCb::Particle* g1 = pi0(1) ;
    if ( 0 == g1         ) { continue ; }  // CONTINUE    

    const LHCb::Particle* g2 = pi0(2) ;
    if ( 0 == g2         ) { continue ; }  // CONTINUE
    
    // trick with "mirror-background" by Albert Puig
    // invert the first photon :
    const Gaudi::LorentzVector mom1 = g1->momentum() ;
    const Gaudi::LorentzVector mom2 = g2->momentum() ;
    Gaudi::LorentzVector p1 = g1->momentum() ;
    p1.SetPx ( -p1.Px() ) ;
    p1.SetPy ( -p1.Py() ) ;
    const Gaudi::LorentzVector fake = ( p1 + mom2 ) ;

    const bool good    =             ( m12      < 250 * MeV && p12.Pt()  > 800 * MeV ) ;
    bool       goodBkg = m_mirror && ( fake.M() < 250 * MeV && fake.Pt() > 800 * MeV ) ;
    
    if ( m_histograms && 0 != h1 && good ) { h1 -> fill ( m12 / MeV ) ; }
    
    if ( (!good)  && (!goodBkg) ) { continue ; }   // CONTINUE!!!
    
    if ( m_histograms && 0 != h2 && good ) { h2 -> fill ( m12 / MeV ) ; }
    
    double spd1e = seedEnergyFrom ( g1 , spd ) / MeV ;
    
    if ( 0 < spd1e ) { continue ; }
    if ( m_histograms && 0 != h3 && good ) { h3 -> fill ( m12 / MeV ) ; }
    
    double spd2e = seedEnergyFrom ( g2 , spd ) / MeV ;
    if ( 0 < spd2e ) { continue ; }
    
    if ( m_histograms && 0 != h4 && good ) { h4 -> fill ( m12 / MeV ) ; }
    
    double prs1e = energyFrom ( g1 , prs ) / MeV ;
    double prs2e = energyFrom ( g2 , prs ) / MeV ;
    
    if ( prs1e > prs2e ) 
    {
      std::swap ( g1    , g2    ) ;
      std::swap ( prs1e , prs2e ) ; 
    }
        
    const double spd1e3x3 = caloEnergy4Photon( mom1 ) ;
    const double spd2e3x3 = caloEnergy4Photon( mom2 ) ;
    
    const LHCb::CaloCellID cell1 = cellID( g1 ) ;
    const LHCb::CaloCellID cell2 = cellID( g2 ) ;    
    
    // fill N-tuples
    
    if  ( good ) 
    { 
      fillTuple( tuple , mom1 , mom2 , p12 , prs1e , prs2e , spd1e3x3 , spd2e3x3 , cell1 , cell2 , 0 ) ;
    }
    if ( goodBkg ) 
    {
      fillTuple( tuple , mom1 , mom2 , fake , prs1e , prs2e , spd1e3x3 , spd2e3x3 , cell1 , cell2 , 1 ) ;
    }

    // finally save good photons: 

    photons.insert  ( g1 ) ;
    photons.insert  ( g2 ) ;
    //
  }
  
  for ( Photons::const_iterator iphoton = photons.begin() ; 
        photons.end() != iphoton ; ++iphoton ) 
  {
    // keep these photons 
    LHCb::Particle ph (**iphoton) ;
    desktop()->keep( &ph  ) ;
  }
  
  counter ( "#photons" ) += photons .size  () ;
  
  setFilterPassed ( !photons.empty() ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The factory:
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,Pi0)
// ============================================================================
// The END 
// ============================================================================
