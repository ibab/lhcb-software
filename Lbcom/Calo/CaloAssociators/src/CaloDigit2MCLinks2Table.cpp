// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/RelationWeighted1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IRegistry.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/MCParticle.h"
#include "CaloUtils/CaloMCTools.h"
#include "CaloUtils/Calo2MC.h"
// ============================================================================
// LinkerEvent
// ============================================================================
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
// ============================================================================
/** @class  CaloDigit2MCLinks2Table  CaloDigit2MCLinks2Table.cpp
 *
 *  Helper algorithm to "decode" the idiotic linker object 
 *  into the relation table for Monte Carlo associatiations 
 *  from CaloDigits to MCParticle
 *
 *  The important "properties" of this algorithm are:
 *   
 *  - "Inputs"
 *    The default value is            CaloDigitLocation::Ecal  
 *    List of Linker objects for CaloDigit->MCParticle links 
 * 
 *  - "Output"
 *    The default value is "Relations/" + LHCb::CaloDigitLocation::Default
 *    Name of (output) relation table for CaloDigit->MCParticle 
 *
 *  @see CaloDigit
 *  @see MCParticle
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2004-05-03
 */
// ============================================================================
class CaloDigit2MCLinks2Table : public GaudiAlgorithm
{
  // ==========================================================================
  friend class AlgFactory<CaloDigit2MCLinks2Table> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// algorithm execution 
  virtual StatusCode execute    () ;
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc pointer to service locator
   */
  CaloDigit2MCLinks2Table
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    , m_inputs ( 1 , LHCb::CaloDigitLocation::Ecal      ) 
    , m_output ( "Relations/" + LHCb::CaloDigitLocation::Default ) 
  {
    declareProperty 
      ( "Inputs" , 
        m_inputs , 
        "TES-Input Locations of CaloDigits & Linkers") ;
    declareProperty 
      ( "Output" , 
        m_output , 
        "TES-Output Location of Relation Table CaloDigit->MCParticle" ) ;
  } 
  /// virtual destructor (protected)
  virtual ~CaloDigit2MCLinks2Table() {}
  // ==========================================================================
private:
  // ==========================================================================
  // default constructor  is disabled 
  CaloDigit2MCLinks2Table ();
  // copy    constructor  is disabled  
  CaloDigit2MCLinks2Table 
  ( const CaloDigit2MCLinks2Table& ) ;
  // assignement operator is disabled
  CaloDigit2MCLinks2Table& operator=
  ( const CaloDigit2MCLinks2Table& ) ;
  // ==========================================================================
private:
  // ==========================================================================
  typedef std::vector<std::string>  Inputs ;
  Inputs      m_inputs ;
  std::string m_output ;
  // ==========================================================================
};
// ============================================================================
// declare the factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY(CaloDigit2MCLinks2Table)
// ============================================================================
// algorithm execution 
// ============================================================================
StatusCode CaloDigit2MCLinks2Table::execute    () 
{
  /// the actual type of calorimeter digit 
  typedef const LHCb::CaloDigit                      Digit  ;
  /// the actual type of container
  typedef const LHCb::CaloDigits                     Digits ;
  /// the actual type of relation table 
  typedef LHCb::Calo2MC::DigitTable Table  ;
  
  // create and register the relation table 
  Table* table = new Table ( 3000 ) ;
  put ( table , m_output ) ;
  
  if ( m_inputs.empty() ) 
  { return Error ( "No Linker object are specified" ) ; }
  
  // loop over all input linkers 
  bool foundInput(false);
  for ( Inputs::const_iterator input = m_inputs.begin() ; 
        m_inputs.end() != input ; ++input ) 
  {
    
    // get the container of digits
    Digits * digits = getIfExists<Digits> ( *input ) ;
    if ( !digits || digits->empty() ) continue;
    foundInput = true;

    // get linker 
    LHCb::Calo2MC::DigitLinkTo linker( eventSvc() , msgSvc() , (*input) ) ;
    
    if ( linker.notFound() ) 
    { return Error ( "No Linker object is Found '" + (*input) + "' " ) ;}
    
    // loop over all digits 
    for ( Digits::const_iterator idigit = digits->begin() ; 
          digits->end() != idigit ; ++idigit ) 
    {
      const Digit* digit = *idigit ;
      if ( 0 == digit ) { continue ; }
      
      // use Linker
      const LHCb::MCParticle* particle = linker.first( digit ) ;
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
        const LHCb::MCParticle* particle = entry -> first  ;
        const float             energy   = (float)entry -> second ;
        // use fast i_push method! 
        table->i_push ( digit , particle , energy ) ; // NB !!
      }
      
    } // end of the loop over digits in the container 
    
  } // end of loop over all input containers
  
  // mandatory operation after "i_push"!       
  table->i_sort() ;                               // NB !!
  
  if ( foundInput && 
       table->relations().empty() ) { Warning ( "Empty Relation table" ).ignore() ; }
  
  if ( msgLevel ( MSG::DEBUG ) ) 
  { debug() << "Number of MC-links #" 
            << table->relations().size() << endmsg ; }
  
  counter( "#CD2MC links") += table->relations().size() ;
  
  return StatusCode::SUCCESS ;
  
}
// ============================================================================
// The END 
// ============================================================================
