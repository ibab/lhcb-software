// $Id: CaloTrackMatchPhoton.cpp,v 1.3 2004-10-24 12:17:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
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
#include "TrKernel/ITrExtrapolator.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackMatchPhoton.h"
// ============================================================================

// ============================================================================
/** @file CaloTrackMatchPhoton.cpp
 *
 * Implementation file for class : CaloTrackMatchPhoton
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
static const  ToolFactory<CaloTrackMatchPhoton>         s_Factory;
const        IToolFactory&CaloTrackMatchPhotonFactory = s_Factory;

// ============================================================================
/** standard constructor
 *  @param type tool type   (useless)
 *  @param name tool name
 *  @param parent pointer to parent object
 *         (algorithm, service or another tool)
 */
// ============================================================================
CaloTrackMatchPhoton::CaloTrackMatchPhoton
( const std::string &type,
  const std::string &name,
  const IInterface  *parent )
  : CaloTrackMatchBase( type, name , parent )
{
  setProperty( "Extrapolator" ,  "TrLinearExtrapolator" ) ;
  setProperty( "ZMin"         ,  "7000"                 ) ; //  7 * meter
  setProperty( "ZMax"         ,  "30000"                ) ; // 30 * meter
  setProperty( "PID"          ,  "211"                  ) ; // pion
  setProperty( "Tolerance"    ,  "15.0"                 ) ; // 15 * mm
};
// ============================================================================
/** destructor is protected and virtual
 */
// ============================================================================
CaloTrackMatchPhoton::~CaloTrackMatchPhoton() {}

// ============================================================================
/** the main matching method
 *
 *  @param caloObj  pointer to "calorimeter" object (position)
 *  @param trObj    pointer to tracking object (state)
 *  @param chi2     returned value of chi2 of the matching
 *  @return status code for matching procedure
 */
// ============================================================================
StatusCode CaloTrackMatchPhoton::match
( const CaloPosition  *caloObj,
  const TrStoredTrack *trObj,
  double              &chi2_result )
{ 
  // set 'bad' value 
  chi2_result = m_bad ;
  
  // check calo 
  if ( 0 == caloObj   ) { return Error ( "CaloPosition* points to NULL"  ) ; }
  
  // find the closest state and extrapolate it 
  StatusCode sc = findState ( trObj, caloObj->z(), caloObj->z() );
  if ( sc.isFailure() ) { return Error ( "No valid state is found!" , sc ) ; }
  
  if ( 0 == m_state   ) { return Error ( "TrState* points to NULL"       ) ; }
  
  // the resulting function can throw an exception in case of failure,
  // catch it.
  try
  {
    chi2_result = chi2 ( prepareCluster ( caloObj ) , 
                         prepareTrack   ( m_state ) ) ;
  }
  catch ( const GaudiException& exc )
  { return Error ( exc.message(), exc.code() ); }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
