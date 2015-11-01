// $Id:$
// ============================================================================
// Include files
// ============================================================================
// STD&STL 
// ============================================================================
#include <string>
#include <vector>
#include <memory>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/RndmGenerators.h"
// ============================================================================
// GaudiAlgs 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// GaudiUtils 
// ============================================================================
#include "GaudiUtils/Histo2String.h"
#include "GaudiUtils/HistoParsers.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// TrackEvent 
// ============================================================================
#include "Event/Track.h"
#include "Event/State.h"
// ============================================================================
// DAQEvent
// ============================================================================
#include "Event/ODIN.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/RunChangeIncident.h"
#include "DetDesc/Condition.h"
#include "DetDesc/HistoParam.h"
#include "DetDesc/ParamAsHisto.h"
// ============================================================================
// ROOT 
// ============================================================================
#include "TH1D.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
// ============================================================================
/** @class TrackSmearState TrackSmearState.h
 *
 *  Smear the track momenta
 *
 *  @author M.Needham
 *  @date   30/05/2006
 *  
 *  Use on MC-(x)DST to smear the track momenta *before* your 
 *  user algorithms sequence:
 *
 *  @code
 *
 *  from Configurables import TrackSmearState as SMEAR
 *
 *  smear = SMEAR( 'Smear' )  ## default configuration is perfectly fine 
 *
 *  daVinci = DaVinci ( ... ) 
 *  daVinci.UserAlgortithms  = [ smear ]
 *  
 *  seq = ... ## analysis sequence  
 *  daVinci.UserAlgortithms += [ seq    ]
 *  
 *  @endcode
 *
 *  Important properties:
 *  - "Input"       : TES location of input tracks (no need to redefine)
 *  - "Fudge"       : fudge factor for momenutm smearing 
 * 
 *  Property that allows to read data from ROOT-file 
 *  - "RootFile"    : the name of exisiting root file with TGraph "res;1"
 * 
 *  Property that allows to spedcifi histogram as options
 *  - "SmearHisto"    : \f$\delta p/p\f$-histogram for smearing 
 *
 *  Property that allows to modify the parameters from CONDDB 
 *  - "CONDDBpath"  : the path in CONDB
 *
 *  RootFile has a precedence 
 *
 *  OPTION is activated in case                                       
 *  - no RootFile is specified 
 *  - valid input histogram is specified - it overrides CONDB access 
 *
 *  CondDB is activated in case : 
 *  - NO INPUT ROOT file is specified 
 *  - NO VALID OPTIONS for histograms are specified 
 *  - valid CONDD path is specified 
 *
 * How to specify the histograms as options? 
 *  
 *  @code 
 * 
 *  import  ROOT 
 *  import AnalysisPython.PyRoUts ## well, a bit lighter version also exists..
 *  
 *  root_file = ROOT.TFile( ... , 'READ')
 *
 *  ## get Matt's graph 
 *  graph    = root_file.Get('res' ) 
 *
 *  # convert it to the histogram
 *  histo  = graph.GetHistogram ()
 *  # unfortunately here 'histo' could be of type 
 *  # TH1F histiogram, need to convert it to TH1D histgram:
 *  # in case histo is already of type TH1D, next two lines are not needed at all.  
 *  from AnalysisPython.PyRoUts import h1_axis
 *  histo  = h1_axis ( histo.GetXaxis() , 'Momentum smear' , double = True ) 
 *  
 *  histo += graph                  ## NB!!!
 *
 *  from Configurables import TrackSmearState as SMEAR
 *
 *  smear = SMEAR() 
 *  smear.SmearHisto    = histo.toString() 
 * 
 *  ## as alternatively one can use XML:
 *  #smear.SmearHisto    = histo.toXml () 
 *
 *  @endcode 
 *
 *  How to specify input RootFile with graph object? 
 *  
 *  @code 
 * 
 *  from Configurables import TrackSmearState as SMEAR
 *
 *  smear = SMEAR () 
 *  smear.RootFile = "smear12.root"
 *
 *  @endcode 
 *  
 */
class TrackSmearState : public extends1<GaudiAlgorithm,IIncidentListener>
{
  // ==========================================================================
  // friend factory for instantiation 
  friend class AlgFactory<TrackSmearState> ;
  // ==========================================================================
public:  // Algorithm 
  // ==========================================================================
  /// specific algorithm initialization
  virtual StatusCode initialize () ; // specific algorithm initialization
  /// specific algorithm execution 
  virtual StatusCode execute    () ; // specific algorithm execution 
  /// specific algorithm finalization
  virtual StatusCode finalize   () ; // specific algorithm finalization
  // ==========================================================================
public: // Incident Listener 
  // ==========================================================================
  /** handle run change incident 
   *  @see IIncidentListener 
   */
  virtual void handle ( const Incident& ) ; // handle incident 
  // ==========================================================================
protected: // constructors & destructors 
  // ==========================================================================
  /// Standard constructor
  TrackSmearState ( const std::string& name        ,
                    ISvcLocator*       pSvcLocator ) ;
  /// virtual and protected destructor 
  virtual ~TrackSmearState (); // virtual an dprotected destructor 
  // ==========================================================================
private:  // disable everything that is not needed 
  // ==========================================================================
  /// default constructor is disabled 
  TrackSmearState () ; // default constructor is disabled 
  /// copy  constructor is disabled 
  TrackSmearState           ( const TrackSmearState& ) ; // copy  constructor
  /// assignement operator is disabled 
  TrackSmearState& operator=( const TrackSmearState& ) ; // no assignement
  // ==========================================================================
private: // 
  // ==========================================================================
  /// update the data from CONDDB 
  StatusCode i_updateDATA () ;                   // update the data from CONDDB 
  /** the  action for new run:
   *  - store the magnet polarity      (from magnet field service )
   *  - store the run-dependent offset (taking run number from ODIN)
   *  @param run new run number 
   */
  void       new_run      ( const unsigned int run ) ; 
  // ==========================================================================
private: // properties 
  // ==========================================================================
  /// the condition path in CONDDB       (property) 
  std::string m_cond_path   ;  //   the condition path in CONDDB     (property) 
  /// the histogram for smearer          (property) 
  std::string m_h_str       ;  // the histogram                      (property)
  /// fudge                              (property)  
  double      m_fudge       ;  // fudge                              (property) 
  /// input location for tracks          (property) 
  std::string m_input       ;  // input location for tracks          (property) 
  // ==========================================================================
  // the file name to read the graph
  std::string m_filename    ;
  // ==========================================================================
private: // data 
  // ==========================================================================
  /// the histogram for scaler     (property) 
  TH1D                  m_h           ;      // the histogram      
  /// the graph iself (for interpolation)
  std::auto_ptr<TGraph> m_graph       ;      // the graph iself (for interpolation)
  /// the file (if needed)
  std::auto_ptr<TFile>  m_rootfile    ;      // the file (if needed) 
  /// the condition itself 
  Condition*            m_condition   ;      // the condition itself 
  /// random numbers supplier 
  mutable Rndm::Numbers m_gauss       ;      // random numbers supplier 
  // ==========================================================================
};
// ============================================================================
// SOME MACROS
// ============================================================================
// #define ON_DEBUG   if (UNLIKELY(outputLevel() <= MSG::DEBUG))
// #define ON_VERBOSE if (UNLIKELY(outputLevel() <= MSG::VERBOSE))
// ============================================================================
// Standard constructor
// ============================================================================
TrackSmearState::TrackSmearState 
( const std::string& name ,
  ISvcLocator*       pSvc )
  : base_class ( name , pSvc )
//
  , m_cond_path     ( "/dd/Conditions/Calibration/LHCb/MomentumSmear" ) 
  , m_h_str         (    ) // the histogram for state smear (as text)
  , m_fudge         ( 1  ) // the fudge factor 
  , m_input         ( LHCb::TrackLocation::Default ) // input tracks 
  , m_filename      ( "" )
//
  , m_h             () 
  , m_graph         () 
  , m_rootfile      () 
//
  , m_condition     ( 0 )  // condiiton itself 
  , m_gauss         (   )  // random numbers provider (initially invalid) 
//
{
  //
  declareProperty 
    ( "CONDDBpath" , 
      m_cond_path  ,
      "The path in CONDDB with calibration data" ) ;
  //
  declareProperty 
    ( "RootFile"  , 
      m_filename  ,
      "The name of Root-file where Graph 'res' is placed" ) ;
  //
  declareProperty 
    ( "SmearHisto" , 
      m_h_str      ,
      "The 1D-historam for momentum smear" ) ;
  //
  declareProperty 
    ( "Fudge"      , 
      m_fudge      ,
      "The fudge-factor for momentum smearing" ) ;
  //
  declareProperty 
    ( "Input"      , 
      m_input      ,
      "Input location for Tracks" ) ;
  //
}
// ============================================================================
// destructor 
TrackSmearState::~TrackSmearState() {}                            // destructor 
// ============================================================================
// specific algorithm initialization
// ============================================================================
StatusCode TrackSmearState::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) { return sc ; }
  //
  // if all histograms from the options are fine, just use them  
  //
  if ( !m_filename.empty() ) 
  {
    m_rootfile.reset ( TFile::Open ( m_filename.c_str() , "READ" ) ) ;
    if ( 0 == m_rootfile.get() ) 
    { return Error( "Unable to open RootFile '" + m_filename + "'"  ) ; }   // RETURN 
    TObject* o = m_rootfile->Get("res;1") ;
    if ( 0 == o ) { return Error( "Unable to access 'res;1'-object" ) ; }   // RETURN
    TGraph* g = dynamic_cast<TGraph*>( o ) ;
    if ( 0 == g ) { return Error( "'res;1'-object if not TGraph"    ) ; }   // RETURN
    m_graph.reset ( g  ) ;
    info() << "Use the momentum smearing from root-file '"
           << m_filename << "'"
           << endmsg ;
  }
  else if ( Gaudi::Parsers::parse ( m_h , m_h_str ).isSuccess () ) 
  {
    //
    info() << "Use the momentum smearing from options" << endmsg ;
  }
  else if ( !m_cond_path.empty() && existDet<DataObject>( detSvc() , m_cond_path ) ) 
  {
    //
    try 
    {  
      registerCondition ( m_cond_path , m_condition , &TrackSmearState::i_updateDATA );
    }
    catch ( GaudiException &e ) 
    {
      error () << e << endmsg;
      return Error ( "Unable register condition " + m_cond_path , e.code() ) ;  
    }
    //
    sc = runUpdate () ;
    if ( sc.isFailure() ) { return Error ( "Unable update Run" , sc ) ; }
    //
    info() << "Use the momentum smearing from CONDDB"  << endmsg ;
  }
  else 
  {
    return Error ( "Invalid setting of histogam source" ) ;         // RETURN 
  }
  //
  sc = m_gauss.initialize ( randSvc() , Rndm::Gauss ( 0 , 1 ) ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to initialize random numbers provider", sc ) ; }
  if ( !m_gauss ) 
  { return Error ( "Invalid state of random numbers provider"         ) ; }
  //
  // subscribe to run-change incident 
  IIncidentSvc* incsvc = svc<IIncidentSvc>("IncidentSvc") ;
  incsvc -> addListener ( this , IncidentType::RunChange ) ;
  incsvc -> addListener ( this , IncidentType::BeginRun  ) ;
  //
  if ( LHCb::Math::equal_to_double ( m_fudge       , 0.0 ) ) 
  {
    Warning ( "Momentum smearing is explicitly disabled, fudge = 0 " ) ;
    m_fudge = 0 ;
  }
  else if ( LHCb::Math::equal_to_double ( m_h.Integral() , 0.0 ) ) 
  {
    Warning ( "Momentum smearing is implicitly disabled, set e = 0 " ) ;
    m_fudge = 0 ;    
  }
  //
  return sc ;
}
// ============================================================================
// specific algorithm finalization 
// ============================================================================ 
StatusCode TrackSmearState::finalize () 
{
  //
  if ( 0 != m_rootfile.get() ) { m_graph.release() ; }
  //
  m_gauss.finalize () ;
  //
  // unsubscribe to run-change incident 
  IIncidentSvc* incsvc = svc<IIncidentSvc>("IncidentSvc") ;
  incsvc->removeListener ( this , IncidentType::RunChange ) ;
  incsvc->removeListener ( this , IncidentType::BeginRun  ) ;
  //
  // finalize the base class 
  return GaudiAlgorithm::finalize () ;
}
// ============================================================================
/*  handle run change incident 
 *  @see IIncidentListener 
 */
// ============================================================================
void TrackSmearState::handle ( const Incident& /* inc */ ){} // handle incident
// ============================================================================
// update histos from CONDDB 
// ============================================================================
StatusCode TrackSmearState::i_updateDATA ()
{
  //
  if ( !m_condition ) { return StatusCode::FAILURE ; }  //               RETURN
  //
  const TH1D* h1 = 0 ;
  //
  try 
  {
    //
    h1 = DetDesc::Params::paramAsHisto1D ( m_condition , "Smear"   ) ;
    if ( 0 == h1 ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'Smear' from CONDDB") ; 
    }
    //
  }
  catch ( GaudiException& e ) 
  {
    ++counter("#CONDB problem") ;
    return Error ("Unable to get data from CONDDB" , e.code() ) ; // RETURN
  }
  //
  info () << " Condition: " << m_condition -> name()    << "   "
          << ( m_condition ->isValid() ? "  Valid;  " : "Invalid; " ) 
          << " Validity: "  << m_condition -> validSince ().format ( true ) 
          << " -> "         << m_condition -> validTill  ().format ( true )  << endmsg ;
  //
  h1 -> Copy ( m_h       ) ;
  // reset the graph 
  m_graph.reset ( new TGraph( &m_h ) ) ;
  //
  ++counter("#CONDB update") ;
  //
  return StatusCode::SUCCESS;
}
// ============================================================================
// specific algorithm execution 
// ============================================================================
StatusCode TrackSmearState::execute ()
{
  /// no scaling is needes 
  if ( 0 == m_fudge ) { return SUCCESS ;  }
  
  /// get the tracks to scale
  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks> ( m_input );
  /// check 
  if     ( LIKELY ( 0 != tracks ) ) {   /* ok */ }
  else 
  { return Warning ( "No tracks at location '" + m_input + "'" , StatusCode::SUCCESS ) ; }
  //
  /// perform the action for new run (trigger update if needed)
  get<LHCb::ODIN> ( evtSvc() , LHCb::ODINLocation::Default) ;
  //
  StatEntity& scale = counter("SMEAR[%]") ;
  //
  // loop and do the scaling
  for ( LHCb::Tracks::iterator it = tracks->begin() ;  it != tracks->end(); ++it )
  {
    LHCb::Track* track = *it ;
    if ( 0 == track ) { continue ; }
    //
    // smear only LONG tracks 
    if ( LHCb::Track::Long != track->type () ) { continue ; } // smear only Long tracks 
    //
    typedef std::vector<LHCb::State*> STATES ;
    const STATES& states = track->states();
    //
    const int    q     = track   -> charge () ;
    const double shoot = m_gauss () ;
    //
    // loop over the states for the given track 
    for ( STATES::const_iterator ist = states.begin() ;  ist != states.end(); ++ist ) 
    {
      //
      LHCb::State* state = *ist ;
      if ( 0 == state ) { continue ; }
      //
      const double p       = state   -> p () ;
      const double factor  = m_graph -> Eval( p ) * m_fudge ;
      //
      // keep the track for debugging purposes 
      scale += 100 * factor  ;    // keep the track for debugging purposes 
      //
      const double smear   = factor * p     ; // NB: graph is dp/p
      //
      const double delta_p = smear  * shoot ;
      //
      // change momentum! 
      state ->setQOverP( q / ( p + delta_p ) );  // smear momentum! 
      //
    } //                                           end of loop over the states   
  } //                                             end loop over the tracks 
  //
  return StatusCode::SUCCESS;
}
// ============================================================================
// The factory 
// ============================================================================
DECLARE_ALGORITHM_FACTORY( TrackSmearState )
// ============================================================================
//                                                                      The END 
// ============================================================================



