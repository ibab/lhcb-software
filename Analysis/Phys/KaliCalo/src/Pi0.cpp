// $Id:$
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
#include "Event/Vertex.h"
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/IHybridFactory.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/Photons.h"
#include "LoKi/BasicFunctors.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/ClusterFunctors.h"
// ============================================================================
// ICaloDigits4TrackTool
// ============================================================================
#include "CaloInterfaces/ICaloDigits4Track.h"
// ============================================================================
// DeCalorimeter
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
namespace Kali
{
  // ==========================================================================
  /** @class Pi0
   *  Simple algorithm for Calorimeter Cailbration using pi0 peak
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-09-28
   *
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
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
      , m_mirror        ( false )
      , m_veto_dm       ( -1 * Gaudi::Units::MeV )
      , m_veto_chi2     ( -1  )
      , m_pi0CutExp     ( "PT > 200*MeV*(7-ETA)" )
      , m_pi0Cut        ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant ( false ) )
      , m_spdDigitsTool (  0  )
      , m_ecal          (  0  )
        // histograms
      , m_h1    ( 0 )
      , m_h2    ( 0 )
      , m_h3    ( 0 )
      , m_h4    ( 0 )
    {
      declareProperty
        ( "Mirror" ,
          m_mirror ,
          "Flag to activate Albert's trick with backroung estimation" )
        -> declareUpdateHandler ( &Kali::Pi0::mirrorHandler , this ) ;
      //
      declareProperty
        ( "Pi0VetoDeltaMass" ,
          m_veto_dm          ,
          "Delta-Mass for pi0-veto" )
        -> declareUpdateHandler ( &Kali::Pi0::vetoHandler , this ) ;
      declareProperty
        ( "Pi0VetoChi2"      ,
          m_veto_chi2        ,
          "Chi2 for pi0-veto" )
        -> declareUpdateHandler ( &Kali::Pi0::vetoHandler , this ) ;
      //
      declareProperty
        ( "Pi0Cut"   ,
          m_pi0CutExp ,
          "Predicate for Pi0 (LoKi/Bender expression)" ) ;
      //
      Property* histos = Gaudi::Utils::getProperty ( this , "HistoProduce" ) ;
      Assert ( 0 != histos , "Unable to get property 'HistoProduce'" ) ;
      if ( 0 != histos && 0 == histos->updateCallBack() )
      { histos -> declareUpdateHandler ( &Kali::Pi0::histosHandler , this ) ; }
      //
      setProperty ( "HistoProduce" , false ) ;
      //
      ToolMap _combiners ;
      _combiners[""] = "MomentumCombiner" ;
      StatusCode sc = setProperty ( "ParticleCombiners" , _combiners ) ;
      Assert ( sc.isSuccess() , "Unable to set the proper ParticleCombiner" );
      //
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
      const Gaudi::XYZPoint&  ,
      const Gaudi::XYZPoint&  ,
      const LHCb::CaloCluster* ,
      const LHCb::CaloCluster* ,          
      const int               ) ;
    // ========================================================================
    double caloEnergy4Photon ( const Gaudi::LorentzVector&  p ) ;
    double getSeedCellEnergy ( const LHCb::CaloCluster* ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// setup monitoringhistograms
    void setupHistos () ;
    // ========================================================================
  public:
    // ========================================================================
    /// update handler for 'Mirror' property
    void mirrorHandler ( Property& p ) ; // update handler for 'Mirror' property
    /// update handler for 'Veto' properties
    void vetoHandler   ( Property& p ) ; // update handler for 'Veto' properties
    /// update handler for 'HistoProduce' property
    void histosHandler ( Property& p ) ; // update handler for property
    // ========================================================================
  private:
    // ========================================================================
    /// use Albert's trick?
    bool  m_mirror     ;                                // use Albert's trick?
    /// Delta-mass for pi0-veto
    double m_veto_dm   ;                               // Delta-mass for pi0-veto
    /// chi2       for pi0-veto
    double m_veto_chi2 ;                               // Delta-mass for pi0-veto
    /// pi0-Cut functor
    std::string       m_pi0CutExp ;                    //       Pi0-Cut functor
    /// pi0-Cut functor expression 
    LoKi::Types::Cut  m_pi0Cut    ;               // pi0-Cut functor expression 
    /// Tool for retrieving SPD digits info
    ICaloDigits4Track*  m_spdDigitsTool ; // Tool for retrieving SPD digits info
    // DeCalorimeter object for ECAL
    DeCalorimeter*      m_ecal ;          //      DeCalorimeter object for ECAL
    // ========================================================================
  private:
    // ========================================================================
    /// histogram with all pi0s
    AIDA::IHistogram1D* m_h1 ; // histogram with all pi0s
    /// histogram with max(eprs1,eprs2)<10 MeV
    AIDA::IHistogram1D* m_h2 ; // histogram with max(eprs1,eprs2)<10 MeV
    /// histogram with eprs1< 10 MeV , eprs2 >10 MeV
    AIDA::IHistogram1D* m_h3 ; // histogram with eprs1< 10 MeV , eprs2 >10 MeV
    /// histogram with min(eprs1,eprs2)>10 MeV
    AIDA::IHistogram1D* m_h4 ; // histogram with min(eprs1,eprs2)>10 MeV
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
// update handler for 'Veto' properties
// ============================================================================
void Kali::Pi0::vetoHandler ( Property&  p  )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  info() << "Pi0 Veto is : " << p << endmsg ;
  //
}
// ============================================================================
// update handler for 'HistoProduce' property
// ============================================================================
void Kali::Pi0::histosHandler ( Property& /* p */ )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  //
  setupHistos () ;
  //
}
// ============================================================================
// setup monitoring histograms
// ============================================================================
void Kali::Pi0::setupHistos ()
{
  using Gaudi::Units::MeV ;

  if ( produceHistos() )
  {
    Warning ( "Monitoring histograms are   activated" , StatusCode::SUCCESS ) ;
    if ( 0 == m_h1 ) { m_h1 = book ( "mpi0"               , 0 , 250 * MeV , 250 ) ; }
    if ( 0 == m_h2 ) { m_h2 = book ( "mpi0-Prs_ll_10_MeV" , 0 , 250 * MeV , 250 ) ; }
    if ( 0 == m_h3 ) { m_h3 = book ( "mpi0-Prs_lg_10_MeV" , 0 , 250 * MeV , 250 ) ; }
    if ( 0 == m_h4 ) { m_h4 = book ( "mpi0-Prs_gg_10_MeV" , 0 , 250 * MeV , 250 ) ; }
  }
  else
  {
    Warning ( "Monitoring histograms are deactivated!", StatusCode::SUCCESS ) ;
    m_h1 = 0 ;
    m_h2 = 0 ;
    m_h3 = 0 ;
    m_h4 = 0 ;
  }
}
// ============================================================================
// tuple fill helper function
// ============================================================================
void Kali::Pi0::fillTuple
( const Tuple&                tuple  ,
  const Gaudi::LorentzVector& p1     ,
  const Gaudi::LorentzVector& p2     ,
  const Gaudi::LorentzVector& p12    ,
  const double                prs1e  ,
  const double                prs2e  ,
  const double                spd1e  ,
  const double                spd2e  ,
  const LHCb::CaloCellID&     cell1  ,
  const LHCb::CaloCellID&     cell2  ,
  const Gaudi::XYZPoint&      point1 ,
  const Gaudi::XYZPoint&      point2 ,
  const LHCb::CaloCluster*    clus1  ,
  const LHCb::CaloCluster*    clus2  ,
  const int                   bkg    )
{

  using Gaudi::Units::GeV ;

  // fill N-tuple
  tuple -> column ( "m12"  , p12.M()  ) ;

  tuple -> column ( "p0"   , p12      ) ;
  tuple -> column ( "g1"   , p1       ) ;
  tuple -> column ( "g2"   , p2       ) ;

  tuple -> column ( "prs1" , prs1e    ) ;
  tuple -> column ( "prs2" , prs2e    ) ;

  // here 1 MeV precision is OK for us...
  const unsigned int ipt  =
    LHCb::Math::round ( std::max ( 0.0 , std::min ( p12.Pt() , 5 * GeV ) ) ) ;
  const unsigned int ipt1 =
    LHCb::Math::round ( std::max ( 0.0 , std::min ( p1 .Pt() , 5 * GeV ) ) ) ;
  const unsigned int ipt2 =
    LHCb::Math::round ( std::max ( 0.0 , std::min ( p2 .Pt() , 5 * GeV ) ) ) ;
  //
  tuple -> column ( "pt"   , ipt  ) ;
  tuple -> column ( "pt1"  , ipt1 ) ;
  tuple -> column ( "pt2"  , ipt2 ) ;

  const unsigned short _indx1 = cell1.index() ;
  const unsigned short _indx2 = cell2.index() ;

  tuple -> column ( "ind1" , _indx1 ) ;
  tuple -> column ( "ind2" , _indx2 ) ;

  const int ispd1 =
    LHCb::Math::round ( std::max ( 0.0 , std::min ( 5 * spd1e , 30.0 ) ) ) ;
  const int ispd2 =
    LHCb::Math::round ( std::max ( 0.0 , std::min ( 5 * spd2e , 30.0 ) ) ) ;
  //
  tuple -> column ( "spd1" , ispd1 , 0 , 31 ) ;
  tuple -> column ( "spd2" , ispd2 , 0 , 31 ) ;

  tuple -> column ( "bkg"  , bkg , 0 , 2    ) ;

  Gaudi::XYZVector vec = point2 - point1 ;
  double cSize = std::max (  m_ecal->cellSize ( cell1 ) ,
                             m_ecal->cellSize ( cell2 ) ) ;
  double dist = ( cSize > 0) ? vec.Rho() / cSize : 0 ;
  tuple -> column ( "dist" , dist ) ;

  // Energies
  const double eRaw1 = clus1->position().e() ;
  const double eRaw2 = clus2->position().e() ;
  
  tuple -> column ( "eClus1" , eRaw1 ) ;
  tuple -> column ( "eClus2" , eRaw2 ) ;

  const int nClus1 = clus1->entries().size() ;
  const int nClus2 = clus2->entries().size() ;

  tuple -> column ( "nClus1" , nClus1 ) ;
  tuple -> column ( "nClus2" , nClus2 ) ;

  const double eSeed1 = getSeedCellEnergy( clus1 ) ;
  const double eSeed2 = getSeedCellEnergy( clus2 ) ;

  tuple -> column ( "eSeed1" , eSeed1 ) ;
  tuple -> column ( "eSeed2" , eSeed2 ) ;
  
  tuple -> write () ;
}
// ============================================================================
// Photon CaloDigitEnergy
// ============================================================================
double Kali::Pi0::caloEnergy4Photon( const Gaudi::LorentzVector&  p )
{
  typedef LHCb::CaloDigit::Set SET ;
  SET digits ;
  Gaudi::XYZPoint point( 0 , 0 , 0 ) ;
  Gaudi::XYZVector vector = p.Vect() ;
  typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> LINE ;
  LINE line( point , vector ) ;
  m_spdDigitsTool->collect ( line , digits );
  double e = 0.0 ;
  for ( SET::const_iterator digit = digits.begin() ;
        digits.end() != digit ; digit++ ){
    e += (*digit)->e() ;
  }
  return e ;
}
// ============================================================================
// Energy of the seed cell of a CaloCluster
// ============================================================================
double Kali::Pi0::getSeedCellEnergy ( const LHCb::CaloCluster* cluster )
{
  const LHCb::CaloCluster::Entries& entries = cluster->entries();
  LHCb::CaloCluster::Entries::const_iterator iseed = 
    LHCb::ClusterFunctors::locateDigit 
    ( entries.begin () ,entries.end   () , LHCb::CaloDigitStatus::SeedCell ) ;
  //
  if( entries.end() == iseed ) return 0.0;
  //
  return iseed->digit()->e();
}
// ============================================================================
// the proper initialization
// ============================================================================
StatusCode Kali::Pi0::initialize  ()                // the proper initialzation
{
  //
  StatusCode sc = LoKi::Algo::initialize();
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( m_mirror )
  { Warning ( "Albert's trick         is   activated!", StatusCode::SUCCESS ) ; }
  else
  { Warning ( "Albert's trick         is deactivated!", StatusCode::SUCCESS ) ; }
  //
  if ( 0 < m_veto_dm || 0 < m_veto_chi2 )
  { Warning ( "Pi0-Veto               is   activated!", StatusCode::SUCCESS ) ; }
  //
  setupHistos() ;
  //
  LoKi::IHybridFactory* factory = 
    tool<LoKi::IHybridFactory> ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" ) ;
  sc = factory -> get ( m_pi0CutExp , m_pi0Cut ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to compile Pi0-Cut predicate" , sc ) ; }   
  //
  // Load tool for SPD
  m_spdDigitsTool = 
    tool<ICaloDigits4Track>( "SpdEnergyForTrack" , this ) ; 
  sc = Gaudi::Utils::setProperty ( m_spdDigitsTool , "AddNeighbours" , 1 ) ;
  if ( sc.isFailure() )
  { return Error ( "Unable to configure 'SpdEnergyForTrack' tool", sc ) ; }
  //
  m_ecal = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal);
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// the only one essential method
// ============================================================================
StatusCode Kali::Pi0::analyse    ()            // the only one essential method
{
  using namespace LoKi          ;
  using namespace LoKi::Types   ;
  using namespace LoKi::Cuts    ;
  using namespace LoKi::Photons ;

  using Gaudi::Units::MeV ;

  LHCb::CaloDataFunctor::DigitFromCalo spd ( "Spd" ) ;
  LHCb::CaloDataFunctor::DigitFromCalo prs ( "Prs" ) ;

  const double ptCut_Gamma = cutValue ( "PtGamma" ) ;
  const double spdCut      = cutValue ( "SpdCut"  ) ;

  // get all photons with
  Range all   = select ( "all_g" ,        "gamma" == ID       ) ;
  Range gamma = select ( "g"     , all , ( PT > ptCut_Gamma ) ) ;
  
  counter ( "#gamma_all") += all   . size () ;
  counter ( "#gamma"    ) += gamma . size () ;
  
  Tuple tuple = nTuple ( "Pi0-Tuple" ) ;
  
  const bool make_tuples = produceNTuples() ;
  
  LHCb::CaloDigit::Set digits ;
  
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
    Gaudi::LorentzVector _p1 = g1->momentum() ;
    _p1.SetPx ( -_p1.Px () ) ;
    _p1.SetPy ( -_p1.Py () ) ;
    const Gaudi::LorentzVector fake = ( _p1 + g2->momentum() ) ;
    LHCb::Particle * fakePi0 = pi0.particle()->clone() ;
    fakePi0->setMomentum(fake) ;
    
    bool good    =             ( m12      < 335 * MeV ) ;
    bool goodBkg = m_mirror && ( fake.M() < 335 * MeV ) ;
    
    if ( (!good)  && (!goodBkg) ) { continue ; }   // CONTINUE!!!
    
    double spd1e = seedEnergyFrom ( g1 , spd ) ;
    if ( 0 < spd1e ) { continue ; }                // CONTINUE  
    
    double spd2e = seedEnergyFrom ( g2 , spd ) ;
    if ( 0 < spd2e ) { continue ; }                // CONITUNE 
    
    // order the photons according energy in preshower
    double prs1e = energyFrom ( g1 , prs ) ;
    double prs2e = energyFrom ( g2 , prs ) ;
    if ( prs1e > prs2e )
    {
      std::swap ( g1    , g2    ) ;
      std::swap ( prs1e , prs2e ) ;
      std::swap ( spd1e , spd2e ) ;
    }
    
    const LHCb::CaloHypo* hypo1 = hypo ( g1 )  ;
    if ( 0 == hypo1 ) { continue ; }                       // CONTINUE 
    const LHCb::CaloHypo* hypo2 = hypo ( g2 )  ;
    if ( 0 == hypo2 ) { continue ; }                       // CONTINUE 
    
    const LHCb::CaloCluster* cluster1 = cluster ( g1 )  ;
    if ( 0 == cluster1 ) { continue ; }                    // CONITNUE 
    const LHCb::CaloCluster* cluster2 = cluster ( g2 )  ;
    if ( 0 == cluster2 ) { continue ; }                    // CONTINUE
    
    /// apply pi0-cut:
    //if  ( !m_pi0Cut ( pi0 ) ) { continue ; }               // CONTINUE
    good    = good    && m_pi0Cut ( pi0 )     ;
    goodBkg = goodBkg && m_pi0Cut ( fakePi0 ) ;
    if ( (!good) && (!goodBkg) ) { continue ; }            // CONTINUE
    
    const Gaudi::LorentzVector mom1 = g1->momentum() ;
    const Gaudi::LorentzVector mom2 = g2->momentum() ;
    
    const double spd1e3x3 = caloEnergy4Photon ( g1 -> momentum () ) ;
    const double spd2e3x3 = caloEnergy4Photon ( g2 -> momentum () ) ;

    // apply cut on 3x3 SPD
    if ( spdCut < spd1e3x3 ) { continue ; }                 // CONTINUE 
    if ( spdCut < spd2e3x3 ) { continue ; }                 // CONTINUE 
    
    // pi0-veto ?
    bool veto = true ;
    if ( 0 < m_veto_dm || 0 < m_veto_chi2 ) 
    { veto = pi0Veto ( g1 , g2 , all   , m_veto_dm , m_veto_chi2 ) ; }
    if ( !veto ) { continue ; }                                           // CONTINUE
    

    if ( good && m12 < 250 * MeV )
    {
      if ( 0 != m_h1                                         ) { m_h1 -> fill ( m12 ) ; }
      if ( 0 != m_h2 && prs2e < 10 * MeV                     ) { m_h2 -> fill ( m12 ) ; }
      if ( 0 != m_h3 && prs1e < 10 * MeV && prs2e > 10 * MeV ) { m_h3 -> fill ( m12 ) ; }
      if ( 0 != m_h4 &&                     prs1e > 10 * MeV ) { m_h4 -> fill ( m12 ) ; }
    }
    
    // finally save good photons:
    photons.insert  ( g1 ) ;
    photons.insert  ( g2 ) ;
    
    if  ( !make_tuples ) { continue ; }                      // CONTINUE 
    
    const LHCb::CaloCellID cell1 = cellID( g1 ) ;
    const LHCb::CaloCellID cell2 = cellID( g2 ) ;
    

    Gaudi::XYZPoint point1 ( hypo1->position()->x() ,
                             hypo1->position()->y() ,
                             hypo1->position()->z() );
    

    // fill N-tuples
    if  ( good )
    {
      const LHCb::CaloPosition* p2 = hypo2->position() ;
      const Gaudi::XYZPoint point2 ( p2 -> x () ,
                                     p2 -> y () ,
                                     p2 -> z () );
      fillTuple ( tuple , mom1 , mom2 , p12 , prs1e , prs2e ,
                  spd1e3x3 , spd2e3x3 , cell1 , cell2 , point1 , point2 ,
                  cluster1 , cluster2 , 0 ) ;
    }
    if ( goodBkg )
    {
      const LHCb::CaloPosition* p2 = hypo2->position() ;
      const Gaudi::XYZPoint point2Sym ( - p2 -> x () , - p2 -> y () , p2 -> z () );
      fillTuple( tuple , mom1 , mom2 , fake , prs1e , prs2e ,
                 spd1e3x3 , spd2e3x3 , cell1 , cell2 , point1 , point2Sym ,
                 cluster1 , cluster2 , 1 ) ;
    }
    
    //
  }
  
  for ( Photons::const_iterator iphoton = photons.begin() ;
        photons.end() != iphoton ; ++iphoton )
  {
    // keep these photons
    LHCb::Particle ph (**iphoton) ;
    this->markTree( &ph  ) ;
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
