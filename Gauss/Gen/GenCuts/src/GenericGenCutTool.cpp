// $Id: GenericGenCutTool.cpp,v 1.1 2010-08-01 11:18:14 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// STD & STL 
// =============================================================================
#include <cmath>
// =============================================================================
// GaudiKernel
// =============================================================================
#include "GaudiKernel/ToolFactory.h"
// =============================================================================
// GaudiAlg 
// =============================================================================
#include "GaudiAlg/GaudiHistoTool.h"
// =============================================================================
// GaudiUtils
// =============================================================================
#include "GaudiUtils/Aida2ROOT.h"
// =============================================================================
// AIDA 
// =============================================================================
#include "AIDA/IHistogram2D.h"
// =============================================================================
// Generators 
// =============================================================================
#include "MCInterfaces/IGenCutTool.h"
// =============================================================================
// PartProp
// =============================================================================
#include "Kernel/iNode.h"
// =============================================================================
// LHCbMath
// =============================================================================
#include "LHCbMath/LHCbMath.h"
#include "LHCbMath/ValueWithError.h"
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/GenTypes.h"
#include "LoKi/IGenDecay.h"
#include "LoKi/IGenHybridFactory.h"
#include "LoKi/Trees.h"
#include "LoKi/PrintHepMCDecay.h"
#include "LoKi/GenDecayChain.h"
#include "LoKi/GenOscillated.h"
// =============================================================================
// ROOT
// =============================================================================
#include "TH2D.h"
#include "TAxis.h"
// =============================================================================
// Boost
// =============================================================================
#include "boost/algorithm/string/join.hpp"
// =============================================================================
namespace LoKi 
{
  // ===========================================================================
  /** @class   GenCutTool 
   *  
   *  Simple generic implementation of IGenCutTool interface to check if the 
   *  marked daughter are "good", e.g. in LHCbAcceptance  
   *
   *  @code
   *                                            
   *  from Cofigurable import LoKi__GenCutTool as GenCutTool 
   * 
   *  myalg.addTool ( GenCutTool , 'MyGenCutTool' ) 
   * 
   *  myAlg.MyGenCutTool.Decay = " [B0 => ^pi+ ^pi-]CC "
   *
   *  myAlg.MyGenCutTool.Cuts = { 
   *   '[pi+]cc'  : " in_range( 0.010 , GTHETA , 0.400 ) "
   *  }
   *  
   *  @endcode 
   *
   *  One can specify different crietria for different particles,
   *  ignore certain particles (e.g. neutrinos) etc...
   *
   *  @code
   *                                            
   *  from Cofigurable import LoKi__GenCutTool as GenCutTool 
   * 
   *  myalg.addTool ( GenCutTool , 'MyGenCutTool' ) 
   * 
   *  myAlg.MyGenCutTool.Decay = " [B0 -> ( K*(892)0 => ^K- ^pi+) gamma ]CC "
   *
   *  ## no cuts for gamma
   *  myAlg.MyGenCutTool.Cuts = { 
   *   '[pi+]cc'  : " in_range( 0.010 , GTHETA , 0.400 ) "
   *   '[K+]cc'   : " in_range( 0.010 , GTHETA , 0.300 ) "
   *  }
   *
   *  @endcode 
   *
   *  The tool also produce the generatorcut efficiencty 2D-histogram.
   *  The histogram can be cofigured therough property:
   * 
   *  @code
   * 
   *  ## efficiency 2D-histogram: pt ( in GeV) versus rapidity:
   *  myAlg.MyGenCutTool.XAxis= ( 0.0 , 10.0 , 20 , 'GPT/GeV') 
   *  myAlg.MyGenCutTool.YAxis= ( 2.0 ,  5.0 ,  6 , 'GY'     ) 
   *  
   *  ## efficiency 2D-histogram: pt ( in GeV) versus pseudo-rapidity:
   *  myAlg.MyGenCutTool.XAxis= ( 0.0 , 10.0 , 20 , 'GPT/GeV' ) 
   *  myAlg.MyGenCutTool.YAxis= ( 2.0 ,  5.0 ,  6 , 'GETA'    ) 
   *  
   *  @endcode 
   *  @see Gaudi::Histo1DDef  
   * 
   *  @see IGenCutTool
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   */
  class GenCutTool : 
    public         GaudiHistoTool , 
    public virtual     IGenCutTool
  {
    // friend factor for instantiation 
    friend class ToolFactory<LoKi::GenCutTool> ;
    // =========================================================================
  public:
    // =========================================================================
    /// Initialization
    virtual StatusCode initialize () ;
    /// Finalization 
    virtual StatusCode finalize   () ;
    /** Accept events with 'good' particles 
     *  @see IGenCutTool::applyCut.
     */
    virtual bool applyCut
    ( ParticleVector&              particles       , 
      const HepMC::GenEvent*    /* theEvent     */ , 
      const LHCb::GenCollision* /* theCollision */ ) const ;
    // =========================================================================
  protected:
    // =========================================================================
    /** standard constructor 
     *  @param type the tool type (???)
     *  @param name the actual instance name 
     *  @param parent the tool parent 
     */
    GenCutTool 
    ( const std::string&  type   ,                     // the tool type (???)
      const std::string&  name   ,                     // the tool isntance name 
      const IInterface*   parent ) ;                   // the tool parent 
    /// virtual & protected desctructor 
    virtual ~GenCutTool ();                // virtual & protected desctructor 
    // =========================================================================
  protected:
    // =========================================================================
    /// accept the particle 
    bool accept ( const HepMC::GenParticle* particle ) const ;
    // =========================================================================
  protected:
    // =========================================================================
    /// update-handler for the property "Decay" 
    void updateDescriptor ( Property& /* p */ ) ;
    /// update-handler for the properties "Cuts","Preambulo","Factory" 
    void updateCuts       ( Property& /* p */ ) ;
    /// update-handler for the properties "XAxis","YAxis"
    void updateHistos     ( Property& /* p */ ) ;
    // =========================================================================
  private:
    // =========================================================================
    /// decode the decay descriptor 
    StatusCode decodeDescriptor ()  const ;
    /// decode cuts 
    StatusCode decodeCuts       ()  const ;
    /// decode histograms 
    StatusCode decodeHistos     ()  const ;
    // =========================================================================
  private:
    // =========================================================================
    struct TwoCuts
    {
      TwoCuts ( const Decays::iNode&         c1 ,
                const LoKi::GenTypes::GCuts& c2 ) ;
      TwoCuts () ;
      // =======================================================================
      /// the first  cut (particle selector) 
      Decays::Node          first  ; // the first  cut (particle selector) 
      /// the second cut (e.g. acceptance criteria) 
      LoKi::GenTypes::GCut  second ; // the second cut (e.g. acceptance criteria) 
      // =======================================================================
    } ;
    /// vector of cuts 
    typedef std::vector<TwoCuts>  VTwoCuts ; // vector of cuts 
    // =========================================================================
  private:
    // =========================================================================
    /// construct preambulo string 
    std::string preambulo() const ;
    /// calculate the efficiency histo 
    StatusCode getEfficiency() ;
    // =========================================================================
  private:
    // =========================================================================
    /// the decay descriptor 
    std::string                        m_descriptor ;   // the decay descriptor 
    /// the decay finder 
    mutable Decays::IGenDecay::Finder  m_finder     ;   // the decay finder
    /// the LHCb-Acceptance cuts 
    typedef std::map<std::string,std::string>   CMap ;
    CMap                               m_cuts       ; // the 'LHCb-Acceptance cuts
    /// the list of criteria 
    mutable VTwoCuts                   m_criteria   ;   // the list of criteria 
    // =========================================================================    
    /// the preambulo 
    std::vector<std::string>           m_preambulo  ;   // the preambulo 
    /// the factory 
    std::string                        m_factory    ;   // the factory 
    // =========================================================================    
    /// the x-axis for the histogram 
    Gaudi::Histo1DDef              m_xaxis ; // the x-axis for the histogram 
    /// the y-axis for the histogram 
    Gaudi::Histo1DDef              m_yaxis ; // the y-axis for the histogram 
    /// the x-value for histogram 
    mutable LoKi::GenTypes::GFun   m_x     ; // the x-value for histogram 
    /// the y-value for histogram 
    mutable LoKi::GenTypes::GFun   m_y     ; // the y-value for histogram 
    // =========================================================================
    /// the first  histogram ("flow")
    mutable AIDA::IHistogram2D* m_histo1 ;  // the first histogram ("flow")
    /// the second histogram ("accepted")
    mutable AIDA::IHistogram2D* m_histo2 ;  // the second histogram ("accepted")
    /// the third  histogram ("efficiency")
    mutable AIDA::IHistogram2D* m_histo3 ;  // the second histogram ("efficiency")
    // =========================================================================
    mutable bool m_update_decay  ;
    mutable bool m_update_cuts   ;
    mutable bool m_update_histos ;
    // =========================================================================
  } ;
  // ===========================================================================
} //                                                       end of namespace LoKi 
// =============================================================================
namespace 
{
  // ===========================================================================
  /// the invalid tree 
  const Decays::IGenDecay::Tree s_TREE = 
    Decays::Trees::Invalid_<const HepMC::GenParticle*>() ;
  /// the invalid node 
  const Decays::Node            s_NODE = Decays::Nodes::Invalid() ;
  /// the invalid function 
  const LoKi::GenTypes::GFun    s_FUNC = 
    LoKi::BasicFunctors<const HepMC::GenParticle*>::Constant ( -1 )  ;
  /// the invalid predicate 
  const LoKi::GenTypes::GCut    s_PRED = 
    LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant ( false )  ;
  // ===========================================================================
} //                                                  end of anonymous namespace 
// =============================================================================
// constructor 
// =============================================================================
LoKi::GenCutTool::TwoCuts::TwoCuts 
( const Decays::iNode&         c1 ,
  const LoKi::GenTypes::GCuts& c2 ) 
  : first  ( c1 ) 
  , second ( c2 ) 
{}
// =============================================================================
// constructor 
// =============================================================================
LoKi::GenCutTool::TwoCuts::TwoCuts ()  
  : first  ( s_NODE )  
  , second ( s_PRED ) 
{}
// =============================================================================
/* standard constructor 
 *  @param type the tool type (???)
 *  @param name the actual instance name 
 *  @param parent the tool parent 
 */
// =============================================================================
LoKi::GenCutTool::GenCutTool 
( const std::string&  type   ,                     // the tool type (???)
  const std::string&  name   ,                     // the tool isntance name 
  const IInterface*   parent )                     // the tool parent
// : base_class ( type , name , parent ) 
  : GaudiHistoTool ( type , name , parent ) 
// 
  , m_descriptor ( "<Invaild-Decay-Descriptor>" ) 
  , m_finder     ( s_TREE )
  , m_cuts       () 
  , m_criteria   () 
  , m_preambulo  () 
  , m_factory    ( "LoKi::Hybrid::GenTool:PUBLIC" ) 
  , m_xaxis      ( 0.0 , 10.0 , 20 , "GPT/GeV" ) // transverse momentum 
  , m_yaxis      ( 2.0 ,  5.0 ,  6 , "GY"      ) // rapidity
  , m_x          ( s_FUNC )
  , m_y          ( s_FUNC )
//
  , m_histo1 ( 0 ) 
  , m_histo2 ( 0 ) 
  , m_histo3 ( 0 ) 
//
{
  //
  declareInterface<IGenCutTool>( this ) ;
  //
  declareProperty 
    ( "Decay"           ,
      m_descriptor      , 
      "The decay descriptor (with marked particles) to be tested" ) 
    -> declareUpdateHandler 
    ( &LoKi::GenCutTool::updateDescriptor  , this ) ;
  //
  m_cuts [ "StableCharged" ] = " in_range ( 0.010 , GTHETA , 0.400 ) " ;
  m_cuts [ "Gamma"         ] = " in_range ( 0.005 , GTHETA , 0.400 ) " ;
  declareProperty 
    ( "Cuts"      , 
      m_cuts      , 
      "The map with selection criteria : { Node : Cuts } " ) 
    -> declareUpdateHandler 
    ( &LoKi::GenCutTool::updateCuts  , this ) ;
  //
  m_preambulo.push_back("from GaudiKernel.SystemOfUnits import MeV,GeV,mm,cm") ;
  declareProperty 
    ( "Preambulo" , 
      m_preambulo , 
      "Preambulo to be used for LoKi/Bender Hybrid factory" ) 
    -> declareUpdateHandler 
    ( &LoKi::GenCutTool::updateCuts  , this ) ;
  //
  declareProperty 
    ( "Factory"   , 
      m_factory   , 
      "The type/name for LoKi/Bender Hybrid factory" ) 
    -> declareUpdateHandler 
    ( &LoKi::GenCutTool::updateCuts  , this ) ;
  // 
  declareProperty 
    ( "XAxis"     , m_xaxis , "X-axis for efficiency histogram" ) 
    -> declareUpdateHandler 
    ( &LoKi::GenCutTool::updateHistos  , this ) ;
  //
  declareProperty 
    ( "YAxis"     , m_yaxis , "Y-axis for efficiency histogram" ) 
    -> declareUpdateHandler 
    ( &LoKi::GenCutTool::updateHistos  , this ) ;
}
// =============================================================================
// destructor 
// =============================================================================
LoKi::GenCutTool::~GenCutTool() {}
// =============================================================================
// update-handler for the property "DecayDescriptor" 
// =============================================================================
void LoKi::GenCutTool::updateDescriptor ( Property& /* p */ ) 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // 
  m_update_decay = true ;
}
// =============================================================================
// update-handler for the properties "Cuts","Preambulo","Factory" 
// =============================================================================
void LoKi::GenCutTool::updateCuts       ( Property& /* p */ ) 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // 
  m_update_cuts   = true ;
  m_update_histos = true ;
}
// =============================================================================
// update-handler for the properties "XAxis","YAxis"
// =============================================================================
void LoKi::GenCutTool::updateHistos     ( Property& /* p */ ) 
{
  // no action if not yet initialized 
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  // 
  m_update_histos = true ;
}
// =============================================================================
// decode the decay descriptor 
// =============================================================================
StatusCode LoKi::GenCutTool::decodeDescriptor ()  const 
{
  //
  m_update_decay = true ;
  // get the factory:
  Decays::IGenDecay* factory = tool<Decays::IGenDecay>("LoKi::GenDecay", this ) ;
  // use the factory:
  Decays::IGenDecay::Tree tree = factory->tree ( m_descriptor ) ;
  if ( !tree ) 
  { return Error( "Unable to decode the descriptor '" + m_descriptor + "'") ; }
  //
  m_finder = Decays::IGenDecay::Finder ( tree ) ;
  if ( !m_finder ) 
  { return Error( "Unable to create the finder '" + m_descriptor + "'") ; }
  //
  m_update_decay = false ;
  //
  release ( factory ) ;
  //
  return StatusCode::SUCCESS ;
} 
// =============================================================================
// decode cuts 
// =============================================================================
StatusCode LoKi::GenCutTool::decodeCuts ()  const 
{
  //
  m_update_cuts = true ;
  // 
  m_criteria.clear() ;
  //
  // get the factory:
  LoKi::IGenHybridFactory* factory = 
    tool<LoKi::IGenHybridFactory>( m_factory , this ) ;
  // get the factory:
  Decays::IGenDecay*       nodes = tool<Decays::IGenDecay>("LoKi::GenDecay", this ) ;
  // 
  // decode cuts :
  for ( CMap::const_iterator entry = m_cuts.begin() ; 
        m_cuts.end() != entry ; ++entry ) 
  {
    TwoCuts item ;
    // use node-factory
    item.first  = nodes->node ( entry->first ) ;
    if ( !item.first ) 
    { return Error ("Unable to decode the node '" + entry->first + "'") ; }
    // use the cut-factory:
    StatusCode sc = factory->get 
      ( entry->second   , 
        item.second     , 
        preambulo()     ) ;
    if ( sc.isFailure() ) 
    { return Error ("Unable to decode the cut '" + entry->second+ "'", sc ) ; }
    //
    m_criteria.push_back ( item ) ;
  }
  //
  m_update_cuts = false ;
  //
  release ( nodes   ) ;
  release ( factory ) ;
  // 
  return StatusCode::SUCCESS ;
}
// =============================================================================
// decode Histos 
// =============================================================================
StatusCode LoKi::GenCutTool::decodeHistos ()  const 
{
  m_update_histos = true  ;
  // reset histos 
  if ( 0 != m_histo1 ) { m_histo1 -> reset() ; m_histo1 = 0 ; }
  if ( 0 != m_histo2 ) { m_histo2 -> reset() ; m_histo2 = 0 ; }
  if ( 0 != m_histo3 ) { m_histo3 -> reset() ; m_histo3 = 0 ; }
  // reset functions 
  m_x = s_FUNC ;
  m_y = s_FUNC ;
  //
  // aquire the factory: 
  LoKi::IGenHybridFactory* factory = 
    tool<LoKi::IGenHybridFactory>( m_factory , this ) ;
  //
  StatusCode sc = factory -> get ( m_xaxis.title() , 
                                   m_x             , 
                                   preambulo()     ) ;
  
  if ( sc.isFailure() ) 
  { return Error ("Unable to decode the X-axis '" + m_xaxis.title() + "'", sc ) ; }
  sc = factory -> get ( m_yaxis.title() , 
                        m_y             , 
                        preambulo()     ) ;
  if ( sc.isFailure() ) 
  { return Error ("Unable to decode the Y-axis '" + m_xaxis.title() + "'", sc ) ; }
  
  if ( produceHistos() ) 
  {
    m_histo1 = book2D 
      ( "All"               , 
        m_descriptor        , 
        //
        m_xaxis.lowEdge  () , 
        m_xaxis.highEdge () , 
        m_xaxis.bins     () , 
        //
        m_yaxis.lowEdge  () , 
        m_yaxis.highEdge () , 
        m_yaxis.bins     () ) ; 
    //
    m_histo2 = book2D 
      ( "Accepted"          , 
        m_descriptor        , 
        //
        m_xaxis.lowEdge  () , 
        m_xaxis.highEdge () , 
        m_xaxis.bins     () , 
        //
        m_yaxis.lowEdge  () , 
        m_yaxis.highEdge () , 
        m_yaxis.bins     () ) ; 
    //
    m_histo3 = book2D 
      ( "Efficiency"        , 
        m_descriptor        , 
        //
        m_xaxis.lowEdge  () , 
        m_xaxis.highEdge () , 
        m_xaxis.bins     () , 
        //
        m_yaxis.lowEdge  () , 
        m_yaxis.highEdge () , 
        m_yaxis.bins     () ) ; 
  }
  // ==========================================================================
  m_update_histos = false ;
  release ( factory ) ;
  // 
  return StatusCode::SUCCESS ;
}
// =============================================================================
// intialize the tool 
// =============================================================================
StatusCode LoKi::GenCutTool::initialize() 
{
  // initialize the base 
  StatusCode sc = GaudiHistoTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  // decode the decay descriptor 
  sc = decodeDescriptor () ;
  if ( sc.isFailure() ) 
  { return Error("Unable to decode the descriptor '"+m_descriptor+"'", sc ) ; }
  //
  // decode the cuts
  sc = decodeCuts       () ;
  if ( sc.isFailure() ) { return Error("Unable to decode cuts"   , sc ) ; }
  //
  // decode the histos 
  sc = decodeHistos     () ;
  if ( sc.isFailure() ) { return Error("Unable to decode histos" , sc ) ; }
  //
  return StatusCode::SUCCESS ;
}
// =============================================================================
// finalize the tool 
// =============================================================================
StatusCode LoKi::GenCutTool::finalize () 
{
  /// calculate the efficiency 
  getEfficiency() ;
  /// finalize the base class 
  return GaudiHistoTool::finalize () ;
}
// =============================================================================
// accept the particle 
// =============================================================================
bool LoKi::GenCutTool::accept ( const HepMC::GenParticle* particle ) const 
{
  /// check the argument 
  if ( 0 == particle ) { return false ; }                    // RETURN
  ///
  LoKi::GenTypes::GenContainer particles ;
  m_finder.findDecay ( LoKi::GenTypes::GenContainer( 1 , particle ) , particles ) ;
  //
  bool result = true ;
  if ( particles.empty() ) 
  { 
    Warning ( "No particles are selected" , 10 , StatusCode::SUCCESS ) . ignore() ;
    result = false ;
  }
  //
  for ( LoKi::GenTypes::GenContainer::const_iterator ip = particles.begin() ; 
        result && particles.end() != ip ; ++ip ) 
  {
    const HepMC::GenParticle* p = *ip ;
    if ( 0 == p ) { continue ; }
    const LHCb::ParticleID pid( p->pdg_id() ) ;
    //
    for ( VTwoCuts::const_iterator icuts = m_criteria.begin() ; 
          result && m_criteria.end() != icuts ; ++icuts ) 
    {
      if ( icuts->first( pid ) && !icuts->second( p ) ) { result = false ; } // RESULT! 
    }
  }
  //
  // fill the histogram 
  //
  if ( 0 != m_histo1 && 0 != m_histo2 ) 
  {
    const double x = m_x ( particle ) ;
    const double y = m_y ( particle ) ;
    m_histo1 -> fill ( x , y , 1      ) ;
    m_histo2 -> fill ( x , y , result ) ;
  }
  //
  return result ;
} 
// ============================================================================
// construct preambulo string 
// ============================================================================
std::string LoKi::GenCutTool::preambulo() const    
{ return boost::algorithm::join( m_preambulo , "\n" ) ; }
// ============================================================================
namespace 
{
  // ==========================================================================
  /// integer number ?
  inline bool non_integer ( const double value ) 
  {
    return 
      !LHCb::Math::equal_to_double ( value , std::floor ( value ) ) ; 
  }
  // ==========================================================================
  /// evaluate the binomial efficiency  
  inline Gaudi::Math::ValueWithError 
  efficiency ( const double n , const double N ) 
  {
    if ( 0 >= N ) { Gaudi::Math::ValueWithError ( 0 , 1 ) ; }
    //
    const double n1 = std::max ( n      , 1.0 ) ;
    const double n2 = std::max ( N - n  , 1.0 ) ;
    //
    return Gaudi::Math::ValueWithError ( n/N , n1*n2/(N*N*N) ) ;  
  }
  // ==========================================================================
}
// ============================================================================
// calculate the efficiency histo 
// ============================================================================
StatusCode LoKi::GenCutTool::getEfficiency() 
{
  //
  if ( 0 == m_histo1 || 0 == m_histo2 || 0 == m_histo3 ) 
  { return Error("Unable to calculate the efficiency") ; }
  //
  // reset the efficiency histo 
  m_histo3->reset() ;
  //
  TH2D* h1 = Gaudi::Utils::Aida2ROOT::aida2root ( m_histo1 ) ;
  TH2D* h2 = Gaudi::Utils::Aida2ROOT::aida2root ( m_histo2 ) ;
  TH2D* h3 = Gaudi::Utils::Aida2ROOT::aida2root ( m_histo3 ) ;
  //
  if ( 0 == h1 || 0 == h2 || 0 == h3 ) 
  { return Error("Unable to access ROOT historgams") ; }
  //
  TAxis* xaxis = h1 -> GetXaxis() ;
  TAxis* yaxis = h1 -> GetYaxis() ;
  //
  unsigned int bad_bins  = 0 ;
  unsigned int null_bins = 0 ;
  //
  for ( int ix = 1 ; ix <= xaxis->GetNbins() ; ++ix ) 
  {
    for ( int iy = 1 ; iy <= yaxis->GetNbins() ; ++iy ) 
    {
      const double N  = (long) h1 -> GetBinContent ( ix , iy ) ;
      // const double Ne = h1 -> GetBinError   ( ix , iy ) ;
      //
      if ( N < 0  ) // || !non_integer ( N )  ) 
      {
        Warning ("Can't calculate the efficiency: illegal content N") ;
        ++bad_bins ;
        continue ;
      }
      //
      const double n  = (long) h2 -> GetBinContent ( ix , iy ) ;
      // const double ne = h2 -> GetBinError   ( ix , iy ) ;
      //
      if ( n < 0 || N < n ) //  || !non_integer ( N )  ) 
      {
        Warning ("Can't calculate the efficicency: illegal content n") ;
        ++bad_bins ;
        continue ;
      }
      // 
      if ( 0 == N ) { ++null_bins ; }
      //
      Gaudi::Math::ValueWithError eff = efficiency ( n , N ) ;
      if ( eff.cov2() < 0 ) 
      {
        Warning ("Can't calculate the binomial efficiency") ;
        ++bad_bins ;
        continue ;
      }
      // 
      h3->SetBinContent ( ix , iy , eff.value () ) ;
      h3->SetBinError   ( ix , iy , eff.error () ) ;
    }
  }
  //
  if ( 0 != bad_bins || 0 != null_bins ) 
  { warning() << " # of bad/null bins: " 
              << bad_bins << "/" << null_bins << endmsg ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// Accept events with 'good' particles 
// ============================================================================
bool LoKi::GenCutTool::applyCut
( ParticleVector&              particles       , 
  const HepMC::GenEvent*    /* theEvent     */ , 
  const LHCb::GenCollision* /* theCollision */ ) const 
{
  //
  StatEntity& cnt1 = counter("accept_particles" ) ;
  StatEntity& cnt2 = counter("accept_events"    ) ;
  //
  ParticleVector good ;
  good.reserve ( particles.size() ) ;
  //
  for ( ParticleVector::const_iterator it = particles.begin () ;
        particles.end   () != it ; ++it ) 
  {
    HepMC::GenParticle* p = *it ;
    if ( 0 == p ) { continue ; }
    //
    if ( accept ( p ) ) 
    {
      good.push_back ( p ) ;
      cnt1 += true ;
    }
  }
  //
  particles.clear() ;
  particles.insert ( particles.end() , good.begin () , good.end () ) ;
  //
  bool result = !particles.empty() ;
  //
  cnt2 += result ;
  //
  return result ;
}
// ============================================================================
// factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,GenCutTool) 
// ============================================================================
// The END 
// ============================================================================
