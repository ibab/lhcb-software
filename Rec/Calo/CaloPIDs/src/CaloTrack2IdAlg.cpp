// $Id: CaloTrack2IdAlg.cpp,v 1.2 2003-07-17 12:45:16 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2003/03/13 18:52:02  ibelyaev
// The first import of new package 
//
// Revision 1.1  2002/11/17 17:09:26  ibelyaev
//  new set of options and tools
// 
// ============================================================================
// Include files
// from LHcbKernel
#include "Relations/Relation1D.h"
#include "Relations/IAssociator.h"
// AIDA
#include "AIDA/IHistogram2D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
// CaloUtils 
#include  "CaloUtils/dirHbookName.h"
/// Event 
#include "Event/TrStoredTrack.h"
// local
#include "CaloTrack2IdAlg.h"

// ============================================================================
/** @file 
 *  
 *  Implementation file for class CaloTrack2IdAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-14
 */
// ============================================================================

// ============================================================================
/** @var CaloTrack2IdAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloTrack2IdAlg>         s_factory ;
const        IAlgFactory&CaloTrack2IdAlgFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloTrack2IdAlg::CaloTrack2IdAlg
( const std::string& name ,
  ISvcLocator*       pSvc )
  : CaloAlgorithm ( name , pSvc ) 
  , m_associatorType ("Associator<TrStoredTrack,float>" )
  , m_associatorName ()
  // 
  , m_associator  (  0    )
  ///
  , m_unique      ( false ) // Use ALL tracks
  ///
  , m_error       ( false ) // Do not use Error tracks
  ///
  , m_forward     ( true   ) // Use forward tracks 
  , m_matched     ( true   ) // Use matched tracks 
  , m_seed        ( false  ) // DO NOT use seed  tracks 
  , m_velo        ( false  ) // DO NOT use forward tracks 
  , m_veloTT      ( false  ) // DO NOT use forward tracks 
  , m_veloBack    ( false  ) // DO NOT use veloback tracks 
  , m_upstream    ( true   ) // Use upstream tracks 
  /// 
  , m_vBins       ( 0 )
  , m_pBins       ( 0 )
  , m_vNorm       ( 0 ) 
  , m_pNorm       ( 0 )
  , m_signalData  (   )
  , m_backgrData  (   )
  , m_signal      ( 0 ) 
  , m_backgr      ( 0 ) 
  , m_signalN     ( 0 ) 
  , m_backgrN     ( 0 ) 
  , m_deltaLL     ( 0 ) 
{
  declareProperty ( "AssociatorType"  , m_associatorType ) ;
  declareProperty ( "AssociatorName"  , m_associatorName ) ;
  // track flags 
  declareProperty ( "UseUnique"       , m_unique         ) ;
  declareProperty ( "UseError"        , m_error          ) ;
  declareProperty ( "UseForward"      , m_forward        ) ;
  declareProperty ( "UseMatched"      , m_matched        ) ;
  declareProperty ( "UseVelo"         , m_velo           ) ;
  declareProperty ( "UseVeloTT"       , m_veloTT         ) ;
  declareProperty ( "UseVeloBack"     , m_veloBack       ) ;
  declareProperty ( "UseSeed"         , m_seed           ) ;
  declareProperty ( "UseUpstream"     , m_upstream       ) ;
  //
  declareProperty ( "ValueBins"       , m_vBins          ) ;
  declareProperty ( "MomentumBins"    , m_pBins          ) ;
  declareProperty ( "ValueNorm"       , m_vNorm          ) ;
  declareProperty ( "MomentumNorm"    , m_pNorm          ) ;
  declareProperty ( "SignalData"      , m_signalData     ) ;
  declareProperty ( "BackgroundData"  , m_backgrData     ) ;  
};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloTrack2IdAlg::~CaloTrack2IdAlg() {}; 
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrack2IdAlg::initialize() 
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Base class could not be initialized",sc);}
  
  m_associator = tool( m_associatorType , m_associatorName , m_associator );
  if( 0 == m_associator ) { return Error("Associator is not located");}
  
  
  if( 0 == m_vBins ) { return Error("Invalid 'ValueBins'    property" ) ; }
  if( 0 == m_pBins ) { return Error("Invalid 'MomentumBins' property" ) ; }
  if( 0 >= m_vNorm ) { return Error("Invalid 'ValueNorm'    property" ) ; }
  if( 0 >= m_pNorm ) { return Error("Invalid 'MomentumNorm' property" ) ; }
  
  if( m_vBins * m_pBins * 3 + 1 != m_signalData.size()     )
    { return Error(" Wrong amount of 'SignalData'     " ); }
  if( m_vBins * m_pBins * 3 + 1 != m_backgrData.size()     )
    { return Error(" Wrong amount of 'BackgroundData' " ); }
  
  std::string dir( dirHbookName( "Calo/" + name() ) );
  
  m_signal  = histoSvc()->book( dir                       , 
                                1                         , 
                                "Signal"                  , 
                                m_pBins      , 0.0 , 1.02 , 
                                m_vBins      , 0.0 , 1.02 ) ;
  if( 0 == m_signal  ) 
    { return Error("'Signal' histogram is not booked!"); }
  
  m_backgr  = histoSvc()->book( dir                       , 
                                2                         , 
                                "Background"              , 
                                m_pBins      , 0.0 , 1.02 , 
                                m_vBins      , 0.0 , 1.02 ) ;
  if( 0 == m_backgr  ) 
    { return Error("'Background' histogram is not booked!"); }
  
  m_signalN = histoSvc()->book( dir                       , 
                                3                         , 
                                "Signal(Normalized)"      , 
                                m_pBins      , 0.0 , 1.02 , 
                                m_vBins      , 0.0 , 1.02 ) ;
  if( 0 == m_signalN  ) 
    { return Error("'Signal(Normalized)' histogram is not booked!"); }
  
  m_backgrN = histoSvc()->book( dir                       , 
                                4                         , 
                                "Background(Normalized)"  , 
                                m_pBins      , 0.0 , 1.02 , 
                                m_vBins      , 0.0 , 1.02 ) ;
  if( 0 == m_backgrN ) 
    { return Error("'Background(Normalized)' histogram is not booked!"); }
  
  m_deltaLL = histoSvc()->book( dir                       , 
                                5                         , 
                                "Delta Log Likelihood"    , 
                                m_pBins      , 0.0 , 1.02 , 
                                m_vBins      , 0.0 , 1.02 ) ;
  if( 0 == m_deltaLL )
    { return Error("'Delta Log Likelihood' histogram is not booked!"); }
  
  const unsigned long size = m_vBins * m_pBins * 3 ;
  
  unsigned int idata = 0 ;
  while( idata < size ) 
    {
      const double xS = m_signalData[ idata + 0 ] ;
      const double yS = m_signalData[ idata + 1 ] ;
      double       wS = m_signalData[ idata + 2 ] ;
      
      if( 0 >= wS) { Warning("Non-positive S-weight replaced by 1"); wS = 1 ; }
      m_signal -> fill( xS , yS , wS );
      
      const double xB = m_backgrData[ idata + 0 ] ;
      const double yB = m_backgrData[ idata + 1 ] ;
      double       wB = m_backgrData[ idata + 2 ] ;      
      if( 0 >= wB) { Warning("Non-positive B-weight replaced by 1"); wB = 1 ; }
      m_backgr -> fill( xB , yB , wB );
      
      idata += 3 ;   // go to next entry
    }
  
  { // create the normalized histograms and Delta Log Likelihood histogram 
    for( size_t ip = 0 ; ip < m_pBins ; ++ip ) 
      {
        const  double  x =  m_signal -> binCentreX ( ip ) ;
        const  double aS =  m_signal -> binHeightX ( ip ) ;
        const  double aB =  m_backgr -> binHeightX ( ip ) ;
        
        if( 0 >= aS || 0 >= aB ) 
          { return Error("Illegal histogram column content!") ; }
        
        for( size_t iv = 0 ; iv < m_vBins ; ++iv ) 
          {
            const double y = m_signal -> binCentreY (    iv   )      ;
            const double S = m_signal -> binHeight  ( ip , iv ) / aS ;
            const double B = m_backgr -> binHeight  ( ip , iv ) / aB ;
            
            if( 0 >= S || 0 >= B ) 
              { return Error("Illegal histogram bin content!"); }
            
            m_signalN -> fill ( x , y ,      S       ) ;
            m_backgrN -> fill ( x , y ,          B   ) ;
            m_deltaLL -> fill ( x , y , log( S / B ) ) ;
          }
      }
  }
  
  // clear the intermediate data storages 
  m_signalData.clear() ;
  m_backgrData.clear() ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm finalization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloTrack2IdAlg::finalize() 
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG  << "==> Finalize" << endreq;
  
  // release evaluator
  if( 0 != m_associator ) { m_associator->release()  ; m_associator = 0 ; }
  
  return CaloAlgorithm::finalize() ;
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
StatusCode CaloTrack2IdAlg::execute() 
{
  
  /// avoid long names 
  typedef const TrStoredTrack             Track ;
  typedef IAsct::DirectType               Table ;
  typedef Table::Range                    Range ;
  typedef Relation1D<TrStoredTrack,float> Output ;
  
  // get the table from Associator 
  const Table* input = m_associator->direct() ;
  if( 0 == input     )             { return StatusCode::FAILURE ; } // RETURN 
  
  // "convert" "input" to "output"
  Range range = input->relations();
  
  /// create and register in the store the output tables 
  Output* output     = new Output ( range.size() ) ;
  StatusCode sc      = put( output , outputData() ) ;
  if( sc.isFailure() )             { return StatusCode::FAILURE ; } // RETURN
  
  /// convert the old table into the new table 
  for( Range::const_iterator rel = range.begin() ; 
       range.end() != rel ; ++rel ) 
    {
      const Track* track = rel->from() ; 
      // skip the invalid tracks 
      if( 0 == track ) { continue ; }
      //
       // use only unique  tracks ? 
      if(  m_unique   && 1 != track->unique    () ) { continue ; }
      
      // use 'error'   tracks ?
      if( !m_error    && 0 != track->errorFlag () ) { continue ; }
      
      // use 'forward'   tracks ?
      if( !m_forward  && 1 == track->forward   () ) { continue ; }
      
      // use 'match'     tracks ?
      if( !m_matched  && 1 == track->match     () ) { continue ; }
      
      // use 'seed'      tracks ?
      if( !m_seed     && 1 == track->seed      () ) { continue ; }
      
      // use 'velo'      tracks ?
      if( !m_velo     && 1 == track->velo      () ) { continue ; }      
      
      // use 'veloTT'    tracks ?
      if( !m_veloTT   && 1 == track->veloTT    () ) { continue ; }      
      
      // use 'veloBack'    tracks ?
      if( !m_veloBack && 1 == track->veloBack  () ) { continue ; }      
      
      // use 'upstream'  tracks ?
      if( !m_upstream && 1 == track->upstream  () ) { continue ; }
      
      // get the momentum from the state nearest to 0,0,0
      const TrState* state   = track->closestState( 0.0 ) ;
      if( 0 == state ) 
        {
          Warning("Track has no state closest to z=0");
          continue ;
        }
      const TrStateP* stateP = dynamic_cast<const TrStateP*>( state );
      if( 0 == stateP ) 
        {
          Warning("Momentum information is not available");
          continue ;
        }
      
      // get the momentum and transform it 
      const double p       = pFunc ( stateP -> p  () ) ;
      // get the estimator and transform it 
      const double v       = vFunc ( rel    -> to () ) ;
      
      const double deltaLL = 
        m_deltaLL -> binHeight( m_deltaLL -> coordToIndexX ( p ) ,
                                m_deltaLL -> coordToIndexY ( v ) );
      output      -> relate( track , deltaLL ) ;
      
    }
  
  
  MsgStream log( msgSvc() , name() );
  log << MSG::DEBUG 
      << " The total number of booked relations " 
      << output     -> relations().size()  << endreq ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


