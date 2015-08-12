// Include files 

#include <math.h>
// from Gaudi
#include "DetDesc/HistoParam.h"
#include <GaudiUtils/Aida2ROOT.h>

// local
#include "CaloPhotonIdAlg.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CaloPhotonIdAlg
//
// 2010-02-27 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloPhotonIdAlg)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloPhotonIdAlg::CaloPhotonIdAlg(const std::string& name,
    ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator), m_tracking(true), m_extrapol(true),
      m_seed(false), m_neig(false), m_type("PhotonID"),
      m_pdfEPrs(true), m_pdfChi2(true), m_pdfSeed(true),
      m_dlnL(true),
      m_isRunnable( true ),
      m_useCondDB(),
      m_conditionName(),
      m_cond(0),
      m_estimator(0),
      m_histo_path("CaloNeutralPIDs/PhotonID/"),

      m_title_Sig_EPrs_10("Signal_Prs_noSpdHit_10"),
      m_title_Sig_EPrs_11("Signal_Prs_noSpdHit_11"),
      m_title_Sig_EPrs_12("Signal_Prs_noSpdHit_12"),
      m_title_Sig_EPrs_15("Signal_Prs_SpdHit_15"),
      m_title_Sig_EPrs_16("Signal_Prs_SpdHit_16"),
      m_title_Sig_EPrs_17("Signal_Prs_SpdHit_17"),

      m_title_Sig_Chi2_20("Signal_Chi2Tk_noSpdHit_20"),
      m_title_Sig_Chi2_21("Signal_Chi2Tk_noSpdHit_21"),
      m_title_Sig_Chi2_22("Signal_Chi2Tk_noSpdHit_22"),
      m_title_Sig_Chi2_25("Signal_Chi2Tk_SpdHit_25"),
      m_title_Sig_Chi2_26("Signal_Chi2Tk_SpdHit_26"),
      m_title_Sig_Chi2_27("Signal_Chi2Tk_SpdHit_27"),

      m_title_Sig_Seed_30("Signal_ESeed_noSpdHit_30"),
      m_title_Sig_Seed_31("Signal_ESeed_noSpdHit_31"),
      m_title_Sig_Seed_32("Signal_ESeed_noSpdHit_32"),
      m_title_Sig_Seed_35("Signal_ESeed_SpdHit_35"),
      m_title_Sig_Seed_36("Signal_ESeed_SpdHit_36"),
      m_title_Sig_Seed_37("Signal_ESeed_SpdHit_37"),

      m_title_Bkg_EPrs_110("Background_Prs_noSpdHit_110"),
      m_title_Bkg_EPrs_111("Background_Prs_noSpdHit_111"),
      m_title_Bkg_EPrs_112("Background_Prs_noSpdHit_112"),
      m_title_Bkg_EPrs_115("Background_Prs_SpdHit_115"),
      m_title_Bkg_EPrs_116( "Background_Prs_SpdHit_116"),
      m_title_Bkg_EPrs_117("Background_Prs_SpdHit_117"),

      m_title_Bkg_Chi2_120("Background_Chi2Tk_noSpdHit_120"),
      m_title_Bkg_Chi2_121("Background_Chi2Tk_noSpdHit_121"),
      m_title_Bkg_Chi2_122("Background_Chi2Tk_noSpdHit_122"),
      m_title_Bkg_Chi2_125("Background_Chi2Tk_SpdHit_125"),
      m_title_Bkg_Chi2_126("Background_Chi2Tk_SpdHit_126"),
      m_title_Bkg_Chi2_127("Background_Chi2Tk_SpdHit_127"),

      m_title_Bkg_Seed_130("Background_ESeed_noSpdHit_130"),
      m_title_Bkg_Seed_131("Background_ESeed_noSpdHit_131"),
      m_title_Bkg_Seed_132("Background_ESeed_noSpdHit_132"),
      m_title_Bkg_Seed_135("Background_ESeed_SpdHit_135"),
      m_title_Bkg_Seed_136("Background_ESeed_SpdHit_136"),
      m_title_Bkg_Seed_137("Background_ESeed_SpdHit_137")

      , m_Sig_EPrs_10(0), m_Sig_EPrs_11(0), m_Sig_EPrs_12(0), m_Sig_EPrs_15(0),
      m_Sig_EPrs_16(0), m_Sig_EPrs_17(0)

      , m_Sig_Chi2_20(0), m_Sig_Chi2_21(0), m_Sig_Chi2_22(0), m_Sig_Chi2_25(0),
      m_Sig_Chi2_26(0), m_Sig_Chi2_27(0)

      , m_Sig_Seed_30(0), m_Sig_Seed_31(0), m_Sig_Seed_32(0), m_Sig_Seed_35(0),
      m_Sig_Seed_36(0), m_Sig_Seed_37(0)

      , m_Bkg_EPrs_110(0), m_Bkg_EPrs_111(0), m_Bkg_EPrs_112(0),
      m_Bkg_EPrs_115(0), m_Bkg_EPrs_116(0), m_Bkg_EPrs_117(0)

      , m_Bkg_Chi2_120(0), m_Bkg_Chi2_121(0), m_Bkg_Chi2_122(0),
      m_Bkg_Chi2_125(0), m_Bkg_Chi2_126(0), m_Bkg_Chi2_127(0)

      , m_Bkg_Seed_130(0), m_Bkg_Seed_131(0), m_Bkg_Seed_132(0),
      m_Bkg_Seed_135(0), m_Bkg_Seed_136(0), m_Bkg_Seed_137(0)

{

  // TODO : split PID estimator (Prs-Spd-Ecal(Chi2, seed/cluster) + add estimator ( E4/E9, Hcal/Ecal, shower-shape,  ...)
  // and let NeutralProtoParticle to combine ...

  declareProperty("Type", m_type = "PhotonID");
  declareProperty("Inputs", m_inputs); // CaloHypo location (default set in initialize)
  declareProperty("Output", m_output); // relation table output location (default set in initialize)

  declareProperty("Tracking", m_tracking);

//  declareProperty("Extrapolation", m_extrapol = true); // Spd/Prs matching
//  declareProperty("AddSeed", m_seed = false);
//  declareProperty("AddNeighbors", m_neig = false);

  declareProperty("PdfEPrs", m_pdfEPrs );
  declareProperty("PdfChi2", m_pdfChi2 );
  declareProperty("PdfSeed", m_pdfSeed );

  declareProperty("DlnL", m_dlnL = true );

  declareProperty("UseCondDB", m_useCondDB = true,
      "get DLLs from CondDB or from a root file via THS");
  declareProperty("ConditionName", m_conditionName = "Conditions/ParticleID/Calo/PhotonID");
}

//=============================================================================
// Destructor
//=============================================================================
CaloPhotonIdAlg::~CaloPhotonIdAlg() {
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloPhotonIdAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if (sc.isFailure())
    return sc; // error printed already by GaudiAlgorithm

  if (msgLevel(MSG::DEBUG))
    debug() << "==> Initialize" << endmsg;

  if (m_dlnL) {
    info() << "PhotonId : Delta Log Likelihood calculation." << endmsg;
  }
  else {
    info() << "PhotonId : Likelihood estimator." << endmsg;
  }
  // Initialize histogram access
  if (m_useCondDB && !existDet<DataObject>(detSvc(), m_conditionName) ){
    warning() << "Initialise: Condition '" << m_conditionName
              << "' not found -- switch to reading the DLLs from THS!" << endmsg; 
    m_useCondDB = false;
  }

  sc = m_useCondDB ? initializeWithCondDB() : initializeWithoutCondDB();

  // context-dependent inputs/output (if not defined via options)
  using namespace LHCb::CaloAlgUtils;
  if (m_type == "MergedID") {
    if (m_inputs.empty())
      m_inputs.push_back(CaloHypoLocation("MergedPi0s", context()));
    if ("" == m_output)
      m_output = CaloIdLocation("MergedID", context());
  }
  else if (m_type == "PhotonID") {
    if (m_inputs.empty())
      m_inputs.push_back(CaloHypoLocation("Photons", context()));
    if ("" == m_output)
      m_output = CaloIdLocation("PhotonID", context());
  } else if(m_type == "PhotonFromMergedID") {
    if(m_inputs.empty())m_inputs.push_back(CaloHypoLocation( "SplitPhotons"    , context() ) );
    if(""==m_output)m_output =  CaloIdLocation( "PhotonFromMergedID" , context() )  ;
  }  

  // Warning : the algorithm settings overwrite the caloHypo2Calo default settings
  m_estimator = tool<ICaloHypoEstimator>("CaloHypoEstimator","CaloHypoEstimator",this);
  std::string seed = m_seed ? "true" : "false";
  std::string line = m_extrapol ? "true" : "false";
  std::string neig = m_neig ? "true" : "false";
  m_estimator->hypo2Calo()->_setProperty("Seed", seed).ignore();
  m_estimator->hypo2Calo()->_setProperty("PhotonLine", line).ignore();
  m_estimator->hypo2Calo()->_setProperty("AddNeighbors", neig).ignore();
  m_estimator->_setProperty("SkipNeutralID","true").ignore(); // avoid recursive loop

  return sc;
}

//=============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
//=============================================================================
StatusCode CaloPhotonIdAlg::execute() {

  if (msgLevel(MSG::DEBUG))
    debug() << "==> Execute" << endmsg;

  // create the table
  typedef LHCb::Relation1D<LHCb::CaloHypo, float> Table;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::IHypoEvalTable));
  Table* table = new Table(200);
  put(table, m_output);

  // fill the table
  for (std::vector<std::string>::iterator iloc = m_inputs.begin(); m_inputs.end()
      != iloc; ++iloc) {
    std::string loc = *iloc;
    if (exist<LHCb::CaloHypos> (loc)) {
      const LHCb::CaloHypos* hypos = get<LHCb::CaloHypos> (loc);
      counter("#Hypos in " + loc) += hypos->size();
      for (LHCb::CaloHypos::const_iterator h = hypos->begin(); hypos->end()
          != h; ++h) {
        const LHCb::CaloHypo* hypo = *h;
        if (NULL == hypo) {
          counter("hypo points to NULL in " + loc) += 1;
          continue;
        }
        double l = m_isRunnable ? likelihood(hypo) : -999.;
        counter("likelihood") += l;
        table->i_push(hypo, (float)(l));
      }
    }
    else {
      if (msgLevel(MSG::DEBUG))
        debug() << "CaloHypo not found at location : " << loc << endmsg;
      counter("Empty location " + loc) += 1;
    }
  }
  table->i_sort();
  // statistics
  counter(Gaudi::Utils::toString(m_inputs) + " ==> " + m_output)
      += table->i_relations().size();
  return StatusCode::SUCCESS;
}

//============================================================================
double CaloPhotonIdAlg::likelihood(const LHCb::CaloHypo* hypo) {
  // Get the relevant information - basic checks
  if (0 == hypo) {
    Warning("CaloHypo points to NULL", StatusCode::SUCCESS).ignore();
    counter("Null hypo") += 1;
    return -999.;
  }

  double energy, et, eSeed,  ePrs;
  int nSpd;
  unsigned int area;
  
  double chi2 = -999.;

  // parameter evaluation
  if (evalParam(hypo,energy,et,eSeed,nSpd,ePrs,area).isFailure())return -999.;

  // get chi2
  chi2=-999.;
  if (m_tracking)chi2 = m_estimator->data(hypo,CaloDataType::ClusterMatch, +999.);
  // evaluate
  return evalLikelihood(energy,et,eSeed,nSpd,ePrs,chi2,area);
}



//***********************************************************
StatusCode CaloPhotonIdAlg::evalParam(const LHCb::CaloHypo* hypo, 
                                      double &energy,
                                      double &et, 
                                      double &eSeed, 
                                      int    &nSpd, 
                                      double &ePrs, 
                                      unsigned int &area) const {

  using namespace CaloDataType;
  energy = m_estimator->data(hypo, HypoE);
  et     = m_estimator->data(hypo, HypoEt);
  eSeed  = m_estimator->data(hypo, E1Hypo) ;
  nSpd   = (int) m_estimator->data(hypo, ToSpdM);
  ePrs   = m_estimator->data(hypo, ToPrsE);
  area   = LHCb::CaloCellID( (LHCb::CaloCellID::ContentType) m_estimator->data(hypo, CellID) ).area();
  if( !m_estimator->status() )return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//***********************************************************
double CaloPhotonIdAlg::evalLikelihood(double energy, double et, double eSeed,
                                       int nSpd, double ePrs, double chi2, unsigned int area) {

  // Initialization
  double estimator;
  double signal, backgr;
  const double epsilon = 1.e-20;
  // SPD Hit

  signal=epsilon;
  backgr=epsilon;

  if (nSpd == 0) {
    signal = CaloPhotonIdAlg::dLL(energy, ePrs, chi2, eSeed,
        CaloPhotonIdAlg::SIGNAL, area);
    if (m_isRunnable){
      backgr = CaloPhotonIdAlg::dLL(energy, ePrs, chi2, eSeed,
                                    CaloPhotonIdAlg::BACKGROUND, area);
    }
  }
  else {
    signal = CaloPhotonIdAlg::dLL(energy, ePrs, chi2, eSeed,
        CaloPhotonIdAlg::SIGNAL_SPD, area);
    if (m_isRunnable){
      backgr = CaloPhotonIdAlg::dLL(energy, ePrs, chi2, eSeed,
                                    CaloPhotonIdAlg::BACKGROUND_SPD, area);
    }
  }

  if (m_dlnL) {
    if (m_isRunnable){
      if (signal < epsilon)
        signal = epsilon;
      if (backgr < epsilon)
        backgr = epsilon;
      estimator = log(signal) - log(backgr);
    }
    else estimator=-999.;
  }
  else {
    if (m_isRunnable){
      estimator = (signal + backgr > 0.) ? signal / (signal + backgr) : -1.;
    }
    else {
      estimator = -999.;
    }
  }




  if (msgLevel(MSG::DEBUG)) {
    debug() << "Photon Candidate :" << endmsg;
    debug() << " -E         =" << energy <<endmsg;
    debug() << " -Et        =" << et << endmsg;
    debug() << " -#Spd hits =" << nSpd << endmsg;
    debug() << " -EPrs      =" << ePrs << endmsg;
    debug() << " -Chi2      =" << chi2 << endmsg;
    debug() << " -ESeed     =" << eSeed << endmsg;
    if (m_dlnL)
      debug() << "      => DlnL     = " << estimator << endmsg;
    else
      debug() << "      => estimator= " << estimator << endmsg;
  }

  return estimator;
}

// ============================================================================
StatusCode CaloPhotonIdAlg::initializeWithCondDB() {
  info() << "init with CondDB, m_conditionName = " << m_conditionName << endmsg;
  try {
    registerCondition(m_conditionName, m_cond, &CaloPhotonIdAlg::i_updateDLL);
  }
  catch (GaudiException &e) {
    fatal() << e << endmsg;
    m_isRunnable=false;
    return StatusCode::FAILURE;
  }

  StatusCode sc = runUpdate(); // load the conditions

  return sc;
}

// ============================================================================
StatusCode CaloPhotonIdAlg::initializeWithoutCondDB() {
  info() << "init w/o CondDB, pdf histos path is " 
         << m_histo_path
         << endmsg;

  m_Sig_EPrs_10 = locateHistoOnDisk(m_title_Sig_EPrs_10);
  m_Sig_EPrs_11 = locateHistoOnDisk(m_title_Sig_EPrs_11);
  m_Sig_EPrs_12 = locateHistoOnDisk(m_title_Sig_EPrs_12);
  m_Sig_EPrs_15 = locateHistoOnDisk(m_title_Sig_EPrs_15);
  m_Sig_EPrs_16 = locateHistoOnDisk(m_title_Sig_EPrs_16);
  m_Sig_EPrs_17 = locateHistoOnDisk(m_title_Sig_EPrs_17);

  m_Sig_Chi2_20 = locateHistoOnDisk(m_title_Sig_Chi2_20);
  m_Sig_Chi2_21 = locateHistoOnDisk(m_title_Sig_Chi2_21);
  m_Sig_Chi2_22 = locateHistoOnDisk(m_title_Sig_Chi2_22);
  m_Sig_Chi2_25 = locateHistoOnDisk(m_title_Sig_Chi2_25);
  m_Sig_Chi2_26 = locateHistoOnDisk(m_title_Sig_Chi2_26);
  m_Sig_Chi2_27 = locateHistoOnDisk(m_title_Sig_Chi2_27);

  m_Sig_Seed_30 = locateHistoOnDisk(m_title_Sig_Seed_30);
  m_Sig_Seed_31 = locateHistoOnDisk(m_title_Sig_Seed_31);
  m_Sig_Seed_32 = locateHistoOnDisk(m_title_Sig_Seed_32);
  m_Sig_Seed_35 = locateHistoOnDisk(m_title_Sig_Seed_35);
  m_Sig_Seed_36 = locateHistoOnDisk(m_title_Sig_Seed_36);
  m_Sig_Seed_37 = locateHistoOnDisk(m_title_Sig_Seed_37);

  m_Bkg_EPrs_110 = locateHistoOnDisk(m_title_Bkg_EPrs_110);
  m_Bkg_EPrs_111 = locateHistoOnDisk(m_title_Bkg_EPrs_111);
  m_Bkg_EPrs_112 = locateHistoOnDisk(m_title_Bkg_EPrs_112);
  m_Bkg_EPrs_115 = locateHistoOnDisk(m_title_Bkg_EPrs_115);
  m_Bkg_EPrs_116 = locateHistoOnDisk(m_title_Bkg_EPrs_116);
  m_Bkg_EPrs_117 = locateHistoOnDisk(m_title_Bkg_EPrs_117);

  m_Bkg_Chi2_120 = locateHistoOnDisk(m_title_Bkg_Chi2_120);
  m_Bkg_Chi2_121 = locateHistoOnDisk(m_title_Bkg_Chi2_121);
  m_Bkg_Chi2_122 = locateHistoOnDisk(m_title_Bkg_Chi2_122);
  m_Bkg_Chi2_125 = locateHistoOnDisk(m_title_Bkg_Chi2_125);
  m_Bkg_Chi2_126 = locateHistoOnDisk(m_title_Bkg_Chi2_126);
  m_Bkg_Chi2_127 = locateHistoOnDisk(m_title_Bkg_Chi2_127);

  m_Bkg_Seed_130 = locateHistoOnDisk(m_title_Bkg_Seed_130);
  m_Bkg_Seed_131 = locateHistoOnDisk(m_title_Bkg_Seed_131);
  m_Bkg_Seed_132 = locateHistoOnDisk(m_title_Bkg_Seed_132);
  m_Bkg_Seed_135 = locateHistoOnDisk(m_title_Bkg_Seed_135);
  m_Bkg_Seed_136 = locateHistoOnDisk(m_title_Bkg_Seed_136);
  m_Bkg_Seed_137 = locateHistoOnDisk(m_title_Bkg_Seed_137);

  return StatusCode::SUCCESS;
}

// ============================================================================
TH2D* CaloPhotonIdAlg::locateHistoOnDisk(std::string histoname) { // locate the histogram
  TH2D *histo = 0;
  if (!histoname.empty()) {
    AIDA::IHistogram2D *aida = get<AIDA::IHistogram2D> (histoSvc(),m_histo_path + histoname);
    if (0==aida) {
      warning()<<"Could not find AIDA::IHistogram2D* "
               <<m_histo_path + histoname<<"."<<endmsg;
      m_isRunnable=false;
      return 0;
    }
    
    histo = Gaudi::Utils::Aida2ROOT::aida2root(aida);
  }
  return histo;
}

// ============================================================================
StatusCode CaloPhotonIdAlg::i_updateDLL() {
  if (msgLevel(MSG::DEBUG)) debug() << "i_updateDLL() called" << endmsg;
  if (!m_cond)
    return StatusCode::FAILURE;

  try {
    m_Sig_EPrs_10 = locateHistoOnDB(m_title_Sig_EPrs_10);
    m_Sig_EPrs_11 = locateHistoOnDB(m_title_Sig_EPrs_11);
    m_Sig_EPrs_12 = locateHistoOnDB(m_title_Sig_EPrs_12);
    m_Sig_EPrs_15 = locateHistoOnDB(m_title_Sig_EPrs_15);
    m_Sig_EPrs_16 = locateHistoOnDB(m_title_Sig_EPrs_16);
    m_Sig_EPrs_17 = locateHistoOnDB(m_title_Sig_EPrs_17);

    m_Sig_Chi2_20 = locateHistoOnDB(m_title_Sig_Chi2_20);
    m_Sig_Chi2_21 = locateHistoOnDB(m_title_Sig_Chi2_21);
    m_Sig_Chi2_22 = locateHistoOnDB(m_title_Sig_Chi2_22);
    m_Sig_Chi2_25 = locateHistoOnDB(m_title_Sig_Chi2_25);
    m_Sig_Chi2_26 = locateHistoOnDB(m_title_Sig_Chi2_26);
    m_Sig_Chi2_27 = locateHistoOnDB(m_title_Sig_Chi2_27);

    m_Sig_Seed_30 = locateHistoOnDB(m_title_Sig_Seed_30);
    m_Sig_Seed_31 = locateHistoOnDB(m_title_Sig_Seed_31);
    m_Sig_Seed_32 = locateHistoOnDB(m_title_Sig_Seed_32);
    m_Sig_Seed_35 = locateHistoOnDB(m_title_Sig_Seed_35);
    m_Sig_Seed_36 = locateHistoOnDB(m_title_Sig_Seed_36);
    m_Sig_Seed_37 = locateHistoOnDB(m_title_Sig_Seed_37);

    m_Bkg_EPrs_110 = locateHistoOnDB(m_title_Bkg_EPrs_110);
    m_Bkg_EPrs_111 = locateHistoOnDB(m_title_Bkg_EPrs_111);
    m_Bkg_EPrs_112 = locateHistoOnDB(m_title_Bkg_EPrs_112);
    m_Bkg_EPrs_115 = locateHistoOnDB(m_title_Bkg_EPrs_115);
    m_Bkg_EPrs_116 = locateHistoOnDB(m_title_Bkg_EPrs_116);
    m_Bkg_EPrs_117 = locateHistoOnDB(m_title_Bkg_EPrs_117);

    m_Bkg_Chi2_120 = locateHistoOnDB(m_title_Bkg_Chi2_120);
    m_Bkg_Chi2_121 = locateHistoOnDB(m_title_Bkg_Chi2_121);
    m_Bkg_Chi2_122 = locateHistoOnDB(m_title_Bkg_Chi2_122);
    m_Bkg_Chi2_125 = locateHistoOnDB(m_title_Bkg_Chi2_125);
    m_Bkg_Chi2_126 = locateHistoOnDB(m_title_Bkg_Chi2_126);
    m_Bkg_Chi2_127 = locateHistoOnDB(m_title_Bkg_Chi2_127);

    m_Bkg_Seed_130 = locateHistoOnDB(m_title_Bkg_Seed_130);
    m_Bkg_Seed_131 = locateHistoOnDB(m_title_Bkg_Seed_131);
    m_Bkg_Seed_132 = locateHistoOnDB(m_title_Bkg_Seed_132);
    m_Bkg_Seed_135 = locateHistoOnDB(m_title_Bkg_Seed_135);
    m_Bkg_Seed_136 = locateHistoOnDB(m_title_Bkg_Seed_136);
    m_Bkg_Seed_137 = locateHistoOnDB(m_title_Bkg_Seed_137);
  }
  catch (GaudiException &exc) {
    fatal() << "DLL update failed! msg ='" << exc << "'" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// ============================================================================
TH2D* CaloPhotonIdAlg::locateHistoOnDB(std::string histoname) { // locate the histogram
  TH2D* histo = 0;
  if (!histoname.empty()) {
    histo = reinterpret_cast<TH2D *> (&m_cond->param<DetDesc::Params::Histo2D> (
            histoname));
  }
  return histo;
}
