// $Id: CaloTrackMatchElectron.cpp,v 1.3 2004-03-08 13:45:25 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// Include files
// GaudiKernel
#include <functional>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GaudiException.h"

// CaloInterfaces
#include "CaloKernel/CaloPrint.h"

// Calo related
#include "Event/CaloCluster.h"

// track related
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"
#include "TrKernel/ITrExtrapolator.h"

// local
#include "CaloTrackMatchElectron.h"

// ============================================================================
/** @file CaloTrackMatchElectron.cpp
 *
 * Implementation file for class : CaloTrackMatchElectron
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
static const  ToolFactory<CaloTrackMatchElectron>         s_Factory;
const        IToolFactory&CaloTrackMatchElectronFactory = s_Factory;

// ============================================================================
/** standard constructor
 *  @param type tool type   (useless)
 *  @param name tool name
 *  @param parent pointer to parent object
 *         (algorithm, service or another tool)
 */
// ============================================================================
CaloTrackMatchElectron::CaloTrackMatchElectron( const std::string &type,
                                                const std::string &name,
                                                const IInterface  *parent )
  : CaloTrackMatchBase( type, name , parent )
{
  setProperty( "Extrapolator" ,  "TrLinearExtrapolator" ) ;
  setProperty( "ZMin"         ,  "7000"                 ) ; //  7 * meter
  setProperty( "ZMax"         ,  "30000"                ) ; // 30 * meter
  setProperty( "PID"          ,  "211"                  ) ; // pion  
  setProperty( "Tolerance"    ,  "1.0"                  ) ; //  1 * mm
  // supress a little bit the output level 
  setProperty( "OutputLevel"  ,  "5"                    ) ; // MSG::ERROR
};
// ============================================================================
/** destructor is protected and virtual
 */
// ============================================================================
CaloTrackMatchElectron::~CaloTrackMatchElectron() {}
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
StatusCode CaloTrackMatchElectron::match 
( const CaloPosition*  caloObj     ,
  const TrStoredTrack* trObj       ,
  double&              chi2_result )
{
  // set 'bad' value 
  chi2_result = m_bad ;
  
  // check calo 
  if( 0 == caloObj ) { return Error("CaloPosition* points to NULL"); }
  
  // find/extrapolate  the correct state 
  StatusCode sc = findState( trObj , caloObj->z() , caloObj->z() ); 
  if( sc.isFailure() ) { return Error("Correct state is not found" , sc ) ; }
  
  // TrStateP ?
  const TrStateP *state = dynamic_cast<TrStateP*>( m_state );
  if( 0 == state ) { return Error( "Closest state is not 'TrStateP'"); }
  
  { // some trivial checks 
    const HepSymMatrix& cov = state->stateCov() ;
    if( 0 >= cov.fast( 1 , 1 ) || 
        0 >= cov.fast( 2 , 2 ) || 
        0 >= cov.fast( 5 , 5 ) ) 
      {
        MsgStream log( msgSvc() , name() ) ;
        log << MSG::DEBUG 
            << " Problems with state covarinace matrix: \n "
            << " Track Flags/Bits : "
            << " e: " << trObj -> errorFlag ()
            << " u: " << trObj -> unique    () 
            << " f: " << trObj -> forward   () 
            << " m: " << trObj -> match     () 
            << " v: " << trObj -> velo      () 
            << " s: " << trObj -> seed      () 
            << " d: " << trObj -> isDownstream  ()
            << " h: " << trObj -> history   ()
            << " k: " << trObj -> key       ()
            << " \n" 
            << " matrix: \n" << cov 
            << " vector: \n" << state->stateVector() 
            << endreq ;
        Warning("Negative diagonal elements of track covariance matrix "); 
      } 
  }
  
  try { 
    chi2_result = chi2( prepareCluster( caloObj ) , 
                        prepareTrack  ( state ) ) ;
    
  }
  catch( const GaudiException& exc )
    { return Error( exc.message(), exc.code() ); }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
