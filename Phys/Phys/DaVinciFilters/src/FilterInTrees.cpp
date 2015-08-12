// $Id$
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysExtract.h"
// ============================================================================
// local
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @class FilterInTrees 
 *  Simple variation of FilterDesktop algorithm, that allows to 
 *  select/filter particles in decay trees, e.g. daughter muons 
 *  @see FilterDesktop 
 *  @author Vanya BELYAEV   Ivan.Belyaev@itep.ru
 *  @date 2010-04-05
 */
class FilterInTrees : public FilterDesktop 
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FilterInTrees> ;
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
  FilterInTrees                                   // standard contructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc )                  // pointer to Service Locator
    : FilterDesktop ( name , pSvc ) 
  {}
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~FilterInTrees() {} ;               // virtual & protected destructor 
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
    //
    LoKi::Extract::particles ( input.begin () , 
                               input.end   () , 
                               std::back_inserter ( filtered ) , 
                               predicate   () ) ;
    //
    // mark & store filtered particles in DVAlgorithm local container 
    //
    markParticles ( filtered ) ;
    //
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FilterInTrees () ;                      // the default consructor is disabled
  /// copy constructor is disabled 
  FilterInTrees ( const FilterInTrees& ) ;      // copy constructor is disabled 
  /// assignement operator is disabled 
  FilterInTrees& operator=( const FilterInTrees& ) ;          // no assignement
  // ==========================================================================
};
// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(FilterInTrees) 
// ============================================================================ 
// The END 
// ============================================================================ 


  
