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
#include "Event/RecHeader.h"
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
 *  Important properties:
 *  - "Input"         : TES location of input tracks (no need to redefine)
 *  - "DeltaScale"    : global modification for the scale factor 
 * 
 *  Properties that allows to modify the parameters from CONDDB 
 *  - "CONDDBpath"    : the path in CONDB
 * 
 *  - "Delta"         : global modification for the scale factor 
 *  - "IdpPlusHisto"  : "IdpPlus",  2D-histogram of scaling factor as function of \f$t_x,t_y\f$ for \f$q \times B>0\f$
 *  - "IdpMinusHisto" : "IdpMinus", 2D-histogram of scaling factor as function of \f$t_x,t_y\f$ for \f$q \times B<0\f$
 *  - "RunOffsets"    : 1D-histogram for run-dependen offsets
 *  - "CovScaleHisto" : 1D-historam of scaling factor for covariance matrix as function of p 
 * 
 *  For momentum scaling CondDB is activated in case : 
 *  - NO VALID OPTIONS for three histograms are specified 
 *  - valid CONDD path is specified
 *
 *  For monmentum scaling OPTION is activated in case 
 *  - three input valid histos are specified - it overrides CONDB access 
 *
 *  For covariance scaling CondDB is activated in case : 
 *  - NO VALID OPTIONS for input histogram is specified 
 *  - valid CONDD path is specified
 *
 *  For covariance scaling OPTION is activated in case 
 *  - input valid histo is specified - it overrides CONDB access 
 *
 * How to specify the histograms as options? 
 *  
 *  @code 
 * 
 *  iport  ROOT 
 *  import AnalysisPython.PyRoUts ## well, a bit lighter version also exists..
 *  
 *  root_file = TFile( ... , 'READ')
 *
 *  h1          = root_file.Get('ipd-plus' )
 *  h2          = root_file.Get('ipd-minus')
 *  h_offsets   = ... 
 *  h_cov_scale = ... 
 *
 *  from Configurables import TrackScaleState as SCALER
 *
 *  scaler = SCALER() 
 *  scaler.IdpPlusHisto  = h1.toString() 
 *  scaler.IdpMinusHisto = h2.toString() 
 *  scaler.RunOffsets    = h_offsets.toString() 
 *  scaler.CovScaleHisto = h_cov_scale.toString() 
 * 
 *  ## as alternatively one can use XML:
 *  scaler.IdpPlusHisto  = h1.toXML() 
 *  scaler.IdpMinusHisto = h2.toXML() 
 *  scaler.RunOffsets    = h_offsets.toXML() 
 *  scaler.CovScaleHisto = h_cov_scale.toXML() 
 *
 *  @endcode 
 *
 *  The dedicated script calib.py is provided for :
 *   - conversion of run off-set table into offset-histo
 *   - converison of histos into CONDB format
 *
 *  Other cross-checks: Track slope test:
 *  
 *   - "ScaleSlope" 
 *   - "DeltaSlope" 
 *
 *  @code 
 * 
 *  from Configurables import TrackScaleState as SCALER
 *
 *  ## conservative setting:
 *  scaler = SCALER(
 *       'Scaler' , 
 *       ScaleSlope = 1 - 1.0e-3 ,
 *       DeltaSlope =     0.9e-3 ,
 *       )
 *  
 *  ## settings in accordance to Delta m_s paper:
 *  scaler = SCALER(
 *       'Scaler' , 
 *       ScaleSlope = 1 -  2.0e-4 ,
 *       DeltaSlope =      2.0e-4 
 *       )
 *     
 *  @endcode 
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
  /// update the momentum scale data from CONDDB 
  StatusCode i_updateSCALE () ;                   // update the data from CONDDB 
  /// update the covariance scale from CONDDB 
  StatusCode i_updateCOV2  () ;                   // update the data from CONDDB 
  /** the  action for new run:
   *  - store the magnet polarity      (from magnet field service )
   *  - store the run-dependent offset (taking run number from ODIN)
   *  @param run new run number 
   */
  void       new_run      ( const unsigned int run ) ; 

  /// which track types are allowed
  bool allowedType(const LHCb::Track* track) const;

  // ==========================================================================
private:
  // ==========================================================================
  /** scale the covarince matrix 
   *  @param mtrx (in/out) the marix 
   *  @param sf   scale factor 
   */
  inline void scaleCov       
  (  Gaudi::TrackSymMatrix& mtrx , 
     const double           sf    ) const ;
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
    inline int  bin    ( const double v  , const TAxis* axis  ) const ;
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
  std::string m_cond_path     ; //   the condition path in CONDDB    (property) 
  /// the condition path in CONDDB       (property) 
  std::string m_cond_path_cov ; //   the condition path in CONDDB    (property) 
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
  // scale factors for covariance matrix  (property) 
  std::string m_hc_str      ;          // scale factors for covariance matrix
  /// input location for tracks  (property) 
  std::string m_input       ;       // input location for tracks     (property) 
  // ==========================================================================
private:
  // ==========================================================================
  /// scale factor for slopes (==z-scale) 
  double m_slope       ;  // scale factor for slopes (==z-scale)
  /// compensation term for slope scale 
  double m_delta_slope ;  // compensation term for slope scale 
  // ==========================================================================
private: // data 
  // ==========================================================================  
  // the first  histogram for scaler     (property) 
  TH2D        m_h1           ;          // the first  histogram      
  // the second histogram for the scaler (property) 
  TH2D        m_h2           ;          // the second histogram      
  // the run-dependent offsets           (property) 
  TH1D        m_offsets      ;          // the run-dependent offsets
  // the scale factors for covariance matrix (property) 
  TH1D        m_hc           ;          // scale factors for covarinace 
  /// the condition itself (for momentum scale) 
  Condition* m_condition     ;     // the condition itself  (momentum)
  /// the condition itself (for covarinace rescale 
  Condition* m_condition_cov ;     // the condition for covariance 
  /// the run-offset for current run 
  double m_run_offset        ;     // the run-offset  for current cut 
  /// magnet polarity for current run 
  bool   m_down              ;     // magnet polarity for current run
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
TrackScaleState::CalScale::bin ( const double value , const TAxis* axis ) const 
{
  if ( 0 == axis ) { return 1 ; } // arbitrary....
  const int i = axis->FindFixBin ( value ) ;
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
// ============================================================================
// destructor 
// ============================================================================
TrackScaleState::CalScale::~CalScale(){}
// ============================================================================
// anonymous namespace to keep local functions
// ============================================================================
namespace
{
  // ==========================================================================
  /// get the value from the histogram 
  inline double histo_val 
  ( const TH1D&  histo     , 
    const double x         , 
    const double def   = 0 )
  {
    const TAxis* axis = histo.GetXaxis() ;
    if ( 0 == axis ) { return def ; }                     // RETURN
    const int bin = axis->FindFixBin ( x )  ;
    if ( 0 == bin || axis->GetNbins() + 1 ==  bin ) { return def ; } // RETURN
    //
    return histo.GetBinContent ( bin ) ;
  } 
  // ==========================================================================
  // temporary fix, to be propagated into XmlBaseConditionCnv.cpp
  inline void _fix_dir_ ( const TH1& histo ) 
  { const_cast<TH1&>(histo).SetDirectory( 0 ) ; }
  // ==========================================================================
}
// ============================================================================
// scale the covarince matrix 
// ============================================================================
void TrackScaleState::scaleCov
( Gaudi::TrackSymMatrix& mtrx , 
  const double           sf   ) const 
{
  //
  for (int i = 0; i < 5; ++i ) { mtrx [i][4] *= sf ; }  // scale matrix 
  mtrx [4][4] *= sf ;                                   // squared correction 
  //
}
// ============================================================================
// SOME MACROS
// ============================================================================
#define ON_DEBUG   if (UNLIKELY(outputLevel() <= MSG::DEBUG))
#define ON_VERBOSE if (UNLIKELY(outputLevel() <= MSG::VERBOSE))
// ============================================================================
// Standard constructor
// ============================================================================
TrackScaleState::TrackScaleState 
( const std::string& name ,
  ISvcLocator*       pSvc )
  : base_class ( name , pSvc )
//
  , m_cond_path     ( "/dd/Conditions/Calibration/LHCb/MomentumScale"   ) 
  , m_cond_path_cov ( "/dd/Conditions/Calibration/LHCb/CovarianceScale" ) 
  , m_h1_str        ()  // the first  histogram for state scaling 
  , m_h2_str        ()  // the second histogram for state scaling 
  , m_offsets_str   ()  // run-dependent offsets for the scaling 
  , m_delta         ( 2.4e-4 ) 
  , m_deltaScale    ( 0      )
  , m_hc_str        ()  // the histogram for covariance scaling 
  , m_input         ( LHCb::TrackLocation::Default )
// scale factor for slopes (==z-scale) 
  , m_slope         ( 1.0 )  // scale factor for slopes (==z-scale)
// compensation term for slope scale 
  , m_delta_slope   ( 0.0 )  //  compensation term for slope scale 
//
  , m_h1            ()  // the first histogram 
  , m_h2            ()  // second histogram
  , m_offsets       ()  // run-dependent off-sets 
  , m_hc            ()  // histogram for covarinace matrix 
//
  , m_condition     ( 0      )  
  , m_condition_cov ( 0      )  
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
  declareProperty 
    ( "ScaleSlope"    , 
      m_slope         ,
      "Scaling factor for track slopes" ) ;
  //
  declareProperty 
    ( "DeltaSlope"    , 
      m_delta_slope   ,
      "Compensation factor for track slopes scaling" ) ;
  // 
  declareProperty 
    ( "CovScaleHisto" , 
      m_hc_str ,
      "The 1D-historam for covarinace scaling factor as function of momenta" ) ;
  //
  _fix_dir_ ( m_h1      ) ;
  _fix_dir_ ( m_h2      ) ;
  _fix_dir_ ( m_offsets ) ;
  _fix_dir_ ( m_hc      ) ;
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
  // if all histograms from the options are fine, just use them  
  //
  if ( Gaudi::Parsers::parse ( m_h1      , m_h1_str      ).isSuccess () && 
       Gaudi::Parsers::parse ( m_h2      , m_h2_str      ).isSuccess () && 
       Gaudi::Parsers::parse ( m_offsets , m_offsets_str ).isSuccess () ) 
  {
    //
    m_scaler.reset ( new CalScale ( &m_h1 , &m_h2 , m_delta ) ) ;
    info() << "Use the momentum   calibration from options" << endmsg ;
  }
  else if ( !m_cond_path.empty() && existDet<DataObject>( detSvc() , m_cond_path ) ) 
  {
    //
    try 
    {  
      registerCondition ( m_cond_path                     , 
                          m_condition                     , 
                          &TrackScaleState::i_updateSCALE );
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
    info() << "Use the momentum   calibration from CONDDB"  << endmsg ;
  }
  else 
  {
    return Error ( "Invalid setting of histogram source(1)" ) ;           // RETURN 
  }
  //  scale covariance matrices 
  if ( Gaudi::Parsers::parse ( m_hc      , m_hc_str      ).isSuccess () ) 
  {
    info() << "Use the covariance calibration from options" << endmsg ;
  }
  else if ( !m_cond_path_cov.empty() && 
            existDet<DataObject>( detSvc() , m_cond_path_cov ) ) 
  {
    //
    try 
    {  
      registerCondition ( m_cond_path_cov                , 
                          m_condition_cov                ,
                          &TrackScaleState::i_updateCOV2 );
    }
    catch ( GaudiException &e ) 
    {
      error () << e << endmsg;
      return Error ( "Unable register condition " + m_cond_path_cov , e.code() ) ;  
    }
    //
    sc = runUpdate () ;
    if ( sc.isFailure() ) { return Error ( "Unable update Run" , sc ) ; }
    //
    info() << "Use the covariance calibration from CONDDB"  << endmsg ;
  }
  else 
  {
    return Error ( "Invalid setting of histogam source(2)" ) ;           // RETURN 
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
  //
  if ( !LHCb::Math::equal_to_double ( m_slope       , 1.0 ) || 
       !LHCb::Math::equal_to_double ( m_delta_slope , 0.0 ) ) 
  {
    Warning ( "Track slope scaling is  activated" ) ;
    warning () << " SlopeScale " << m_slope 
               << " DeltaSlope " << m_delta_slope << endmsg ;
  }
  //
  return sc ;
}
// ============================================================================
// specific algorithm finalization 
// ============================================================================ 
StatusCode TrackScaleState::finalize () 
{
  //
  if ( !LHCb::Math::equal_to_double ( m_slope       , 1.0 ) || 
       !LHCb::Math::equal_to_double ( m_delta_slope , 0.0 ) ) 
  {
    Warning ( "Track slope scaling was used"      ) ;
    warning () << " SlopeScale " << m_slope 
               << " DeltaSlope " << m_delta_slope << endmsg ;
  }
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
StatusCode TrackScaleState::i_updateSCALE ()
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
    _fix_dir_ ( *h1 ) ;
    //
    ON_DEBUG
    { info () << "Comment for IdpPlus  : " << m_condition->comment ( "IdpPlus"  ) << endmsg ; }
    //
    h2 = DetDesc::Params::paramAsHisto2D ( m_condition , "IdpMinus"  ) ;
    if ( 0 == h2 ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'IdpMinus' from CONDDB") ; 
    }
    _fix_dir_ ( *h2 ) ;
    //
    ON_DEBUG
    { info () << "Comment for IdpMinus : " << m_condition->comment ( "IdpMinus" ) << endmsg ; }
    //
    ro = DetDesc::Params::paramAsHisto1D ( m_condition , "Offsets"   ) ;
    if ( 0 == ro ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'Offsets' from CONDDB") ; 
    }
    _fix_dir_ ( *ro ) ;
    //
    ON_DEBUG
    { info () << "Comment for Offsets  : " << m_condition->comment ( "Offsets"  ) << endmsg ; }
    //
    if ( m_condition->exists ( "Delta" ) && m_condition->is<double>( "Delta" ) ) 
    { 
      m_delta = m_condition->paramAsDouble ( "Delta" ) ;
      counter("#DELTA update") += m_delta ;
      //
      ON_DEBUG
      { info () << "Comment for Delta    : " << m_condition->comment ( "Delta"    ) << endmsg ; }
      //
    }
    //
  }
  catch ( GaudiException& e ) 
  {
    ++counter("#CONDB problem") ;
    return Error("Unable to get data from CONDDB" , e.code() ) ; // RETURN
  }
  //
  info () << " Condition: " << m_condition -> name()    << "   "
          << ( m_condition ->isValid() ? "  Valid;  " : "Invalid; " ) 
          << " Validity: "  << m_condition -> validSince ().format ( true ) 
          << " -> "         << m_condition -> validTill  ().format ( true )  << endmsg ;
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
// update histos from CONDDB 
// ============================================================================
StatusCode TrackScaleState::i_updateCOV2 ()
{
  //
  if ( !m_condition_cov ) { return StatusCode::FAILURE ; }  //               RETURN
  //
  const TH1D* h1 = 0 ;
  //
  try 
  {
    //
    h1 = DetDesc::Params::paramAsHisto1D ( m_condition_cov , "Scale"   ) ;
    //
   if ( 0 == h1 ) 
    { 
      ++counter("#CONDB problem") ;
      return Error ( "Unable to get 'Scale' from CONDDB") ; 
    }
   _fix_dir_ ( *h1 ) ;
   //
  }
  catch ( GaudiException& e ) 
  {
    ++counter("#CONDB problem") ;
    return Error("Unable to get data from CONDDB" , e.code() ) ; // RETURN
  }
  //
  info () << " Condition: " << m_condition_cov -> name()    << "   "
          << ( m_condition_cov ->isValid() ? "  Valid;  " : "Invalid; " ) 
          << " Validity: "  << m_condition_cov -> validSince ().format ( true ) 
          << " -> "         << m_condition_cov -> validTill  ().format ( true )  << endmsg ;
  //
  h1 -> Copy ( m_hc      ) ;
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
  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks> ( m_input );
  //
  if ( LIKELY ( NULL!= tracks) ) {}
  else 
  { return Warning ( "No tracks at location '" + m_input + "'" , StatusCode::SUCCESS ) ; }
  //
  /// perform the action for new run 
  //  const LHCb::ODIN* odin = get<LHCb::ODIN>( evtSvc() , LHCb::ODINLocation::Default) ;
   LHCb::RecHeader* odin =  get<LHCb::RecHeader>(LHCb::ProcessHeaderLocation::Rec,false);

  //
  if ( m_new_run || ( m_current_run != odin->runNumber () ) )
  { new_run ( odin->runNumber () ) ; }
  //
  // get the magnetic field sign
  const int polarity = m_down ? -1 : 1 ;
  //
  StatEntity& scale = counter("SCALE"     ) ;
  StatEntity& cov2  = counter("SCALECOV2" ) ;
  //
  // loop and do the scaling
  for ( LHCb::Tracks::iterator itrack = tracks->begin() ; 
        itrack != tracks->end(); ++itrack )
  {
    LHCb::Track* track = *itrack ;
    if ( 0 == track ) { continue ; }
    //
    // scale only LONG tracks 
    if ( allowedType(track ) == false ) { continue ; } // scale only Long tracks 
    //
    
    typedef std::vector<LHCb::State*> STATES ;
    const STATES& states = track->states();
    //
    const int idp = track->charge()*polarity; // charge * magnet polarity
    //
    const double p = track->p() ;
    //
    // scale factor for the covariace matrix
    const double sf = histo_val ( m_hc , p , 1.0 ) ;
    //
    cov2 += sf ;
    // loop over the states for the given track 
    for ( STATES::const_iterator istate = states.begin(); 
          istate != states.end(); ++istate ) 
    {
      LHCb::State* state = *istate ;
      if ( 0 == state ) { continue ; }
      //
      const double qOverP = state->qOverP(); 
      //
      const double tx = state -> tx () ;
      const double ty = state -> ty () ;
      // 
      // calculate  the scale factor :
      const double theScale = m_scaler -> eval ( tx , ty , idp , m_run_offset ) 
        + m_deltaScale + m_delta_slope ;
      //
      scale += theScale  ;
      //
      // the actual scaling: 
      //
      state -> setQOverP ( qOverP / theScale ) ;          // THE ACTUAL SCALING 
      //
      // scale slopes:                                    // ATTENTION!
      //
      state -> setTx ( tx * m_slope ) ;
      state -> setTy ( ty * m_slope ) ;      
      //
      // scale the covariance matrix 
      scaleCov ( state -> covariance () , sf ) ;     // scale covariance matrix 
    } //                                           end of loop over the states   
  } //                                             end loop over the tracks 
  //
  return StatusCode::SUCCESS;
}

bool TrackScaleState::allowedType(const LHCb::Track* track) const{

  if (track->type() == LHCb::Track::Long) return true;
  if (track->type() == LHCb::Track::Downstream) return true; 
  return false;
}

// ============================================================================
// The factory 
// ============================================================================
DECLARE_ALGORITHM_FACTORY( TrackScaleState )
// ============================================================================
// The END 
// ============================================================================


