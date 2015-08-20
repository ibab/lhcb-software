// $Id: $
// Include files

#include <cmath>
#include <algorithm>

#include "Event/RecVertex.h"
#include <Event/MuonPID.h>

//LoKi
#include <LoKi/ParticleCuts.h>

// local
#include "StrippingNBBhh.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StrippingNBBhh
//
// 2011-01-25 : Ulrich Kerzel
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( StrippingNBBhh )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  StrippingNBBhh::StrippingNBBhh( const std::string& name,
                                  ISvcLocator* pSvcLocator )
    : DaVinciHistoAlgorithm  ( name , pSvcLocator ),
      m_NetworkCut ( -1 ),
      m_PlotHisto  ( false ),
      m_PlotMassMin(  5.0   ),
      m_PlotMassMax(  5.6   ),
      m_PlotNBins  (120     )
{
  declareProperty( "Expertise"     , m_ExpertiseName                );
  declareProperty( "NetworkVersion", m_netVersion  = "TuneMC10"     );
  declareProperty( "NetworkCut"    , m_NetworkCut  =  -1            );
  declareProperty( "PlotHisto"     , m_PlotHisto   =  false         );
  declareProperty( "PlotMassMin"   , m_PlotMassMin =   5.0          );
  declareProperty( "PlotMassMax"   , m_PlotMassMax =   5.6          );
  declareProperty( "PlotNBins"     , m_PlotNBins   = 120            );
} // constructor

//=============================================================================
// Destructor
//=============================================================================
StrippingNBBhh::~StrippingNBBhh() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode StrippingNBBhh::initialize()
{
  StatusCode sc = DaVinciHistoAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  //
  // print settings
  //

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() <<  "Expertise      " <<  m_ExpertiseName << endmsg;
    debug() <<  "NetworkVersion " <<  m_netVersion    << endmsg;
    debug() <<  "NetworkCut     " <<  m_NetworkCut    << endmsg;
    debug() <<  "PlotHisto      " <<  m_PlotHisto     << endmsg;
    debug() <<  "PlotMassMin    " <<  m_PlotMassMin   << endmsg;
    debug() <<  "PlotMassMax    " <<  m_PlotMassMax   << endmsg;
    debug() <<  "PlotNBins      " <<  m_PlotNBins     << endmsg;
  }

  //
  // setup NeuroBayes
  //
#ifdef __GNUC__

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Setup NeuroBayes for Expertise " << m_ExpertiseName << endmsg;

  if ( m_ExpertiseName.empty() )
    return Error( "No NeuroBayes Expertise specified" );

  std::string fullPath;

  // Expertise are in PARAM group of packages
  const std::string paramEnv = "STRIPPINGNEUROBAYESROOT";
  if ( getenv(paramEnv.c_str()) ) {
    //if ( msgLevel(MSG::DEBUG) )
    // debug() << "found environment for Expertise " << paramEnv << endmsg;
    const std::string paramRoot = ( std::string(getenv(paramEnv.c_str())) +
                                    "/expertise/" + m_netVersion + "/" );
    fullPath = paramRoot+m_ExpertiseName;
  } else {
    return Error("PARAM file not found",StatusCode::FAILURE,1);
  } // if paramEnv

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Take Expertise from " << fullPath << endmsg;

  // FPE Guard for NB call
  FPE::Guard guard(true);
  m_NBExpert = new Expert(fullPath.c_str());

  // prepare arry of input variables, use max. allowed length
  // (easier for switching between different networks)
  m_inArray = new float[NB_MAXNODE];

#endif

  return sc;
} //initialise

//=============================================================================
// Main execution
//=============================================================================
StatusCode StrippingNBBhh::execute() {

  if ( msgLevel(MSG::DEBUG) )
    debug() << "==> Execute" << endmsg;


  StatusCode sc = StatusCode::SUCCESS;

#ifdef __GNUC__

  // dont' accept by default
  setFilterPassed(false);

  // get particles - mother candidate
  const LHCb::Particle::ConstVector& cands = this->i_particles();
  const int nCand = cands.size();

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "got # particles from local storage " << nCand << endmsg;
  }

  LHCb::Particle::ConstVector::const_iterator iCand = cands.begin();
  LHCb::Particle::ConstVector::const_iterator iCandEnd   = cands.end();

  for ( ; iCand != iCandEnd; iCand++) {

    bool filledInputVar =  StrippingNBBhh::getInputVar(*(*iCand));

    if (filledInputVar) {

      if ( msgLevel(MSG::DEBUG) ) {
        for (int i = 0; i<23; i++) {
          debug() << "input var [" << i << "] = " << m_inArray[  i ] << endmsg;
        } //for i
      } // if msg

      const double netOut = m_NBExpert->nb_expert(m_inArray);
      const double prob   = (1.0 + netOut)*0.5;

      if ( msgLevel(MSG::DEBUG) ) {
        debug() << "Bhh cand: Network output " << netOut
                << " probability " << prob << endmsg;
      }

      const double mass = (*iCand)->measuredMass()/Gaudi::Units::GeV;

      if (m_PlotHisto) {
        plot1D(netOut, "BhhNet"  , "NeuroBayes Bhh  network output ", -1.0          , 1.0          ,         120);
        plot1D(prob  , "BhhProb" , "NeuroBayes Bhh  network prob "  ,  0.0          , 1.0          ,         120);
        plot1D(mass  , "mAll"    , "mass, all cand"                 ,  m_PlotMassMin, m_PlotMassMax, m_PlotNBins);
      } //if

      // accept candidate?
      if (prob > m_NetworkCut) {
        if ( msgLevel(MSG::DEBUG) )
          debug() << "Bhh cand pass cut "  << m_NetworkCut << endmsg;
        if (m_PlotHisto) {
          plot1D(mass  , "mAcc"        , "mass, acc cand"                         ,   m_PlotMassMin, m_PlotMassMax, m_PlotNBins);
          plot1D(prob  , "BhhProbAcc"  , "NeuroBayes Bhh  network prob acc cand"  ,  0.0           , 1.0          ,         120);
        } //if
        LHCb::Particle* particle = (*iCand)->clone();
        setFilterPassed(true);
        particle->addInfo(LHCb::Particle::LastGlobal +  1, netOut);
        particle->addInfo(LHCb::Particle::LastGlobal +  2, prob);
        this->markNewTree(particle);
      } // if prob
    } // if sc
  } // for iCand

  return sc;

#endif
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode StrippingNBBhh::finalize() {

  if ( msgLevel(MSG::DEBUG) )
    debug() << "==> Finalize" << endmsg;
#ifdef __GNUC__

  if (m_NBExpert)
    delete m_NBExpert;

  if (m_inArray)
    delete[] m_inArray;
#endif

  return DaVinciHistoAlgorithm::finalize();
}

//=============================================================================
bool StrippingNBBhh::getInputVar(const LHCb::Particle& particle) {



#ifdef __GNUC__
  // get daughters
  const SmartRefVector< LHCb::Particle > &daughters = particle.endVertex()->outgoingParticles();

  if (daughters.size() != 2) {
    warning() << "number of daughters at vertex, skip candidate " << daughters.size() << endmsg;
    return false;
  }//if #Kaons

  const LHCb::Particle* hPlus  = NULL;
  const LHCb::Particle* hMinus = NULL;
  SmartRefVector< LHCb::Particle >::const_iterator iDaughter;
  SmartRefVector< LHCb::Particle >::const_iterator iDaughterBegin = daughters.begin();
  SmartRefVector< LHCb::Particle >::const_iterator iDaughterEnd   = daughters.end();

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " h 1 charge " << (*daughters[0]).charge()
              << " h 2 charge " << (*daughters[1]).charge()
              << endmsg;

  for (iDaughter = iDaughterBegin; iDaughter != iDaughterEnd; iDaughter++){
    if ( (*iDaughter)->charge() > 0) {
      hPlus  = *iDaughter;
    } else {
      hMinus = *iDaughter;
    }// if
  }//for iKaon

  // sanity check
  if (!hPlus || !hMinus) {
    warning() << " Daughters obtained from not valid, skip candidate" << endmsg;
    return false;
  }// if


  //
  // get variables needed for pre-cuts
  //
  double hPlusPt         = hPlus->pt()/Gaudi::Units::GeV;
  double hMinusPt        = hMinus->pt()/Gaudi::Units::GeV;
  double hPlusP          = hPlus->p()/Gaudi::Units::GeV;
  double hMinusP         = hMinus->p()/Gaudi::Units::GeV;
  double hPlusDllK       = hPlus ->proto()->info(LHCb::ProtoParticle::CombDLLk,-999);
  double hMinusDllK      = hMinus->proto()->info(LHCb::ProtoParticle::CombDLLk,-999);
  double hPlusTrackChi2  = hPlus ->proto()->track()->probChi2();
  double hMinusTrackChi2 = hMinus->proto()->track()->probChi2();

  // CRJ - Not used and generates gcc 4.6 warnings.
  //   bool hPlusAboveThresholdK  = false;
  //   bool hMinusAboveThresholdK = false;
  //   if (hPlus ->proto()->richPID()) {
  //     hPlusAboveThresholdK   = hPlus ->proto()->richPID()->kaonHypoAboveThres();
  //   }
  //   if (hMinus->proto()->richPID()) {
  //     hMinusAboveThresholdK = hMinus ->proto()->richPID()->kaonHypoAboveThres();
  //   }

  //double mB              = particle.measuredMass()/Gaudi::Units::GeV;
  double chi2B           = particle.endVertex()->chi2();

  double     docaB    = -999;
  double     docaChi2 = -999;
  StatusCode sc2;
  sc2 = distanceCalculator()->distance(hPlus, hMinus, docaB, docaChi2);
  if (sc2 != StatusCode::SUCCESS) {
    docaB     = -999;
    docaChi2  = -999;
  } //if

  //double minP            = std::min(hPlusP    , hMinusP);
  //double maxP            = std::max(hPlusP    , hMinusP);

  double minPt           = std::min(hPlusPt    , hMinusPt);
  double maxPt           = std::max(hPlusPt    , hMinusPt);

  //double minDllK         = std::min(hPlusDllK  , hMinusDllK);
  //double maxDllK         = std::max(hPlusDllK  , hMinusDllK);




  //
  // appply pre-cuts
  //
  //if (mB               < 5.0      ||
  //    mB               > 5.8      ||
  //    !hPlusAboveThresholdK       ||
  //    !hMinusAboveThresholdK      ||
  //    hPlusTrackChi2   >  5       ||
  //    hMinusTrackChi2  >  5       ||
  //    minP             < 10.0     ||
  //    minPt            <  1.5     ||
  //    (hPlusDllK   > -0.1 && hPlusDllK  < 0.1) ||
  //    (hMinusDllK  > -0.1 && hMinusDllK < 0.1) ||
  //    docaB            > 0.10     ||
  //    chi2B            > 25.0     ||
  //    maxPt            <  2.0     ||
  //    maxDllK          <  0.1)
  //  return false;


  //
  // get remaining variables
  //


  double hPlusCaloE      = hPlus ->proto()->info(LHCb::ProtoParticle::CaloEcalE, -999);
  double hMinusCaloE     = hMinus->proto()->info(LHCb::ProtoParticle::CaloEcalE, -999);
  double hPlusDllP       = hPlus ->proto()->info(LHCb::ProtoParticle::CombDLLp,-999);
  double hMinusDllP      = hMinus->proto()->info(LHCb::ProtoParticle::CombDLLp,-999);
  double hPlusDllMu      = hPlus ->proto()->info(LHCb::ProtoParticle::CombDLLmu,-999);
  double hMinusDllMu     = hMinus->proto()->info(LHCb::ProtoParticle::CombDLLmu,-999);
  double hPlusGhost      = hPlus ->proto()->track()->ghostProbability();
  double hMinusGhost     = hMinus->proto()->track()->ghostProbability();

  double hPlusIsMuon     = 0;
  double hMinusIsMuon    = 0;
  const LHCb::MuonPID *hPlusMuonPID  = hPlus ->proto()->muonPID();
  const LHCb::MuonPID *hMinusMuonPID = hMinus->proto()->muonPID();
  if (hPlusMuonPID)
    hPlusIsMuon = hPlusMuonPID->IsMuon();
  if (hMinusMuonPID)
    hMinusIsMuon = hMinusMuonPID->IsMuon();


  double mErrB           = particle.measuredMassErr()/Gaudi::Units::GeV;
  //double ptB             = particle.pt()/Gaudi::Units::GeV;
  double pB              = particle.p()/Gaudi::Units::GeV;


  double cosTheta        = LoKi::Cuts::LV01(&particle);
  if ( std::isnan(cosTheta) || !std::isfinite(cosTheta) ) { cosTheta = -999; }



  //
  // move delta functions to -999
  //

  if (hPlusCaloE > -0.01 && hPlusCaloE < 0.01)
    hPlusCaloE = -999;
  if (hMinusCaloE > -0.01 && hMinusCaloE < 0.01)
    hMinusCaloE = -999;
  if (hPlusCaloE < -999)
    hPlusCaloE = -999;
  if (hMinusCaloE < -999)
    hMinusCaloE = -999;

  if (hPlusDllK > -0.01 && hPlusDllK < 0.01)
    hPlusDllK = -999;
  if (hMinusDllK > -0.01 && hMinusDllK < 0.01)
    hMinusDllK = -999;


  if (hPlusDllP > -0.01 && hPlusDllP < 0.01)
    hPlusDllP = -999;
  if (hMinusDllP > -0.01 && hMinusDllP < 0.01)
    hMinusDllP = -999;


  if (hPlusDllMu > -0.1 && hPlusDllMu < 0.01)
    hPlusDllK = -999;
  if (hMinusDllMu > -0.1 && hMinusDllMu < 0.01)
    hMinusDllMu = -999;

  if (hPlusGhost < 0)
    hPlusGhost = -999;

  if (hMinusGhost < 0)
    hMinusGhost = -999;


  //
  // fill input array
  //
  m_inArray[  0 ] =  hPlusIsMuon;
  m_inArray[  1 ] =  hMinusIsMuon;
  m_inArray[  2 ] =  hPlusDllK;
  m_inArray[  3 ] =  hMinusDllK;
  m_inArray[  4 ] =  hPlusGhost;
  m_inArray[  5 ] =  hMinusGhost;
  m_inArray[  6 ] =  hPlusDllMu;
  m_inArray[  7 ] =  hMinusDllMu;
  m_inArray[  8 ] =  minPt;
  m_inArray[  9 ] =  maxPt;
  m_inArray[ 10 ] =  hPlusP;
  m_inArray[ 11 ] =  hMinusP;
  m_inArray[ 12 ] =  hPlusDllP;
  m_inArray[ 13 ] =  hMinusDllP;
  m_inArray[ 14 ] =  hPlusCaloE;
  m_inArray[ 15 ] =  hMinusCaloE;
  m_inArray[ 16 ] =  chi2B;
  m_inArray[ 17 ] =  mErrB;
  m_inArray[ 18 ] =  pB;
  m_inArray[ 19 ] =  docaB;
  m_inArray[ 20 ] =  hPlusTrackChi2;
  m_inArray[ 21 ] =  hMinusTrackChi2;
  m_inArray[ 22 ] =  fabs(cosTheta);


#endif

  return true;
} //double getInputVar
//=============================================================================
