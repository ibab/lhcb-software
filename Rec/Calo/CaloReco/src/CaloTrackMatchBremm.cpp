// $Id: CaloTrackMatchBremm.cpp,v 1.3 2004-02-17 12:08:11 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/12/01 14:22:58  ibelyaev
//  Hcal stuff and updated S-coprrections
//
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// ============================================================================
// Include files
#include "CLHEP/Matrix/Matrix.h"
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GaudiException.h"

// Calo related
#include "Event/CaloCluster.h"
#include "CaloKernel/CaloPrint.h"

// track related
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"

// local
#include "CaloTrackMatchBremm.h"

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
  declareProperty( "BremZ"       , m_bremZ  ) ;
  declareProperty( "MaxPrints"   , m_prints ) ;
  
  setProperty( "Extrapolator" ,  "TrLinearExtrapolator" ) ;
  setProperty( "ZMin"         ,  "500"                  ) ; //  0.5 * meter
  setProperty( "ZMax"         ,  "4500"                 ) ; //  4.5 * meter
  setProperty( "PID"          ,  "22"                   ) ; // photon 
  setProperty( "Tolerance"    ,  "50.0"                 ) ; // 50 * mm  

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
  if( 0 == caloObj   ) { return Error("CaloPosition* points to NULL"); }
  
  // find/extrapolate  the correct state 
  StatusCode sc = findState( trObj , m_bremZ , caloObj->z() ); 
  if( sc.isFailure() ) { return Error("Correct state is not found" , sc ) ; }
  
  if( 0 == m_state   ) { return Error("TrState* points to NULL!"); }
  
  /// the resulting function can throw an exception in case of failure
  try
    { 
      chi2_result = chi2( prepareCluster( caloObj ) ,
                          prepareTrack  ( m_state ) );
    }
  catch( const GaudiException &exc )
    { 
      if( m_counts++ <= m_prints ) 
        {    
          MsgStream log( msgSvc() , name() );
          log << MSG::ERROR << " Matrix Problems: " ;
          CaloPrint print;
          std::string msg( "bits: ") ;
          msg +=  "E:" + print( (int) trObj -> errorFlag () ) ;
          msg += "/U:" + print( (int) trObj -> unique    () ) ;
          msg += "/H:" + print( (int) trObj -> history   () ) ;
          msg += "/f:" + print( (int) trObj -> forward   () ) ;
          msg += "/m:" + print( (int) trObj -> match     () ) ;
          msg += "/v:" + print( (int) trObj -> velo      () ) ;
          msg += "/t:" + print( (int) trObj -> veloTT    () ) ;
          msg += "/b:" + print( (int) trObj -> veloBack  () ) ;
          msg += "/s:" + print( (int) trObj -> seed      () ) ;
          msg += "/u:" + print( (int) trObj -> upstream  () ) ;
          log << msg 
              << "/k:" << (int) trObj -> key () 
              << endreq ;
          MatchType mt1( prepareCluster ( caloObj ) );
          MatchType mt2( prepareTrack   ( m_state ) );
          log << " Matrix1 " << mt1.cov    << endreq ;
          log << " Vector1 " << mt1.params << endreq ;
          log << " Matrix2 " << mt2.cov    << endreq ;
          log << " Vector2 " << mt2.params << endreq ;
        } 
      return Error( exc.message() ,  exc.code() ); 
    }
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End
// ============================================================================
