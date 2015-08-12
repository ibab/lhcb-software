// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinci
#include "Kernel/DaVinciFun.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHybridFactory.h"
#include "LoKi/select.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/algorithm/string.hpp"
// ============================================================================
// local
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @file
 *  Implementation for for class FilterDesktop
 *  @see FilterDesktop
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef,nl
 *  @date 2008-09-22
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
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
// the specific initialization
// ============================================================================
StatusCode FilterDesktop::initialize ()             // the specific initialization
{
  // initialize the base class
  StatusCode sc = DaVinciAlgorithm::initialize () ;
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
// ============================================================================
// finalize /reset functors
// ============================================================================
StatusCode FilterDesktop::finalize   ()
{
  // reset functors
  m_cut         = _PBOOL ( false ) ;
  m_preMonitor  = _CBOOL ( false ) ;
  m_postMonitor = _CBOOL ( false ) ;
  m_to_be_updated1 = true ;
  // finalize the base
  return DaVinciAlgorithm::finalize () ;
}
// ============================================================================
/* standard constructor
 *  @see DaVinciAlgorithm
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see Algorithm
 *  @see AlgFactory
 *  @see IAlgFactory
 *  @param name the algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// ============================================================================
FilterDesktop::FilterDesktop                  // standard contructor
( const std::string& name ,                   // the algorithm instance name
  ISvcLocator*       pSvc )                   // pointer to Service Locator
  : DaVinciAlgorithm ( name , pSvc )
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
  , m_cloneFilteredParticles (false)
{
  //
  if      ( 0 == name.find("Hlt1") ) 
  { m_factory = "LoKi::Hybrid::Tool/Hlt1HybridFactory:PUBLIC" ; }
  else if ( 0 == name.find("Hlt2") )
  { m_factory = "LoKi::Hybrid::Tool/Hlt2HybridFactory:PUBLIC" ; }
  //
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

  // cloning of selected particles and secondary vertices
  declareProperty
    ( "CloneFilteredParticles" ,
      m_cloneFilteredParticles ,
      "Clone filtered particles and end-vertices into KeyedContainers" ) ;
  // Histogramming
  declareProperty
    ( "HistoProduce"          ,
      m_produceHistos = false ,
      "Switch on/off the production of histograms" ) 
    -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ;
}
// ============================================================================
// virtual & protected destructor
// ============================================================================
FilterDesktop::~FilterDesktop (){} // virtual & protected destructor
// ============================================================================
// update the structural property
// ============================================================================
void FilterDesktop::updateHandler1 ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated"
  m_to_be_updated1 = true ;
  Warning ( "The structural property '" + p.name() +
            "' is updated. It will take effect at the next 'execute'" ,
            StatusCode( StatusCode::SUCCESS, true ) ) ;
}
// ============================================================================
// update the histogram property
// ============================================================================
void FilterDesktop::updateHandler2 ( Property& p )
{
  // no action if not initialized yet:
  if ( Gaudi::StateMachine::INITIALIZED > FSMState() ) { return ; }
  /// mark as "to-be-updated"
  m_to_be_updated2 = true ;
  debug () << "The histogramming property is updated: " << p << endreq ;
}
// ============================================================================
void FilterDesktop::writeEmptyTESContainers()
{

  return m_cloneFilteredParticles ?
    writeEmptyKeyedContainers()  :
    writeEmptySharedContainers() ;

}
// ============================================================================
// construct the preambulo string
// ============================================================================
std::string FilterDesktop::preambulo() const
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
// ============================================================================
// update the major properties
// ============================================================================
StatusCode FilterDesktop::updateMajor ()
{
  // decode the code
  StatusCode sc = decodeCode () ;
  if ( sc.isFailure() ) { return Error ( "Error from decodeCode()'" ) ; }

  // locate the factory
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;

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
// ============================================================================
// update histos
// ============================================================================
StatusCode FilterDesktop::updateHistos ()
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
// the most interesting method
// ============================================================================
StatusCode FilterDesktop::execute ()       // the most interesting method
{

  m_accepted.clear();//
  if  ( m_to_be_updated1 )
  {
    StatusCode sc = updateMajor () ;
    if ( sc.isFailure() )
    { return Error ( "Error from updateMajor"  , sc ) ; }    // RETURN
  }
  if  ( m_to_be_updated2 )
  {
    StatusCode sc = updateHistos () ;
    if ( sc.isFailure() )
    { return Error ( "The error from updateHistos" , sc ) ; }    // RETURN
  }

  // get the input particles
  const LHCb::Particle::ConstVector& particles = i_particles () ;

  // monitor input (if required)
  if ( monitor() && !m_preMonitorCode.empty() ) { m_preMonitor ( particles ) ; }

  // make plots
  if ( produceHistos () && 0 != m_inputPlots )
  {
    StatusCode sc = m_inputPlots -> fillPlots ( particles ) ;
    if ( sc.isFailure () )
    { return Error ( "Error from Input Plots tool", sc ) ; }
  }

  //
  m_accepted.reserve ( particles.size() );
  //
  // Filter particles!!  - the most important line :-)
  StatusCode sc = filter ( particles , m_accepted ) ;

  // store particles in DaVinciAlgorithm local container
  // this section has been moved into "filter"-method
  // this -> markParticles ( m_accepted ) ;
  // this -> markTrees     ( m_accepted ) ;

  // make the final plots
  if ( produceHistos () && 0 != m_outputPlots )
  {
    const StatusCode scc = m_outputPlots -> fillPlots ( i_markedParticles()  ) ;
    if ( scc.isFailure () )
    { return Error ( "Error from Output Plots tool", scc ) ; }
  }

  // monitor output (if required)
  if ( monitor() && !m_postMonitorCode.empty() ) { m_postMonitor ( i_markedParticles() ) ; }

  // make the filter decision
  setFilterPassed ( !i_markedParticles().empty() ) ;

  // some statistics
  counter ( "#passed" ) += i_markedParticles().size();

  //
  return StatusCode::SUCCESS;
}
// ============================================================================
/*  the major method to filter input particles
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code
 */
// ============================================================================
StatusCode FilterDesktop::filter
( const LHCb::Particle::ConstVector& input    ,
  LHCb::Particle::ConstVector&       filtered )
{
  // Filter particles!!  - the most important line :-)
  LoKi::select ( input.begin () ,
                 input.end   () ,
                 std::back_inserter ( filtered ) , m_cut ) ;
  //
  // mark & store filtered particles in DaVinciAlgorithm local container
  markParticles ( filtered ) ;
  //
  // some countings
  StatEntity& cnt = counter ( "efficiency" ) ;
  //
  const size_t size1 = filtered  . size () ;
  const size_t size2 = input     . size () ;
  //
  for ( size_t i1 = 0     ; size1 > i1 ; ++i1 ) { cnt += true  ; }
  for ( size_t i2 = size1 ; size2 > i2 ; ++i2 ) { cnt += false ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// save (clone if needed) selected particles in TES
// ============================================================================
template <class PARTICLES, class VERTICES, class CLONER>
StatusCode FilterDesktop::_save ( ) const
{
  //
  PARTICLES* p_tes = new PARTICLES () ;
  VERTICES*  v_tes = new VERTICES  () ;
  //
  put ( p_tes ,    particleOutputLocation () ) ;
  put ( v_tes , decayVertexOutputLocation () ) ;
  //
  CLONER cloner ;
  //
  for ( LHCb::Particle::Range::iterator ip = i_markedParticles().begin(); 
        ip != i_markedParticles().end(); ++ip )
  {
    //
    const LHCb::Particle* p = *ip ;
    if ( NULL == p ) { continue ; } // CONTINUE

    // clone if needeed
    LHCb::Particle* p_cloned = cloner ( p ) ;
    p_tes -> insert ( p_cloned ) ;
    //
    this  -> cloneP2PVRelation ( p , p_cloned ) ;
    //
    const LHCb::Vertex* v = p->endVertex() ;
    if ( NULL != v )
    {
      LHCb::Vertex* v_cloned = cloner ( v ) ;
      p_cloned -> setEndVertex ( v_cloned ) ;
      v_tes    -> insert( v_cloned ) ;
    }
  }
  //
  // check that the decay trees are fully in the TES
  //
  for ( typename PARTICLES::const_iterator ip = p_tes->begin() ;
        p_tes->end () != ip ; ++ip )
  {
    if (! DaVinci::Utils::decayTreeInTES( *ip ) )
    {
      return Error ( "FilterDesktop::_save Element of saved decay tree not in TES. Likely memory leak!");
    }
  }
  //
  return ( i_markedParticles().size() != p_tes->size() ?
           StatusCode::FAILURE : StatusCode::SUCCESS   );
}
// ============================================================================
namespace
{
  // ==========================================================================
  /// the trivial cloner
  struct _Cloner
  {
    template <class T>
    T* operator() ( const T* a ) const { return a->clone() ; }
  };
  /// helper structure (fake cloner)
  struct _Caster
  {
    template <class T>
    T* operator() ( const T* a ) const { return const_cast<T*> ( a ) ; }
  };
  // ==========================================================================
} // end of anonymous namespace
// ============================================================================
StatusCode FilterDesktop::_saveInTES ()
{
  if (msgLevel(MSG::VERBOSE))
  {
    verbose() << "FilterDesktop::_saveInTES " << i_markedParticles().size() << " Particles" << endmsg;
  }
  return m_cloneFilteredParticles ?
    this -> _save<LHCb::Particle::Container,LHCb::Vertex::Container,_Cloner> ():
    this -> _save<LHCb::Particle::Selection,LHCb::Vertex::Selection,_Caster> ();
}
// ============================================================================
void FilterDesktop::cloneP2PVRelation
( const LHCb::Particle*   particle ,
  const LHCb::Particle*   clone       ) const
{
  const LHCb::VertexBase* bestPV = getStoredBestPV(particle);
  if ( 0!= bestPV ) { this->relate ( clone , bestPV ) ; }

}
// ============================================================================
void FilterDesktop::writeEmptyKeyedContainers() const
{
  LHCb::Particle::Container* container = new LHCb::Particle::Container();
  put(container, particleOutputLocation());
  return;
}
// ============================================================================
void FilterDesktop::writeEmptySharedContainers() const
{
  LHCb::Particle::Selection* container = new LHCb::Particle::Selection();
  put(container, particleOutputLocation());
  return;
}
// ============================================================================
// decode the code
// ============================================================================
StatusCode FilterDesktop::decodeCode ()
{
  // locate the factory
  LoKi::IHybridFactory* factory_ = tool<LoKi::IHybridFactory> ( factory() , this ) ;
  //
  // use the factory
  StatusCode sc = factory_ -> get ( code() , m_cut , preambulo() ) ;
  if ( sc.isFailure() )
  { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='"
                   + code() + "'" , sc )  ; }
  //
  release ( factory_ ) ;
  //
  return sc ;
}
// ============================================================================
/// the factory
DECLARE_ALGORITHM_FACTORY(FilterDesktop)
// ============================================================================
// The END
// ============================================================================
