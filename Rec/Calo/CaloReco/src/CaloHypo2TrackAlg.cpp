// $Id: CaloHypo2TrackAlg.cpp,v 1.2 2004-02-17 12:08:07 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// ============================================================================
// Include files
#include "Relations/RelationWeighted2D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// Event 
#include "Event/CaloHypo.h"
#include "Event/TrStoredTrack.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackMatch.h"
// local
#include "CaloHypo2TrackAlg.h"

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
static const  AlgFactory<CaloHypo2TrackAlg>         s_Factory ;
const        IAlgFactory&CaloHypo2TrackAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloHypo2TrackAlg::CaloHypo2TrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  , m_tracks      ( TrStoredTrackLocation::Default )
  , m_cut         ( 1.e+30     )
  //
  , m_unique      ( false      )  // match ALL tracks, including clones 
  // 
  // use 'error'   tracks  
  , m_error       ( false      )
  //
  , m_forward     ( true       )   
  , m_matched     ( true       )
  , m_seed        ( true       )
  , m_velo        ( false      )  // skip velo tracks 
  , m_veloTT      ( false      )  // skip veloTT tracks 
  , m_veloBack    ( false      )  // skip veloBack tracks 
  , m_upstream    ( true       )
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
  declareProperty( "UseUnique"   , m_unique    ) ;
  declareProperty( "UseError"    , m_error     ) ;
  declareProperty( "UseForward"  , m_forward   ) ;
  declareProperty( "UseMatched"  , m_matched   ) ;
  declareProperty( "UseVelo"     , m_velo      ) ;
  declareProperty( "UseVeloTT"   , m_veloTT    ) ;
  declareProperty( "UseVeloBack" , m_veloBack  ) ;
  declareProperty( "UseSeed"     , m_seed      ) ;
  declareProperty( "UseUpstream" , m_upstream  ) ;
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloHypo2TrackAlg::~CaloHypo2TrackAlg() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloHypo2TrackAlg::initialize() 
{
  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
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
StatusCode CaloHypo2TrackAlg::execute() 
{
  // avoid long names 
  typedef const TrStoredTracks                              Tracks   ;
  typedef const TrStoredTrack                               Track    ; 
  typedef const CaloHypos                                   Hypos    ;
  typedef const CaloHypo                                    Hypo     ;
  typedef RelationWeighted2D<CaloHypo,TrStoredTrack,float>  Table    ;
  
  // get Hypos from Transient Store  
  Hypos*    hypos    = get<Hypos>    ( inputData() ) ;
  if( 0 ==  hypos              )     { return StatusCode::FAILURE ; }
  
  // get tracks   from Transient Store  
  Tracks*   tracks   = get<Tracks>   ( m_tracks    );
  if( 0 ==  tracks             )     { return StatusCode::FAILURE ; }
  
  // create relation table and register it in the store
  Table*    table = new Table();
  StatusCode sc = put( table , outputData() );
  if( sc.isFailure()           )     { return StatusCode::FAILURE ; }
  
  if( 0 == tracks   -> size () )
  { Warning("Empty container of tracks   '" + m_tracks    + "'"); }
  if( 0 == hypos    -> size () )
  { Warning("Empty container of hypos    '" + inputData() + "'"); }
  
  if( 0 == tracks   -> size () || 
      0 == hypos    -> size () )     { return StatusCode::SUCCESS ; }
  
  // loop over tracks  
  for( Tracks::const_iterator track = tracks->begin() ; 
       tracks->end() != track ; ++track )
  {  
    // skip NULLS 
    if( 0 == *track             ) { continue ; }             /// CONTINUE
    
    
    // use only unique  tracks ? 
    if(  m_unique   && 1 != (*track)->unique    () ) { continue ; }
    
    // use 'error'   tracks ?
    if( !m_error    && 0 != (*track)->errorFlag () ) { continue ; }
    
    // use 'forward'   tracks ?
    if( !m_forward  && 1 == (*track)->forward   () ) { continue ; }
    
    // use 'match'     tracks ?
    if( !m_matched  && 1 == (*track)->match     () ) { continue ; }
    
    // use 'seed'      tracks ?
    if( !m_seed     && 1 == (*track)->seed      () ) { continue ; }
    
    // use 'velo'      tracks ?
    if( !m_velo     && 1 == (*track)->velo      () ) { continue ; }      
    
    // use 'veloTT'    tracks ?
    if( !m_veloTT   && 1 == (*track)->veloTT    () ) { continue ; }      
    
    // use 'veloBack'    tracks ?
    if( !m_veloBack && 1 == (*track)->veloBack  () ) { continue ; }      
    
    // use 'upstream'  tracks ?
    if( !m_upstream && 1 == (*track)->upstream  () ) { continue ; }
    
    // loop over hypos 
    for( Hypos::const_iterator hypo = hypos->begin() ; 
         hypos->end() != hypo ; ++hypo )
    {
      // skip NUULs 
      if( 0 == *hypo          ) { continue ; }             /// CONTINUE 
      
      // valid hypo ? 
      if( 0 == (*hypo)->position() )
      {
        Warning("CaloPosition* points to NULL!");
        continue ;                                      /// CONTINUE 
      }
      
      // perform the matching
      double chi2 = 0 ;
      StatusCode sc = 
        m_match->match( (*hypo)->position() , *track , chi2 );
      
      if( sc.isFailure() )
      {
        Warning("Hypo/Track: matching failure, skip pair" );
        continue ;
      }
      else if ( 0 <= chi2 && chi2 <=  m_cut ) 
      { table->relate( *hypo , *track , chi2 ); }
      
    }; // loop over all hypos 
    
  }; // loop over all tracks 
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
