// $Id: CaloPIDsData.cpp,v 1.1 2004-03-17 16:11:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/TupleObj.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"
// ============================================================================
// LHCbKernel/Relations
// ============================================================================
#include "Relations/IAssociator.h"
#include "Relations/IAssociatorWeighted.h"
// ============================================================================
// local
// ============================================================================
#include "CaloPIDsData.h"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloPIDsData
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-03-17 
 */
// ============================================================================

// ============================================================================
/** @var s_Factory
 *  Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloPIDsData>         s_Factory ;
// ============================================================================
/** @var CaloPIDsDataFactory
 *  Abstract Algorithm Factory
 */
// ============================================================================
const        IAlgFactory&CaloPIDsDataFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see  CaloTupleAlg
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm 
 *  @see      Algorithm 
 *  @see      AlgFactory 
 *  @see     IAlgFactory
 *  @param name algorithm instance name 
 *  @param iscv pointer to Service Locator 
 */
// ============================================================================
CaloPIDsData::CaloPIDsData 
( const std::string& name  , 
  ISvcLocator*       isvc  ) 
  : CaloTupleAlg( name , isvc ) 
  //
  , m_name_Tr2MC 
("AssociatorWeighted<TrStoredTrack,MCParticle,double>/TrackToMCP")
  //
  , m_name_PrsE     ( "AssociatorWeighted<TrStoredTrack,float>/PrsE"     )
  , m_name_EcalChi2 ( "AssociatorWeighted<TrStoredTrack,float>/EcalChi2" )
  , m_name_BremChi2 ( "AssociatorWeighted<TrStoredTrack,float>/BremChi2" )
  , m_name_EcalE    ( "AssociatorWeighted<TrStoredTrack,float>/EcalE"    )
  , m_name_HcalE    ( "AssociatorWeighted<TrStoredTrack,float>/HcalE"    )
  //
  , m_Tr2MC     ( 0 )
  //
  , m_PrsE      ( 0 ) 
  , m_EcalChi2  ( 0 ) 
  , m_BremChi2  ( 0 ) 
  , m_EcalE     ( 0 ) 
  , m_HcalE     ( 0 ) 
{
  // MC for Tracks
  declareProperty ( "Tr2MCP"    , m_name_Tr2MC    ) ;
  // estimators 
  declareProperty ( "PrsE"      , m_name_PrsE     ) ;
  declareProperty ( "EcalChi2"  , m_name_EcalChi2 ) ;
  declareProperty ( "BremChi2"  , m_name_BremChi2 ) ;
  declareProperty ( "EcalE"     , m_name_EcalE    ) ;
  declareProperty ( "HcalE"     , m_name_HcalE    ) ;
  
  setProperty     ( "Input" , TrStoredTrackLocation::Default ) ;  
};
// ============================================================================

// ============================================================================
/// virtual destructor 
// ============================================================================
CaloPIDsData::~CaloPIDsData() {}; 
// ============================================================================

// ============================================================================
/** initialzation of the algorithm
 *  @see  CaloTupleAlg
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPIDsData::initialize() 
{
  // initialiae teh base class 
  StatusCode sc = CaloTupleAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  // locate all tools/associators
  
  m_Tr2MC    = tool<ITrAsct> ( m_name_Tr2MC    ) ;
  
  m_PrsE     = tool<IAsct>   ( m_name_PrsE     ) ;
  m_EcalChi2 = tool<IAsct>   ( m_name_EcalChi2 ) ;
  m_BremChi2 = tool<IAsct>   ( m_name_BremChi2 ) ;
  m_EcalE    = tool<IAsct>   ( m_name_EcalE    ) ;
  m_HcalE    = tool<IAsct>   ( m_name_HcalE    ) ;

  if ( 0 == m_Tr2MC     ) { return StatusCode::FAILURE ; }
  if ( 0 == m_PrsE      ) { return StatusCode::FAILURE ; }
  if ( 0 == m_EcalChi2  ) { return StatusCode::FAILURE ; }
  if ( 0 == m_BremChi2  ) { return StatusCode::FAILURE ; }
  if ( 0 == m_EcalE     ) { return StatusCode::FAILURE ; }
  if ( 0 == m_HcalE     ) { return StatusCode::FAILURE ; }
  
  return StatusCode::SUCCESS ;

};
// ============================================================================

// ============================================================================
/** extract the momentum from TrStoredTrack
 *  @param  track pointer to the track
 *  @return the momentum of the track 
 */
// ============================================================================
double CaloPIDsData::momentum( const TrStoredTrack* track ) const 
{
  if( 0 == track ) 
  { 
    Error ( "momentum(): TrStoredTrack* points to NULL, return -100*GeV" ) ;
    return -100 * GeV ;
  };
  
  // get the momentum from the state nearest to 0,0,0
  const TrState* state   = track->closestState( 0.0 ) ;
  if( 0 == state ) 
  { 
    Error ( "momentum(): Track has no state closest to z=0, return -100*GeV");  
    return -100 * GeV ;
  }
  
  const TrStateP* stateP = dynamic_cast<const TrStateP*>( state );
  if( 0 == stateP ) 
  { 
    Error ( "momentum(): TrState is not TrStateP! ,return -100*GeV");  
    return -100 * GeV ;
  }
  
  return stateP -> p  () ;
}
// ============================================================================


// ============================================================================
// ============================================================================
StatusCode CaloPIDsData::execute() 
{
  using namespace Tuples ;
  
  typedef TrStoredTrack  Track  ;
  typedef TrStoredTracks Tracks ;
  
  // locate input data 
  const Tracks* tracks = get<Tracks>( inputData() ) ;
  if ( 0 == tracks ) { return StatusCode::FAILURE ; }
  
  const TrTable* tr2MC    = m_Tr2MC    -> direct() ;
  
  if ( 0 == tr2MC     ) { return Error ( " Tr2MC    table points to NULL" ) ; }
  
  const Table*   prsE     = m_PrsE     -> direct() ;
  if ( 0 == prsE      ) { return Error ( " PrsE     table points to NULL" ) ; }
  
  const Table*   ecalChi2 = m_EcalChi2 -> direct() ;
  if ( 0 == ecalChi2  ) { return Error ( " EcalChi2 table points to NULL" ) ; }
  
  const Table*   bremChi2 = m_BremChi2 -> direct() ;
  if ( 0 == bremChi2  ) { return Error ( " BremChi2 table points to NULL" ) ; }
  
  const Table*   ecalE    = m_EcalE    -> direct() ;
  if ( 0 == ecalE     ) { return Error ( " EcalE    table points to NULL" ) ; }
  
  const Table*   hcalE    = m_HcalE    -> direct() ;
  if ( 0 == hcalE     ) { return Error ( " HcalE    table points to NULL" ) ; }
  
  
  Tuple tuple = nTuple( 415 , " CaloPIDs moni/test Alg=" + name() ) ;
  
  for ( Tracks::const_iterator itrack = tracks->begin() ; 
        tracks->end() != itrack  ; ++itrack ) 
  {
    const Track* track  = *itrack ;
    if ( 0 == track ) { continue ; }                                // CONTINUE
    
    const double mom = momentum ( track ) ;
    if ( mom  < 0 ) { Warning( "Negative |Momentum| is detected" ) ; }
    
    // get all MC relations from given track 
    TrRange mc = tr2MC -> relations ( track ) ;
    
    // get the relation with highest weight 
    const MCParticle* mcp = 0 ;
    if ( !mc.empty() ) { mcp = mc.back().to() ; }
    
    long pid = 0 ;
    if ( 0 != mcp ) { pid = mcp->particleID().pid() ; }
    
    double v1 = -1 * TeV ;
    Range r   = prsE->relations ( track ) ;
    if ( !r.empty() ) { v1 = r.front().to() ; }
    
    double v2 = -1 * TeV ;
    r         = ecalChi2 -> relations ( track ) ;
    if ( !r.empty() ) { v2 = r.front().to() ; }
    
    double v3 = -1 * TeV ;
    r         = bremChi2 -> relations ( track ) ;
    if ( !r.empty() ) { v3 = r.front().to() ; }
    
    double v4 = -1 * TeV ;
    r         = ecalE -> relations ( track ) ;
    if ( !r.empty() ) { v4 = r.front().to() ; }
    
    double v5 = -1 * TeV ;
    r         = hcalE -> relations ( track ) ;
    if ( !r.empty() ) { v5 = r.front().to() ; }
    
    tuple -> column ( "p"  , mom ) ;
    
    tuple -> column ( "PrsE"     , v1 ) ;
    tuple -> column ( "EcalChi2" , v2 ) ;
    tuple -> column ( "BremChi2" , v3 ) ;
    tuple -> column ( "EcalE"    , v4 ) ;
    tuple -> column ( "HcalE"    , v5 ) ;
    
    tuple -> column ( "id" , pid  , -25000 , 25000 ) ;
    
    tuple -> column ( "IsLong"   , track->isLong       () ) ;
    tuple -> column ( "IsUp"     , track->isUpstream   () ) ;
    tuple -> column ( "IsDown"   , track->isDownstream () ) ;
    tuple -> column ( "IsVelo"   , track->isVelotrack  () ) ;
    tuple -> column ( "IsBack"   , track->isBackward   () ) ;
    tuple -> column ( "IsT"      , track->isTtrack     () ) ;
    
    tuple -> column ( "unique"   , track -> unique     () ) ;
    tuple -> column ( "velo"     , track -> velo       () ) ;
    tuple -> column ( "seed"     , track -> seed       () ) ;
    tuple -> column ( "match"    , track -> match      () ) ;
    tuple -> column ( "forward"  , track -> forward    () ) ;
    tuple -> column ( "follow"   , track -> follow     () ) ;
    tuple -> column ( "veloTT"   , track -> veloTT     () ) ;
    tuple -> column ( "veloBack" , track -> veloBack   () ) ;
    tuple -> column ( "ksTrack"  , track -> ksTrack    () ) ;

    tuple -> write () ;
    
  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
