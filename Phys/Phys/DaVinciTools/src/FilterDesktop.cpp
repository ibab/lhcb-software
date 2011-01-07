// $Id: FilterDesktop.cpp,v 1.31 2010-07-10 14:22:37 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
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
    return DVAlgorithm::finalize () ;
  }
// ============================================================================
/* standard constructor 
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
// ============================================================================
FilterDesktop::FilterDesktop                  // standard contructor 
( const std::string& name ,                   // the algorithm instance name 
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
  , m_cloneFilteredParticles (false) 
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
  
  // cloning of selected particles and secondary vertices
  declareProperty 
    ( "CloneFilteredParticles" ,
      m_cloneFilteredParticles ,
      "Clone filtered particles and end-vertices into KeyedContainers" ) ;
  // 
  StatusCode sc = setProperty ( "HistoProduce" , false ) ;
  Assert ( sc.isSuccess() , "Could not reset property HistoProduce" ) ;
  {
    Property* p = Gaudi::Utils::getProperty ( this , "HistoProduce" ) ;
    if ( 0 != p && 0 == p->updateCallBack() ) 
    { p -> declareUpdateHandler ( &FilterDesktop::updateHandler2 , this ) ; }
  }
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
            "' is updated. It will take affect at the next 'execute'" , 
            StatusCode( StatusCode::SUCCESS, true ) ) ;
  info () << "The updated property is: " << p << endreq ;
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

  // make sure local storage is cleared whenever we go out of scope.
  //  DaVinci::Utils::ClearContainerGuard<LHCb::Particle::ConstVector> guard(m_accepted);
  m_accepted.clear();
  
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
  
  // make the final plots 
  if ( produceHistos () && 0 != m_outputPlots ) 
  {
    StatusCode sc = m_outputPlots -> fillPlots ( m_accepted  ) ;
    if ( sc.isFailure () ) 
    { return Error ( "Error from Output Plots tool", sc ) ; }
  }
  
  // monitor output (if required) 
  if ( monitor() && !m_postMonitorCode.empty() ) { m_postMonitor ( m_accepted  ) ; }
  
  // save (clone if needed) accepted particles in TES 
  //  LHCb::Particle::Range saved = saveInTES ( accepted ) ;
  
  // make the filter decision
  setFilterPassed ( !m_accepted.empty() );


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
( const LHCb::Particle::Range& input    , 
  LHCb::Particle::ConstVector& filtered ) const 
{
  // Filter particles!!  - the most important line :-) 
  LoKi::select ( input.begin () , 
                 input.end   () , 
                 std::back_inserter ( filtered ) , m_cut ) ;
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
template <class PARTICLES, class VERTICES, class CLONER>
StatusCode FilterDesktop::_save ( ) const 
{

  //
  PARTICLES*              p_tes = new PARTICLES () ;
  VERTICES*               v_tes = new VERTICES  () ;
  Particle2Vertex::Table* table = new Particle2Vertex::Table    ( m_accepted.size() ) ;
//   //
  put ( p_tes , particleOutputLocation()        ) ;
  put ( v_tes , decayVertexOutputLocation() ) ;
  put ( table , tableOutputLocation()           ) ;
  //
  CLONER cloner ;
  //
  LHCb::Particle::Range::iterator ip = m_accepted.begin ();
  LHCb::Particle::Range::iterator ipEnd = m_accepted.end ();

  for (  ; ip != ipEnd ; ++ip ) 
  {
    //
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { continue ; } // CONTINUE
    
    // clone if needeed 
    LHCb::Particle* p_cloned = cloner ( p ) ;
    p_tes ->insert ( p_cloned ) ;
    //
    this->cloneP2PVRelation ( p , p_cloned , table ) ;
    //
    const LHCb::Vertex* v = p->endVertex() ;
    if ( 0 != v ) 
    {
      LHCb::Vertex* v_cloned = cloner ( v ) ;
      p_cloned->setEndVertex ( v_cloned ) ;
      v_tes->insert( v_cloned ) ;
    }
  }
  
  this->saveOrphanRelatedPVs(table);

  // commented out due to bug in Gaudi..
  // Gaudi::Utils::GetData<LHCb::Particle::Range> helper ;
  //  Gaudi::Utils::GetData<Gaudi::Range_<LHCb::Particle::ConstVector> > helper ;
  //

  // some statistics 
  counter ( "#input"  ) += i_particles().size () ;
  counter ( "#passed" ) += p_tes->size () ;

  return (m_accepted.size() != p_tes->size() ) ? StatusCode::FAILURE 
    : StatusCode::SUCCESS;

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
}
// ===========================================================================
StatusCode FilterDesktop::saveInTES 
( )
{
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "FilterDesktop::SaveInTES " << m_accepted.size() << " Particles" << endmsg;
  }
  return m_cloneFilteredParticles ? 
    this->_save<LHCb::Particle::Container,LHCb::Vertex::Container,_Cloner>( ): 
    this->_save<LHCb::Particle::Selection,LHCb::Vertex::Selection,_Caster>( );
}
// ============================================================================
void FilterDesktop::cloneP2PVRelation 
( const LHCb::Particle*   particle ,
  const LHCb::Particle*   clone    ,
  Particle2Vertex::Table* table    ) const 
{
  const LHCb::VertexBase* bestPV = getStoredBestPV(particle);
  if ( 0!= bestPV ) { table->relate ( clone , bestPV ) ; }
    
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
void  FilterDesktop::saveOrphanRelatedPVs(const Particle2Vertex::Table* table) const 
{
  Particle2Vertex::Table::Range relations = table->relations();
  if (relations.empty()) return;

  LHCb::RecVertices* orphanPVs = new LHCb::RecVertex::Container();
  put (orphanPVs, this->outputLocation() + "/_RelatedPVs") ;
  
  Particle2Vertex::Table::Range::const_iterator beginRel = relations.begin () ;
  Particle2Vertex::Table::Range::const_iterator endRel   = relations.end   () ;
  
  for (Particle2Vertex::Table::Range::const_iterator irel = beginRel;
       irel != endRel;
       ++irel) {
    const LHCb::RecVertex* relPV = dynamic_cast<const LHCb::RecVertex*>(irel->to());
    if (relPV && !DaVinci::Utils::inTES(relPV) ) {
      orphanPVs->insert(const_cast<LHCb::RecVertex*>(relPV));
    }
  }
  
}
// ============================================================================
// decode the code 
// ============================================================================
StatusCode FilterDesktop::decodeCode () 
{
  // locate the factory
  LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory> ( m_factory , this ) ;
  
  // use the factory 
  StatusCode sc = factory-> get ( code() , m_cut , preambulo() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='" 
                   + code() + "'" , sc )  ; }
  //
  release ( factory ) ;
  //
  return sc ;
}
// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(FilterDesktop) 
// ============================================================================
// The END 
// ============================================================================
