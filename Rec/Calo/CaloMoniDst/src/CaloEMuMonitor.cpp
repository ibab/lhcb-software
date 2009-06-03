// $Id: CaloEMuMonitor.cpp,v 1.3 2009-06-03 13:07:23 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "CaloEMuMonitor.h"

// from gsl
#include "gsl/gsl_cdf.h"

// =====================================================================
/** @file
 *
 *  Implementation file for class CaloEMuMonitor - another algorithm to monitor
 *  the quality of Calo Electron and Muon PID.
 *
 *  @author Dmitry Golubkov
 *  @date   2009-03-24  */
// =====================================================================

/// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloEMuMonitor );


/// Standard constructor, initializes variables
CaloEMuMonitor::CaloEMuMonitor( const std::string &name,
                                ISvcLocator *pSvcLocator )
  : CaloMoniAlg ( name, pSvcLocator ),
    m_var( NULL )
{
  m_var = new MoniVars();

  declareBasicProperties();


  /* ------------------ Set here the parameters appropriate for no-MC selections ------------------------ */
  declareProperty( "pTmin",              m_minPt        =  500., "minimal pT of the track [MeV/c]");
  declareProperty( "pTmax",              m_maxPt        = 1.e10, "maximal pT of the track [MeV/c]");
  declareProperty( "RichDLLe",           m_RichDLLe     = 4.,    "minimal ProtoParticle::RichDLLe for electron identification");
  declareProperty( "maxEHcalE",          m_maxEHcalE    = 1000., "maximal ProtoParticle::CaloHcalE for electron identification [MeV/c]");


  m_refPar.insert(std::pair<std::string, RefPar>("chi2e",this));
  m_refPar["chi2e" ].htitle = "ProtoParticle::CaloEcalChi2";
  declareProperty( "eCaloEcalChi2_histo",     m_refPar["chi2e" ].v_def = boost::assign::list_of<double> (100)(0.)(10000.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "eCaloEcalChi2_params",    m_refPar["chi2e" ].v_par = boost::assign::list_of<double> (1080.)(2790.)(0.669)(0.0)(0.0)(0.074)(0.0)(0.0) );
  declareProperty( "eCaloEcalChi2_errors",    m_refPar["chi2e" ].v_err = boost::assign::list_of<double> ( 100.)( 120.)(0.015)(-1.)(-1.)(0.008)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("tanhe",this));
  m_refPar["tanhe" ].htitle = "tanh(CaloEcalChi2 / Chi2eNormalization)";
  declareProperty( "eCaloEcalChi2Tanh_histo", m_refPar["tanhe" ].v_def = boost::assign::list_of<double> (100)(0.)(1.)(150) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "eCaloEcalChi2Tanh_params",m_refPar["tanhe" ].v_par = boost::assign::list_of<double> (0.159)(0.313)(0.418)(0.0)(0.0)(0.087)(0.0)(0.0) );
  declareProperty( "eCaloEcalChi2Tanh_errors",m_refPar["tanhe" ].v_err = boost::assign::list_of<double> (0.014)(0.013)(0.005)(-1.)(-1.)(0.010)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("chi2d",this));
  m_refPar["chi2d"].htitle = "ProtoParticle::CaloTrMatch";
  declareProperty( "eCaloTrMatch_histo",      m_refPar["chi2d" ].v_def = boost::assign::list_of<double> (500)(0.)(10.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "eCaloTrMatch_params",     m_refPar["chi2d" ].v_par = boost::assign::list_of<double> (0.503)(0.96)(0.0)(0.0)(0.0)(0.0)(0.0)(0.041) );
  declareProperty( "eCaloTrMatch_errors",     m_refPar["chi2d" ].v_err = boost::assign::list_of<double> (0.012)(0.05)(-1.)(-1.)(-1.)(-1.)(-1.)(0.005) );

  m_refPar.insert(std::pair<std::string, RefPar>("echi2",this));
  m_refPar["echi2"].htitle = "(E-p)^{2}/(cov(E,E)+cov(p,p))";
  declareProperty( "eChi2_histo",             m_refPar["echi2" ].v_def = boost::assign::list_of<double> (100)(0.)(1000.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "eChi2_params",            m_refPar["echi2" ].v_par = boost::assign::list_of<double> (67.)(190.)(0.0)(0.0)(0.0)(0.0202)(0.0)(0.0) );
  declareProperty( "eChi2_errors",            m_refPar["echi2" ].v_err = boost::assign::list_of<double> ( 7.)(  8.)(-1.)(-1.)(-1.)(0.0015)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("epull",this));
  m_refPar["epull"].htitle = "(E-p) pull";
  declareProperty( "ePull_histo",             m_refPar["epull" ].v_def = boost::assign::list_of<double> (100)(-10.)(10.)(100)  );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "ePull_params",            m_refPar["epull" ].v_par = boost::assign::list_of<double> (-0.13)(2.90)(0.0)(-0.48)(1.6)(0.0)(0.0)(0.0) );
  declareProperty( "ePull_errors",            m_refPar["epull" ].v_err = boost::assign::list_of<double> ( 0.03)(0.12)(-1.)( 0.07)(0.2)(-1.)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("prse",this));
  m_refPar["prse"].htitle = "ProtoParticle::CaloPrsE";
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "ePrseE_histo",            m_refPar["prse" ].v_def = boost::assign::list_of<double> (100)(0.)(275.)(100)     );
  declareProperty( "ePrseE_params",           m_refPar["prse" ].v_par = boost::assign::list_of<double> (80.)(56.)(0.0)(0.0)(0.0)(0.0066)(0.0)(0.0) );
  declareProperty( "ePrseE_errors",           m_refPar["prse" ].v_err = boost::assign::list_of<double> ( 5.)( 2.)(-1.)(-1.)(-1.)(0.0015)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("ecalem",this));
  m_refPar["ecalem"].htitle = "ProtoParticle::CaloEcalE (mu)";
  declareProperty( "muCaloEcalE_histo",       m_refPar["ecalem"].v_def = boost::assign::list_of<double> (100)(0.)(5000.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "muCaloEcalE_params",      m_refPar["ecalem"].v_par = boost::assign::list_of<double> (520.)(830.)(0.46)(0.0)(0.0)(0.0)(0.0)(0.0) ); 
  declareProperty( "muCaloEcalE_errors",      m_refPar["ecalem"].v_err = boost::assign::list_of<double> ( 50.)( 60.)(0.02)(-1.)(-1.)(-1.)(-1.)(-1.) ); 

  m_refPar.insert(std::pair<std::string, RefPar>("hcalem",this));
  m_refPar["hcalem"].htitle = "ProtoParticle::CaloHcalE (mu)";
  declareProperty( "muCaloHcalE_histo",       m_refPar["hcalem"].v_def = boost::assign::list_of<double> (100)(0.)(20000.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "muCaloHcalE_params",      m_refPar["hcalem"].v_par = boost::assign::list_of<double> (2550.)(2590.)(0.068)(0.0)(0.0)(0.0)(0.0)(0.0) ); 
  declareProperty( "muCaloHcalE_errors",      m_refPar["hcalem"].v_err = boost::assign::list_of<double> (  40.)(  50.)(0.011)(-1.)(-1.)(-1.)(-1.)(-1.) ); 

  for (std::map<std::string,RefPar>::iterator it = m_refPar.begin(); it != m_refPar.end(); ++it)
    (*it).second.hname = (*it).first;

  declareProperty( "NSigmaThreshold",         m_nSigmaThreshold = boost::assign::list_of<double> (3.)(7.), "Warning and Error thresholds in sigmas"); 
}
/// initialization @see IAlgTool
StatusCode CaloEMuMonitor::initialize() 
{
  StatusCode sc = CaloMoniAlg::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  m_ext          = tool<ITrackExtrapolator>("TrackMasterExtrapolator", this);

  m_calo         = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ) ;

  m_plane        = m_calo->plane(CaloPlane::ShowerMax);

  m_ppSvc        = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);


  bookMonitoringHistograms();

  if (m_extraHistograms) bookExtraHistograms();

  return StatusCode::SUCCESS ;
};

/// comple the initialization of RefPar (book the histograms)
void CaloEMuMonitor::bookMonitoringHistograms(){
  if ( m_refPar.find("tanhe") == m_refPar.end() ){
    warning() << "'tanhe' monitoring histogram missing" << endmsg;
  }else{
    if ( fabs(m_chi2eNorm) < 1e-10 ){
      error() << "Chi2eNormalization=" << m_chi2eNorm << " too small" << endmsg;
      return;
    }

    char buf[64];
#ifdef _WIN32
	_snprintf(buf, sizeof(buf)-1, "tanh(CaloEcalChi2 / %f)", m_chi2eNorm);
#else
	snprintf(buf, sizeof(buf)-1, "tanh(CaloEcalChi2 / %f)", m_chi2eNorm);
#endif
	m_refPar["tanhe"].htitle = buf; 
  }

  // book monitoring histograms
  for (std::map<std::string,RefPar>::iterator it = m_refPar.begin(); it != m_refPar.end(); ++it)
    (*it).second.hbook1();


  // --------------------- test 
  for (std::map<std::string,RefPar>::iterator it = m_refPar.begin(); it != m_refPar.end(); ++it){
    debug() << "Initialize: " << (*it).first         << " hname=" << (*it).second.hname
            << " :  mean="    << (*it).second.mean() << "+-"      << (*it).second.meanErr() 
            << " rms = "      << (*it).second.rms()  << "+-"      << (*it).second.rmsErr() << endmsg; 
  }
  debug() << "Chi2eNormalization = " << m_chi2eNorm << endmsg;
  // --------------------- test 
}

/// book additional monitoring histograms (not checked in finalize())
void CaloEMuMonitor::bookExtraHistograms()
{
  hBook1("p", "momentum", 0.,30000.,100);
  hBook1("pt","pt",       0., 5000.,100);

  hBook1("inecal", "LHCb::ProtoParticle::InAccEcal", 0.,2.,2);
  hBook1("inhcal", "LHCb::ProtoParticle::InAccHcal", 0.,2.,2);
  hBook1("inprs",  "LHCb::ProtoParticle::InAccPrs",  0.,2.,2);
  hBook1("inbrem", "LHCb::ProtoParticle::InAccBrem", 0.,2.,2);

  hBook1("chi2b",  "ProtoParticle::CaloBremChi2",    0.,10000.,100);

  hBook1("spde",   "ProtoParticle::CaloSpdE",  0.,   10.,  10);
  hBook1("ecale",  "ProtoParticle::CaloEcalE", 0.,50000., 100);
  hBook1("hcale",  "ProtoParticle::CaloHcalE", 0.,50000., 100);

  hBook1("spdc",   "ProtoParticle::CaloChargedSpd",  0.,   10.,  10);
  hBook1("prsc",   "ProtoParticle::CaloChargedPrs",  0.,  300., 100);
  hBook1("ecalc",  "ProtoParticle::CaloChargedEcal", 0.,50000., 100);

  hBook1("e_rel_err","sqrt(cov(E,E))/E", 0., 0.05,100);
  hBook1("p_rel_err","sqrt(cov(p,p))/p", 0., 0.10,100);
  hBook1("eop",      "E/p",              0., 4.,  100);
  hBook1("ndigit","number of digits per CaloHypo",  0.,10.,10);
}
/// fill monitoring histograms from the contents of the struct MoniVars m_var 
void CaloEMuMonitor::fillMonitoringHistograms()
{
  if (m_var->eCandidate){
    m_refPar["chi2e"].histo->fill(m_var->chi2e);
    m_refPar["tanhe"].histo->fill(m_var->tanhe);
    m_refPar["chi2d"].histo->fill(m_var->chi2d);
    m_refPar["echi2"].histo->fill(m_var->echi2);
    m_refPar["epull"].histo->fill(m_var->epull);
    m_refPar["prse" ].histo->fill(m_var->prse );
  }

  if (m_var->muCandidate){
    m_refPar["hcalem"].histo->fill(m_var->hcale);
    m_refPar["ecalem"].histo->fill(m_var->ecale);
  }
}
/**
 * fill additional histograms from the contents of the struct MoniVars m_var
 */
void CaloEMuMonitor::fillExtraHistograms()
{
  hFill1("eop",     m_var->eoverp);

  hFill1("p",       m_var->p);
  hFill1("pt",      m_var->pt);

  hFill1("inecal",  m_var->inecal);
  hFill1("inhcal",  m_var->inhcal);
  hFill1("inprs",   m_var->inprs);
  hFill1("inbrem",  m_var->inbrem);

  hFill1("chi2b",   m_var->chi2b);

  hFill1("spde",    m_var->spde);
  hFill1("ecale",   m_var->ecale);
  hFill1("hcale",   m_var->hcale);

  hFill1("spdc",    m_var->spdd);
  hFill1("prsc",    m_var->prsd);
  hFill1("ecalc",   m_var->ecalc);

  hFill1("ndigit",    m_var->h_ndigit);

  hFill1("e_rel_err", m_var->e_rel_err);
  hFill1("p_rel_err", m_var->p_rel_err);
}
/** apply PID-related cuts to select/reject muon candidate ProtoParticle's for histogramming
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted  */
bool CaloEMuMonitor::acceptMu(const LHCb::ProtoParticle *proto) const
{
  if ( !proto ) return false;

  if ( !proto->info( LHCb::ProtoParticle::InAccEcal, false )     ) return false;
  if ( !proto->info( LHCb::ProtoParticle::InAccHcal, false )     ) return false;

  bool ismuon = (bool) proto->muonPID() ? proto->muonPID()->IsMuon() : false;
  if ( !ismuon ) return false;

  return true;
}
/** apply PID-related cuts to select/reject electron candidate ProtoParticle's for histogramming
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted  */
bool CaloEMuMonitor::acceptE(const LHCb::ProtoParticle *proto) const
{
  if ( !proto ) return false;

  //e.g.:  "pt>0.5&&inecal==1&&(hcale<=0||hcale>0&&inhcal==1&&hcale<1000.)&&rdlle>4"
  if ( !proto->info( LHCb::ProtoParticle::InAccEcal, false )    ) return false;

  float rdlle = proto->info( LHCb::ProtoParticle::RichDLLe, -9999.);
  if ( rdlle < m_RichDLLe ) return false;

  float hcale = proto->info(LHCb::ProtoParticle::CaloHcalE, -1 * Gaudi::Units::GeV);
  bool  inhcal= (bool) proto->info( LHCb::ProtoParticle::InAccHcal, false );
  if ( inhcal && hcale > m_maxEHcalE ) return false;

  return true;
}
// ========================================================================================================================
/// Main execution
StatusCode CaloEMuMonitor::execute()
{
  ++ counter("nEvents");

  const LHCb::ProtoParticle::Container* particles =
    get<LHCb::ProtoParticle::Container>( LHCb::ProtoParticleLocation::Charged ) ;
  if ( particles == 0 ) return StatusCode::SUCCESS;

  // Track loop
  for ( LHCb::ProtoParticle::Container::const_iterator iproto = particles->begin() ;
        particles->end() != iproto ; ++iproto )
  {
    const LHCb::ProtoParticle* proto = *iproto ;
    if ( 0 == proto ) continue ;

    const LHCb::Track* track = proto->track() ;
    if ( 0 == track ) continue ;

    if ( !acceptTrack(proto) ) continue ;

    fillMVar(proto);

    fillMonitoringHistograms();
    if (m_extraHistograms) fillExtraHistograms();
  } // end loop over ProtoParticles


  return StatusCode::SUCCESS;
}

/// declare some properties common to MC and RD
void CaloEMuMonitor::declareBasicProperties()
{
  declareProperty( "Chi2eNormalization", m_chi2eNorm       = 2500., "scale factor for the tanh(chi2e / Chi2eNormalization) histogram");
  declareProperty( "minTrackType",       m_minTrackType    =     3, "minimal ProtoParticle::TrackType");
  declareProperty( "nEventMin",          m_nEventMin       =   200, "minimal number of events to check");

  declareProperty( "ExtraHistograms",    m_extraHistograms = false, "fill additional histograms (not monitored)");

  declareProperty( "RefSample",          m_RefSample       = "Brunel_v34r3: Bs_Jpsiphi,ee/mumu + b_inc  (~=DC09)", "description of the reference sample");
}
/** Finalize - calculate mean, rms, relative overflow, etc. and
 *  compare with the reference parameters, generate warnins or
 *  error messages if the difference exceeds NSigmaThreshold's  */
StatusCode CaloEMuMonitor::finalize()
{
  debug() << "==> Finalize" << endmsg;

  if (counter("nEvents").nEntries() >= m_nEventMin)
    this->check();

  if (m_var) delete m_var;
  m_var = NULL;

  return StatusCode::SUCCESS;
}
/** check- calculate mean, rms, relative overflow, etc. and
 *  compare with the reference parameters, generate warnins or
 *  error messages if the difference exceeds NSigmaThreshold's  */
void CaloEMuMonitor::check()
{
  info ()<<"-- Calo Electron/Muon PID Monitor << '" << this->name() << "' --" <<  endmsg;
  debug()<<"------------------------------------------------------------------" <<  endmsg;
  debug()<<" Calo PID        These Events       : Reference values for" <<  endmsg;
  debug()<<"                                    : " << m_RefSample      <<  endmsg;
  debug()<<"------------------------------------------------------------------" <<  endmsg;

  int rcm =-2;
  for (std::map<std::string,RefPar>::iterator it = m_refPar.begin(); it != m_refPar.end(); ++it){
    int rc = (*it).second.check();
    msgStream( rc <= 0 ? MSG::INFO : ( rc == 1 ? MSG::WARNING : MSG::ERROR))
      << format( "%-32s", (*it).second.histo->title().c_str() ) << (*it).second.msg << endmsg;
    if ( rc > rcm ) rcm = rc;
  }

  if       (rcm ==-2){
    info() << "Calo PID quality was not checked" << endmsg;
  }else if (rcm ==-1){
    info() << "Calo PID quality could not be estimated" << endmsg;
  }else if (rcm == 0){
    info() << "Calo PID quality looks ok" << endmsg;
  }else if (rcm == 1){
    warning() << "Warning(s) from testing Calo PID quality!" << endmsg;
  }else if (rcm == 2){
    error() << "Error(s) from testing Calo PID quality!" << endmsg;
  }else{
    warning() << "Warning unexpected result of the Calo PID quality! rc = " << rcm << endmsg;
  }

  info()<<"------------------------------------------------------------------" <<  endmsg;
  if ( rcm > 1 ) warning() << " *** \033[1;1mCalo PIDs might require re-tuning\033[0m !!! ***" << endmsg;
}
/** apply track-quality cuts to select/reject candidate ProtoParticle's for histogramming
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted  */
bool CaloEMuMonitor::acceptTrack(const LHCb::ProtoParticle *proto)
{
  if ( !proto ) return false;
  const LHCb::Track* track = proto->track() ;
  if ( !track ) return false;

  if (track->pt()   < m_minPt       ) return false;
  if (track->pt()   > m_maxPt       ) return false;
  if (track->type() < m_minTrackType) return false;

  // apply selection functions
  m_var->eCandidate  = acceptE(proto);
  m_var->muCandidate = acceptMu(proto);
  if ( !m_var->eCandidate && !m_var->muCandidate ) return false;

  return true;
}
/** fill m_var from the protparticle for subsequent histogramming */
void CaloEMuMonitor::fillMVar(const LHCb::ProtoParticle *proto)
{
  if ( !proto ) return;

  const LHCb::Track* track = proto->track() ;
  if ( !track ) return;

  // track momentum
  m_var->p     = track->p();
  m_var->pt    = track->pt();
  // track type
  m_var->ttype = track->type();

  const SmartRefVector < LHCb::CaloHypo > & hypos      = proto->calo();
  const LHCb::CaloHypo                    * m_electron = NULL;

  for (SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo =  hypos.begin(); ihypo != hypos.end() ; ++ihypo){
    const LHCb::CaloHypo* hypo = *ihypo;
    if (NULL == hypo) continue;
    if (LHCb::CaloHypo::EmCharged == hypo->hypothesis() ) m_electron = hypo;
  }


  m_var->echi2    =-10000.;
  m_var->cov_track=-10000.;
  m_var->cov_hypo =-10000.;
  m_var->p_state  =-10000.;
  m_var->eoverp   =-1.;
  m_var->e_hypo   =-10000.;


  if ( m_electron ){
    m_var->e_hypo = m_electron->position()->e();
    m_var->eoverp = m_var->e_hypo/m_var->p;
    m_var->cov_hypo = m_electron->position()->covariance()(LHCb::CaloPosition::E, LHCb::CaloPosition::E);  // (2,2)

    if ( track->hasStateAt( LHCb::State::ECalShowerMax ) ){
      const LHCb::State *state_ecal = track->stateAt( LHCb::State::ECalShowerMax ) ;
      m_var->cov_track               = state_ecal->errP2();
      m_var->p_state                 = state_ecal->p();
    }else{
      LHCb::State state = track->closestState(m_plane);
      Gaudi::XYZPoint inPlane(m_plane.ProjectOntoPlane(state.position()));

      StatusCode sc = m_ext->propagate( *track, inPlane.z(), state, LHCb::ParticleID(211));
      if ( sc ){
        m_var->cov_track = state.errP2();
        m_var->p_state   = state.p();
      }
    }

    m_var->echi2 = std::pow(m_var->e_hypo-m_var->p, 2)/(m_var->cov_hypo + m_var->cov_track);
    if (m_var->echi2 > 999.) m_var->echi2 = 999.;
  }

  // 3D chi2 for Track and Calo matching (equivalent to CaloElectronMatch)
  m_var->chi2e = proto->info( LHCb::ProtoParticle::CaloEcalChi2 , -999.);
  if (m_var->chi2e > 9999.) m_var->chi2e = 9999.;
  m_var->tanhe = tanh( m_var->chi2e / m_chi2eNorm );


  // Acceptance flag for Prs/Ecal/Hcal/Brem
  m_var->inecal= (bool) proto->info( LHCb::ProtoParticle::InAccEcal, false );
  m_var->inhcal= (bool) proto->info( LHCb::ProtoParticle::InAccHcal, false );
  m_var->inbrem= (bool) proto->info( LHCb::ProtoParticle::InAccBrem, false );
  m_var->inprs = (bool) proto->info( LHCb::ProtoParticle::InAccPrs,  false );

  // Ecal/Hcal/Prs-based DLL for electron-ID
  m_var->epide = proto->info( LHCb::ProtoParticle::EcalPIDe, -99. );
  m_var->hpide = proto->info( LHCb::ProtoParticle::HcalPIDe, -99. );
  m_var->ppide = proto->info( LHCb::ProtoParticle::PrsPIDe,  -99. );

  // Ecal/Hcal-based DLL for muon-ID
  m_var->epidm = proto->info( LHCb::ProtoParticle::EcalPIDmu , -99. );
  m_var->hpidm = proto->info( LHCb::ProtoParticle::HcalPIDmu , -99. );

  // 3D chi2 for Track/CaloHypo(Brem)
  m_var->chi2b = proto->info( LHCb::ProtoParticle::CaloBremChi2 , -1);

  // 3D chi2 for Track/CaloHypo(e) matching
  // -999. coresponds to 10000. for chi21, chi2e, chi23
  m_var->chi23d= proto->info(LHCb::ProtoParticle::CaloElectronMatch, -999.);
  // 2D chi2 for Track/CaloCluster matching (neutral)
  m_var->chi2d = proto->info(LHCb::ProtoParticle::CaloTrMatch,       -999.);

  // Prs/Ecal energy deposit, Spd hit
  m_var->spdd = proto->info(LHCb::ProtoParticle::CaloChargedSpd,  -10.);
  m_var->prsd = proto->info(LHCb::ProtoParticle::CaloChargedPrs,  -10.);
  m_var->ecalc= proto->info(LHCb::ProtoParticle::CaloChargedEcal, -10.);

  // digits or energy associated to CaloHypo (charged)
  m_var->spde = proto->info(LHCb::ProtoParticle::CaloSpdE,  -1 * Gaudi::Units::GeV);
  m_var->prse = proto->info(LHCb::ProtoParticle::CaloPrsE,  -1 * Gaudi::Units::GeV);
  m_var->ecale= proto->info(LHCb::ProtoParticle::CaloEcalE, -1 * Gaudi::Units::GeV);
  m_var->hcale= proto->info(LHCb::ProtoParticle::CaloHcalE, -1 * Gaudi::Units::GeV);


  m_var->epull    = (m_var->cov_track > 0 && m_var->cov_hypo > 0)
                     ? (proto->track()->p() - m_var->e_hypo)/sqrt(m_var->cov_track+m_var->cov_hypo) : -9999.;

  m_var->h_ndigit = m_electron ? (int) m_electron->digits().size() : -1;
  m_var->p_rel_err= (m_var->cov_track > 0 && m_var->p      > 0) ? sqrt(m_var->cov_track)/m_var->p      : -1;
  m_var->e_rel_err= (m_var->cov_hypo  > 0 && m_var->e_hypo > 0) ? sqrt(m_var->cov_hypo )/m_var->e_hypo : -1;

  // fill the PID-related fields before applying selections
  m_var->rdlle  = proto->info( LHCb::ProtoParticle::RichDLLe, -9999.);
  m_var->rdllmu = proto->info( LHCb::ProtoParticle::RichDLLmu , -9999.);
  m_var->ismuon = (bool) proto->muonPID() ? proto->muonPID()->IsMuon() : false;
}
/// compare histogram paramaters with the reference values
int CaloEMuMonitor::RefPar::check()
{
  status = -2;

  if ( !histo ){
    msg = "ERROR - undefined histogram " + hname + "!!!";
    return status;
  }

  status = -1;
  const double total_sum = histo->sumAllBinHeights();
  if ( total_sum == 0 ){
    msg = " cannot assess Calo PID quality -- empty histogram ";
    return status;
  }
  if ( total_sum < minStat() ){
    msg = " cannot assess Calo PID quality -- too low statistics (Entries="
          + boost::lexical_cast<std::string>( total_sum ) + " < "
          + boost::lexical_cast<std::string>( minStat() ) + ") ";
    return status;
  }

  double sig, chi2(0);

/*double (RefPar::*diffs[])(void) = {&RefPar::diffMean, &RefPar::diffRms,
    &RefPar::diffFirstBin, &RefPar::diffLastBin, &RefPar::diffOverflow};

  for (int ii=0; ii<5; ii++){
    updateStatus( (sig = (this->*(diffs[ii]))()) );
    chi2 += sig*sig;
  }  */

  updateStatus( (sig = diffMean    ()) );
  chi2 += sig*sig;

  updateStatus( (sig = diffRms     ()) );
  chi2 += sig*sig;

  updateStatus( (sig = diffFirstBin()) );
  chi2 += sig*sig;

  updateStatus( (sig = diffLastBin ()) );
  chi2 += sig*sig;

  updateStatus( (sig = diffOverflow()) );
  chi2 += sig*sig;

  double prob(-1) ;
  if ( ndof>0 ) {
    const double limit = 1e-10;
    double chi2max = gsl_cdf_chisq_Qinv (limit, ndof);
    prob = chi2 < chi2max ? gsl_cdf_chisq_Q(chi2,ndof) : -1;
  }

  caloMoniAlgPtr->debug() << hname << " Parameter comparison chi2-like sum  = " << chi2
     << " NDOF =" << ndof << " Prob = " << prob << endmsg;

  return status;
}
/// @return difference in sigmas between the Mean and the reference value
double CaloEMuMonitor::RefPar::diffMean()
{
  double sig(0);
  std::ostringstream stream;

  if (meanErr() < 0) return 0;
  tmp_msg.clear();

  float val = histo->mean();
  float err = Gaudi::Utils::HistoStats::meanErr(histo);

  sig = fabs(val - mean()) / sqrt(std::pow(err,2) + std::pow(meanErr(),2)); 
  ndof ++;

  stream << format( " Mean     = %7.5f +- %7.5f", val, err) << format(" : Ref.= %7.5f +- %7.5f ", mean(), meanErr());
  caloMoniAlgPtr->debug() << hname << stream.str() << endmsg;
  tmp_msg+= stream.str();

  return sig;
}
/// @return difference in sigmas between the the (first bin divided by the total entries) and the reference value
double CaloEMuMonitor::RefPar::diffFirstBin()
{
  double sig(0);
  if ( first_binErr() < 0 ) return 0;

  tmp_msg.clear();
  std::ostringstream stream;

  const double bin_val   = histo->binHeight( 0 );
  const double total_sum = histo->sumAllBinHeights();

  double val(-1.), err(-1.);
  if ( total_sum > 0 ){
    val = bin_val / total_sum;
    err = binomialError(bin_val, total_sum);
    sig = fabs(val - first_bin()) / sqrt(std::pow(err,2) + std::pow(first_binErr(),2));
    ndof ++;
  }

  stream << format( " First_bin = %7.5f +- %7.5f", val, err) << format(" : Ref.= %7.5f +- %7.5f ", first_bin(), first_binErr());
  caloMoniAlgPtr->debug() << hname << stream.str() << endmsg;
  tmp_msg+= stream.str();

  return sig;
}
/// @return difference in sigmas between the Rms and the reference value
double CaloEMuMonitor::RefPar::diffRms()
{
  double sig(0);
  if ( rmsErr() < 0 ) return 0;
  tmp_msg.clear();

  std::ostringstream stream;
  float val = histo->rms();
  float err = Gaudi::Utils::HistoStats::rmsErr(histo);

  sig = fabs(val - rms()) / sqrt(std::pow(err,2) + std::pow(rmsErr(),2)); 
  ndof ++;
  stream << format( " RMS      = %7.5f +- %7.5f", val, err) << format(" : Ref.= %7.5f +- %7.5f ", rms(), rmsErr());
  caloMoniAlgPtr->debug() << hname << stream.str() << endmsg;
  tmp_msg+= stream.str();

  return sig;
}
/// @return difference in sigmas between the the (last bin divided by the total entries) and the reference value
double CaloEMuMonitor::RefPar::diffLastBin()
{
  double sig(0);
  if ( last_binErr() < 0 ) return 0;

  tmp_msg.clear();
  std::ostringstream stream;

  const IAxis &axis      = histo->axis();
  const int nBins        = axis.bins();

  const double bin_val   = histo->binHeight( nBins-1 );
  const double total_sum = histo->sumAllBinHeights();

  double val(-1.), err(-1.);
  if ( total_sum > 0 ){
    val = bin_val / total_sum;
    err = binomialError(bin_val, total_sum);
    sig = fabs(val - last_bin()) / sqrt(std::pow(err,2) + std::pow(last_binErr(),2));
    ndof ++;
  }

  stream << format( " Last_bin = %7.5f +- %7.5f", val, err) << format(" : Ref.= %7.5f +- %7.5f ", last_bin(), last_binErr());
  caloMoniAlgPtr->debug() << hname << stream.str() << endmsg;
  tmp_msg+= stream.str();

  return sig;
}
/// @return difference in sigmas between the the overflow divided by the total entries and the reference value
double CaloEMuMonitor::RefPar::diffOverflow()
{
  double sig(0);
  if ( overflowErr() < 0 ) return 0;

  tmp_msg.clear();
  std::ostringstream stream;

  const IAxis &axis = histo->axis();

  const double bin_val   = histo->binHeight( axis.OVERFLOW_BIN );
  const double total_sum = histo->sumAllBinHeights();

  double val(-1.), err(-1.);
  if ( total_sum > 0 ){
    val = bin_val / total_sum;
    err = binomialError(bin_val, total_sum);
    sig = fabs(val - overflow()) / sqrt(std::pow(err,2) + std::pow(overflowErr(),2));
    ndof ++;
  }

  stream << format( " Overflow = %7.5f +- %7.5f", val, err) << format(" : Ref.= %7.5f +- %7.5f ", overflow(), overflowErr());
  caloMoniAlgPtr->debug() << hname << stream.str() << endmsg;
  tmp_msg+= stream.str();

  return sig;
}
/// create the monitored histogram
void CaloEMuMonitor::RefPar::hbook1()
{
  (void) caloMoniAlgPtr->hBook1(hname, htitle, v_def[1], v_def[2], (unsigned int)v_def[0]);
  histo = caloMoniAlgPtr->h1[hname];
}
/// calculate binomial error
double CaloEMuMonitor::RefPar::binomialError(double divident, double divisor)
{
  return (divisor > 0 && divident > 0 && divident < divisor)
         ? sqrt(divident/divisor*(1.-divident/divisor)/divisor) : -1;
}
/// update the value of the RefPar::status variable
void CaloEMuMonitor::RefPar::updateStatus(double sig)
{
  int rc = 0;
  if (sig > caloMoniAlgPtr->nSigmaThreshold(0) ) rc = 1;
  if (sig > caloMoniAlgPtr->nSigmaThreshold(1) ) rc = 2;
  if ( rc == 0 ) tmp_msg = " looks OK"; // tmp_msg = " =====> looks OK";
  if ( rc > status ){
    msg    = tmp_msg;
    status = rc;
  }
}
// ----------------------------------------------------

