// $Id: CaloPIDsMonitor.cpp,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/17 12:00:58  ibelyaev
//  add new algorithm and update for new base
// 
// ============================================================================
// Include files
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"
// ============================================================================
// LHCbKErnel/Relations 
// ============================================================================
#include "Relations/IAssociator.h"
#include "Relations/IAssociatorWeighted.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================
// Local
// ============================================================================
#include "CaloPIDsMonitor.h"
// ============================================================================


// ============================================================================
/** @file 
 *  Implementation file for class : CaloPIDsMonitor
 * 
 *  @date 2004-02-15 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** @var s_Factory 
 *  Local static Algorithm Factory
 *  @see  AlgFactory
 *  @see IAlgFactory
 *  @see    IFactory
 */
// ============================================================================
static const  AlgFactory<CaloPIDsMonitor>         s_Factory ;
// ============================================================================
/** @var CaloPIDsMonitorFactory
 *  @see IAlgFactory
 *  @see    IFactory
 *  abstarct algorithm factory 
 */
// ============================================================================
const        IAlgFactory&CaloPIDsMonitorFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see     AlgFactory
 *  @see    IAlgFactory
 *  @param name name of the algorithm instance 
 *  @param isvc pointer to  Service Locator 
 */
// ============================================================================
CaloPIDsMonitor::CaloPIDsMonitor
( const std::string& name ,
  ISvcLocator*       isvc )
  : CaloHistoAlg ( name , isvc )
  //
  , m_tr2caloName () // no default value 
  , m_tr2calo     ( 0 )  
  //
  , m_tr2mcName
("AssociatorWeighted<TrStoredTrack,MCParticle,double>/TrackToMCP")
  , m_tr2mc       ( 0  )
  //
  , m_pid         ( 11       )
  , m_cut         ( 0        )
  , m_pNorm       ( 50 * GeV )
{
  //
  declareProperty ( "Tr2CaloPID"    , m_tr2caloName ) ;
  declareProperty ( "Tr2MCP"        , m_tr2mcName   ) ;
  //
  declareProperty ( "Particle"      , m_pid         ) ;
  declareProperty ( "Cut"           , m_cut         ) ;
  //
  declareProperty ( "Normalization" , m_pNorm       ) ;
  // 
  setProperty     ( "Input" , TrStoredTrackLocation::Default ) ;
};
// ============================================================================

// ============================================================================
/// virtual destructor
// ============================================================================
CaloPIDsMonitor::~CaloPIDsMonitor() {};

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorihtm 
 *  @see     Algorithm
 *  @see    IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPIDsMonitor::initialize () 
{ 
  StatusCode sc = CaloHistoAlg::initialize() ; 
  if( sc.isFailure() ) 
  { return Error( " Unable to initialize 'CaloHistoAlg' base class" , sc ) ; }
  
  m_tr2calo = tool<ITr2Calo> ( m_tr2caloName ) ;
  if( 0 == m_tr2calo ) { return StatusCode::FAILURE ; }
  
  m_tr2mc   = tool<ITr2MC>   ( m_tr2mcName   ) ;
  if( 0 == m_tr2mc   ) { return StatusCode::FAILURE ; }
  
  const std::string mom = " tanh( P/" + 
    boost::lexical_cast<std::string> ( m_pNorm / GeV ) + " GeV/c ) ";
  const std::string cut  = " with cut 'DLL>" + 
    boost::lexical_cast<std::string> ( m_cut ) + "' ";
  
  // pre-book few histograms
  book (  1 , " DLL for 'Signal'     ; DLL ; N " , -5 , 5 , 100 ) ;
  book (  2 , " DLL for 'Ghosts'     ; DLL ; N " , -5 , 5 , 100 ) ;
  book (  3 , " DLL for 'Background' ; DLL ; N " , -5 , 5 , 100 ) ;
  
  book ( 11 , mom       + " for 'Signal'     ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  book ( 12 , mom       + " for 'Ghosts'     ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  book ( 13 , mom       + " for 'Background' ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  
  book ( 21 , mom + cut + " for 'Signal'     ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  book ( 22 , mom + cut + " for 'Ghosts'     ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  book ( 23 , mom + cut + " for 'Background' ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
/** standard algorithm execution
 *  @see CaloAlgorihtm 
 *  @see     Algorithm
 *  @see    IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPIDsMonitor::execute    ()  
{  
  typedef TrStoredTrack   Track  ;
  typedef TrStoredTracks  Tracks ;
  
  // get the tracks 
  const Tracks* tracks = get<Tracks>( inputData() ) ;
  if ( 0 ==  tracks         ) { return StatusCode::FAILURE ; }
  if ( 0 ==  tracks->size() ) { return StatusCode::SUCCESS ; }
  
  // get Tr -> MC relation table 
  IMCTable*    mcTable   = m_tr2mc   -> direct() ;
  if ( 0 ==   mcTable ) { return StatusCode::FAILURE ; }
  
  // get Tr -> CaloPID relation table 
  ICaloTable * caloTable = m_tr2calo -> direct() ;
  if ( 0 == caloTable ) { return StatusCode::FAILURE ; }
  
  AIDA::IHistogram1D* h1   = histo (  1 ) ;
  AIDA::IHistogram1D* h2   = histo (  2 ) ;
  AIDA::IHistogram1D* h3   = histo (  3 ) ;

  AIDA::IHistogram1D* h11  = histo ( 11 ) ;
  AIDA::IHistogram1D* h12  = histo ( 12 ) ;
  AIDA::IHistogram1D* h13  = histo ( 13 ) ;
  
  AIDA::IHistogram1D* h21  = histo ( 21 ) ;
  AIDA::IHistogram1D* h22  = histo ( 22 ) ;
  AIDA::IHistogram1D* h23  = histo ( 23 ) ;
  
  if ( 0 == h1  || 0 == h2  || 0 == h3  ||
       0 == h11 || 0 == h12 || 0 == h13 ||
       0 == h21 || 0 == h22 || 0 == h23  ) 
  { return Error ( "Invalid pre-booked histogram!" ) ; }
  
  for ( Tracks::const_iterator itrack = 
          tracks->begin() ; tracks->end() != itrack ; ++itrack )
  {
    const Track* track = *itrack ;
    if ( 0 == track         ) { continue ; }                     // CONTINUE 
    
    if ( !track->unique  () ) { continue ; }                     // CONTINUE
    if ( !track->match   () &&
         !track->forward () ) { continue ; }                     // CONTINUE
    
    // get MC truth    
    IMCTable::Range mc  = mcTable -> relations( track ) ;
    
    const MCParticle* mcp = 0 ;
    if ( !mc.empty() ) { mcp = mc.back().to() ; } // Get THE LAST particle
    
    // retrieve calo pid information 
    ICaloTable::Range calo = caloTable -> relations( track ) ;
    // get THE LAST pid 
    const double DLL = calo.empty() ? -1000.0 : calo.back().to() ;
    
    if      ( 0 == mcp )                              // ghosts 
    { h2 -> fill( DLL ) ; }
    else if ( mcp->particleID().abspid() == m_pid )   // signal 
    { h1 -> fill( DLL ) ; }
    else                                              // background 
    { h3 -> fill( DLL ) ; }
    
    // evaluate the function of momentum 
    const double pMom = pFunc( momentum( track ) ) ;    
    
    if      ( 0 == mcp )                              // ghosts 
    { h12 -> fill ( pMom ) ; }
    else if ( mcp->particleID().abspid() == m_pid )   // signal 
    { h11 -> fill ( pMom ) ; }
    else                                              // background 
    { h13 -> fill ( pMom ) ; }
    
    // apply DLL cut 
    if ( DLL < m_cut ) { continue ; }                         // CONTINUE 
    
    if      ( 0 == mcp )                              // ghosts 
    { h22 -> fill ( pMom ) ; }
    else if ( mcp->particleID().abspid() == m_pid )   // signal 
    { h21 -> fill ( pMom ) ; }
    else                                              // background 
    { h23 -> fill ( pMom ) ; }
    
  };

  return StatusCode::SUCCESS ;
    
};
// ============================================================================


// ============================================================================
/** extract the momentum from TrStoredTrack
 *  @param  track pointer to the track
 *  @return the momentum of the track 
 */
// ============================================================================
double CaloPIDsMonitor::momentum( const TrStoredTrack* track ) const 
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
// The END 
// ============================================================================
