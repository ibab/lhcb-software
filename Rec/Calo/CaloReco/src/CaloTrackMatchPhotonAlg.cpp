// $Id: CaloTrackMatchPhotonAlg.cpp,v 1.1.1.1 2002-11-13 20:46:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/06/19 09:54:37  ibelyaev
//  remove compiler problem kindly reported by Chris Jones
//
// Revision 1.1.1.1  2002/05/21 14:18:21  ibelyaev
// New package
//
// ============================================================================
// include 
#include "Relations/RelationWeighted2D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloTrackMatch.h"
// Event/CaloEvent 
#include "Event/CaloCluster.h"
// Event/TrEvent 
#include "Event/TrStoredTrack.h"
// local
#include "CaloTrackMatchPhotonAlg.h"

// ============================================================================
/** @file 
 *  
 * Implementation file for class : CaloTrackMatchPhotonAlg
 * 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 27/04/2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloTrackMatchPhotonAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloTrackMatchPhotonAlg>          s_factory ;
const        IAlgFactory& CaloTrackMatchPhotonAlgFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @param name algorithm name
 *  @param pSvc service locator
 */
// ============================================================================
CaloTrackMatchPhotonAlg::CaloTrackMatchPhotonAlg
( const std::string& name  ,
  ISvcLocator*       pSvc  )
  : CaloAlgorithm ( name , pSvc ) 
  , m_matchTypeName (    ) 
  , m_match         ( 0  ) 
  , m_trackData     (    ) // no default value (yet!)
  , m_chi2Cut       ( 1.e+30 )
{
  declareProperty ( "MatchingTool" , m_matchTypeName );
  declareProperty ( "Tracks"       , m_trackData     );
  declareProperty ( "Chi2Cut"      , m_chi2Cut       );
};
// ============================================================================

// ============================================================================
/// destructor (protected and virtual)
// ============================================================================
CaloTrackMatchPhotonAlg::~CaloTrackMatchPhotonAlg() {}; 
// ============================================================================

// ============================================================================
/**  standard Algorithm initialization
 *   @see CaloAlgorithm
 *   @see     Algorithm
 *   @see    IAlgorithm
 *   @return status code 
 */
// ============================================================================
StatusCode CaloTrackMatchPhotonAlg::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize () ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm" , sc );}
  // locate matching tool
  m_match = tool( m_matchTypeName , m_match );
  // 
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm finalization
 *   @see CaloAlgorithm
 *   @see     Algorithm
 *   @see    IAlgorithm
 *   @return status code 
 */
// ============================================================================
StatusCode CaloTrackMatchPhotonAlg::finalize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  // release match tool 
  if( 0 != m_match ) { m_match->release() ; m_match = 0 ; }
  // finalize the base class 
  return CaloAlgorithm::finalize () ;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm execution
 *   @see     Algorithm
 *   @see    IAlgorithm
 *   @return status code 
 */
// ============================================================================
StatusCode CaloTrackMatchPhotonAlg::execute() 
{
  /// container of clusters (const)
  typedef const CaloClusters                                  Clusters ; 
  /// container of tracks   (const)
  typedef const TrStoredTracks                                Tracks   ; 
  /// relation table
  typedef RelationWeighted2D<CaloCluster,TrStoredTrack,float> Table    ; 
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input clusteres 
  Clusters* clusters = get( eventSvc() , inputData() , clusters );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // get tracks 
  Tracks*   tracks   = get( eventSvc() , m_trackData , tracks   );
  if( 0 == tracks   ) { return StatusCode::FAILURE ; }
  
  // create the relation table and register it in Gaudi Event Transient Store
  Table* table = new Table( clusters->size() * tracks->size() );
  put( table , outputData() );
  
  // loop over clusters and tracks, match all-by-all 
  for( Clusters::const_iterator cluster = clusters->begin() ;
       clusters->end() != cluster ; ++cluster )
    {
      // skip NULLs 
      if( 0 == *cluster )       { continue ; }               // CONTINUE !
      for( Tracks::const_iterator track = tracks->begin() ;
           tracks->end() != track ; ++track )
        {
          if( 0 == *track )     { continue ; }               // CONTINUE !
          /// get matching 
          ICaloTrackMatch::MatchingPair p = 
            (*m_match)( &(*cluster)->position() , *track );
          if( p.first.isFailure() )
            { 
              Error("Error from matching tool, skip the pair" , p.first );
              continue ;                                     // CONTINUE ! 
            }
          /// make relations 
          if( p.second < m_chi2Cut )
            { table->relate( *cluster , *track , (float) p.second ); }
        }
    }
  ///
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
