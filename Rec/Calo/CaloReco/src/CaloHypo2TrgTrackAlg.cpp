// $Id: CaloHypo2TrgTrackAlg.cpp,v 1.1 2004-12-10 17:12:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// Relations 
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
#include "Event/CaloHypo.h"
#include "Event/TrgTrack.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// local
// ============================================================================
#include "CaloHypo2TrgTrackAlg.h"
// ============================================================================

// ============================================================================
/** @file
 * 
 *  implementation file for class CaloHypo2TrackAlg
 *  @see CaloHypo2TrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloHypo2TrackAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHypo2TrgTrackAlg>         s_Factory ;
const        IAlgFactory&CaloHypo2TrgTrackAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloHypo2TrgTrackAlg::CaloHypo2TrgTrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  , m_tracks      ( TrgTrackLocation::Particle )
  , m_cut         ( 1.e+30     )
  //
  , m_matchType   ( "SomeType" ) 
  , m_matchName   ( ""         ) 
  , m_match       ( 0      ) 
{
  declareProperty( "Tracks"     , m_tracks     ) ;
  declareProperty( "MatchType"  , m_matchType  ) ;
  declareProperty( "MatchName"  , m_matchName  ) ;
  declareProperty( "Cut"        , m_cut        ) ;
  // 
 };
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloHypo2TrgTrackAlg::~CaloHypo2TrgTrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypo2TrgTrackAlg::initialize() 
{
  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize();
  if ( sc.isFailure() ) 
  { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  // retrieve the tool from Tool service 
  m_match = tool<ICaloTrackMatch>( m_matchType , m_matchName );
  
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
StatusCode CaloHypo2TrgTrackAlg::execute() 
{
  // avoid long names 
  typedef const TrgTracks                                   Tracks   ;
  typedef const TrgTrack                                    Track    ; 
  typedef const CaloHypos                                   Hypos    ;
  typedef const CaloHypo                                    Hypo     ;
  typedef RelationWeighted2D<CaloHypo,TrgTrack,float>       Table    ;
  
  // get Hypos from Transient Store  
  Hypos*    hypos    = get<Hypos>    ( inputData() ) ;
  if ( 0 ==  hypos              )     { return StatusCode::FAILURE ; }
  
  // get tracks   from Transient Store  
  Tracks*   tracks   = get<Tracks>   ( m_tracks    );
  if ( 0 ==  tracks             )     { return StatusCode::FAILURE ; }
  
  // create relation table and register it in the store
  Table*    table = new Table();
  StatusCode sc = put( table , outputData() );
  if ( sc.isFailure()           )     { return StatusCode::FAILURE ; }
  
  if ( 0 == tracks   -> size () )
  { Warning ( "Empty container of Tracks " , StatusCode::SUCCESS ) ; }
  if ( 0 == hypos    -> size () )
  { Warning ( "Empty container of Hypos  " , StatusCode::SUCCESS ) ; }
  
  if( 0 == tracks   -> size () || 
      0 == hypos    -> size () )     { return StatusCode::SUCCESS ; }
  
  // loop over tracks  
  for ( Tracks::const_iterator track = tracks->begin() ; 
        tracks->end() != track ; ++track )
  {  
    // skip NULLS 
    if ( 0 == *track            ) { continue ; }             // CONTINUE
    
    // loop over hypos 
    for ( Hypos::const_iterator hypo = hypos->begin() ; 
          hypos->end() != hypo ; ++hypo )
    {
      // skip NUULs 
      if ( 0 == *hypo          ) { continue ; }             /// CONTINUE 
      
      // valid hypo ? 
      if ( 0 == (*hypo)->position() )
      {
        Warning("CaloPosition* points to NULL!");
        continue ;                                      /// CONTINUE 
      }
      
      // perform the matching
      double chi2 = 0 ;
      StatusCode sc = m_match -> 
        match( (*hypo)->position() , *track , chi2 );
      
      if ( sc.isFailure() )
      {
        Warning("Hypo/Track: matching failure, skip pair" );
        continue ;
      }
      else if ( 0 <= chi2 && chi2 <=  m_cut ) 
      { table -> relate( *hypo , *track , chi2 ); }
      
    }; // loop over all hypos 
    
  }; // loop over all tracks 
  
  if ( msgLevel( MSG::DEBUG ) ) 
  { debug() << "Entries in the table " << table->relations().size() << endreq ; }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
