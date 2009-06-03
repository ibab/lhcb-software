// $Id: CaloEMuChecker.cpp,v 1.2 2009-06-03 13:07:23 cattanem Exp $
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
 *  Implementation file for class CaloEMuChecker - another algorithm to monitor
 *  the quality of Calo Electron and Muon PID (using MC-truth information).
 *
 *  @author Dmitry Golubkov
 *  @date   2009-03-24  */
// =====================================================================


/// another algorithm to monitor the quality of Calo Electron and Muon PID (using MC-truth information)
class CaloEMuChecker : public CaloEMuMonitor {
public:
  CaloEMuChecker(const std::string &name, ISvcLocator *pSvcLocator); ///< standard constructor
  virtual ~CaloEMuChecker(){ }           ///< standard destructor

  virtual StatusCode initialize   ();    ///< Algorithm init
  virtual StatusCode finalize     ();    ///< standard aligorithm finalization - check the histograms

protected:

  class MoniVarsMC : public MoniVars {
  public:
    virtual ~MoniVarsMC(){ }

    int   mcid;      ///< PDG code of the matched MC particle
    int   mothpid;   ///< PDG code of the mother partice (JPsi=443)
  };

  /// select/reject electron candidates
  bool acceptE(const LHCb::ProtoParticle *proto) const;
  /// select/reject muon candidates
  bool acceptMu(const LHCb::ProtoParticle *proto) const;
  /// select/reject track candidates
  virtual bool acceptTrack(const LHCb::ProtoParticle *proto);
  /// return the threshold value in sigmas for Warning (level=0) and Error (level=1)

  /// try to associate MC truth information and apply MC-related cuts
  bool mcTruth(const LHCb::ProtoParticle *proto) const;
  /// fill monitoring histograms from m_var
  virtual void  fillMonitoringHistograms();

  /// require the lepton to be from a J/psi decay
  bool  m_motherJpsi;

  /// Track -> MC associator
  Object2MCLinker<LHCb::Track>* m_track2MCLink;
};


/// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloEMuChecker );


/// Standard constructor, initializes variables
CaloEMuChecker::CaloEMuChecker( const std::string &name,
                                ISvcLocator *pSvcLocator )
  : CaloEMuMonitor ( name, pSvcLocator )
{
  m_var = new CaloEMuChecker::MoniVarsMC;

  declareBasicProperties();

  declareProperty( "pTmin",              m_minPt        =  500., "minimal pT of the track [MeV/c]");
  declareProperty( "pTmax",              m_maxPt        = 1300., "maximal pT of the track [MeV/c]");
  declareProperty( "motherJpsi",         m_motherJpsi   = false, "require the lepton to be from a J/psi decay");


  m_refPar.insert(std::pair<std::string, RefPar>("chi2e",this));
  m_refPar["chi2e" ].htitle = "ProtoParticle::CaloEcalChi2";
  declareProperty( "CaloEcalChi2_histo",      m_refPar["chi2e" ].v_def = boost::assign::list_of<double> (100)(0.)(10000.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "CaloEcalChi2_params",     m_refPar["chi2e" ].v_par = boost::assign::list_of<double> (930.)(2350.)(0.534)(0.0)(0.0)(0.046)(0.0)(0.0) );
  declareProperty( "CaloEcalChi2_errors",     m_refPar["chi2e" ].v_err = boost::assign::list_of<double> ( 60.)(  90.)(0.014)(-1.)(-1.)(0.004)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("tanhe",this));
  m_refPar["tanhe" ].htitle = "tanh(CaloEcalChi2 / Chi2eNormalization)";
  declareProperty( "CaloEcalChi2Tanh_histo",  m_refPar["tanhe" ].v_def = boost::assign::list_of<double> (100)(0.)(1.)(150) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "CaloEcalChi2Tanh_params", m_refPar["tanhe" ].v_par = boost::assign::list_of<double> (0.173)(0.291)(0.286)(0.0)(0.0)(0.060)(0.0)(0.0) );
  declareProperty( "CaloEcalChi2Tanh_errors", m_refPar["tanhe" ].v_err = boost::assign::list_of<double> (0.009)(0.008)(0.008)(-1.)(-1.)(0.005)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("chi2d",this));
  m_refPar["chi2d"].htitle = "ProtoParticle::CaloTrMatch";
  declareProperty( "CaloTrMatch_histo",       m_refPar["chi2d" ].v_def = boost::assign::list_of<double> (500)(0.)(10.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "CaloTrMatch_params",      m_refPar["chi2d" ].v_par = boost::assign::list_of<double> (1.01)(1.72)(0.0)(0.0)(0.0)(0.0)(0.0)(0.112) );
  declareProperty( "CaloTrMatch_errors",      m_refPar["chi2d" ].v_err = boost::assign::list_of<double> (0.04)(0.04)(-1.)(-1.)(-1.)(-1.)(-1.)(0.008) );

  m_refPar.insert(std::pair<std::string, RefPar>("echi2",this));
  m_refPar["echi2"].htitle = "(E-p)^{2}/(cov(E,E)+cov(p,p))";
  declareProperty( "eChi2_histo",             m_refPar["echi2" ].v_def = boost::assign::list_of<double> (100)(0.)(1000.)(300) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "eChi2_params",            m_refPar["echi2" ].v_par = boost::assign::list_of<double> (92.)(190.)(0.0)(0.0)(0.0)(0.0151)(0.0)(0.0) );
  declareProperty( "eChi2_errors",            m_refPar["echi2" ].v_err = boost::assign::list_of<double> ( 3.)(  4.)(-1.)(-1.)(-1.)(0.0007)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("epull",this));
  m_refPar["epull"].htitle = "(E-p) pull";
  declareProperty( "ePull_histo",             m_refPar["epull" ].v_def = boost::assign::list_of<double> (100)(-10.)(10.)(100)  );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "ePull_params",            m_refPar["epull" ].v_par = boost::assign::list_of<double> (-0.61)(3.77)(0.0)(-0.17)(0.24)(0.0)(0.0)(0.0) );
  declareProperty( "ePull_errors",            m_refPar["epull" ].v_err = boost::assign::list_of<double> ( 0.03)(0.06)(-1.)( 0.05)(0.07)(-1.)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("prse",this));
  m_refPar["prse"].htitle = "ProtoParticle::CaloPrsE";
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "ePrseE_histo",            m_refPar["prse" ].v_def = boost::assign::list_of<double> (100)(0.)(275.)(100)     );
  declareProperty( "ePrseE_params",           m_refPar["prse" ].v_par = boost::assign::list_of<double> (58.)(53.5)(0.0)(0.0)(0.0)(0.0063)(0.0)(0.0) );
  declareProperty( "ePrseE_errors",           m_refPar["prse" ].v_err = boost::assign::list_of<double> ( 3.)( 1.7)(-1.)(-1.)(-1.)(0.0014)(-1.)(-1.) );

  m_refPar.insert(std::pair<std::string, RefPar>("ecalem",this));
  m_refPar["ecalem"].htitle = "ProtoParticle::CaloEcalE (mu)";
  declareProperty( "muCaloEcalE_histo",       m_refPar["ecalem"].v_def = boost::assign::list_of<double> (100)(0.)(5000.)(200) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "muCaloEcalE_params",      m_refPar["ecalem"].v_par = boost::assign::list_of<double> (417.)(651.)(0.470)(0.0)(0.0)(0.0)(0.0)(0.0) ); 
  declareProperty( "muCaloEcalE_errors",      m_refPar["ecalem"].v_err = boost::assign::list_of<double> ( 10.)( 19.)(0.013)(-1.)(-1.)(-1.)(-1.)(-1.) ); 

  m_refPar.insert(std::pair<std::string, RefPar>("hcalem",this));
  m_refPar["hcalem"].htitle = "ProtoParticle::CaloHcalE (mu)";
  declareProperty( "muCaloHcalE_histo",       m_refPar["hcalem"].v_def = boost::assign::list_of<double> (100)(0.)(20000.)(200) );
  // mean, rms, first_bin, skewness, kurtosis, last_bin, underflow, overflow
  declareProperty( "muCaloHcalE_params",      m_refPar["hcalem"].v_par = boost::assign::list_of<double> (2330.)(2330.)(0.062)(0.0)(0.0)(0.0)(0.0)(0.0) ); 
  declareProperty( "muCaloHcalE_errors",      m_refPar["hcalem"].v_err = boost::assign::list_of<double> (  90.)( 150.)(0.009)(-1.)(-1.)(-1.)(-1.)(-1.) ); 

  for (std::map<std::string,RefPar>::iterator it = m_refPar.begin(); it != m_refPar.end(); ++it)
    (*it).second.hname = (*it).first;

  declareProperty( "NSigmaThreshold",         m_nSigmaThreshold = boost::assign::list_of<double> (3.)(7.), "Warning and Error thresholds in sigmas"); 
}
/// initialization @see IAlgTool
StatusCode CaloEMuChecker::initialize() 
{
  StatusCode sc = CaloEMuMonitor::initialize();
  if ( sc.isFailure() ) { return sc ; }
  
  //get tool - for empty inputs() the list will be built up from the containers of the particles for which an association is requested 
  m_track2MCLink = new Object2MCLinker<LHCb::Track>( this, "", "", inputs() );

  m_ppSvc        = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc", true);

  const LHCb::ParticleProperty *jpsi = m_ppSvc->find( "J/psi(1S)" );
  const LHCb::ParticleProperty *ele = m_ppSvc->find( "e-" );
  const LHCb::ParticleProperty *muo = m_ppSvc->find( "mu-" );
  if ( !jpsi || !ele || !muo ){
    error() << "cannot get ParticleProperty for 'J/psi(1S)' or 'e-'" << endmsg;
    return StatusCode::FAILURE;
  }

  m_eID    = ele ->pdgID().abspid();
  m_muID   = muo ->pdgID().abspid();
  m_JPsiID = jpsi->pdgID().abspid();


  return StatusCode::SUCCESS ;
};
/// fill monitoring histograms from the contents of the struct MoniVars m_var 
void CaloEMuChecker::fillMonitoringHistograms()
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
/** Finalize - calculate mean, rms, relative overflow, etc. and
 *  compare with the reference parameters, generate warnins or
 *  error messages if the difference exceeds NSigmaThreshold's  */
StatusCode CaloEMuChecker::finalize()
{
  CaloEMuMonitor::finalize();

  // cleanup
  if( m_track2MCLink ) delete m_track2MCLink;
  m_track2MCLink = NULL;

  return StatusCode::SUCCESS;
}
/** apply PID-related cuts to select/reject muon candidate ProtoParticle's for histogramming
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted  */
bool CaloEMuChecker::acceptMu(const LHCb::ProtoParticle *proto) const
{
  if ( !proto->info( LHCb::ProtoParticle::InAccEcal, false )     ) return false;
  if ( !proto->info( LHCb::ProtoParticle::InAccHcal, false )     ) return false;

  if ( int(abs(static_cast<MoniVarsMC *>(m_var)->mcid)) != int(abs(m_muID)) ) return false;

  return true;
}
/** apply PID-related cuts to select/reject electron candidate ProtoParticle's for histogramming
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted  */
bool CaloEMuChecker::acceptE(const LHCb::ProtoParticle *proto) const
{
  if ( !proto->info( LHCb::ProtoParticle::InAccEcal, false )    ) return false;
  if ( int(abs(static_cast<MoniVarsMC *>(m_var)->mcid)) != int(abs(m_eID)) ) return false;

  return true;
}
/** apply track-quality cuts to select/reject candidate ProtoParticle's for histogramming
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted  */
bool CaloEMuChecker::acceptTrack(const LHCb::ProtoParticle *proto)
{
  if ( !proto ) return false;
  const LHCb::Track* track = proto->track() ;
  if ( !track ) return false;

  if (track->pt()   < m_minPt       ) return false;
  if (track->pt()   > m_maxPt       ) return false;
  if (track->type() < m_minTrackType) return false;

  // fill MCID fields before applying selections
  if ( !mcTruth(proto) ) return false;

  // apply selection functions
  m_var->eCandidate  = acceptE(proto);
  m_var->muCandidate = acceptMu(proto);
  if ( !m_var->eCandidate && !m_var->muCandidate ) return false;

  return true;
}
/** associate MC truth information to ProtoParticle @param proto
 *  sets m_var->mcid    to PDG code of the particle
 *  sets m_var->mothpid to PDG code of the mother particle
 *
 *  @param proto pointer to the ProtoParticle
 *  @return true if particle is accepted according to the MC-truth requirements  */
bool CaloEMuChecker::mcTruth(const LHCb::ProtoParticle *proto) const
{
  const LHCb::Track* track = proto->track() ;
  if ( !track ) return false;

  // get MC truth
  double maxWeight = 0;
  const LHCb::MCParticle* mc = m_track2MCLink->first(track, maxWeight);
  LHCb::MCParticle* mcp = (LHCb::MCParticle*) mc;      
  if( NULL != mc ){
    do{
      double weight =0;
      mc = m_track2MCLink->next(weight);
      if(weight>maxWeight){
        maxWeight=weight;
        mcp = (LHCb::MCParticle*) mc;
      }
    }while(mc !=NULL);
  }
  const LHCb::MCParticle *mother = mcp ? mcp->mother() : 0;


  MoniVarsMC *mvmc = static_cast<MoniVarsMC *>(m_var);
  mvmc->mcid       = mcp    ? mcp->particleID().pid()    : 0;
  mvmc->mothpid    = mother ? mother->particleID().pid() : 0;

  if ( mvmc->mcid == 0 ) return false;
  if ( m_motherJpsi && mvmc->mothpid != m_JPsiID ) return false;

  return true;
}
