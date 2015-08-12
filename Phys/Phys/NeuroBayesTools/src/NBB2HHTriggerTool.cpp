// $Id: $
// Include files
#include <cmath>
#include <algorithm>

#include "Event/RecVertex.h"
#include <Event/MuonPID.h>

//LoKi
#include <LoKi/ParticleCuts.h>

// local
#include "NBB2HHTriggerTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NBB2HHTriggerTool
//
// 2011-02-16 : Ulrich Kerzel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( NBB2HHTriggerTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NBB2HHTriggerTool::NBB2HHTriggerTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
: base_class(type,name,parent),
  m_NetworkCut (       -1.0         ),
  m_DoPrecuts  (       false        ),
  m_UsePID     (       true         ),
  m_DoPlot     (       false        ),
  m_DistCalc   (       NULL         ),
  m_nVar       (       16           ),
  m_HistoTool  (       NULL         ),
  m_inArray    (       NULL         )
{
#ifdef __GNUC__ 
  m_NBExpert = NULL;
#endif
  declareProperty("Expertise"    , m_Expertise        , "NeuroBayes Expertise");
  declareProperty("NetworkCut"   , m_NetworkCut       , "Cut on Network output");
  declareProperty("DoPrecuts"    , m_DoPrecuts        , "Apply pre-cuts");
  declareProperty("UsePID"       , m_UsePID           , "Use PID info");
  declareProperty("DoPlot"       , m_DoPlot           , "Create control histograms");
}

// ===========================================================================
StatusCode NBB2HHTriggerTool::initialize() 
{
  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;

  // get tools and algs
  m_DistCalc  = tool<IDistanceCalculator>("LoKi::DistanceCalculator");

  m_HistoTool = tool<IHistoTool>("HistoTool", this);

  m_nVar = ( m_UsePID ? 16 : 12 );

  //
  // print settings
  //
  verbose() <<  "Expertise      " <<  m_Expertise     << endmsg;
  debug()   <<  "NetworkCut     " <<  m_NetworkCut    << endmsg;
  debug()   <<  "Apply precuts  " <<  m_DoPrecuts     << endmsg;
  debug()   <<  "Use PID        " <<  m_UsePID        << endmsg;
  debug()   <<  "#variables     " <<  m_nVar          << endmsg;

  //
  // setup NeuroBayes
  //
#ifdef __GNUC__

  m_NBExpert = new Expert(&m_Expertise.front());

  // prepare arry of input variables, use max. allowed length
  // (easier for switching between different networks)
  m_inArray = new float[NB_MAXNODE];

#else

  return Error( "Platform not supported for NeuroBayes" );

#endif


  return StatusCode::SUCCESS;
} 

// ============================================================================

bool NBB2HHTriggerTool::operator()(const LHCb::Particle* p) const {

  if(0 == p) {
    Error("LHCb::Particle* points to NULL, return false").ignore();
    return false ;
  } // null

  // safe side: reject everything by default
  bool returnValue = false;

  bool filledInputVar =  NBB2HHTriggerTool::getInputVar(p);

  if (filledInputVar) {

    if ( msgLevel(MSG::VERBOSE) ) {
      for (int i = 0; i< m_nVar; i++) {
        debug() << "input var [" << i << "] = " << m_inArray[  i ] << endmsg;
      } //for i
    } // if msg

    double netOut =  -1.0;
#ifdef __GNUC__
    netOut = m_NBExpert->nb_expert(m_inArray);
#endif

    double prob   = (1.0 + netOut)*0.5;

    if (m_DoPlot) {
      double mB   = p->measuredMass()/Gaudi::Units::GeV;

      m_HistoTool->plot1D(netOut, "BhhNet" , "NeuroBayes Bhh network output ", -1.0  , 1.0 ,  120);
      m_HistoTool->plot1D(mB    , "mBAll"  , "mass all cand"                 ,  5.0  , 6.0 ,  120);
    } //if plot


    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Bhh cand: Network output " << netOut
              << " probability " << prob << endmsg;
    } // if msg

    // accept candidate?
    if (prob > m_NetworkCut) {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Bhh cand pass cut "  << m_NetworkCut << endmsg;

      if (m_DoPlot) {
        double mB   = p->measuredMass()/Gaudi::Units::GeV;
        m_HistoTool->plot1D(mB    , "mBCut"  , "mass acc cand"                 ,  5.0  , 6.0 ,  120);
      } //if plot

      // accept candidate
      returnValue = true;
    } // if prob
  } // if sc




  return returnValue;

} //bool operator

// ============================================================================
bool NBB2HHTriggerTool::getInputVar(const LHCb::Particle* particle) const {

#ifdef __GNUC__
  // get daughters
  const SmartRefVector< LHCb::Particle > &daughters = particle->endVertex()->outgoingParticles();

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
  int    hPlusTrackNDoF  = hPlus ->proto()->track()->nDoF();
  double hPlusTrackChi2  = -999;
  if (hPlusTrackNDoF > 0)
    hPlusTrackChi2  = hPlus ->proto()->track()->chi2()/hPlusTrackNDoF;
  int    hMinusTrackNDoF = hMinus->proto()->track()->nDoF();
  double hMinusTrackChi2 = -999;
  if (hMinusTrackNDoF > 0)
    hMinusTrackChi2 = hMinus->proto()->track()->chi2()/hMinusTrackNDoF;


  double chi2B           = particle->endVertex()->chi2();

  double     docaB    = -999;
  double     docaChi2 = -999;
  StatusCode sc2;
  sc2 = m_DistCalc->distance(hPlus, hMinus, docaB, docaChi2);
  if (sc2 != StatusCode::SUCCESS) {
    docaB     = -999;
    docaChi2  = -999;
  } //if

  double minP            = std::min(hPlusP    , hMinusP);
  //double maxP            = std::max(hPlusP    , hMinusP);

  double minPt           = std::min(hPlusPt    , hMinusPt);
  double maxPt           = std::max(hPlusPt    , hMinusPt);

  //double minDllK         = std::min(hPlusDllK  , hMinusDllK);
  double maxDllK         = std::max(hPlusDllK  , hMinusDllK);


  //
  // appply pre-cuts
  //
  if (m_DoPrecuts) {
    double mB              = particle->measuredMass()/Gaudi::Units::GeV;

    bool hPlusAboveThresholdK  = false;
    bool hMinusAboveThresholdK = false;
    if (hPlus ->proto()->richPID()) {
      hPlusAboveThresholdK   = hPlus ->proto()->richPID()->kaonHypoAboveThres();
    }
    if (hMinus->proto()->richPID()) {
      hMinusAboveThresholdK = hMinus ->proto()->richPID()->kaonHypoAboveThres();
    }


    if (mB               < 5.0      ||
        mB               > 5.8      ||
        hMinusTrackChi2  >  5       ||
        minP             < 10.0     ||
        minPt            <  1.5     ||
        docaB            > 0.10     ||
        chi2B            > 25.0     ||
        maxPt            <  2.0    )
      return false;

    if (m_UsePID) {
      if (!hPlusAboveThresholdK       ||
          !hMinusAboveThresholdK      ||
          (hPlusDllK   > -0.1 && hPlusDllK  < 0.1) ||
          (hMinusDllK  > -0.1 && hMinusDllK < 0.1) ||
          maxDllK          <  0.1)
        return false;
    } // if usePID
    return false;
  } //if doPrecut


  //
  // get remaining variables
  //
  double hPlusIsMuon     = 0;
  double hMinusIsMuon    = 0;
  if (m_UsePID) {
    const LHCb::MuonPID *hPlusMuonPID  = hPlus ->proto()->muonPID();
    const LHCb::MuonPID *hMinusMuonPID = hMinus->proto()->muonPID();
    if (hPlusMuonPID)
      hPlusIsMuon = hPlusMuonPID->IsMuon();
    if (hMinusMuonPID)
      hMinusIsMuon = hMinusMuonPID->IsMuon();
  } // if usePID


  double mErrB           = particle->measuredMassErr()/Gaudi::Units::GeV;
  double ptB             = particle->pt()/Gaudi::Units::GeV;
  double pB              = particle->p() /Gaudi::Units::GeV;


  double cosTheta        = LoKi::Cuts::LV01(particle);
  if ( std::isnan(cosTheta) || !std::isfinite(cosTheta) )
  {
    cosTheta = -999;
  }
  else
  {
    cosTheta = fabs(cosTheta);
  }



  //
  // move delta functions to -999
  //

  if (hPlusDllK > -0.01 && hPlusDllK < 0.01)
    hPlusDllK = -999;
  if (hMinusDllK > -0.01 && hMinusDllK < 0.01)
    hMinusDllK = -999;

  //
  // fill input array
  //
  if (m_UsePID) {
    m_inArray[  0 ] =  hPlusIsMuon;
    m_inArray[  1 ] =  hMinusIsMuon;
    m_inArray[  2 ] =  hPlusDllK;
    m_inArray[  3 ] =  hMinusDllK;
    m_inArray[  4 ] =  minPt;
    m_inArray[  5 ] =  maxPt;
    m_inArray[  6 ] =  hPlusP;
    m_inArray[  7 ] =  hMinusP;
    m_inArray[  8 ] =  chi2B;
    m_inArray[  9 ] =  mErrB;
    m_inArray[ 10 ] =  pB;
    m_inArray[ 11 ] =  docaB;
    m_inArray[ 12 ] =  hPlusTrackChi2;
    m_inArray[ 13 ] =  hMinusTrackChi2;
    m_inArray[ 14 ] =  cosTheta;
    m_inArray[ 15 ] =  ptB;
  } else {
    m_inArray[  0 ] =  minPt;
    m_inArray[  1 ] =  maxPt;
    m_inArray[  2 ] =  hPlusP;
    m_inArray[  3 ] =  hMinusP;
    m_inArray[  4 ] =  chi2B;
    m_inArray[  5 ] =  mErrB;
    m_inArray[  6 ] =  pB;
    m_inArray[  7 ] =  docaB;
    m_inArray[  8 ] =  hPlusTrackChi2;
    m_inArray[  9 ] =  hMinusTrackChi2;
    m_inArray[ 10 ] =  cosTheta;
    m_inArray[ 11 ] =  ptB;
  } // if UsePID

  if (m_DoPlot) {
    m_HistoTool->plot1D(hPlusDllK      , "DllK"      , "Delta LL K" , -20,     20.0, 120);
    m_HistoTool->plot1D(hMinusDllK     , "DllK"      , "Delta LL K" , -20,     20.0, 120);
    m_HistoTool->plot1D(hPlusPt        , "pt"        , "Kaon pt"    ,   0,     10.0, 120);
    m_HistoTool->plot1D(hMinusPt       , "pt"        , "Kaon pt"    ,   0,     10.0, 120);
    m_HistoTool->plot1D(hPlusP         , "p"         , "Kaon p"     ,  10,     50.0, 120);
    m_HistoTool->plot1D(hMinusP        , "p"         , "Kaon p"     ,  10,     50.0, 120);
    m_HistoTool->plot1D(chi2B          , "chi2"      , "B chi2"     ,   0,     25.0, 120);
    m_HistoTool->plot1D(mErrB          , "mErrB"     , "B mErr"     ,   0,      0.1, 120);
    m_HistoTool->plot1D(pB             , "pB"        , "B p"        ,  25,    100.0, 120);
    m_HistoTool->plot1D(docaB          , "doca"      , "doca"       ,   0,      0.1, 120);
    m_HistoTool->plot1D(hPlusTrackChi2 , "trackChi2" , "trackChi2"  ,   0,      5.0, 120);
    m_HistoTool->plot1D(hMinusTrackChi2, "trackChi2" , "trackChi2"  ,   0,      5.0, 120);
    m_HistoTool->plot1D(cosTheta       , "cosTheta"  , "hel angle"  ,  0.0,     1.0, 120);
    m_HistoTool->plot1D(ptB            , "ptB"       , "B pt"       ,   0,     10.0, 120);
  } //if do Plot

#endif

  return true;
} //getInputVar
