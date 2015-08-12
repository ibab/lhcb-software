// $Id$
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Primitives.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
// local
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @class FilterStream
 *  Simple variation of FilterDesktop algorithm, that allows to 
 *  use "pipe" or "streamer" functors for selection of particles.
 *  In particular it allows to select e.g. the best candidate..
 *
 *  @code 
 * 
 *   myAlg = FilterStream ( ... ) 
 * 
 *   myAlg.Code = """
 *       select      ( PT > 5 * GeV          )  >> 
 *       min_element ( DTF_CHI2NDOF(  True ) ) 
 *       """
 *  @endcode 
 *
 *   Note that for FilterDesktop the effective functor has the signature:
 *   - "Cut": "const LHCb::Particle*" -> "bool";
 *   For FilterStream, the effective functor has signature:
 *   - "Pipe": "vector<const LHCb::Particle*>" -> "vector<const LHCb::Particle*>" -> "bool";
 *
 * 
 *  @see FilterDesktop 
 *  @author Vanya BELYAEV   Ivan.Belyaev@cern.ch
 *  @date 2011-10-30
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
class FilterStream : public FilterDesktop 
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FilterStream> ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @see FilterDesktop
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
  FilterStream                                 // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc ) ;                // pointer to Service Locator
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~FilterStream() {} ;               // virtual & protected destructor 
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
    LHCb::Particle::ConstVector&       filtered ) ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// decode the code 
  virtual StatusCode decodeCode () ;          //                decode the code 
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FilterStream () ;                       // the default consructor is disabled
  /// copy constructor is disabled 
  FilterStream ( const FilterStream& ) ;        // copy constructor is disabled 
  /// assignement operator is disabled 
  FilterStream& operator=( const FilterStream& ) ;            // no assignement
  // ==========================================================================
private:
  // ==========================================================================
  /// the pipe itself 
  typedef LoKi::BasicFunctors<const LHCb::Particle*>::Pipe Pipe ;
  LoKi::Assignable<Pipe>::Type m_pipe ;                      // the pipe itself 
  // ==========================================================================
};
// ============================================================================
/* standard constructor 
 *  @see FilterDesktop
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
FilterStream::FilterStream                   // standard contructor 
( const std::string& name ,                  // the algorithm instance name 
  ISvcLocator*       pSvc )                  // pointer to Service Locator
  : FilterDesktop ( name , pSvc ) 
  , m_pipe ( LoKi::Identity<LHCb::Particle::ConstVector>() ) 
{}
// ==============================================================================
/* the major method for filter input particles 
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code 
 */
// ============================================================================
StatusCode FilterStream::filter 
( const LHCb::Particle::ConstVector& input    , 
  LHCb::Particle::ConstVector&       filtered ) 
{
  //
  if ( !filtered.empty() ) { filtered.clear() ; }
  //
  if ( input.empty() ) { return StatusCode::SUCCESS ;  }    // RETURN 
  //
  filtered.reserve ( input.size() ) ;
  //
  // use the pipe!
  //
  filtered = m_pipe ( input ) ; 
  //
  // mark & store filtered particles in DVAlgorithm local container 
  //
  markParticles ( filtered ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// decode the code 
// ============================================================================
StatusCode FilterStream::decodeCode () 
{
  // locate the factory
  LoKi::IHybridFactory* factory_ = tool<LoKi::IHybridFactory> ( factory() , this ) ;
  //
  // use the factory 
  StatusCode sc = factory_ -> get ( code() , m_pipe, preambulo() ) ;
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
DECLARE_ALGORITHM_FACTORY(FilterStream) 
// ============================================================================ 
// The END 
// ============================================================================ 


  
  
