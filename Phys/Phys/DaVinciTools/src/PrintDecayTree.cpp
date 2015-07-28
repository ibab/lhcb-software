// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IPrintDecay.h"
// ============================================================================
/** @class PrintDecayTree 
 *  The simplified version of the algorithm PrintTree, 
 *  which deals only with the reconstructed particles 
 *  @see PrintTree 
 *  @see IPRintDecay
 *  @author Vanya BELYAEV Ivan.Belayev@nikhef.nl
 *  @date 2008-03-30
 */
class PrintDecayTree : public DaVinciAlgorithm
{
  // ==========================================================================
  // the friend factory for instantiation 
  friend class AlgFactory<PrintDecayTree> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// the standard execution of the algorithm
  virtual StatusCode execute() 
  {
    // get the tool 
    if ( !m_printDecay ) 
    { m_printDecay = tool<IPrintDecay>( m_printDecayName , this ) ; }
    // get the particles 
    const LHCb::Particle::ConstVector& parts = this->i_particles();
    //
    m_printDecay->printTree ( parts.begin() , parts.end () , m_maxDepth ) ;
    //
    setFilterPassed ( m_forceFilterPassedTrue || !parts.empty() ) ;
    //
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /** the standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc service locator 
   */
  PrintDecayTree ( const std::string& name , 
                   ISvcLocator*       pSvc ) 
    : DaVinciAlgorithm ( name , pSvc ) 
    , m_printDecayName ( "PrintDecayTreeTool/PrintDecay" )
    , m_printDecay     ( NULL ) 
    , m_maxDepth       ( 6    )  
  {
    declareProperty 
      ( "PrintDecayTool" , m_printDecayName , 
        "The type/name of the IPrintDecay tool" ) ;
    declareProperty 
      ( "MaxDepth"       , m_maxDepth       , 
        "The maximal depth (number of levels)"  ) ;
    declareProperty
      ( "ForceFilterPassed", m_forceFilterPassedTrue = false,
        "Flag to turn on the forcing of filter passed to true always" );
  }
  /// virtual & protected destructor 
  virtual ~PrintDecayTree () {}
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  PrintDecayTree () ; // the default constructor is disabled 
  /// the copy constructor is disabled 
  PrintDecayTree ( const PrintDecayTree& ) ; // no copy constructor 
  /// the assignement operator is disabled 
  PrintDecayTree& operator=( const PrintDecayTree& ) ; // no assignment
  // ==========================================================================
private:
  // ==========================================================================
  /// the type/name of the IPrintDecay tool 
  std::string  m_printDecayName ; // the type/name of the IPrintDecay tool 
  /// the IPrintDecay tool itself
  IPrintDecay* m_printDecay     ; // the IPrintDecay tool itself
  /// the maximal printout depth 
  int          m_maxDepth       ; // the maximal printout depth 
  /// Force filter passed to true ?
  bool         m_forceFilterPassedTrue;
  // ==========================================================================
};
// ============================================================================
/// declare the factory (needed for instantiation)
DECLARE_ALGORITHM_FACTORY(PrintDecayTree) 
// ============================================================================
// The END 
// ============================================================================
