// $Id: CaloTrackMatchPhoton.cpp,v 1.6 2004-10-26 20:35:58 ibelyaev Exp $
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
// ============================================================================
#include "TrKernel/ITrExtrapolator.h"
// ============================================================================
#include "Event/TrgTrack.h"
#include "Event/TrgState.h"
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
  // optimization variables 
    , m_matchCalo ( HepVector( 2 , 0 ) , HepSymMatrix  ( 2 , 0 ) ) 
  , m_matchTrk1 ( HepVector( 2 , 0 ) , HepSymMatrix  ( 2 , 0 ) ) 
  , m_matchTrk2 ( HepVector( 2 , 0 ) , HepDiagMatrix ( 2 , 0 ) ) 
  //
{
  setProperty ( "Extrapolator" ,  "TrLinearExtrapolator" ) ;
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
( const CaloPosition  *caloObj,
  const TrStoredTrack *trObj,
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
    const double covXX = caloObj->spread().fast(1,1) ;
    const double covYY = caloObj->spread().fast(2,2) ;  
    sc = findState ( trObj        , 
                     caloObj->z() , 
                     caloObj->z() , 
                     covXX        , 
                     covYY        ) ;
  }

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

namespace 
{
  template <class STATE>
  inline STATE findTrgState ( STATE               first , 
                              STATE               last  , 
                              TrgState::StateType type  )
  {
    for ( ; first != last ; ++first )
    { if ( type == first->type() ) { return first ; } }
    return last ;
  };
};

// ============================================================================
/** the main matching method  
 *
 *  @param caloObj  pointer to "calorimeter" object (position)
 *  @param trObj    pointer to tracking object (track)
 *  @param chi2res  returned value of chi2 of the matching
 *  @return status code for matching procedure 
 */
// ============================================================================
StatusCode CaloTrackMatchPhoton::match 
( const CaloPosition* caloObj , 
  const TrgTrack*     trObj   ,
  double&             chi2res ) 
{
  if ( 0 == caloObj ) { return Error ( "match(): invalid CaloPosition " ) ; }
  if ( 0 == trObj   ) { return Error ( "match(): invalid TrgTrack     " ) ; }
  
  typedef std::vector<TrgState> States ;
  const States& states = trObj->otherStates() ;
  States::const_iterator calo = findTrgState( states.begin ()  , 
                                              states.end   ()  , 
                                              TrgState::afterT ) ;
  //States::const_iterator calo = states.begin () ;
  
  if ( states.end() == calo ) 
  { return Error( "No state 'Calo' is found ") ; }
  
  // the resulting function can throw an exception in case of failure,
  // catch it.
  try
  {
    chi2res = chi2 ( prepareCluster ( caloObj              ) , 
                     prepareTrack   ( *calo , caloObj->z() ) ) ;
  }
  catch ( const GaudiException& exc )
  { return Error ( exc.message(), exc.code() ); }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
