// $Id: CaloTrackMatchBremm.cpp,v 1.8 2005-11-07 12:12:43 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2004/10/26 17:51:42  ibelyaev
//  add 'photon' matching for Trg Tracks
//
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
#include "Event/Track.h"
#include "Event/State.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
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
  //
  , m_matchCalo ( HepVector ( 2 , 0 ) , HepSymMatrix  ( 2 , 0 ) ) 
  , m_matchTrk1 ( HepVector ( 2 , 0 ) , HepSymMatrix  ( 2 , 0 ) ) 
  , m_matchTrk2 ( HepVector ( 2 , 0 ) , HepDiagMatrix ( 2 , 0 ) )
  //
  , m_bremZ  ( 2.165 * meter )
{
  declareProperty ( "BremZ"       , m_bremZ  ) ;
  setProperty ( "Extrapolator" ,  "TrackLinearExtrapolator" ) ;
  setProperty ( "ZMin"         ,  "500"                  ) ; //  0.5 * meter
  setProperty ( "ZMax"         ,  "4500"                 ) ; //  4.5 * meter
  setProperty ( "PID"          ,  "22"                   ) ; // photon 
  setProperty ( "Tolerance"    ,  "15.0"                 ) ; //   15 * mm  
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
  const Track *trObj,
  double              &chi2_result )
{
  // set 'bad' value 
  chi2_result = m_bad ;
  
  // check calo 
  if ( 0 == caloObj   ) { return Error("CaloPosition* points to NULL"); }
  
  // find/extrapolate  the correct state 
  StatusCode sc = StatusCode::SUCCESS ;
  if ( !m_optimized )
  {
    sc = findState ( trObj , m_bremZ , caloObj->z() ); 
  }
  else 
  {
    const double covXX = caloObj->covariance().fast(1,1) ;
    const double covYY = caloObj->covariance().fast(2,2) ;  
    sc = findState ( trObj        , 
                     m_bremZ      , 
                     caloObj->z() , 
                     covXX        , 
                     covYY        ) ; 
  }
  if ( sc.isFailure() ) { return Error("Correct state is not found" , sc ) ; }
  
  if ( 0 == m_state   ) { return Error("State* points to NULL!"); }
  
  /// the resulting function can throw an exception in case of failure
  try
  { 
    chi2_result = chi2 ( prepareCluster( caloObj ) ,
                         prepareTrack  ( m_state ) );
  }
  catch ( const GaudiException &exc )
  { return Error ( exc.message() ,  exc.code() ) ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The End
// ============================================================================
