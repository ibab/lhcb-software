// Include files
#include "Kernel/HitPattern.h"
#include "Event/FTCluster.h"
#include "Event/Particle.h"
#include "Event/STCluster.h"
#include "Event/Track.h"
#include "Event/KalmanFitResult.h"
#include "Event/VPCluster.h"
#include "Event/GhostTrackInfo.h" 
#include "TMVA/TMVAUp_1_MLP_1.class.C"
#include "TMVA/TMVAUp_3_MLP_3.class.C"
#include "TMVA/TMVAUp_4_MLP_4.class.C"
#include "TMVA/TMVAUp_5_MLP_5.class.C"
#include "TMVA/TMVAUp_6_MLP_6.class.C"


#include "TMath.h"

// local
#include "UpgradeGhostId.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UpgradeGhostId
//
// 2014-12-30 : Paul Seyfert
// following an earlier version by Angelo Di Canto
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( UpgradeGhostId )

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
UpgradeGhostId::UpgradeGhostId( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
: GaudiTool ( type, name, parent )
{
  declareProperty( "Expectors" , m_expectorNames=std::vector<std::string>{"UTHitExpectation","FTHitExpectation"} );
  //declareProperty( "Expectors" , m_expectorNames=std::vector<std::string>({"ITHitExpectation","TTHitExpectation","OTHitExpectation"}) );
  declareInterface<IGhostProbability>(this);

}

//=============================================================================
//

namespace {
 static const int largestChannelIDype = 1+std::max(LHCb::LHCbID::UT,std::max(LHCb::LHCbID::VP,LHCb::LHCbID::FT));
 static const int largestTrackTypes = 1+LHCb::Track::Ttrack;}


StatusCode UpgradeGhostId::finalize()
{

  delete m_expectedHits;
  delete m_readers[LHCb::Track::Velo];
  delete m_readers[LHCb::Track::Upstream];
  delete m_readers[LHCb::Track::Downstream];
  delete m_readers[LHCb::Track::Long];
  delete m_readers[LHCb::Track::Ttrack];
  //IIncidentSvc* incsvc = svc<IIncidentSvc>("IncidentSvc") ;
  //incsvc->removeListener(this, IncidentType::BeginEvent);
  return GaudiTool::finalize();
}

StatusCode UpgradeGhostId::initialize()
{
  if( !GaudiTool::initialize() ) return StatusCode::FAILURE;
  //IIncidentSvc* incsvc = svc<IIncidentSvc>("IncidentSvc") ;
  //incsvc->addListener(this, IncidentType::BeginEvent);

  if (largestTrackTypes<=std::max(LHCb::Track::Ttrack,std::max(
     std::max(LHCb::Track::Velo,LHCb::Track::Upstream),
     std::max(LHCb::Track::Ttrack,LHCb::Track::Downstream))))
    return Warning("ARRAY SIZE SET WRONG (largestTrackTypes is smaller than enum LHCb::Track::Types",StatusCode::FAILURE);

  // expect hit patterns
  //if (true) {
    m_vpExpectation = tool<IVPExpectation>("VPExpectation");
    //m_veloExpectation = NULL;
//  } else {
//    m_vpExpectation = NULL;
//    m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");
//    m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
//  }
  for (auto entry : m_expectorNames) {
    m_Expectations.push_back(tool<IHitExpectation>(entry));
  }
  m_expectedHits = new std::vector<int>(m_expectorNames.size(),0);


  m_readers = std::vector<IClassifierReader*> (largestTrackTypes,NULL);
  std::vector<std::string> names = veloVars();
  m_readers[LHCb::Track::Velo] = new ReadMLP_1(names);
  names = upstreamVars();
  m_readers[LHCb::Track::Upstream] = new ReadMLP_4(names);
  names = downstreamVars();
  m_readers[LHCb::Track::Downstream] = new ReadMLP_5(names);
  names = longVars();
  m_readers[LHCb::Track::Long] = new ReadMLP_3(names);
  names = ttrackVars();
  m_readers[LHCb::Track::Ttrack] = new ReadMLP_6(names);

  return StatusCode::SUCCESS;
}

StatusCode UpgradeGhostId::countHits() {
  const LHCb::VPClusters* vpCont = NULL;
  const LHCb::STClusters* utCont = NULL;
  vpCont = getIfExists<LHCb::VPClusters>(LHCb::VPClusterLocation::Default);
  utCont = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::UTClusters);
  if (vpCont) m_veloHits = vpCont->size();
  if (utCont) m_utHits = utCont->size();
  //const bool run2 = false;
  //const LHCb::VeloLiteCluster::VeloLiteClusters* veloCont = NULL;
  //const LHCb::STClusters* ttCont = NULL;
  //const LHCb::STClusters* itCont = NULL;
  //typedef FastClusterContainer<LHCb::FTRawCluster,int> FTRawClusters;
  //FTRawClusters* ftCont = NULL;
  //if (!run2) {
  //  ftCont = getIfExists<FTRawClusters>( LHCb::FTRawClusterLocation::Default );
  //  if (!ftCont) {
  //    //FTRawClusters* tmp = 
  //    getIfExists<FTRawClusters>("Raw/FT/RawClusters");//needed to create clusters in LHCb::FTRawClusterLocation::Default
  //    ftCont = getIfExists<FTRawClusters>( LHCb::FTRawClusterLocation::Default );
  //  }
  //} else {
  //  veloCont = getIfExists<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default);
  //  ttCont = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
  //  itCont = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
  //}
  //if (veloCont) m_veloHits = veloCont->size();
  //if (ttCont) m_ttHits = ttCont->size();;
  //if (ftCont) m_ftHits = ftCont->size();;
  //if (itCont) m_itHits = itCont->size();;
  //if (run2) m_otHits = m_otdecoder->totalNumberOfHits();

  //  if (!((vpCont && utCont && ftCont) // this is upgrade, all have to be there
  //      || (veloCont && ttCont && itCont && ot) // otherwise we're in runI/II
  //     )) return Warning("detector missing",StatusCode::FAILURE,10);
  if (!((vpCont)&&(utCont))) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

namespace {
  inline std::vector<double> subdetectorhits(const LHCb::Track& aTrack) {

    std::vector<double> returnvalue = std::vector<double>(largestChannelIDype,0.);
    // optimised out:
    //returnvalue[LHCb::LHCbID::TT] = 0.;
    //returnvalue[LHCb::LHCbID::IT] = 0.;
    //returnvalue[LHCb::LHCbID::OT] = 0.;
    returnvalue[LHCb::LHCbID::UT] = 0.;
    returnvalue[LHCb::LHCbID::FT] = 0.;
    returnvalue[LHCb::LHCbID::VP] = 0.;
    for (auto lhcbid : aTrack.lhcbIDs()) {
      if (lhcbid.detectorType()>=returnvalue.size()) {continue;} // may be a hit in a non-tracking detector
      (returnvalue[lhcbid.detectorType()]) += 1.;
    }
    return returnvalue;
  }
}


//=============================================================================
StatusCode UpgradeGhostId::execute(LHCb::Track& aTrack) const
{
  /// code for debuging:
  ///if (true) {
  ///  int veloHits;
  ///  const LHCb::VPClusters* vpCont = NULL;
  ///  vpCont = getIfExists<LHCb::VPClusters>(LHCb::VPClusterLocation::Default);
  ///  if (vpCont) veloHits = vpCont->size();
  ///  if (veloHits != m_veloHits) return Error("This is very bad!!!",StatusCode::FAILURE,10);
  ///}

  std::vector<double> obsarray = subdetectorhits(aTrack);
  const LHCb::KalmanFitResult* fit = static_cast<const LHCb::KalmanFitResult*>(aTrack.fitResult());

  std::vector<double> variables;
  //if (LHCb::Track::Velo == tracktype || LHCb::Track::Long == tracktype || LHCb::Track::Upstream == tracktype) {
  if (aTrack.hasVelo()) {
    variables.push_back(obsarray[LHCb::LHCbID::VP] );
    variables.push_back(m_vpExpectation->nExpected(aTrack));
    variables.push_back(aTrack.info(LHCb::Track::FitVeloChi2, -999));
    variables.push_back(aTrack.info(LHCb::Track::FitVeloNDoF, -999));
  }
  //if (LHCb::Track::Downstream == tracktype || LHCb::Track::Long == tracktype || LHCb::Track::Ttrack == tracktype) {
  if (aTrack.hasT()) {
    variables.push_back(obsarray[LHCb::LHCbID::FT] );
    variables.push_back(m_Expectations[1]->nExpected(aTrack));
    variables.push_back(aTrack.info(LHCb::Track::FitTChi2, -999));
    variables.push_back(aTrack.info(LHCb::Track::FitTNDoF, -999));
  }
  //if (LHCb::Track::Downstream == tracktype || LHCb::Track::Long == tracktype || LHCb::Track::Upstream == tracktype) {
  if (aTrack.hasUT()) { // includes longtracks w/o ut hits
    variables.push_back(obsarray[LHCb::LHCbID::UT] );
    variables.push_back(m_Expectations[0]->nExpected(aTrack));
  }
  if (LHCb::Track::Long == aTrack.type()) {
    variables.push_back(aTrack.info(LHCb::Track::FitMatchChi2, -999));
  }
  if (aTrack.hasUT()) {
    variables.push_back((fit->nMeasurements(LHCb::Measurement::UT) - fit->nActiveMeasurements(LHCb::Measurement::UT)));// "UpgradeGhostInfo_UToutlier",'F'
  }
  variables.push_back(m_veloHits);
  variables.push_back(  m_utHits);
  variables.push_back(aTrack.chi2());
  if (LHCb::Track::Long != aTrack.type() && LHCb::Track::Downstream != aTrack.type()) {
    variables.push_back(aTrack.nDoF());
  }
  if (LHCb::Track::Velo != aTrack.type()) {
    variables.push_back(aTrack.pt());
  }
  variables.push_back(aTrack.pseudoRapidity());

  //float netresponse = m_readers[aTrack.type()]->GetRarity(variables); // TODO rarity would be nice, see https://sft.its.cern.ch/jira/browse/ROOT-7050
  float netresponse = m_readers[aTrack.type()]->GetMvaValue(variables);

  aTrack.setGhostProbability(1.-netresponse);

  return StatusCode::SUCCESS;
}

std::vector<std::string> UpgradeGhostId::veloVars() const {
  std::vector<std::string> retval;
  retval.push_back("UpgradeGhostInfo_obsVP");
  retval.push_back("UpgradeGhostInfo_expVP");
  retval.push_back("UpgradeGhostInfo_FitVeloChi2");
  retval.push_back("UpgradeGhostInfo_FitVeloNDoF");
  retval.push_back("UpgradeGhostInfo_veloHits");
  retval.push_back("UpgradeGhostInfo_utHits");
  retval.push_back("TRACK_CHI2");
  retval.push_back("TRACK_NDOF");
  retval.push_back("TRACK_ETA");
  return retval;
}


std::vector<std::string> UpgradeGhostId::upstreamVars() const {
  std::vector<std::string> retval;
  retval.push_back("UpgradeGhostInfo_obsVP");
  retval.push_back("UpgradeGhostInfo_expVP");
  retval.push_back("UpgradeGhostInfo_FitVeloChi2");
  retval.push_back("UpgradeGhostInfo_FitVeloNDoF");
  retval.push_back("UpgradeGhostInfo_obsUT");
  retval.push_back("UpgradeGhostInfo_expUTHitExpectation");
  retval.push_back("UpgradeGhostInfo_UToutlier");
  retval.push_back("UpgradeGhostInfo_veloHits");
  retval.push_back("UpgradeGhostInfo_utHits");
  retval.push_back("TRACK_CHI2");
  retval.push_back("TRACK_NDOF");
  retval.push_back("TRACK_PT");
  retval.push_back("TRACK_ETA");
  return retval;
}



std::vector<std::string> UpgradeGhostId::downstreamVars() const {
  std::vector<std::string> retval;
  retval.push_back("UpgradeGhostInfo_obsFT");
  retval.push_back("UpgradeGhostInfo_expFTHitExpectation");
  retval.push_back("UpgradeGhostInfo_FitTChi2");
  retval.push_back("UpgradeGhostInfo_FitTNDoF");
  retval.push_back("UpgradeGhostInfo_obsUT");
  retval.push_back("UpgradeGhostInfo_expUTHitExpectation");
  retval.push_back("UpgradeGhostInfo_UToutlier");
  retval.push_back("UpgradeGhostInfo_veloHits");
  retval.push_back("UpgradeGhostInfo_utHits");
  retval.push_back("TRACK_CHI2");
  retval.push_back("TRACK_PT");
  retval.push_back("TRACK_ETA");
  return retval;
}



std::vector<std::string> UpgradeGhostId::longVars() const {
  std::vector<std::string> retval;
  retval.push_back("UpgradeGhostInfo_obsVP");
  retval.push_back("UpgradeGhostInfo_expVP");
  retval.push_back("UpgradeGhostInfo_FitVeloChi2");
  retval.push_back("UpgradeGhostInfo_FitVeloNDoF");
  retval.push_back("UpgradeGhostInfo_obsFT");
  retval.push_back("UpgradeGhostInfo_expFTHitExpectation");
  retval.push_back("UpgradeGhostInfo_FitTChi2");
  retval.push_back("UpgradeGhostInfo_FitTNDoF");
  retval.push_back("UpgradeGhostInfo_obsUT");
  retval.push_back("UpgradeGhostInfo_expUTHitExpectation");
  retval.push_back("UpgradeGhostInfo_FitMatchChi2");
  retval.push_back("UpgradeGhostInfo_UToutlier");
  retval.push_back("UpgradeGhostInfo_veloHits");
  retval.push_back("UpgradeGhostInfo_utHits");
  retval.push_back("TRACK_CHI2");
  retval.push_back("TRACK_PT");
  retval.push_back("TRACK_ETA");
  return retval;
}



std::vector<std::string> UpgradeGhostId::ttrackVars() const {
  std::vector<std::string> retval;
  retval.push_back("UpgradeGhostInfo_obsFT");
  retval.push_back("UpgradeGhostInfo_expFTHitExpectation");
  retval.push_back("UpgradeGhostInfo_FitTChi2");
  retval.push_back("UpgradeGhostInfo_FitTNDoF");
  retval.push_back("UpgradeGhostInfo_veloHits");
  retval.push_back("UpgradeGhostInfo_utHits");
  retval.push_back("TRACK_CHI2");
  retval.push_back("TRACK_NDOF");
  retval.push_back("TRACK_PT");
  retval.push_back("TRACK_ETA");
  return retval;
}
