// $Id: CaloTrackMatchElectron.cpp,v 1.9 2006-06-14 16:49:22 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2006/05/30 09:42:06  odescham
// first release of the CaloReco migration
//
// Revision 1.7  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.6  2004/10/26 17:51:42  ibelyaev
//  add 'photon' matching for Trg Tracks
//
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include <functional>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GaudiException.h"
// Calo related
#include "Event/CaloCluster.h"
// track related
#include "Event/Track.h"
#include "Event/State.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// local
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
  , m_matchCalo ( Gaudi::Vector3() , Gaudi::SymMatrix3x3() ) 
  , m_matchTrack ( Gaudi::Vector3() , Gaudi::SymMatrix3x3() ) 
{
  setProperty ( "Extrapolator" ,  "TrackLinearExtrapolator" ) ;
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
( const LHCb::CaloPosition*  caloObj     ,
  const LHCb::Track* trObj       ,
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
    const double covXX = caloObj->covariance()(0,0) ;
    const double covYY = caloObj->covariance()(1,1) ;  
    sc = findState ( trObj        , 
                     caloObj->z() , 
                     caloObj->z() , 
                     covXX        , 
                     covYY        ) ; 
  }
  if ( sc.isFailure() ) { return Error("Correct state is not found" , sc ) ; }
  
  // State ?
  const LHCb::State *state = dynamic_cast<LHCb::State*>( m_state );
  if ( 0 == state ) { return Error( "Closest state is not 'State'"); }
  { // some trivial checks 
    const Gaudi::TrackSymMatrix& cov = state->covariance() ;
    if ( 0 >= cov( 0 , 0 ) || 
         0 >= cov( 1 , 1 ) || 
         0 >= cov( 4 , 4 ) ) 
    {
      debug() 
        << " Problems with state covariance matrix: "
        << bits( trObj ) << endreq ;
      Warning("Negative diagonal elements of track covariance matrix "); 
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
// The End
// ============================================================================
