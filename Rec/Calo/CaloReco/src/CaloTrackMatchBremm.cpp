// $Id: CaloTrackMatchBremm.cpp,v 1.5 2004-10-24 12:17:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// CLHEP 
// ============================================================================
#include "CLHEP/Matrix/Matrix.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GaudiException.h"
// ============================================================================
// Calo related
// ============================================================================
#include "Event/CaloCluster.h"
#include "CaloKernel/CaloPrint.h"
// ============================================================================
// track related
// ============================================================================
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackMatchBremm.h"
// ============================================================================


// ============================================================================
/** @file CaloTrackMatchBremm.cpp
 *
 * Implementation file for class : CaloTrackMatchBremm
 *
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */
// ============================================================================

// ============================================================================
/** instantiation of Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloTrackMatchBremm>         s_Factory;
const        IToolFactory&CaloTrackMatchBremmFactory = s_Factory;

// ============================================================================
/** standard constructor
 *  @param type tool type   (useless)
 *  @param name tool name
 *  @param parent pointer to parent object
 *         (algorithm, service or another tool)
 */
// ============================================================================
CaloTrackMatchBremm::CaloTrackMatchBremm
( const std::string &type,
  const std::string &name,
  const IInterface  *parent )
  : CaloTrackMatchBase( type, name , parent )
  , m_bremZ  ( 2.165 * meter )
  , m_counts ( 0  )
  , m_prints ( 20 )
{
  declareProperty ( "BremZ"       , m_bremZ  ) ;
  declareProperty ( "MaxPrints"   , m_prints ) ;
  
  setProperty ( "Extrapolator" ,  "TrLinearExtrapolator" ) ;
  setProperty ( "ZMin"         ,  "500"                  ) ; //  0.5 * meter
  setProperty ( "ZMax"         ,  "4500"                 ) ; //  4.5 * meter
  setProperty ( "PID"          ,  "22"                   ) ; // photon 
  setProperty ( "Tolerance"    ,  "50.0"                 ) ; // 50 * mm  
  
};
// ============================================================================

// ============================================================================
/** destructor is protected and virtual
 */
// ============================================================================
CaloTrackMatchBremm::~CaloTrackMatchBremm() {};
// ============================================================================

// ============================================================================
/** the main matching method
 *
 *  @param caloObj  pointer to "calorimeter" object (position)
 *  @param trObj    pointer to tracking object (state)
 *  @param chi2     returned value of chi2 of the matching
 *  @return status code for matching procedure
 */
// ============================================================================
StatusCode CaloTrackMatchBremm::match
( const CaloPosition  *caloObj,
  const TrStoredTrack *trObj,
  double              &chi2_result )
{
  // set 'bad' value 
  chi2_result = m_bad ;
  
  // check calo 
  if ( 0 == caloObj   ) { return Error("CaloPosition* points to NULL"); }
  
  // find/extrapolate  the correct state 
  StatusCode sc = findState( trObj , m_bremZ , caloObj->z() ); 
  if ( sc.isFailure() ) { return Error("Correct state is not found" , sc ) ; }
  
  if ( 0 == m_state   ) { return Error("TrState* points to NULL!"); }
  
  /// the resulting function can throw an exception in case of failure
  try
  { 
    chi2_result = chi2 ( prepareCluster( caloObj ) ,
                         prepareTrack  ( m_state ) );
  }
  catch ( const GaudiException &exc )
  { 
    if ( msgLevel( MSG::DEBUG ) ) 
    {
      debug() << "  Exception " << exc.message() 
              << "\nMatrix problems, track " << bits ( trObj ) << endreq ;
      MatchType1 mt1( prepareCluster ( caloObj ) ) ;
      MatchType1 mt2( prepareTrack   ( m_state ) ) ;
      debug() << " Err1/Matrix1 " << mt1.error << "/" << mt1.cov    << endreq ;
      debug() << " Vector1 "      << mt1.params                     << endreq ;
      debug() << " Err2/Matrix2 " << mt2.error << "/" << mt2.cov    << endreq ;
      debug() << " Vector2 "      << mt2.params                     << endreq ;
    } 
    return Error ( exc.message() ,  exc.code() ) ; 
  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End
// ============================================================================
