// $Id: CaloReCreateMCLinks.cpp,v 1.1 2005-01-12 09:59:14 ocallot Exp $

// Include files 
// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCTruth.h"

// CaloEvent
#include "Event/CaloDigit.h"
#include "Event/MCCaloDigit.h"

// local
#include "CaloReCreateMCLinks.h"

/** @file 
 *  Implementation file for class CaloReCreateMCLinks
 * 
 *  @date   2004-02-23 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

static const  AlgFactory<CaloReCreateMCLinks>         s_factory ;
const        IAlgFactory&CaloReCreateMCLinksFactory = s_factory ; 

// ============================================================================
/** Standard constructor
 *  @see GaudiAlgorithm
 *  @see      Algorithm 
 *  @see      AlgFactory
 *  @see     IAlgFactory 
 *  @param name algorithm instance name 
 *  @param pSvc service locator
 */
// ============================================================================
CaloReCreateMCLinks::CaloReCreateMCLinks( const std::string& name , 
                                          ISvcLocator*       pSvc ) 
  : GaudiAlgorithm ( name , pSvc ) 
  , m_raw ()
  , m_mc  () 
{
  m_raw   .push_back (    CaloDigitLocation::Spd   ) ;
  m_mc    .push_back (  MCCaloDigitLocation::Spd   ) ;
  m_raw   .push_back (    CaloDigitLocation::Prs   ) ;
  m_mc    .push_back (  MCCaloDigitLocation::Prs   ) ;
  m_raw   .push_back (    CaloDigitLocation::Ecal  ) ;
  m_mc    .push_back (  MCCaloDigitLocation::Ecal  ) ;
  m_raw   .push_back (    CaloDigitLocation::Hcal  ) ;
  m_mc    .push_back (  MCCaloDigitLocation::Hcal  ) ;
  
  declareProperty ( "Digits"   , m_raw ) ;
  declareProperty ( "MCDigits" , m_mc  ) ;
};
// ============================================================================
/// Destructor
// ============================================================================
CaloReCreateMCLinks::~CaloReCreateMCLinks() {}; 

// ============================================================================
/** execution of the algorithm
 *  @see IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloReCreateMCLinks::execute() {
  
  if ( m_raw.size() != m_mc.size () ) { 
    return Error(" 'Raw' and 'MC' containers in a contradiction " ) ; 
  }
  
  for ( size_t index = 0 ; index < m_raw.size() ; ++index ) {
    const std::string& addr1 = m_raw [index] ;
    const std::string& addr2 = m_mc  [index] ;
    
    if ( msgLevel( MSG::DEBUG ) ) { 
      debug () << " Set MClinks between containers '" 
               << addr1 << "' and '"
               << addr2 << "'" << endreq ;
    }
    
    CaloDigits*    raw = get<CaloDigits>    ( m_raw [ index ] ) ;
    MCCaloDigits*  mc  = get<MCCaloDigits>  ( m_mc  [ index ] ) ;
    
    StatusCode sc = setMCTruth ( raw , mc ) ;
    if( sc.isFailure() ) { 
      return Error ( " setMCTruth: unable to set MC-link between '" + 
                     addr1 + "' and '" + addr2 + "'" , sc ) ; 
    }
  }
  return StatusCode::SUCCESS;
};
// ============================================================================
