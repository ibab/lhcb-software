// $Id: ITDigit2MC.cpp,v 1.1 2005-02-16 09:03:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "Event/ITDigit.h"
#include "Event/MCHit.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================

// ============================================================================
/** @class ITDigit2MC 
 *
 *  Trivial algorithm  to check the 
 *  validity of relations from "Digi"-tape 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date 2005-02-14
 */
// ============================================================================
class ITDigit2MC : public GaudiHistoAlg
{
  /// factory for instantiation
  friend class AlgFactory<ITDigit2MC> ;
public:
  
  // the relation type 
  typedef IRelationWeighted<ITDigit,MCHit,double> ITable ;
  
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
  ITDigit2MC ( const std::string& name , 
               ISvcLocator*       pSvc ) 
    : GaudiHistoAlg( name , pSvc ) 
      , m_table ( "Rec/Relations/ITDigits2MCHits" )
  {
    declareProperty ( "Table" , m_table ) ;
  };
  
  /// destructor 
  virtual ~ITDigit2MC(){} ;
  
private:
  // default constructor is disabled 
  ITDigit2MC();
  // copy constructor is disabled 
  ITDigit2MC ( const ITDigit2MC& ) ;
  // assignement operator is disabled 
  ITDigit2MC& operator= ( const ITDigit2MC& ) ;
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
  const  AlgFactory<ITDigit2MC>  s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var ITDigit2MCFactory
 *  an abstract factory for instantiation
 */
// ============================================================================
const IAlgFactory& ITDigit2MCFactory = s_Factory ; 
// ============================================================================


// ============================================================================
/** algorithm execution
 *  @see IAlgorithm
 *  @return statsu code 
 */ 
// ============================================================================
StatusCode ITDigit2MC::execute() 
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
    
    const ITDigit* digit  = entry->from   () ;
    const MCHit*   mchit  = entry->to     () ;
    const double   weight = entry->weight () ;
    // plot the weight 
    plot ( weight                         , 
           2 , " weight of the relation " , 
           0 , 1.01 , 101 ) ;
    // check pointers 
    if ( 0 == digit ) { Warning ( "ITDigit* points to NULL" ) ; }
    if ( 0 == mchit ) { Warning ( "MCHit*   points to NULL" ) ; }
    //
    ITable::From fr = entry->from() ;
    std::cout << " from " 
              << " hintID " << fr.hintID() 
              << " linkID " << fr.linkID() 
              << " data "   << fr.data() << std::endl ;
  };
  
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// The END 
// ============================================================================
