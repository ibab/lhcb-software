// $Id: CaloTrackMatchPhoton.cpp,v 1.8 2006-05-30 09:42:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.6  2004/10/26 20:35:58  ibelyaev
//  improve properties of all Track-related algorithms
//
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// Calo related
#include "Event/CaloCluster.h"
// track related
#include "Event/Track.h"
#include "Event/State.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// local
#include "CaloTrackMatchPhoton.h"

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
  // optimization variables 
  , m_matchCalo   ( Gaudi::Vector2() , Gaudi::SymMatrix2x2()  ) 
  , m_matchTrack  ( Gaudi::Vector2() , Gaudi::SymMatrix2x2()  ) 

  //
{
  setProperty ( "Extrapolator" ,  "TrackLinearExtrapolator" ) ;
  setProperty ( "ZMin"         ,  "7000"                 ) ; //  7 * meter
  setProperty ( "ZMax"         ,  "30000"                ) ; // 30 * meter
  setProperty ( "PID"          ,  "211"                  ) ; // pion
  setProperty ( "Tolerance"    ,  "15"                   ) ; // 15 * mm
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
( const LHCb::CaloPosition  *caloObj,
  const LHCb::Track *trObj,
  double              &chi2_result )
{ 
  // set 'bad' value 
  chi2_result = m_bad ;
  
  // check calo 
  if ( 0 == caloObj   ) { return Error ( "CaloPosition* points to NULL"  ) ; }
  
  // find the closest state and extrapolate it 
  StatusCode sc = StatusCode::SUCCESS ;
  if ( !m_optimized ) 
  {
    sc = findState ( trObj, caloObj->z(), caloObj->z() );
  }
  else 
  {  
    const double covXX = caloObj->spread()(0,0) ;
    const double covYY = caloObj->spread()(1,1) ;  
    sc = findState ( trObj        , 
                     caloObj->z() , 
                     caloObj->z() , 
                     covXX        , 
                     covYY        ) ;
  }

  if ( sc.isFailure() ) { return Error ( "No valid state is found!" , sc ) ; }
  
  if ( 0 == m_state   ) { return Error ( "State* points to NULL"       ) ; }
  
  

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
