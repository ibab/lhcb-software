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
#include "LoKi/Primitives.h"
// ============================================================================
// local 
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @class DVFilter1
 *  Simple variation of FilterDesktop algorithm, that does not produce
 *  the output. It is similar to "Void"-filter
 *  @see FilterDesktop 
 *  @see FilterInTree
 *  @see LoKi::FilterAlg 
 *  @see LoKi::VoidFilter
 *  @author Vanya BELYAEV   Ivan.Belyaev@cern.ch
 *  @date 2011-02-27
 */
class DVFilter1 : public FilterDesktop 
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<DVFilter1> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the main execution method 
  virtual StatusCode execute () ;                 // the main execution method
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
  DVFilter1                                    // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc )                  // pointer to Service Locator
    : FilterDesktop ( name , pSvc ) 
    , m_cutval ( LoKi::Constant<LHCb::Particle::ConstVector,bool> ( false ) )
  {}
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~DVFilter1 () {} ;                  // virtual & protected destructor 
  // ==========================================================================
protected:
  // ==========================================================================
  /// decode the code 
  virtual StatusCode decodeCode () ;          //                decode the code 
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  DVFilter1 () ;                          // the default consructor is disabled
  /// copy constructor is disabled 
  DVFilter1 ( const DVFilter1& ) ;        // copy constructor is disabled 
  /// assignement operator is disabled 
  DVFilter1& operator=( const DVFilter1& ) ;// no assignement
  // ==========================================================================
private:
  // ==========================================================================
  /// the vector predicate itself 
  LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Particle*>::CutVal>::Type  
  m_cutval ;                                     // the vector predicate itself 
  // ==========================================================================
};
// ============================================================================
/** @class DVFilter2
 *  Simple variation of FilterDesktop algorithm, that does not produce
 *  the output. It is similar to "Void"-filter
 *  @see FilterDesktop 
 *  @see FilterInTree
 *  @see LoKi::FilterAlg 
 *  @see LoKi::VoidFilter
 *  @author Vanya BELYAEV   Ivan.Belyaev@cern.ch
 *  @date 2011-02-27
 */
class DVFilter2 : public FilterDesktop 
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<DVFilter2> ;
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
  DVFilter2                                    // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc )                  // pointer to Service Locator
    : FilterDesktop ( name , pSvc ) 
    , m_pipe ( LoKi::Identity<LHCb::Particle::ConstVector> () )  
  {}
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~DVFilter2 () {}                  // virtual & protected destructor 
  // ==========================================================================
public:
  // ==========================================================================
  /** the major method for filter input particles 
   *  @param input    (INPUT) the input  container of particles
   *  @param filtered (OUPUT) the output container of particles
   *  @return Status code 
   */
  virtual StatusCode filter 
  ( const LHCb::Particle::ConstVector& input    , 
    LHCb::Particle::ConstVector&       filtered )
  {
    filtered = m_pipe ( input ) ;
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// decode the code 
  virtual StatusCode decodeCode () ;          //                decode the code 
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  DVFilter2 () ;                          // the default consructor is disabled
  /// copy constructor is disabled 
  DVFilter2 ( const DVFilter2& ) ;        // copy constructor is disabled 
  /// assignement operator is disabled 
  DVFilter2& operator=( const DVFilter2& ) ; // no assignement
  // ==========================================================================
private:
  // ==========================================================================
  /// the vector function itself 
  LoKi::Assignable<LoKi::BasicFunctors<const LHCb::Particle*>::Pipe>::Type  
  m_pipe ;                                        // the vector function itself 
  // ==========================================================================
};
// ============================================================================
// the most interesting method 
// ============================================================================
StatusCode DVFilter1::execute ()       // the most interesting method 
{
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
  
  // make the decision:
  const bool decision = m_cutval ( particles ) ;
  
  // make the filter decision
  setFilterPassed ( decision );
  
  //
  return StatusCode::SUCCESS;
}
// ============================================================================
// decode the code 
// ============================================================================
StatusCode DVFilter1::decodeCode () 
{
  // locate the factory
  LoKi::IHybridFactory* factory_ = 
    tool<LoKi::IHybridFactory> ( factory() , this ) ;
  //
  // use the factory 
  StatusCode sc = factory_ -> get ( code() , m_cutval , preambulo() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi/Bender 'hybrid' factory for Code='" 
                   + code() + "'" , sc )  ; }
  //
  release ( factory_ ) ;
  //
  return sc ;
}
// ============================================================================
// decode the code 
// ============================================================================
StatusCode DVFilter2::decodeCode () 
{
  // locate the factory
  LoKi::IHybridFactory* factory_ = 
    tool<LoKi::IHybridFactory> ( factory() , this ) ;
  //
  // use the factory 
  StatusCode sc = factory_ -> get ( code() , m_pipe , preambulo() ) ;
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
DECLARE_ALGORITHM_FACTORY(DVFilter1) 
// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(DVFilter2) 
// ============================================================================ 
// The END 
// ============================================================================ 
