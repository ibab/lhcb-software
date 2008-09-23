// $Id: FilterDesktop.cpp,v 1.4 2008-09-23 13:52:23 pkoppenb Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/StatEntity.h"
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
using namespace LoKi ;
  // ==========================================================================
  /** @class FilterDesktop 
   *  LoKi/Bender "Hybrid" (re)implementation of simple algorithm with 
   *  filters the input particles ("FilterDesktop")
   *
   *  The important properties (in addtion to the base class' properties)
   *    - "Factory" : the type/name of LoKi/Bender 'hybrid' factory
   *    - "Code"    : the actual Bender/Python code 
   *    - "InputPlotsTool"   : the type/name of PlotTool for 'input' particles 
   *    - "InputPlotsPath"   : THS path for 'input' plots 
   *    - "OutputPlotsTool"  : the type/name of PlotTool for 'output' particles 
   *    - "OutputPlotsPath"  : THS path for 'output' plots 
   *    - "CloneFinalStates" : ? 
   *    - "CloneDaughetrs"   : ? 
   *    - "CloneTree"        : ? 
   *
   *  The important counters (in addition to counters form DValgorithm)
   *    - "#inputs"    : number of inptu particles 
   *    - "#passed"    : number of particles, which passed criteria
   *    - "#cloned"    : number of cloned particles
   *    - "efficiency" : the selection efficiency (per particle) 
   *    - "ratio"      : the selection efficiency (per event) 
   *  
   *  @see DVAlgorithm 
   *  @see CombineParticles 
   *  @see IHybridFactory
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef,nl
   *  @date 2008-09-22
   */
  class FilterDesktop : public DVAlgorithm
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class AlgFactory<FilterDesktop> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the specific initialization 
    virtual StatusCode initialize ()             // the specific initialization 
    {
      // initialize the base class 
      StatusCode sc = DVAlgorithm::initialize () ;
      if ( sc.isFailure () ) { return sc ; }                          // RETURN 
      // ensure the proper initialization of LoKi service 
      svc<IService> ( "LoKiSvc" ) ;
      // locate the factory
      IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;
      // use the factory 
      sc = factory-> get ( m_code , m_cut ) ;
      if ( sc.isFailure() ) 
      { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='" 
                       + m_code + "'" , sc )  ; }
      // 
      if ( produceHistos ()            && 
           !m_inputPlotsTool.empty()   && 
           "none" != m_inputPlotsTool  && 
           "NONE" != m_inputPlotsTool  && 
           "None" != m_inputPlotsTool    ) 
      {
        m_inputPlots = tool<IPlotTool>( m_inputPlotsTool, this ) ;
        m_inputPlots -> setPath ( m_inputPlotsPath ) ;
      }
      //
      if ( produceHistos ()            && 
           !m_outputPlotsTool.empty()  && 
           "none" != m_outputPlotsTool && 
           "NONE" != m_outputPlotsTool && 
           "None" != m_outputPlotsTool    ) 
      {
        m_outputPlots = tool<IPlotTool>( m_outputPlotsTool, this ) ;
        m_outputPlots -> setPath ( m_outputPlotsPath ) ;
      }
      //
      return StatusCode::SUCCESS ;
    }  
    /// the most interesting method 
    virtual StatusCode execute    () ;           // the most interesting method 
    // ========================================================================    
  protected:
    // ========================================================================
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
      // the selection functor (predicate) itself 
      , m_code ( "PNONE" )
      , m_cut  ( LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant ( false ) ) 
      // input plots 
      , m_inputPlotsTool   ( "LoKi::Hybrid::PlotTool/InputPlots"  )
      , m_inputPlots       (  0  )
      , m_inputPlotsPath   ( "I" + name  ) 
      // output plots 
      , m_outputPlotsTool  ( "LoKi::Hybrid::PlotTool/OutputPlots" )
      , m_outputPlots      (  0  )
      , m_outputPlotsPath  ( "O" + name  )
      // cloning rules:
      , m_cloneFinalStates ( false ) 
      , m_cloneDaughters   ( false ) 
      , m_cloneTree        ( false )
    {
      // the factory 
      declareProperty 
        ( "Factory"          , 
          m_factory , 
          "The Type/Name for C++/Python Hybrid Factory" ) ;
      // the functor (predicate) 
      declareProperty 
        ( "Code" , m_code , "The valid LoKi/Bender 'hybrid' functor name" ) ;
      // input plot tool 
      declareProperty 
        ( "InputPlotsTool"  , 
          m_inputPlotsTool  ,
          "The Type/Name for 'input' Plots tool" ) ;
      // output plot tool 
      declareProperty 
        ( "OutputPlotsTool" , 
          m_outputPlotsTool ,
          "The Type/Name for 'output' Plots tool" ) ;
      // the path for the input plots 
      declareProperty 
        ( "InputPlotsPath"  , 
          m_inputPlotsPath  ,
          "The THS path for the input plots" ) ;
      // the path for the output plots 
      declareProperty 
        ( "OutputPlotsPath" , 
          m_outputPlotsPath ,
          "The THS path for the output plots" ) ;
      //
      declareProperty ( "CloneFinalState" , m_cloneFinalStates ) ;
      declareProperty ( "CloneDaughters"  , m_cloneDaughters   ) ;
      declareProperty ( "CloneTree"       , m_cloneTree        ) ;
      // 
      StatusCode sc = setProperty ( "HistoProduce" , false ) ;
      Assert ( sc.isSuccess() , "Could not reset property HistoProduce" ) ;
    }
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~FilterDesktop() {}               // virtual & protected destructor 
    // ========================================================================    
  private:
    // ========================================================================
    /// the default constructor is disabled 
    FilterDesktop () ;                    // the default consructor is disabled
    /// copy constructor is disabled 
    FilterDesktop ( const FilterDesktop& ) ;    // copy constructor is disabled 
    /// assignement operator is disabled 
    FilterDesktop& operator=( const FilterDesktop& ) ;        // no assignement
    // ========================================================================
  private:
    // ========================================================================
    /// LoKi/Bender "hybrid" factory name 
    std::string m_factory ; // LoKi/Bender "hybrid" factory name 
    /// the code for the functor/predicate 
    std::string m_code    ; // the code for the functor/predicate
    /// the predicate itself 
    LoKi::BasicFunctors<const LHCb::Particle*>::PredicateFromPredicate m_cut ;
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
    // cloning rules:
    //
    bool m_cloneFinalStates ;
    bool m_cloneDaughters   ;
    bool m_cloneTree        ;
    // ========================================================================
  };
  // ==========================================================================
// ============================================================================
// the most interesting method 
// ============================================================================
StatusCode FilterDesktop::execute ()       // the most interesting method 
{
  // get the input particles 
  const LHCb::Particle::ConstVector particles = desktop() -> particles () ;
  // make plots 
  if ( produceHistos () && 0 != m_inputPlots ) 
  { 
    StatusCode sc = m_inputPlots -> fillPlots ( particles ) ; 
    if ( sc.isFailure () ) 
    { return Error ( "Error from Input Plots tool", sc ) ; }
  }
  //
  // copy the lines from the previous implementation 
  //
  LHCb::Particle::ConstVector accepted  ;
  accepted.reserve ( particles.size () ) ;
  LHCb::Particle::ConstVector daughters ;
  //
  unsigned int nPass = 0  ;
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
    // statistics 
    ++nPass ;                     // statistics 
    //
    if (!m_cloneFinalStates && !m_cloneDaughters) 
    { accepted.push_back ( p ) ; }
    // 
    if ( m_cloneTree || m_cloneFinalStates || m_cloneDaughters)
    {
      daughters.clear() ;
      if      ( m_cloneDaughters   ) { daughters = p->daughtersVector ()           ; }
      else if ( m_cloneTree        ) { daughters = descendants()->descendants( p ) ; }
      else if ( m_cloneFinalStates ) { daughters = descendants()->finalStates( p ) ; }
      // 
      accepted.insert ( accepted.end() , daughters.begin() , daughters.end() ) ;
    }
    //
  }  
  // clone the selected particles & trees 
  StatusCode sc = desktop()->cloneTrees(accepted);
  if ( sc.isFailure() ) { return Error ("Error from desktop()::clone", sc )  ; }
  // make the final plots 
  if ( produceHistos () && 0 != m_outputPlots ) 
  {
    sc = m_outputPlots -> fillPlots ( accepted ) ;
    if ( sc.isFailure () ) 
    { return Error ( "Error from Output Plots tool", sc ) ; }
  }
  /// make the filter decision
  setFilterPassed ( !accepted.empty() );
  // some statistics 
  counter ( "#input"  ) += particles.size() ;
  counter ( "#passed" ) += nPass            ;
  counter ( "#cloned" ) += accepted.size()  ;
  counter ( "ratio"   ) += 
    particles.empty() ? 1.0 :  double(nPass)/particles.size() ;
  return StatusCode::SUCCESS;
}
// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(FilterDesktop) 
// ============================================================================
// The END 
// ============================================================================
