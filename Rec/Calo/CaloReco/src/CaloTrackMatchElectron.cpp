// $Id: CaloTrackMatchElectron.cpp,v 1.6 2004-10-26 17:51:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include <functional>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GaudiException.h"
// ============================================================================
// CaloInterfaces
// ============================================================================
#include "CaloKernel/CaloPrint.h"
// ============================================================================
// Calo related
// ============================================================================
#include "Event/CaloCluster.h"
// ============================================================================
// track related
// ============================================================================
#include "Event/TrStoredTrack.h"
#include "Event/TrStateP.h"
#include "TrKernel/ITrExtrapolator.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackMatchElectron.h"
// ============================================================================

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
  , m_matchCalo ( HepVector ( 3 , 0 ) , HepSymMatrix  ( 3 , 0 ) ) 
  , m_matchTrk1 ( HepVector ( 3 , 0 ) , HepSymMatrix  ( 3 , 0 ) ) 
  , m_matchTrk2 ( HepVector ( 3 , 0 ) , HepDiagMatrix ( 3 , 0 ) ) 
{
  setProperty ( "Extrapolator" ,  "TrLinearExtrapolator" ) ;
  setProperty ( "ZMin"         ,  "7000"                 ) ; //  7 * meter
  setProperty ( "ZMax"         ,  "30000"                ) ; // 30 * meter
  setProperty ( "PID"          ,  "211"                  ) ; // pion  
  setProperty ( "Tolerance"    ,  "5"                    ) ; //  5 * mm
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
  if ( 0 == caloObj ) { return Error("CaloPosition* points to NULL"); }
  
  // find/extrapolate  the correct state 

  StatusCode sc = StatusCode::SUCCESS ;
  if ( !m_optimized )
  {
    sc = findState ( trObj , caloObj->z() , caloObj->z() ); 
  }
  else 
  {
    const double covXX = caloObj->covariance().fast(1,1) ;
    const double covYY = caloObj->covariance().fast(2,2) ;  
    sc = findState ( trObj        , 
                     caloObj->z() , 
                     caloObj->z() , 
                     covXX        , 
                     covYY        ) ; 
  }
  if ( sc.isFailure() ) { return Error("Correct state is not found" , sc ) ; }
  
  // TrStateP ?
  const TrStateP *state = dynamic_cast<TrStateP*>( m_state );
  if ( 0 == state ) { return Error( "Closest state is not 'TrStateP'"); }
  
  { // some trivial checks 
    const HepSymMatrix& cov = state->stateCov() ;
    if ( 0 >= cov.fast( 1 , 1 ) || 
         0 >= cov.fast( 2 , 2 ) || 
         0 >= cov.fast( 5 , 5 ) ) 
    {
      if ( msgLevel ( MSG::DEBUG ) ) 
      {
        debug() 
          << " Problems with state covarinace matrix: "
          << bits( trObj ) << endreq ;
        Warning("Negative diagonal elements of track covariance matrix "); 
      } 
    }
  }
  
  try 
  { 
    chi2_result = chi2 ( prepareCluster ( caloObj ) , 
                         prepareTrack   ( state   ) ) ;

  }
  catch ( const GaudiException& exc )
  { return Error( exc.message(), exc.code() ); }

  
  return StatusCode::SUCCESS;
  
};
// ============================================================================

// ============================================================================
/** the main matching method  
 *
 *  @param caloObj  pointer to "calorimeter" object (position)
 *  @param trObj    pointer to tracking object (track)
 *  @param chi2     returned value of chi2 of the matching
 *  @return status code for matching procedure 
 */
// ============================================================================
StatusCode CaloTrackMatchElectron::match 
( const CaloPosition*   /* caloObj */  , 
  const TrgTrack*       /* trObj   */  ,
  double&               /* chi2    */  ) 
{ return Error ( "match(TrgTrack*): not implemented(yet)!" ) ; } ;
// ============================================================================

// ============================================================================
// The End
// ============================================================================
