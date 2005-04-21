// $Id: TestVeloCluster2MCHitRel.cpp,v 1.1 2005-04-21 09:53:48 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $ 
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiHistoAlg.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/VeloCluster.h"
#include "Event/MCVeloHit.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================

// ============================================================================
/** @class TestVeloCluster2MCHitRel 
 *
 *  Trivial algorithm  to check the 
 *  validity of relations from "Digi"-tape 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date 2005-02-14
 */
// ============================================================================
class TestVeloCluster2MCHitRel : public GaudiHistoAlg
{
  /// factory for instantiation
  friend class AlgFactory<TestVeloCluster2MCHitRel> ;
public:
  
  // the relation type 
  typedef IRelationWeighted<VeloCluster,MCVeloHit,double> ITable ;
  
public:
  
  /** algorithm execution
   *  @see IAlgorithm
   *  @return statsu code 
   */ 
  virtual StatusCode execute() ;

protected:
  
  /** standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc service locator 
   */
  TestVeloCluster2MCHitRel ( const std::string& name , 
                             ISvcLocator*       pSvc ) 
    : GaudiHistoAlg( name , pSvc ) 
      , m_table ( "Rec/Relations/VeloClusters2MCHits" )
  {
    declareProperty ( "Table" , m_table ) ;
  };
  
  /// destructor 
  virtual ~TestVeloCluster2MCHitRel(){} ;
  
private:
  // default constructor is disabled 
  TestVeloCluster2MCHitRel();
  // copy constructor is disabled 
  TestVeloCluster2MCHitRel ( const TestVeloCluster2MCHitRel& ) ;
  // assignement operator is disabled 
  TestVeloCluster2MCHitRel& operator= ( const TestVeloCluster2MCHitRel& ) ;
private:
  
  std::string m_table ;
};
// ============================================================================


// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  the concrete factpory for instantiation 
   */
  // ==========================================================================
  const  AlgFactory<TestVeloCluster2MCHitRel>  s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var TestVeloCluster2MCHitRelFactory
 *  an abstract factory for instantiation
 */
// ============================================================================
const IAlgFactory&TestVeloCluster2MCHitRelFactory = s_Factory ; 
// ============================================================================


// ============================================================================
/** algorithm execution
 *  @see IAlgorithm
 *  @return statsu code 
 */ 
// ============================================================================
StatusCode TestVeloCluster2MCHitRel::execute() 
{
  
  // get the relation table
  const ITable* table = get<ITable>( m_table ) ;
  
  // get all relations from the table 
  ITable::Range all = table->relations() ;
  
  // plot the overall amount of links 
  plot( ::tanh( all.size() / 2000.0 )   , 
        1 , " tanh(# of links/ 2000 ) " , 
        0 , 1.01 , 101  ) ;
  
  // check links 
  for ( ITable::Range::iterator entry = all.begin() ; 
        all.end() != entry ; ++entry ) 
  {
    const VeloCluster* digit  = entry->from   () ;
    const MCVeloHit*   mchit  = entry->to     () ;
    const double   weight = entry->weight () ;
    // plot the weight 
    plot ( weight                         , 
           2 , " weight of the relation " , 
           0 , 1.01 , 101 ) ;
    // check pointers 
    if ( 0 == digit ) { Warning ( "VeloCluster* points to NULL" ) ; }
    if ( 0 == mchit ) { Warning ( "MCVeloHit*   points to NULL" ) ; }
  };
  
  
  return StatusCode::SUCCESS ;
};
// ============================================================================
// The END 
// ============================================================================
