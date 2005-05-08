// $Id: CaloDigit2MCLinks2Table.cpp,v 1.1 2005-05-08 09:19:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/RelationWeighted1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloMCTools.h"
#include "Event/MCParticle.h"
// ============================================================================
// LinkerEvent
// ============================================================================
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
// ============================================================================

// ============================================================================
/** @class  CaloDigit2MCLinks2Table  CaloDigit2MCLinks2Table.cpp
 *
 *  Helper algorithm to "decode" idirotic linker object 
 *  to relation tabel for Monte Carlo links form CaloDigits to 
 *  MCParticle
 *
 *  The mportant "properties" of this algoeithm are:
 *   
 *  - "Inputs"
 *    The default value is            CaloDigitLocation::Ecal  
 *    List of Linker objects for CaloDigit->MCParticle links 
 * 
 *  - "Output"
 *    Teh default value is "Rec/Relations/CaloDigits2MCParticles"
 *    Name of (output) relation table for CaloDigit->MCParticle 
 *
 *  @see CaloDigit
 *  @see MCParticle
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2004-05-03
 */
// ============================================================================
class CaloDigit2MCLinks2Table : public CaloAlgorithm
{
  friend class AlgFactory<CaloDigit2MCLinks2Table> ;
public:
  /// algorithm execution 
  virtual StatusCode execute    () ;
protected:
  /** standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc pointer to service locator
   */
  CaloDigit2MCLinks2Table
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloAlgorithm ( name , pSvc ) 
  {    
    // set the appropriate default values for input data (linker)
    addToInputs   ( CaloDigitLocation :: Ecal              ) ;
    // set the appropriate default value  for output data
    setOutputData ( "Rec/Relations/CaloDigits2MCParticles" ) ;
  } ;
  /// virtual destructor (protected)
  virtual ~CaloDigit2MCLinks2Table() {};
private:
  // default constructor  is disabled 
  CaloDigit2MCLinks2Table ();
  // copy    constructor  is disabled  
  CaloDigit2MCLinks2Table 
  ( const CaloDigit2MCLinks2Table& ) ;
  // assignement operator is disabled
  CaloDigit2MCLinks2Table& operator=
  ( const CaloDigit2MCLinks2Table& ) ;
};
// ============================================================================

// ============================================================================
/// anonymous namespace to prevent an export of concrete factory
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  (local) concrete algorithm factory for instantiaton of 
   *   objects of type CaloDigitMCTruth 
   *  @see CaloDigit2MCLinks2Table
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-03
   */
  // ==========================================================================
  const  AlgFactory<CaloDigit2MCLinks2Table>         s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var CaloDigit2MCLinks2TableFactory
 *  (exported) abstract algorithm factory for instantiaton of 
 *  objects of type CaloDigitMCTruth 
 *  @see CaloDigit2MCLinks2Table
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-05-03
 */
const   IAlgFactory&CaloDigit2MCLinks2TableFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloDigit2MCLinks2Table::execute    () 
{
  /// the actual type of calorimeter digit 
  typedef const CaloDigit                                Digit  ;
  /// the actual type of container
  typedef const CaloDigits                               Digits ;
  /// the actual type of relation table 
  typedef RelationWeighted1D<CaloDigit,MCParticle,float> Table  ;
  //  the actual tyep of idiotic linker 
  typedef LinkedTo<MCParticle,CaloDigit>                 Linker ;
  //typedef LinkedFrom<MCParticle,CaloDigit>                 Linker ;

  // create and register the relation table 
  Table* table = new Table( 1000 ) ;
  StatusCode sc = put( table , outputData() ) ;
  if ( sc.isFailure() ) { return sc ; }
  
  if ( inputs().empty() ) 
  { return Error ( "No Linker object are specified" ) ; }
  
  // loop over all input linkers 
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    
    // get the container of digits
    Digits* digits = get<Digits> ( *input ) ;
    if ( 0 == digits ) { return StatusCode::FAILURE ; }
    
    // get linker 
    Linker linker ( eventSvc  () ,
                    msgSvc    () , 
                    (*input)     ) ;
    
    if ( linker.notFound() ) 
    { return Error ( "No Linker object is Found '" + (*input) + "'" ) ; }


    // loop over all digits 
    for ( Digits::const_iterator idigit = digits->begin() ; 
          digits->end() != idigit ; ++idigit ) 
    {
      const Digit* digit = *idigit ;
      if ( 0 == digit ) { continue ; }
      
      // use Linker
      const MCParticle* particle = linker.first( digit ) ;
      if ( 0 == particle ) { continue ; }
      
      // use the auxillary container to be immune agains 
      // potential bugs (if any..), like duplicated entries, etc 
      // in the Linkers
      CaloMCTools::CaloMCMap mcMap ;
      
      // loop over Linker entries 
      while ( 0 != particle ) 
      {
        // get the energy from this particle 
        const double weight = linker.weight() ;
        // update the auxillary container 
        mcMap [particle] += weight ;
        // next link
        particle = linker.next() ;
      }
      
      // convert the auxillary container into Relation Table  
      for ( CaloMCTools::CaloMCMap::iterator entry = mcMap.begin() ; 
            mcMap.end() != entry ; ++entry ) 
      {
        const MCParticle* particle = entry -> first  ;
        const double      energy   = entry -> second ;
        // use fast i_push method! 
        table->i_push ( digit , particle , energy ) ; // NB !!
      }
      
    };
    
  }; // end of loop over all input containers
  
  // mandatory operation after "i_push"!       
  table->i_sort() ;                               // NB !!

  if ( table->relations().empty() ) 
  { Warning ( "Empty Relation table" ) ; }
  
  if ( msgLevel ( MSG::DEBUG ) ) 
  { debug() << "Number of MC-links #" 
            << table->relations().size() << endreq ; }
  
  return StatusCode::SUCCESS ;
  
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
