// $Id: OTTimes2MC.cpp,v 1.1 2005-02-16 09:03:56 ibelyaev Exp $
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
#include "Event/OTTime.h"
#include "Event/MCHit.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
// ============================================================================

// ============================================================================
/** @class OTTimes2MC 
 *
 *  Trivial algorithm  to check the 
 *  validity of relations from "Digi"-tape 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date 2005-02-14
 */
// ============================================================================
class OTTimes2MC : public GaudiHistoAlg
{
  /// factory for instantiation
  friend class AlgFactory<OTTimes2MC> ;
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
  OTTimes2MC ( const std::string& name , 
               ISvcLocator*       pSvc ) 
    : GaudiHistoAlg( name , pSvc ) 
      , m_table ( "Rec/Relations/OTTimes2MCHits" )
  {
    declareProperty ( "Table" , m_table ) ;
  };
  /// destructor 
  virtual ~OTTimes2MC(){} ;
  
private:
  // default constructor is disabled 
  OTTimes2MC();
  // copy constructor is disabled 
  OTTimes2MC ( const OTTimes2MC& ) ;
  // assignement operator is disabled 
  OTTimes2MC& operator= ( const OTTimes2MC& ) ;
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
  const  AlgFactory<OTTimes2MC>  s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var OTTimes2MCFactory
 *  an abstract factory for instantiation
 */
// ============================================================================
const IAlgFactory& OTTimes2MCFactory = s_Factory ; 
// ============================================================================


// ============================================================================
/** algorithm execution
 *  @see IAlgorithm
 *  @return statsu code 
 */ 
// ============================================================================
StatusCode OTTimes2MC::execute() 
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

// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// The END 
// ============================================================================
