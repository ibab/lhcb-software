// $Id: FilterDesktop.cpp,v 1.19 2010-03-18 19:16:00 jpalac Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinci Kernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IPlotTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/algorithm/string.hpp"
// ============================================================================
namespace 
{
  // ==========================================================================
  /// make the plots ? 
  inline bool validPlots ( const std::string& name ) 
  {
    if ( name.empty() ) { return false ; }
    return "none" != boost::to_lower_copy ( name ) ;
  }
  /// local "none"
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant       _PBOOL ;  
  /// local "none"
  typedef LoKi::BasicFunctors<LHCb::Particle::ConstVector>::BooleanConstant _CBOOL ;
  // ==========================================================================
}
// ============================================================================
using namespace LoKi ;
// ============================================================================
/** @class FilterDesktop 
 *  LoKi/Bender "Hybrid" (re)implementation of simple algorithm with 
 *  filters the input particles ("FilterDesktop"). Stores pointers to the 
 *  selected input particles, as defined by InputLocations.
 *
 *  The important properties (in addtion to the base class' properties)
 *    - "Factory"   : the type/name of LoKi/Bender 'hybrid' factory
 *    - "Preambulo" : the preambulo, to be used for Bender/Python script
 *    - "Code"      : the actual Bender/Python code 
 *    - "InputPlotsTool"   : the type/name of PlotTool for 'input' particles 
 *    - "InputPlotsPath"   : THS path for 'input' plots 
 *    - "OutputPlotsTool"  : the type/name of PlotTool for 'output' particles 
 *    - "OutputPlotsPath"  : THS path for 'output' plots 
 *
 *  The important counters (in addition to counters form DVAlgorithm)
 *    - "#inputs"    : number of inptu particles 
 *    - "#passed"    : number of particles, which passed criteria
 *    - "efficiency" : the selection efficiency (per particle) 
 *  
 *
 *  @see DVAlgorithm 
 *  @see CombineParticles 
 *  @see IHybridFactory
 * 
 *
 * The embedded monitoring:
 *  
 *    - <c>"Monitor"</c>  : swithch on/off the monitoring functors 
 *    - <c>"PreMonitor"</c> : monitor functor for input particles (before cuts)
 *    - <c>"PostMonitor"</c> : monitor functor for output particles (after all cuts)
 * 
 *  Empty string implies no monitorig.
 *
 *  Each monitoring functor has a signature 
 *  <code> LHCb::Particle::ConstVector -> book </code>, 
 *  that corresponds to internal LoKi type <code>LoKi::types::CutVal</code>
 *   @see LoKi::Types::CutVal
 *   @see LoKi::Types::CutVals
 *   @see LoKi::BasicFunctors 
 *
 *  Examples of embedded monitoring:
 *  
 *  @code 
 * 
 *   myAlg = FilterDesktop ( ... ) 
 * 
 *   myAlg.Monitor     = True 
 *   myAlg.PreMonitor  = "  monitor ( SIZE , ... )   > -1 "
 *   myAlg.PostMonitor = """
 *
 *       tee     ( monitor ( SIZE                  ,  ... ) ) >>
 *       tee     ( monitor ( max_value( PT )       ,  ... ) ) >>
 *       process ( monitor (            PT         ,  ... ) ) >>
 *       tee     ( monitor ( count ( PT > 1*GeV )  ,  ... ) ) >> 
 *       EMPTY 
 *
 *  """
 *
 *  @endcode 
 *
 *  The embedded monitoring are described in detail at 
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/LoKiFAQ#How_to_monitor_various_LoKi_func">
 *  here </a>, also see 
 *  <a href="https://twiki.cern.ch/twiki/bin/view/LHCb/FAQ/LoKiFAQ#Helper_streamer_functions">
 *  helper LoKi streamers & filters</a>
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef,nl
 *  @date 2008-09-22
 */
class FilterDesktop : public DVAlgorithm
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FilterDesktop> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the specific initialization 
  virtual StatusCode initialize ()             // the specific initialization 
  {
    // initialize the base class 
    StatusCode sc = DVAlgorithm::initialize () ;
    if ( sc.isFailure () ) { return sc ; }                          // RETURN 
    // ensure the proper initialization of LoKi service 
    svc<IService> ( "LoKiSvc" ) ;
    
    // (re)lock the context again:  
    
    // register for the algorithm context service 
    IAlgContextSvc* ctx = 0 ;
    if ( registerContext() ) { ctx = contextSvc() ; }  
    // setup sentry/guard
    Gaudi::Utils::AlgContext sentry ( ctx , this ) ;
    
    // decode the cut:  
    sc = updateMajor () ;
    if ( sc.isFailure () ) 
    { return Error ("The error from updateMajor" , sc ) ; }
    
    // take care about the histos
    sc = updateHistos () ;
    if ( sc.isFailure () ) 
    { return Error ("The error from updateHistos" , sc ) ; }
    //
    return StatusCode::SUCCESS ;
  }  
  /// the most interesting method 
  virtual StatusCode execute    () ;           // the most interesting method
  // finalize /reset functors 
  virtual StatusCode finalize   ()
  {
    // reset functors
    m_cut         = _PBOOL ( false ) ;
    m_preMonitor  = _CBOOL ( false ) ;
    m_postMonitor = _CBOOL ( false ) ;
    m_to_be_updated1 = true ;
    // finalize the base
    return DVAlgorithm::finalize () ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @see DVAlgorithm
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see Algorithm
   *  @see AlgFactory
   *  @see IAlgFactory
   *  @param name the algorithm instance name 
   *  @param pSvc pointer to Service Locator 
   */
  FilterDesktop                                        // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc )                   // pointer to Service Locator
    : DVAlgorithm ( name , pSvc ) 
  // LoKi/Bender "hybrid" factory name 
    , m_factory ( "LoKi::Hybrid::Tool/HybridFactory:PUBLIC" )
  // the preambulo 
    , m_preambulo ()
  // the selection functor (predicate) itself 
    , m_code ( "PNONE"          )
    , m_cut  ( _PBOOL ( false ) ) 
  // general flag to switch on/off the monitoring
    , m_monitor ( false ) 
  // (pre-monitoring) functor 
    , m_preMonitorCode  ( )
    , m_preMonitor      ( _CBOOL ( false ) )
  // (post-monitoring) functor 
    , m_postMonitorCode ( )
    , m_postMonitor     ( _CBOOL ( false ) )
  // input plots 
    , m_inputPlotsTool   ( "LoKi::Hybrid::PlotTool/InputPlots"  )
    , m_inputPlots       (  0  )
    , m_inputPlotsPath   ( "I" + name  ) 
  // output plots 
    , m_outputPlotsTool  ( "LoKi::Hybrid::PlotTool/OutputPlots" )
    , m_outputPlots      (  0  )
    , m_outputPlotsPath  ( "O" + name  )
      // update?
    , m_to_be_updated1   ( true ) 
    , m_to_be_updated2   ( true ) 
  {
    // the factory 
    declareProperty 
      ( "Factory" , 
        m_factory , 
        "The Type/Name for C++/Python Hybrid Factory" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , this ) ;
    // the preambulo
    declareProperty 
      ( "Preambulo" , 
        m_preambulo , 
        "The preambulo to be used for Bender/Pthon script" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , this ) ;
    // the functor (predicate) 
    declareProperty 
      ( "Code" , 
        m_code , 
        "The valid LoKi/Bender 'hybrid' functor name" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , this ) ;
    // input plot tool 
    declareProperty 
      ( "InputPlotsTool"  , 
        m_inputPlotsTool  ,
        "The Type/Name for 'input' Plots tool" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ;
    // output plot tool 
    declareProperty 
      ( "OutputPlotsTool" , 
        m_outputPlotsTool ,
        "The Type/Name for 'output' Plots tool" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ;
    // the path for the input plots 
    declareProperty 
      ( "InputPlotsPath"  , 
        m_inputPlotsPath  ,
        "The THS path for the input plots" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ;
    // the path for the output plots 
    declareProperty 
      ( "OutputPlotsPath" , 
        m_outputPlotsPath ,
        "The THS path for the output plots" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ;
    //
    declareProperty
      ( "Monitor" , 
        m_monitor , 
        "The general flag to switch on/off the monitoring" ) 
      -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , this ) ;
    
    // (pre)monitoring code
    declareProperty
      ( "PreMonitor" , 
        m_preMonitorCode  , 
        "The code used for (pre)monitoring of input particles" )
      -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , this ) ;
    
    // (post)monitoring code
    declareProperty
      ( "PostMonitor" , 
        m_postMonitorCode  , 
        "The code used for (post)monitoring of output particles" )
      -> declareUpdateHandler ( &FilterDesktop::updateHandler1 , this ) ;

    // 
    StatusCode sc = setProperty ( "HistoProduce" , false ) ;
    Assert ( sc.isSuccess() , "Could not reset property HistoProduce" ) ;
    {
      Property* p = Gaudi::Utils::getProperty ( this , "HistoProduce" ) ;
      if ( 0 != p && 0 == p->updateCallBack() ) 
      { p -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ; }
    }
  }
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~FilterDesktop() {}               // virtual & protected destructor 
  // ==========================================================================
public:
  // ==========================================================================
  /// update the structural property  
  void updateHandler1 ( Property& p ) 
  {
    // no action if not initialized yet:
    if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
    /// mark as "to-be-updated" 
    m_to_be_updated1 = true ;
    Warning ( "The structural property '" + p.name() + 
              "' is updated. It will take affect at the next 'execute'" , 
              StatusCode( StatusCode::SUCCESS, true ) ) ;
    info () << "The updated property is: " << p << endreq ;
  }
  /// update the histogram property  
  void updateHandler2 ( Property& p ) 
  {
    // no action if not initialized yet:
    if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
    /// mark as "to-be-updated" 
    m_to_be_updated2 = true ;
    debug () << "The histogramming property is updated: " << p << endreq ;
  }  
  // ==========================================================================
public:
  // ==========================================================================
  /// general flag to switch on/off the monitoring
  bool monitor() const { return m_monitor ; }
  // ==========================================================================
protected:
  StatusCode writeEmptyContainerIfNeeded() 
  {
    const std::string& loc = desktop()->getOutputLocation();

    if (!exist<LHCb::Particle::Container>(loc + "/Particles") ) {  
      if (msgLevel(MSG::DEBUG)) debug() << "Saving empty container at " 
                                        << loc + "/Particles"<< endmsg ;
      LHCb::Particle::Container* dummy = new LHCb::Particle::Container;
      put(dummy, loc + "/Particles");
    }
    return StatusCode::SUCCESS;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// construct the preambulo string 
  std::string preambulo() const 
  {
    std::string result ;
    for ( std::vector<std::string>::const_iterator iline = 
            m_preambulo.begin() ; m_preambulo.end() != iline ; ++iline ) 
    {
      if ( m_preambulo.begin() != iline ) { result += "\n" ; }
      result += (*iline) ;
    }
    return result ;
  }
  // ==========================================================================
  // update the major properties 
  StatusCode updateMajor () 
  {
    // locate the factory
    IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;
    
    // use the factory 
    StatusCode sc = factory-> get ( m_code , m_cut , preambulo() ) ;
    if ( sc.isFailure() ) 
    { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='" 
                     + m_code + "'" , sc )  ; }
    
    // pre-monitoring: 
    if ( monitor() && !m_preMonitorCode.empty () ) 
    { sc = factory-> get ( m_preMonitorCode  , m_preMonitor  , preambulo() ) ; }
    else { m_preMonitor  = _CBOOL ( false ) ; }
    
    // post-monitoring: 
    if ( monitor() && !m_postMonitorCode.empty() ) 
    { sc = factory-> get ( m_postMonitorCode , m_postMonitor , preambulo() ) ; }
    else { m_postMonitor = _CBOOL ( false ) ; }
    
    // release the factory (not needed anymore) 
    release ( factory ) ;
    //
    m_to_be_updated1 = false ;
    //
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
  // update histos 
  StatusCode updateHistos () 
  {
    // ========================================================================
    if ( 0 != m_inputPlots  ) 
    { releaseTool ( m_inputPlots  ) ; m_inputPlots  = 0 ; }
    if ( 0 != m_outputPlots ) 
    { releaseTool ( m_outputPlots ) ; m_outputPlots = 0 ; }
    // =========================================================================
    if ( produceHistos () ) 
    {
      // =======================================================================
      if ( validPlots ( m_inputPlotsTool ) )
      {
        m_inputPlots = tool<IPlotTool>( m_inputPlotsTool, this ) ;
        StatusCode sc = m_inputPlots -> setPath ( m_inputPlotsPath ) ;
        if ( sc.isFailure() ) 
        { return Error ( "Unable to set Input Plots path" , sc ) ; }
      }
      //
      if ( validPlots ( m_outputPlotsTool ) ) 
      {
        m_outputPlots = tool<IPlotTool>( m_outputPlotsTool, this ) ;
        StatusCode sc = m_outputPlots -> setPath ( m_outputPlotsPath ) ;
        if ( sc.isFailure() ) 
        { return Error ("Unable to set Output Plots path" , sc ) ; }
      }
    }
    //
    m_to_be_updated2 = false ;
    //
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FilterDesktop () ;                    // the default consructor is disabled
  /// copy constructor is disabled 
  FilterDesktop ( const FilterDesktop& ) ;    // copy constructor is disabled 
  /// assignement operator is disabled 
  FilterDesktop& operator=( const FilterDesktop& ) ;        // no assignement
  // ==========================================================================
  /// Get the related PV of particle and relate it to clone.
  void cloneP2PVRelation(const LHCb::Particle* particle,
                         const LHCb::Particle* clone,
                         Particle2Vertex::Table* table) const;
  // ==========================================================================
private:
  // ==========================================================================
  /// LoKi/Bender "hybrid" factory name 
  std::string m_factory ; // LoKi/Bender "hybrid" factory name 
  /// the preambulo 
  std::vector<std::string> m_preambulo ; // the preambulo 
  /// the code for the functor/predicate 
  std::string m_code    ; // the code for the functor/predicate
  /// the predicate itself 
  LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate m_cut ;
  //
  /// general flag to switch on/off monitoring
  bool                m_monitor ;   // general flag to switch on/off monitoring
  /// (pre-monitoring) code 
  std::string         m_preMonitorCode  ;         // (pre-monitoring)      code
  /// (pre-monitoring) functor 
  LoKi::Types::CutVal m_preMonitor      ;         // (pre-monotoring)   functor 
  /// (post-monitoring) code 
  std::string         m_postMonitorCode ;         // (post-monitoring)     code
  /// (post-monitoring) functor 
  LoKi::Types::CutVal m_postMonitor     ;         // (post-monitoring)  functor 
  //
  // input plots
  //
  /// type/name for the input plots tool 
  std::string m_inputPlotsTool  ;           // type-name for input plots tool 
  /// the inputplots tool itself 
  IPlotTool*  m_inputPlots      ;              // the input plots tool itself
  /// the THS path for the input plots 
  std::string m_inputPlotsPath  ;                     // the input plots path 
  //
  // output plots 
  //
  /// type/name for the output plots tool 
  std::string m_outputPlotsTool ;          // type-name for output plots tool 
  /// the inputplots tool itself 
  IPlotTool*  m_outputPlots     ;             // the output plots tool itself
  /// the THS path for the output plots 
  std::string m_outputPlotsPath ;                    // the output plots path 
  //
  // ==========================================================================
  /// the flag to indicate the nesessity of update 
  bool m_to_be_updated1 ; // the flag to indicate the nesessity of update 
  /// the flag to indicate the nesessity of update 
  bool m_to_be_updated2 ; // the flag to indicate the nesessity of update 
  // ==========================================================================
};
// ============================================================================
// the most interesting method 
// ============================================================================
StatusCode FilterDesktop::execute ()       // the most interesting method 
{
  //
  if  ( m_to_be_updated1 ) 
  {
    StatusCode sc = updateMajor () ;
    if ( sc.isFailure() ) 
    { return Error ( "The error from updateMajor"  , sc ) ; }    // RETURN 
  }
  if  ( m_to_be_updated2 ) 
  {
    StatusCode sc = updateHistos () ;
    if ( sc.isFailure() ) 
    { return Error ( "The error from updateHistos" , sc ) ; }    // RETURN 
  }
    
  // get the input particles 
  const LHCb::Particle::ConstVector particles = desktop() -> particles () ;

  // monitor input (if required) 
  if ( monitor() && !m_preMonitorCode.empty() ) { m_preMonitor ( particles ) ; }

  // make plots 
  if ( produceHistos () && 0 != m_inputPlots ) 
  { 
    StatusCode sc = m_inputPlots -> fillPlots ( particles ) ; 
    if ( sc.isFailure () ) 
    { return Error ( "Error from Input Plots tool", sc ) ; }
  }

  LHCb::Particle::Container* accepted = new LHCb::Particle::Container;
  put(accepted, desktop()->getOutputLocation()+"/Particles");
  Particle2Vertex::Table* table = new Particle2Vertex::Table;
  put(table, desktop()->getOutputLocation()+"/Particle2VertexRelations");
  //
  StatEntity& cnt = counter ( "efficiency" ) ;
  //
  for ( LHCb::Particle::ConstVector::const_iterator ip = particles.begin() ; 
        particles.end() != ip ; ++ip )
  {
    const LHCb::Particle* p = *ip ;
    // satisfy the criteria ? 
    const bool decision = m_cut ( p ) ;
    // some statistics 
    cnt += decision ;
    if  ( !decision ) { continue ; }                       // CONTINUE
    //
    LHCb::Particle* clone = p->clone();
    if (clone) {
      accepted->insert( clone ) ;
      cloneP2PVRelation(p, clone, table) ;
    }
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "Saved " << accepted->size()
                                    << " Particles in " 
                                    << desktop()->getOutputLocation()+"/Particles" << endmsg ;

  // make the final plots 
   if ( produceHistos () && 0 != m_outputPlots ) 
   {
     StatusCode sc = m_outputPlots -> fillPlots ( LHCb::Particle::ConstVector(accepted->begin(), accepted->end()) ) ;
     if ( sc.isFailure () ) 
     { return Error ( "Error from Output Plots tool", sc ) ; }
   }
  
  // monitor output (if required) 
  if ( monitor() && !m_postMonitorCode.empty() ) 
  { m_postMonitor ( particles ) ; }

  /// make the filter decision
  setFilterPassed ( !accepted->empty() );
  // some statistics 
  counter ( "#input"  ) += particles.size() ;
  counter ( "#passed" ) += accepted->size() ;
  //
  return StatusCode::SUCCESS;
}
// ============================================================================
void FilterDesktop::cloneP2PVRelation(const LHCb::Particle* particle,
                                      const LHCb::Particle* clone,
                                      Particle2Vertex::Table* table) const 
{
  const LHCb::VertexBase* bestPV = getStoredBestPV(particle);
  if ( 0!= bestPV ) {
    table->relate(clone, bestPV);
  }
}
// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(FilterDesktop) 
// ============================================================================
// The END 
// ============================================================================
