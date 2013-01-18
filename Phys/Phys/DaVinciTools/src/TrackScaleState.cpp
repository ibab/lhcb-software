// $Id:$ 
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
// LHCbKernel 
// ============================================================================
#include "Kernel/ILHCbMagnetSvc.h"
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
#include "TH2D.h"
#include "TAxis.h"
// ============================================================================
/** @class TrackScaleState TrackScaleState.h
 *
 *  Scale the state
 *
 *  @author M.Needham
 *  @date   30/05/2006
 *  
 *  Use on DST to scale the track states *before* your 
 *  user algorithms sequence:
 *
 *  @code
 *  from Configurables import TrackScaleState as SCALER
 *
 *  scaler = SCALER( 'Scaler' )  ## default configuration is perfectly fine 
 *
 *  daVinci = DaVinci ( ... ) 
 *  daVinci.UserAlgortithms  = [ scaler ]
 *  
 *  seq = ... ## analysis sequence  
 *  daVinci.UserAlgortithms += [ seq    ]
 *  
 *  @endcode
 *
 *  Use on uDST to scale the track states *before* your 
 *  user algorithms sequence:
 *
 *  @code
 *  from Configurables import TrackScaleState as SCALER
 *
 *  rootInTES = "/Event/Leptonic" ## example
 *
 *  scaler = SCALER( 'Scaler' , RootInTES = rootInTES ) ## note the difference 
 *
 *  daVinci = DaVinci ( ... ) 
 *  daVinci.UserAlgorithms   = [ scaler ]
 *  
 *  seq = ... ## analysis sequence  
 *  daVinci.UserAlgortithms += [ seq    ]
 *  
 *  @endcode
 *
 *  Other properties:
 *  - "Input"       : TES location of input tracks (no need to redefine)
 *  - "DeltaScale"  : global modification for the scale factor 
 * 
 *  Properties that allows to modify the parameters from CONDDB 
 *  - "CONDDBpath"  : the path in CONDB
 * 
 *  - "Delta"         : global modification for the scale factor 
 *  - "IdpPlusHisto"  : histogram for "IdpPlus"
 *  - "IdpMinusHisto" : histogram for "IdpMinus"
 *  - "RunOffsets"    : histogram for run-dependen offsets
 * 
 *  CondDB is activated in case : 
 *  - NO VALID OPTIONS for histograms are specified 
 *  - valid CONDD path is specifiede
 *
 *  OPTION is actiavted in case 
 *  - all three input histos are specified - it overrides CONDB access 
 *
 * How to sepcify the histograms as options? 
 *  
 *  @code 
 * 
 *  iport  ROOT 
 *  import AnalysisPython.PyRoUts ## well, a bit lighter version also exists..
 *  
 *  root_file = TFile( ... , 'READ')
 *
 *  h1        = root_file.Get('ipd-plus' )
 *  h2        = root_file.Get('ipd-minus')
 *  h_offsets = ... 
 * 
 *  from Configurables import TrackScaleState as SCALER
 *
 *  scaler = SCALER() 
 *  scaler.IdpPlusHisto  = h1.toString() 
 *  scaler.IdpMinusHisto = h2.toString() 
 *  scaler.RunOffsets    = h_offsets.toString() 
 * 
 *  ## as alternatively one can use XML:
 *  scaler.IdpPlusHisto  = h1.toXML() 
 *  scaler.IdpMinusHisto = h2.toXML() 
 *  scaler.RunOffsets    = h_offsets.toXML() 
 *
 *  @endcode 
 *
 *  The dedicated script calib.py is provided for :
 *   - conversion of run off-set table into offset-histo
 *   - converison of histos into CONDB format
 */
class TrackScaleState : public extends1<GaudiAlgorithm,IIncidentListener>
{
  // ==========================================================================
  // friend factory for instantiation 
  friend class AlgFactory<TrackScaleState> ;
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
  TrackScaleState ( const std::string& name        ,
                    ISvcLocator*       pSvcLocator ) ;
  /// virtual an dprotected destructor 
  virtual ~TrackScaleState (); // virtual an dprotected destructor 
  // ==========================================================================
private:  // disable everything that is not needed 
  // ==========================================================================
  /// default constructor is disabled 
  TrackScaleState () ; // default constructor is disabled 
  /// copy  constructor is disabled 
  TrackScaleState ( const TrackScaleState& ) ; // copy  constructor is disabled 
  /// assignement operator is disabled 
  TrackScaleState& operator=( const TrackScaleState& ) ; // no assignement
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
  // the  action for new run 
  // ==========================================================================
private : // some local private class 
  // ==========================================================================
  /** @class CalScale 
   *  Helper class for momentum scale calibration 
   *  @author M.Needham
   */
  class CalScale
  { 
  public:
    // ========================================================================
    /// constructor 
    CalScale 
    ( const TH2D*  phisto , 
      const TH2D*  mhisto , 
      const double delta  ) ;
    /// destructor 
    ~CalScale() ;
    // ========================================================================
    /** get the value for the scale factor 
     *  @param tx      tx-slope 
     *  @param ty      ty-slope 
     *  @param idp     the charge times polarity 
     *  @param offset  offset (run-dependent) 
     */
    double eval
    ( const double tx      , 
      const double ty      , 
      const int    idp     , 
      const double offset  ) const ;
    // ========================================================================
  private: // histogram manipulations
    // ========================================================================
    inline int  bin    ( const double v  ,       TAxis* axis  ) const ;
    inline int  binTx  ( const double tx , const TH2D*  histo ) const ;
    inline int  binTy  ( const double ty , const TH2D*  histo ) const ;
    // ========================================================================
  private: // data 
    // ========================================================================
    const TH2D*  m_phisto ;  
    const TH2D*  m_mhisto ;  
    double       m_delta  ;
    // ========================================================================
  };
  // ==========================================================================
private: // properties 
  // ==========================================================================
  /// the condition path in CONDDB       (property) 
  std::string m_cond_path   ; //   the condition path in CONDDB      (property) 
  /// =========================================================================
  // the first  histogram for scaler     (property) 
  std::string m_h1_str      ;          // the first  histogram       (property) 
  // the second histogram for the scaler (property) 
  std::string m_h2_str      ;          // the second histogram       (property)
  // the run-dependent offsets           (property) 
  std::string m_offsets_str ;          // the run-dependent offsets  (property)
  // delta       (property)  
  double      m_delta       ;          // delta                      (property) 
  // deltaScale  (property)  
  double      m_deltaScale  ;          // deltaScale                 (property) 
  /// input location for tracks  (property) 
  std::string m_input       ;       // input location for tracks     (property) 
  // ==========================================================================
private: // data 
  // ==========================================================================  
  // the first  histogram for scaler     (property) 
  TH2D        m_h1          ;          // the first  histogram      
  // the second histogram for the scaler (property) 
  TH2D        m_h2          ;          // the second histogram      
  // the run-dependent offsets           (property) 
  TH1D        m_offsets     ;          // the run-dependent offsets
  /// the condition itself 
  Condition* m_condition    ;     // the condition itself 
  /// the run-offset for current run 
  double m_run_offset       ;     // the run-offset  for current cut 
  /// magnet polarity for current run 
  bool   m_down             ;     // magnet polarity for current run
  /// the actual scaler
  std::auto_ptr<CalScale> m_scaler      ;   // the actual scaler
  /// magnetic field service 
  ILHCbMagnetSvc*         m_magfieldsvc ;   // magnetic field service 
  /// new run?
  bool                    m_new_run     ;   // new run? 
  /// current run
  unsigned long           m_current_run ;
  // =========================================================================
};
// ============================================================================
// TrackScaleState::CalScale 
// ============================================================================
inline int 
TrackScaleState::CalScale::bin ( const double value , TAxis* axis ) const 
{
  if ( 0 == axis ) { return 1 ; } // arbitrary....
  const int i = axis->FindBin ( value ) ;
  //
  return 
    0 == i               ? 1 :
    axis->GetNbins() < i ? axis->GetNbins() : i ;  
}
// ============================================================================
inline int 
TrackScaleState::CalScale::binTx ( const double tx , const TH2D* histo ) const 
{
  if ( 0 == histo ) { return 1 ; }  // arbitrary 
  return bin ( tx , histo->GetXaxis() ) ;
}
// ============================================================================
inline int 
TrackScaleState::CalScale::binTy ( const double ty , const TH2D* histo ) const 
{
  if ( 0 == histo ) { return 1 ; }  // arbitrary 
  return bin ( ty , histo->GetYaxis() ) ;
}
// ============================================================================
inline double 
TrackScaleState::CalScale::eval
( const double tx     , 
  const double ty     , 
  const int    idp    , 
  const double offset ) const 
{
  //
  const TH2D* histo = idp > 0 ? m_phisto : m_mhisto ;
  //
  const int txBin = binTx ( tx , histo ) ;
  const int tyBin = binTy ( ty , histo ) ;
  //
  return 1 -  ( ( histo -> GetBinContent ( txBin , tyBin ) / 1000.) + offset + m_delta );
}
// ============================================================================
// constructor 
// ============================================================================
TrackScaleState::CalScale::CalScale 
( const TH2D*  phisto , 
  const TH2D*  mhisto , 
  const double delta  ) 
  : m_phisto ( phisto )
  , m_mhisto ( mhisto )
  , m_delta  ( delta  ) 
{}
// ========================================================================
// destructor 
// ========================================================================
TrackScaleState::CalScale::~CalScale(){}
// ============================================================================
// anonymous namespace to keep local functions
// ============================================================================
namespace
{
  // ==========================================================================
  /// get the value from the histogram 
  inline double histo_val ( const TH1D&  histo , const double x     )
  {
    TAxis* axis = histo.GetXaxis() ;
    if ( 0 == axis ) { return 0 ; }                     // RETURN
    const int bin = axis->FindBin ( x )  ;
    if ( 0 == bin || axis->GetNbins() + 1 ==  bin ) { return 0 ; } 
    //
    return histo.GetBinContent ( bin ) ;
  } 
  // ==========================================================================
}
// ============================================================================
// Standard constructor
// ============================================================================
TrackScaleState::TrackScaleState 
( const std::string& name ,
  ISvcLocator*       pSvc )
  : base_class ( name , pSvc )
//
  , m_cond_path     ( "/dd/Conditions/Calibration/MomentumScale" ) 
  , m_h1_str        ()  // the first  histogram for state scaling 
  , m_h2_str        ()  // the second histogram for state scaling 
  , m_offsets_str   ()  // run-dependent offsets for the scaling 
  , m_delta         ( 2.4e-4 ) 
  , m_deltaScale    ( 0      )
  , m_input         ( LHCb::TrackLocation::Default ) 
//
  , m_h1            () 
  , m_h2            () 
  , m_offsets       ()
//
  , m_condition     ( 0      )  
  , m_run_offset    ( 0      ) // the run-offset for the current run 
  , m_down          ( true   ) // field polarity for the current run 
  , m_scaler        () 
  , m_magfieldsvc   ( 0      )
  , m_new_run       ( true   )
  , m_current_run   ( 0      )
//
{
  //
  declareProperty 
    ( "CONDDBpath" , 
      m_cond_path  ,
      "The path in CONDDB with calibration data" ) ;
  //
  declareProperty 
    ( "IdpPlusHisto" , 
      m_h1_str       ,
      "The 2D-historam for ipd-plus calibration data" ) ;
  //
  declareProperty 
    ( "IdpMinusHisto" , 
      m_h2_str        ,
      "The 2D-historam for ipd-minus calibration data" ) ;
  //
  declareProperty 
    ( "RunOffsets"  , 
      m_offsets_str  ,
      "The 1D-historam for run-dependent offsets" ) ;
  //
  declareProperty 
    ( "Delta" , 
      m_delta ,
      "The global delta to be applied"     ) ;
  //
  declareProperty 
    ( "DeltaScale" , 
      m_deltaScale ,
      "Another global delta to be applied" ) ;
  //
  declareProperty 
    ( "Input"         , 
      m_input         ,
      "Input location for Tracks" ) ;
  //
}
// ============================================================================
// destructor 
TrackScaleState::~TrackScaleState() {}
// ============================================================================
// specific algorithm initialization
// ============================================================================
StatusCode TrackScaleState::initialize() 
{
  /// initialize the base class 
  StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) { return sc ; }
  //
  //  
  // if all histograms from the options are fine, just use them  
  //
  if ( Gaudi::Parsers::parse ( m_h1      , m_h1_str      ).isSuccess () && 
       Gaudi::Parsers::parse ( m_h2      , m_h2_str      ).isSuccess () && 
       Gaudi::Parsers::parse ( m_offsets , m_offsets_str ).isSuccess () ) 
  {
    //
    m_scaler.reset ( new CalScale ( &m_h1 , &m_h2 , m_delta ) ) ;
    info() << "Use the momentum calibration from options" << endreq ;
  }
  else if ( !m_cond_path.empty() && existDet<DataObject>( detSvc() , m_cond_path ) ) 
  {
    //
    try {  
      registerCondition ( m_cond_path , m_condition , &TrackScaleState::i_updateDATA );
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
    info() << "Use the momentum calibration form CONDDB"  << endreq ;
  }
  else 
  {
    return Error ( "Invalid setting of histogam source" ) ;           // RETURN 
  }
  //
  if ( 0 == m_scaler.get() ) { return Error ( "Invalid Scaler" ) ; }  // RETURN 
  //
  if ( 1 ==     m_h1.GetXaxis ()->GetNbins() || 
       1 ==     m_h1.GetYaxis ()->GetNbins() ) 
  { return Error ( "Invalid 'IdpPlus'  Histogram" ) ; }               // RETURN 
  if ( 1 ==     m_h2.GetXaxis ()->GetNbins() || 
       1 ==     m_h2.GetYaxis ()->GetNbins() ) 
  { return Error ( "Invalid 'IdpMinus' Histogram" ) ; }               // RETURN
  if ( 1 == m_offsets.GetXaxis ()->GetNbins()   ) 
  { return Error ( "Invalid 'Offsets'  Histogram" ) ; }               // RETURN
  //
  m_magfieldsvc = svc<ILHCbMagnetSvc> ( "MagneticFieldSvc", true );
  m_down = m_magfieldsvc->isDown();
  //
  // subscribe to run-change incident 
  IIncidentSvc* incsvc = svc<IIncidentSvc>("IncidentSvc") ;
  incsvc -> addListener ( this , IncidentType::RunChange ) ;
  incsvc -> addListener ( this , IncidentType::BeginRun  ) ;
  //
  return sc ;
}
// ============================================================================
// specific algorithm finalization 
// ============================================================================ 
StatusCode TrackScaleState::finalize () 
{
  //
  m_magfieldsvc = 0 ;
  m_scaler.reset()  ;
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
void TrackScaleState::handle ( const Incident& /* inc */ )  // handle incident 
{ m_new_run = true ; }
// ============================================================================
// action for new run 
// ============================================================================
void TrackScaleState::new_run ( const unsigned int run ) 
{
  // get the magnet polarity
  const bool down = m_magfieldsvc->isDown() ;
  //
  StatEntity& p = counter ( "#POLARITY change" ) ;
  p += ( 0 == p.nEntries() ) ? false : ( ( !m_down && down ) || ( m_down && !down ) ) ;
  //
  m_run_offset = histo_val ( m_offsets , run ) ; 
  //
  ++counter ( "#RUN   change"    ) ;
  counter   ( "#RUN   offset"    ) += m_run_offset ;
  //
  m_down        = down  ;
  m_new_run     = false ;
  m_current_run = run   ;
}
// ============================================================================
// update histos from CONDDB 
// ============================================================================
StatusCode TrackScaleState::i_updateDATA ()
{
  //
  if ( !m_condition ) { return StatusCode::FAILURE ; }  //               RETURN
  //
  const TH2D* h1 = 0 ;
  const TH2D* h2 = 0 ;
  const TH1D* ro = 0 ;
  //
  try 
  {
    //
    h1 = DetDesc::Params::paramAsHisto2D ( m_condition , "IdpPlus"   ) ;
    if ( 0 == h1 ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'IdpPlus'  from CONDDB" ) ;
    }
    //
    h2 = DetDesc::Params::paramAsHisto2D ( m_condition , "IdpMinus"  ) ;
    if ( 0 == h2 ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'IdpMinus' from CONDDB") ; 
    }
    //
    ro = DetDesc::Params::paramAsHisto1D ( m_condition , "Offsets"   ) ;
    if ( 0 == ro ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'Offsets' from CONDDB") ; 
    }
    //
    if ( m_condition->exists ( "Delta" ) && m_condition->is<double>( "Delta" ) ) 
    { 
      m_delta = m_condition->paramAsDouble ( "Delta" ) ;
      counter("#DELTA update") += m_delta ;
    }
    //
  }
  catch ( GaudiException& e ) 
  {
    ++counter("#CONDB problem") ;
    return Error("Unable to get data form CONDDB" , e.code() ) ; // RETURN
  }
  //
  h1 -> Copy ( m_h1      ) ;
  h2 -> Copy ( m_h2      ) ;
  ro -> Copy ( m_offsets ) ;
  //
  m_scaler.reset ( new CalScale ( &m_h1 , &m_h2 , m_delta ) ) ;
  //
  ++counter("#CONDB update") ;
  //
  return StatusCode::SUCCESS;
}

// ============================================================================
// specific algorithm execution 
// ============================================================================
StatusCode TrackScaleState::execute ()
{
  // get the tracks to scale
  LHCb::Tracks* trackCont = get<LHCb::Tracks> ( m_input );
  //
  /// perform the action for new run 
  const LHCb::ODIN* odin = get<LHCb::ODIN>( evtSvc() , LHCb::ODINLocation::Default) ;
  //
  if ( m_new_run || ( m_current_run != odin->runNumber () ) )
  { new_run ( odin->runNumber () ) ; }
  //
  // get the magnetic field sign
  const int polarity = m_down ? -1 : 1 ;
  //
  StatEntity& scale = counter("SCALE") ;
  //
  // loop and do the scaling
  for ( LHCb::Tracks::iterator it = trackCont->begin() ; 
        it != trackCont->end(); ++it )
  {
    LHCb::Track* track = *it ;
    if ( 0 == track ) { continue ; }
    //
    typedef std::vector<LHCb::State*> STATES ;
    const STATES& theStates = track->states();
    //
    const int idp = track->charge()*polarity; // charge * magnet polarity
    //
    // loop over the states for the given track 
    for ( STATES::const_iterator iterState = theStates.begin(); 
          iterState != theStates.end(); ++iterState ) 
    {
      LHCb::State* state = *iterState ;
      if ( 0 == state ) { continue ; }
      //
      const double qOverP = state->qOverP(); 
      //
      const double tx = state -> tx () ;
      const double ty = state -> ty () ;
      // 
      // calculate  the scale factor :
      const double theScale = m_scaler -> eval (tx , ty , idp , m_run_offset ) 
        + m_deltaScale;
      //
      scale += theScale ;
      //
      // the actual scaling: 
      state ->setQOverP ( qOverP / theScale ) ;          // THE ACTUAL SCALING 
      //
    } //                                           end of loop over the states   
  } //                                             end loop over the tarcks 
  //
  return StatusCode::SUCCESS;
}
// ============================================================================
// The factory 
// ============================================================================
DECLARE_ALGORITHM_FACTORY( TrackScaleState )
// ============================================================================
// The END 
// ============================================================================
