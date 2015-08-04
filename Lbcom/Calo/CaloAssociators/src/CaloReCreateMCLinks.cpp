// ============================================================================
// Include 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/MCTruth.h"
// ============================================================================
// CaloEvent
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/MCCaloDigit.h"
// ============================================================================

// ============================================================================
/** @class CaloReCreateMCLinks CaloReCreateMCLinks.cpp
 *
 *  Simple algorithm to be invoked after 
 *   CaloDigitFromBuffer to re-create the MC links 
 *
 *  @code 
 *
 *  // (re)create digits from Raw buffer 
 *  ApplicationMgr.TopAlg += { "CaloDigitFromRaw"} ;
 *  // (re)create Digits -> McDigits links 
 *  ApplicationMgr.TopAlg += { "CaloReCreateMCLinks" };
 *
 *  @endcode 
 *
 *   The major properties of the algorithm:
 *
 *    - "Digits"
 *       The default value is      CaloDigitLocation::Spd  +
 *                                 CaloDigitLocation::Prs  + 
 *                                 CaloDigitLocation::Ecal +
 *                                 CaloDigitLocation::Hcal 
 *       The list of addresses in TES for containters of CaloDigit
 *   
 *    - "MCDigits"
 *       The default value is      MCCaloDigitLocation::Spd  +
 *                                 MCCaloDigitLocation::Prs  + 
 *                                 MCCaloDigitLocation::Ecal +
 *                                 MCCaloDigitLocation::Hcal 
 *       The list of addresses in TES for containters of MCCaloDigit
 *  
 *    - "IgnoreMode" 
 *       The default value is                false 
 *       The flag to indicate "ignore" mode. In this mode 
 *       for missing MC-container an error message is generated.
 *       This mode is needed to simplify the 
 *       processing of Brunel for "old"/"new" data
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date   2004-02-23
 */
// ============================================================================
class CaloReCreateMCLinks : public GaudiAlgorithm 
{ 
  /// friend factory for algorithm instantiation
  friend class AlgFactory<CaloReCreateMCLinks> ;
public:
  /** execution of the algorithm
   *  @see IAlgorithm
   *  @return status code
   */
  virtual StatusCode execute() ;
protected:  
  /** Standard constructor
   *  @param name algorithm instance name 
   *  @param pSvc pointer to service locator
   */
  CaloReCreateMCLinks 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    , m_raw    ( )
    , m_mc     ( ) 
    , m_ignore ( true )
  {
    //m_raw   .push_back (    LHCb::CaloDigitLocation::Spd   ) ;
    //m_mc    .push_back (  LHCb::MCCaloDigitLocation::Spd   ) ;
    //m_raw   .push_back (    LHCb::CaloDigitLocation::Prs   ) ;
    //m_mc    .push_back (  LHCb::MCCaloDigitLocation::Prs   ) ;
    m_raw   .push_back (    LHCb::CaloDigitLocation::Ecal  ) ;
    m_mc    .push_back (  LHCb::MCCaloDigitLocation::Ecal  ) ;
    m_raw   .push_back (    LHCb::CaloDigitLocation::Hcal  ) ;
    m_mc    .push_back (  LHCb::MCCaloDigitLocation::Hcal  ) ;
    //
    declareProperty ( "Digits"     , m_raw     ) ;
    declareProperty ( "MCDigits"   , m_mc      ) ;
    declareProperty ( "IgnoreMode" , m_ignore  ) ;
  };
  // destructor 
  virtual ~CaloReCreateMCLinks( ){};
private:
  // default constructor  is disabled 
  CaloReCreateMCLinks() ;
  // copy constructor     is disabled 
  CaloReCreateMCLinks
  ( const CaloReCreateMCLinks& right ) ;
  // assignement operator is disabled 
  CaloReCreateMCLinks& operator=
  ( const CaloReCreateMCLinks& right ) ;  
private:
  typedef std::vector<std::string> Addresses ;
  Addresses m_raw    ;
  Addresses m_mc     ;
  bool      m_ignore ;
};
// ============================================================================

// ============================================================================
DECLARE_ALGORITHM_FACTORY(CaloReCreateMCLinks)
// ============================================================================

// ============================================================================
/** execution of the algorithm
 *  @see IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloReCreateMCLinks::execute() 
{
  //
  //
  if ( m_raw.size() != m_mc.size () ) 
  { return Error ( " 'Raw' and 'MC' containers in a contradiction " ) ; }
  //
  for ( size_t index = 0 ; index < m_raw.size() ; ++index ) 
  {
    // address for MC digits 
    const std::string& addr2 = m_mc  [index] ;
    // "ignore" mode ?
    if ( m_ignore && !exist<LHCb::MCCaloDigits> ( addr2 ) ) 
    { 
      Warning ( "(MC)container is not found '" + addr2 + "', skip" , StatusCode::SUCCESS , 0 ).ignore() ; 
      continue ;                                        // CONTINUE 
    }
    //
    // get MC digits from TES 
    LHCb::MCCaloDigits*  mc  = get<LHCb::MCCaloDigits>  ( addr2 ) ;
    //
    // address of digits 
    const std::string& addr1 = m_raw [index] ;
    //
    // get digits from TES 
    LHCb::CaloDigits*    raw = get<LHCb::CaloDigits>    ( addr1 ) ;
    //
    if ( msgLevel ( MSG::DEBUG ) ) 
    { 
      debug () << " Set MClinks between containers '" 
               << addr1 << "' and '"
               << addr2 << "'" << endmsg ;
    }
    //
    // set MC link
    StatusCode sc = setMCTruth ( raw , mc ) ;
    if ( sc.isFailure() ) 
    { 
      return Error ( " setMCTruth: unable to set MC-link between '" + 
                     addr1 + "' and '" + addr2 + "'" , sc ) ; 
    }
    
    // check the link:
    if ( 0 == mcTruth<LHCb::MCCaloDigits>( raw ) )
    { return Error("EXTRACTED LINK is NULL for '" + addr1 + "'") ; }
    
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END 
// ============================================================================
