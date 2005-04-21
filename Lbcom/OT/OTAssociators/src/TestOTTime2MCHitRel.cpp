// $Id: TestOTTime2MCHitRel.cpp,v 1.2 2005-04-21 09:55:09 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $ 
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
#include "Event/OTTime.h"
#include "Event/MCHit.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
// ============================================================================

// ============================================================================
/** @class TestOTTime2MCHitRel 
 *
 *  Trivial algorithm  to check the 
 *  validity of relations from "Digi"-tape 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date 2005-02-14
 */
// ============================================================================
class TestOTTime2MCHitRel : public GaudiHistoAlg
{
  /// factory for instantiation
  friend class AlgFactory<TestOTTime2MCHitRel> ;
public:
  
  // the relation type 
  typedef IRelation<OTTime,MCHit> ITable ;
  
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
  TestOTTime2MCHitRel ( const std::string& name , 
                        ISvcLocator*       pSvc ) 
    : GaudiHistoAlg( name , pSvc ) 
      , m_table ( "Rec/Relations/OTTimes2MCHits" )
  {
    declareProperty ( "Table" , m_table ) ;
  };
  /// destructor 
  virtual ~TestOTTime2MCHitRel(){} ;
  
private:
  // default constructor is disabled 
  TestOTTime2MCHitRel();
  // copy constructor is disabled 
  TestOTTime2MCHitRel ( const TestOTTime2MCHitRel& ) ;
  // assignement operator is disabled 
  TestOTTime2MCHitRel& operator= ( const TestOTTime2MCHitRel& ) ;
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
  const  AlgFactory<TestOTTime2MCHitRel>  s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var TestOTTime2MCHitRelFactory
 *  an abstract factory for instantiation
 */
// ============================================================================
const IAlgFactory& TestOTTime2MCHitRelFactory = s_Factory ; 
// ============================================================================


// ============================================================================
/** algorithm execution
 *  @see IAlgorithm
 *  @return statsu code 
 */ 
// ============================================================================
StatusCode TestOTTime2MCHitRel::execute() 
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
    const OTTime*  digit  = entry->from   () ;
    const MCHit*   mchit  = entry->to     () ;
    // check pointers 
    if ( 0 == digit ) { Warning ( "OTTimes* points to NULL" ) ; }
    if ( 0 == mchit ) { Warning ( "MCHit*   points to NULL" ) ; }
  };
  
  
  return StatusCode::SUCCESS ;
};
// ============================================================================
// The END 
// ============================================================================
