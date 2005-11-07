// $Id: CaloPIDsMonitor.cpp,v 1.3 2005-11-07 12:16:38 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/05/08 09:58:25  ibelyaev
//  remove associators, update options
//
// ============================================================================
// Include files
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IAxis.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/MCParticle.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================
// Local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloPIDsMonitor CaloPIDsMonitor.cpp
 *
 *  Class for monitoring the CaloPIDs quantities  
 *
 *  The major properties 
 *  
 *  - "Input" 
 *   The default value is                      ""
 *   The PID-quantity to be monitored, relation table of 
 *   type IRelation<Track,float>
 *   
 *  - "Inputs"
 *   The default value is :         TrackLocation::Default
 *   The list of Track containers to be monitored 
 *  
 *   - "Tr2MCP"
 *   The default value is :          "Rec/Relations/Tr2MCP" 
 *   The location in TES the relation table of type 
 *   IRelationWeighted<Track,MCParticle,double>
 *   
 *    - "Particle"
 *    The default value is :         11 
 *    Particle ID for "signal" hypothesis
 *   
 *    - "Cut" 
 *    The default value is :          0
 *    The cut on PID to be used for monitoring 
 *    
 *    - "Normialization" 
 *    The default value is :          50 GeV 
 *    The normalization factor used for histogramming 
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-15
 */
class CaloPIDsMonitor : public CaloMoniAlg
{
  /// friend factory for instantiation 
  friend class AlgFactory<CaloPIDsMonitor>;
protected:
  typedef const IRelation<Track,float>                      PIDTable ;
  typedef const IRelationWeighted<Track,MCParticle,double>  MCTable  ;
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize () ;
  /// standard algorithm execution
  virtual StatusCode execute    () ;
  /// standard algorithm finalization
  virtual StatusCode finalize   () ;
protected:
  /** Standard constructor
   *  @param name nam eof algorithm instance 
   *  @param isvc pointer to  Service Locator 
   */
  CaloPIDsMonitor 
  ( const std::string& name , 
    ISvcLocator*       isvc ) 
    : CaloMoniAlg ( name , isvc )
    //
    , m_tr2mc ("Rec/Relations/Track2MCP")
    //
    , m_pid         ( 11       )
    , m_cut         ( 0        )
    , m_pNorm       ( 50 * GeV )
  {
    //
    declareProperty ( "Tr2MCP"        , m_tr2mc       ) ;
    //
    declareProperty ( "Particle"      , m_pid         ) ;
    declareProperty ( "Cut"           , m_cut         ) ;
    //
    declareProperty ( "Normalization" , m_pNorm       ) ;
    // 
    addToInputs     ( TrackLocation::Default  ) ;
  };  
  /// virtual destructor 
  virtual ~CaloPIDsMonitor(){} ;
private:
  // default constructor is disabled  
  CaloPIDsMonitor() ; 
  // copy constructor is disabled 
  CaloPIDsMonitor           ( const CaloPIDsMonitor& ) ; 
  // assignement operator is disabled 
  CaloPIDsMonitor& operator=( const CaloPIDsMonitor& ) ;
protected:
  /// transformation function for momentum 
  inline double pFunc( const double value ) const 
  { return tanh( value / m_pNorm ); };
  /** extract the momentum from Track
   *  @param  track pointer to the track
   *  @return the momentum of the track 
   */
  double momentum  ( const Track* track  ) const ;
  /// print stat infomration about the histogram 
  StatusCode stats ( const AIDA::IHistogram1D* h ) const ;
  /// h3=h1/h2 
  StatusCode divide 
  ( AIDA::IHistogram1D* h1 , 
    AIDA::IHistogram1D* h2 , 
    AIDA::IHistogram1D* h3 ) const ;
    private:  
  // name for Tr -> MC relation table 
  std::string m_tr2mc ;
  // particle to be identified
  int         m_pid          ;
  // cut value 
  double      m_cut          ;
  // momentum normalization 
  double      m_pNorm        ;
};
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
/** standard algorithm initialization 
 *  @see CaloAlgorihtm 
 *  @see     Algorithm
 *  @see    IAlgorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPIDsMonitor::initialize () 
{ 
  StatusCode sc = CaloMoniAlg::initialize() ; 
  if( sc.isFailure() ) 
  { return Error( " Unable to initialize 'CaloMoniAlg' base class" , sc ) ; }
  
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
  
  book ( 31 , "Eff " + mom + cut + " for 'Signal'     ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  book ( 32 , "Eff " + mom + cut + " for 'Ghosts'     ;" + 
         mom + "; " , 0 , 1.02 , 51 ) ;
  book ( 33 , "Eff " + mom + cut + " for 'Background' ;" + 
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
  
  // get Tr -> MC relation table 
  const MCTable* mcTable   = get<MCTable> ( m_tr2mc ) ;
  if ( 0 ==      mcTable ) { return StatusCode::FAILURE ; }
  
  // get Tr -> CaloPID relation table 
  const PIDTable* pidTable = get<PIDTable>( inputData() ) ;
  if ( 0 == pidTable ) { return StatusCode::FAILURE ; }
  
  if ( inputs().empty() ) 
  { return Error("Input data are not specified") ; }
  
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {    
    // get the tracks 
    const Tracks* tracks = get<Tracks>( *input ) ;
    if ( 0 ==  tracks         ) { return StatusCode::FAILURE ; }
    
    for ( Tracks::const_iterator itrack = 
            tracks->begin() ; tracks->end() != itrack ; ++itrack )
    {
      const Track* track = *itrack ;
      if ( 0 == track         ) { continue ; }                     // CONTINUE 
      
      //OD (2005/11/01) criteria to be revised ...
      if ( !track->checkFlag(Track::Unique)) { continue ; }      // CONTINUE
      if ( !track->checkHistory(Track::PatForward) &&
           !track -> checkHistory(Track::TrackMatching)) { continue ; }     // CONTINUE
      
      // get MC truth    
      MCTable::Range mc  = mcTable -> relations( track ) ;
      
      const MCParticle* mcp = 0 ;
      if ( !mc.empty() ) { mcp = mc.back().to() ; } // Get THE LAST particle
      
      // retrieve calo pid information 
      PIDTable::Range calo = pidTable -> relations( track ) ;
      // get THE LAST pid 
      const double DLL = calo.empty() ? -1000.0 : calo.back().to() ;
      
      if      ( 0 == mcp )                              // ghosts 
      { hFill ( histo(2) , DLL ) ; }
      else if ( mcp->particleID().abspid() == m_pid )   // signal 
      { hFill ( histo(1) , DLL ) ; }
      else                                              // background 
      { hFill ( histo(3) , DLL ) ; }
      
      // evaluate the function of momentum 
      const double pMom = pFunc( momentum( track ) ) ;
      
      if      ( 0 == mcp )                              // ghosts 
      { hFill ( histo(12) , pMom ) ; }
      else if ( mcp->particleID().abspid() == m_pid )   // signal 
      { hFill ( histo(11) , pMom ) ; }
      else                                              // background 
      { hFill ( histo(13) , pMom ) ; }
      
      // apply DLL cut 
      if ( DLL < m_cut ) { continue ; }                 // CONTINUE 
      
      if      ( 0 == mcp )                              // ghosts 
      { hFill ( histo(22) , pMom ) ; }
      else if ( mcp->particleID().abspid() == m_pid )   // signal 
      { hFill ( histo(21) , pMom ) ; }
      else                                              // background 
      { hFill ( histo(23) , pMom ) ; }
      
    };
    
  };

  return StatusCode::SUCCESS ;
    
};
// ============================================================================

// ============================================================================
/** extract the momentum from Track
 *  @param  track pointer to the track
 *  @return the momentum of the track 
 */
// ============================================================================
double CaloPIDsMonitor::momentum 
( const Track* track ) const 
{
  if( 0 == track ) 
  { 
    Error ( "momentum(): Track* points to NULL, return -100*GeV" ) ;
    return -100 * GeV ;
  };
  
  // get the momentum from the state nearest to 0,0,0
  const State state   = track->closestState( 0.0 ) ;
  //OD if( 0 == state ) 
  //  { 
  //  Error ( "momentum(): Track has no state closest to z=0, return -100*GeV");  
  //  return -100 * GeV ;
  //}
  //const TrStateP* stateP = dynamic_cast<const TrStateP*>( state );
  //if( 0 == stateP ) 
  //{ 
  //  Error ( "momentum(): TrState is not TrStateP! ,return -100*GeV");  
  //  return -100 * GeV ;
  // }
  
  return state.p  () ;
}
// ============================================================================

// ============================================================================
// Divide the histograms 
// ============================================================================
StatusCode CaloPIDsMonitor::divide 
( AIDA::IHistogram1D* h1 , 
  AIDA::IHistogram1D* h2 , 
  AIDA::IHistogram1D* h3 ) const 
{
  if ( 0 == h1 || 0 == h2 || 0 == h3 ) 
  { return Error ( "AIDA::IHistogram1D* points to NULL" ) ; } 
  const IAxis& axis1 = h1 -> axis() ;
  const IAxis& axis2 = h2 -> axis() ;
  const IAxis& axis3 = h3 -> axis() ;
  const int nBins = axis3.bins() ;
  if ( axis1.bins() != nBins || axis2.bins() != nBins ) 
  { return Error ( "Different histogram specifications 1 " ) ; }
  for ( int nBin = 0 ; nBin < nBins ; ++nBin ) 
  {
    const double v1 = h1 -> binHeight ( nBin ) ;
    const double v2 = h2 -> binHeight ( nBin ) ;
    const double x  = 0.5 *  ( axis3.binLowerEdge ( nBin ) + 
                               axis3.binUpperEdge ( nBin ) ) ;
    /// fill the histogram 
    if ( 0 != v2 ) { h3 -> fill ( x , v1 / v2 ) ; }
  }
  return StatusCode::SUCCESS ;
};
// ============================================================================

StatusCode CaloPIDsMonitor::stats 
( const AIDA::IHistogram1D* h ) const
{
  if ( 0 == h ) { return StatusCode::FAILURE ; }
  const double content = h->sumBinHeights() ;
  const double nbins   = h->axis().bins  () ;
  info() << "'" << h->title()   << "' = "
         << (content/nbins)*100 << "[%]" << endreq ;
  return StatusCode::SUCCESS ;
};


// ============================================================================
/// standard algorithm finalization
// ============================================================================
StatusCode CaloPIDsMonitor::finalize () 
{
  
  AIDA::IHistogram1D* h11 = histo ( 11 ) ;
  AIDA::IHistogram1D* h12 = histo ( 12 ) ;
  AIDA::IHistogram1D* h13 = histo ( 13 ) ;
  AIDA::IHistogram1D* h21 = histo ( 21 ) ;
  AIDA::IHistogram1D* h22 = histo ( 22 ) ;
  AIDA::IHistogram1D* h23 = histo ( 23 ) ;
  AIDA::IHistogram1D* h31 = histo ( 31 ) ;
  AIDA::IHistogram1D* h32 = histo ( 32 ) ;
  AIDA::IHistogram1D* h33 = histo ( 33 ) ;
  
  divide ( h21 , h11 , h31 ) ; 
  divide ( h22 , h12 , h32 ) ;
  divide ( h23 , h13 , h33 ) ;
  
  stats  ( h31 ) ;
  stats  ( h32 ) ;
  stats  ( h33 ) ;
  
  // finalize the chain of base classes 
  return CaloMoniAlg::finalize () ;
};

// ============================================================================
// The END 
// ============================================================================
