// $Id: CaloCluster2TrackAlg.cpp,v 1.5 2004-10-26 20:35:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/TrStoredTrack.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// local
// ============================================================================
#include "CaloCluster2TrackAlg.h"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloCluster2TrackAlg
 *  @see CaloCluster2TrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloCluster2TrackAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloCluster2TrackAlg>         s_Factory ;
const        IAlgFactory&CaloCluster2TrackAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloCluster2TrackAlg::CaloCluster2TrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloTrackAlg  ( name , svcloc ) 
  , m_tracks      ( TrStoredTrackLocation::Default )
  , m_cut         ( 100        )
  //
  , m_matchType   ( "CaloTrackMatchPhoton" ) 
  , m_matchName   ( ""         ) 
  , m_match       ( 0          ) 
{
  declareProperty ( "Tracks"         , m_tracks    ) ;
  declareProperty ( "MatchType"      , m_matchType ) ;
  declareProperty ( "MatchName"      , m_matchName ) ;
  declareProperty ( "Cut"            , m_cut       ) ;
  // 
  setProperty     ( "UseUniqueOnly"  , "true"      ) ;
  setProperty     ( "UseErrorAlso"   , "false"     ) ;
  //
  setProperty     ( "isLong"         , "true"      ) ;
  setProperty     ( "isUpstream"     , "false"     ) ;
  setProperty     ( "isDownstream"   , "true"      ) ;
  setProperty     ( "isVelotrack"    , "false"     ) ;
  setProperty     ( "isBackward"     , "false"     ) ;
  setProperty     ( "isTtrack"       , "true"      ) ;
  //
  // set the approproate default value for input  data 
  setInputData    ( CaloClusterLocation::  Ecal ) ;
  // set the approproate default value for output data 
  setOutputData   ( "Rec/Calo/PhotonMatch"      ) ;
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloCluster2TrackAlg::~CaloCluster2TrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloCluster2TrackAlg::initialize() 
{
  // initialize the base class 
  StatusCode sc = CaloTrackAlg::initialize();
  if ( sc.isFailure() ) { return sc ; } 
  
  // retrieve the tool from Tool service 
  m_match = tool<ICaloTrackMatch>( m_matchType , m_matchName );
  if ( 0 == m_match ) { return StatusCode::FAILURE ;}
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloCluster2TrackAlg::execute() 
{
  // avoid long names 
  typedef const TrStoredTracks                                 Tracks   ;
  typedef const TrStoredTrack                                  Track    ; 
  typedef const CaloClusters                                   Clusters ;
  typedef const CaloCluster                                    Cluster  ;
  typedef RelationWeighted2D<CaloCluster,TrStoredTrack,float>  Table    ;
  
  // get clusters from Transient Store  
  Clusters* clusters = get<Clusters> ( inputData() ) ;
  if ( 0 ==  clusters           )     { return StatusCode::FAILURE ; }
  
  // get tracks   from Transient Store  
  Tracks*   tracks   = get<Tracks>   (  m_tracks   ) ;
  if ( 0 ==  tracks             )     { return StatusCode::FAILURE ; }
  
  // create relation table and register it in the store
  Table*    table    = new Table();
  StatusCode sc      = put( table , outputData() );
  if ( sc.isFailure()           )     { return StatusCode::FAILURE ; }
  
  if ( 0 == tracks   -> size () )
  { Warning ( "Empty container of Tracks   " , StatusCode::SUCCESS ) ; }
  if ( 0 == clusters -> size () )
  { Warning ( "Empty container of Clusters " , StatusCode::SUCCESS ) ; }
  
  if ( 0 == tracks   -> size () || 
       0 == clusters -> size () )     { return StatusCode::SUCCESS ; }
  
  // loop over all tracks 
  for ( Tracks::const_iterator track = tracks->begin() ; 
        tracks->end() != track ; ++track )
  {  
    // skip NULLs 
    if ( 0 == *track       ) { continue ; }                // CONTINUE  
    
    // use track ? 
    if ( ! use( *track )   ) { continue ; }                // CONTINUE 
    
    // loop over all clusters 
    for ( Clusters::const_iterator cluster = clusters->begin() ; 
          clusters->end() != cluster ; ++cluster )
    {
      // skip NUULs 
      if ( 0 == *cluster  ) { continue ; }               // CONTINUE 
      
      // perform matching
      double chi2 = 0 ;
      StatusCode sc = m_match -> 
        match( &((*cluster)->position()) , *track , chi2 );
      
      if ( sc.isFailure() )
      {
        Warning ( "Cluster/Track: matching failure, skip the pair", sc ) ;
        continue ;                                       /// CONTINUIE 
      }
      else if ( 0 <= chi2 && chi2 <=  m_cut ) 
      { table->relate( *cluster , *track , chi2 ); }
      
    }; // end of loop over all clusters 
  }; // end of loop over all  tracks
  
  if ( msgLevel( MSG::DEBUG ) ) 
  { debug() << "Entries in the table " << table->relations().size() << endreq ; }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
