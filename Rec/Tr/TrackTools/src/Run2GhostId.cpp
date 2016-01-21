// Include files
#include "Kernel/HitPattern.h"
#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/KalmanFitResult.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/GhostTrackInfo.h" 
#include "TMVA/FlattenDownstream.C"
#include "TMVA/FlattenLong.C"
#include "TMVA/FlattenTtrack.C"
#include "TMVA/FlattenUpstream.C"
#include "TMVA/FlattenVelo.C"

#include "Event/RecSummary.h"
class IClassifierReader {

 public:

   // constructor
   IClassifierReader() : fStatusIsClean( true ) {}
   virtual ~IClassifierReader() {}

   // return classifier response
   virtual float GetMvaValue( std::vector<float>& inputValues ) const = 0;

   // returns classifier status
   bool IsStatusClean() const { return fStatusIsClean; }

 protected:

   bool fStatusIsClean;
};


namespace VeloGhostProb {
#include "TMVA/TMVA_1_25nsLL_1d_MLP.class.C"
}
namespace LongGhostProb {
#include "TMVA/TMVA_3_25nsLL_1d_MLP.class.C"
}
namespace UpGhostProb {
#include "TMVA/TMVA_4_25nsLL_1d_MLP.class.C"
}
namespace DownGhostProb {
#include "TMVA/TMVA_5_25nsLL_1d_MLP.class.C"
}
namespace TGhostProb {
#include "TMVA/TMVA_6_25nsLL_1d_MLP.class.C"
}


#include "TMath.h"

// local
#include "Run2GhostId.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Run2GhostId
//
// 2015-02-06 : Paul Seyfert
// following an earlier version by Angelo Di Canto
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( Run2GhostId )

namespace {
 static const int largestChannelIDType = 1+std::max(LHCb::LHCbID::OT,std::max(LHCb::LHCbID::TT,std::max(LHCb::LHCbID::Velo,LHCb::LHCbID::IT)));
 static const int largestTrackTypes = 1+LHCb::Track::Ttrack;}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Run2GhostId::Run2GhostId( const std::string& type,
                                const std::string& name,
                                const IInterface* parent)
: GaudiTool ( type, name, parent ),
  m_varNameMethods( largestTrackTypes, NULL)
{
  //declareProperty( "Expectors" , m_expectorNames=std::vector<std::string>{"UTHitExpectation","FTHitExpectation"} );
  declareProperty( "Expectors" , m_expectorNames=std::vector<std::string>({"TTHitExpectation","ITHitExpectation","OTHitExpectation"}) );
  declareProperty( "InDaVinci" , m_DaVinci = false );
  declareInterface<IGhostProbability>(this);

  m_varNameMethods[LHCb::Track::Velo] =       &Run2GhostId::veloVars;
  m_varNameMethods[LHCb::Track::Long] =       &Run2GhostId::longVars;
  m_varNameMethods[LHCb::Track::Upstream] =   &Run2GhostId::upstreamVars;
  m_varNameMethods[LHCb::Track::Downstream] = &Run2GhostId::downstreamVars;
  m_varNameMethods[LHCb::Track::Ttrack] =     &Run2GhostId::ttrackVars;

}

//=============================================================================
//



StatusCode Run2GhostId::finalize()
{

  delete[] m_vectorsizes;
  delete m_expectedHits;
  delete m_readers[LHCb::Track::Velo];
  delete m_readers[LHCb::Track::Upstream];
  delete m_readers[LHCb::Track::Downstream];
  delete m_readers[LHCb::Track::Long];
  delete m_readers[LHCb::Track::Ttrack];
  delete m_flatters[LHCb::Track::Velo];
  delete m_flatters[LHCb::Track::Upstream];
  delete m_flatters[LHCb::Track::Downstream];
  delete m_flatters[LHCb::Track::Long];
  delete m_flatters[LHCb::Track::Ttrack];
  //IIncidentSvc* incsvc = svc<IIncidentSvc>("IncidentSvc") ;
  //incsvc->removeListener(this, IncidentType::BeginEvent);
  return GaudiTool::finalize();
}

StatusCode Run2GhostId::initialize()
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
    //m_vpExpectation = tool<IVPExpectation>("VPExpectation");
    //m_veloExpectation = NULL;
//  } else {
//    m_vpExpectation = NULL;
    m_otdecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");
    m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
//  }
  for (auto entry : m_expectorNames) {
    m_Expectations.push_back(tool<IHitExpectation>(entry));
  }
  m_expectedHits = new std::vector<int>(m_expectorNames.size(),0);


  m_readers = std::vector<IClassifierReader*> (largestTrackTypes,NULL);
  std::vector<std::string> varnames;
  varnames = variableNames(LHCb::Track::Velo);
  m_readers[LHCb::Track::Velo]       = new VeloGhostProb::ReadMLP(varnames);
  varnames = variableNames(LHCb::Track::Long);
  m_readers[LHCb::Track::Long]       = new LongGhostProb::ReadMLP(varnames);
  varnames = variableNames(LHCb::Track::Upstream);
  m_readers[LHCb::Track::Upstream]   = new UpGhostProb::ReadMLP(varnames);
  varnames = variableNames(LHCb::Track::Downstream);
  m_readers[LHCb::Track::Downstream] = new DownGhostProb::ReadMLP(varnames);
  varnames = variableNames(LHCb::Track::Ttrack);
  m_readers[LHCb::Track::Ttrack]     = new TGhostProb::ReadMLP(varnames);
  m_flatters = std::vector<Rich::TabulatedFunction1D*> (largestTrackTypes,NULL);
  m_flatters[LHCb::Track::Velo] = VeloTable();
  m_flatters[LHCb::Track::Long] = LongTable();
  m_flatters[LHCb::Track::Upstream] = UpstreamTable();
  m_flatters[LHCb::Track::Downstream] = DownstreamTable();
  m_flatters[LHCb::Track::Ttrack] = TtrackTable();

  m_vectorsizes = new int[largestTrackTypes];
  m_vectorsizes[LHCb::Track::Velo] = 11 + 1;
  m_vectorsizes[LHCb::Track::Long] = 21 + 1;
  m_vectorsizes[LHCb::Track::Upstream] = 13 + 1;
  m_vectorsizes[LHCb::Track::Downstream] = 15 + 1;
  m_vectorsizes[LHCb::Track::Ttrack] = 14 + 1;
  return StatusCode::SUCCESS;
}

StatusCode Run2GhostId::countHits() {
  /// only for qmtest
  if (UNLIKELY(m_DaVinci)) {
    const LHCb::RecSummary * rS =
      getIfExists<LHCb::RecSummary>(evtSvc(),LHCb::RecSummaryLocation::Default);
    if ( !rS )
    {
      rS = getIfExists<LHCb::RecSummary>(evtSvc(),LHCb::RecSummaryLocation::Default,false);
    }
    if ( !rS )
    {
      rS = getIfExists<LHCb::RecSummary>(evtSvc(),"/Event/Turbo/Rec/Summary",false);
    }
    if ( !rS )
    {
      return StatusCode::FAILURE;
    }

    m_otHits = rS->info(LHCb::RecSummary::nOTClusters,-1);
    m_veloHits =  rS->info(LHCb::RecSummary::nVeloClusters,-1);
    m_itHits = rS->info(LHCb::RecSummary::nITClusters,-1);
    m_ttHits = rS->info(LHCb::RecSummary::nTTClusters,-1);
  } else {
  //const LHCb::VPClusters* vpCont = NULL;
  //const LHCb::STClusters* utCont = NULL;
  //vpCont = getIfExists<LHCb::VPClusters>(LHCb::VPClusterLocation::Default);
  //utCont = getIfExists<LHCb::STClusters>(LHCb::STClusterLocation::UTClusters);
  //if (vpCont) m_veloHits = vpCont->size();
  //if (utCont) m_utHits = utCont->size();
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
  auto veloCont = getIfExists<LHCb::VeloLiteCluster::VeloLiteClusters>(LHCb::VeloLiteClusterLocation::Default);
  auto ttCont = getIfExists<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::TTClusters);
  auto itCont = getIfExists<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::ITClusters);
  //}
  if (veloCont) m_veloHits = veloCont->size();
  if (ttCont) m_ttHits = ttCont->size();;
  //if (ftCont) m_ftHits = ftCont->size();;
  if (itCont) m_itHits = itCont->size();;
  m_otHits = m_otdecoder->totalNumberOfHits();
    if (!((veloCont)&&(itCont)&&(ttCont))) {
      return StatusCode::FAILURE;
    }

  }

  //  if (!((vpCont && utCont && ftCont) // this is upgrade, all have to be there
  //      || (veloCont && ttCont && itCont && ot) // otherwise we're in runI/II
  //     )) return Warning("detector missing",StatusCode::FAILURE,10);
  return StatusCode::SUCCESS;

}

namespace {
  inline std::vector<float> subdetectorhits(const LHCb::Track& aTrack) {

    std::vector<float> returnvalue = std::vector<float>(largestChannelIDType,0.);
    // optimised out:
    returnvalue[LHCb::LHCbID::Velo] = 0.;
    returnvalue[LHCb::LHCbID::TT] = 0.;
    returnvalue[LHCb::LHCbID::IT] = 0.;
    returnvalue[LHCb::LHCbID::OT] = 0.;
    //returnvalue[LHCb::LHCbID::UT] = 0.;
    //returnvalue[LHCb::LHCbID::FT] = 0.;
    //returnvalue[LHCb::LHCbID::VP] = 0.;
    for (auto lhcbid : aTrack.lhcbIDs()) {
      if (lhcbid.detectorType()>=returnvalue.size()) {continue;} // may be a hit in a non-tracking detector
      (returnvalue[lhcbid.detectorType()]) += 1.;
    }
    return returnvalue;
  }
}


//=============================================================================
StatusCode Run2GhostId::execute(LHCb::Track& aTrack) const
{
  std::vector<float> variables(netInputs(aTrack));
  //float netresponse = m_readers[aTrack.type()]->GetRarity(variables); // TODO rarity would be nice, see https://sft.its.cern.ch/jira/browse/ROOT-7050
  float netresponse = m_readers[aTrack.type()]->GetMvaValue(variables);
  netresponse = m_flatters[aTrack.type()]->value(0.5*(1.-netresponse));
  aTrack.setGhostProbability(netresponse);

  return StatusCode::SUCCESS;
}


std::vector<float> Run2GhostId::netInputs(LHCb::Track& aTrack) const
{
  /// code for debuging:
  ///if (true) {
  ///  int veloHits;
  ///  const LHCb::VPClusters* vpCont = NULL;
  ///  vpCont = getIfExists<LHCb::VPClusters>(LHCb::VPClusterLocation::Default);
  ///  if (vpCont) veloHits = vpCont->size();
  ///  if (veloHits != m_veloHits) return Error("This is very bad!!!",StatusCode::FAILURE,10);
  ///}

  std::vector<float> obsarray = subdetectorhits(aTrack);
  const LHCb::KalmanFitResult* fit = static_cast<const LHCb::KalmanFitResult*>(aTrack.fitResult());

  std::vector<float> variables;
  variables.reserve(m_vectorsizes[aTrack.type()]);
  //if (LHCb::Track::Velo == tracktype || LHCb::Track::Long == tracktype || LHCb::Track::Upstream == tracktype) {
  if (aTrack.hasVelo()) {
    variables.push_back(obsarray[LHCb::LHCbID::Velo] );
    //variables.push_back(m_veloExpectation->nExpected(aTrack));
    variables.push_back(aTrack.info(LHCb::Track::FitVeloChi2, -999));
    variables.push_back(aTrack.info(LHCb::Track::FitVeloNDoF, -999));
  }
  //if (LHCb::Track::Downstream == tracktype || LHCb::Track::Long == tracktype || LHCb::Track::Upstream == tracktype) {
  if (aTrack.hasTT()) { // includes longtracks w/o ut hits
    variables.push_back(obsarray[LHCb::LHCbID::TT] );
    //variables.push_back(m_Expectations[0]->nExpected(aTrack));
    variables.push_back((fit->nMeasurements(LHCb::Measurement::TTLite) - fit->nActiveMeasurements(LHCb::Measurement::TTLite)));
  }
  //if (LHCb::Track::Downstream == tracktype || LHCb::Track::Long == tracktype || LHCb::Track::Ttrack == tracktype) {
  if (aTrack.hasT()) {
    variables.push_back(obsarray[LHCb::LHCbID::IT] );
    variables.push_back(obsarray[LHCb::LHCbID::OT] );
    //variables.push_back(m_Expectations[1]->nExpected(aTrack)); // IT
    //variables.push_back(m_Expectations[2]->nExpected(aTrack)); // OT
    variables.push_back((fit->nMeasurements(LHCb::Measurement::ITLite) - fit->nActiveMeasurements(LHCb::Measurement::ITLite)));
    float nMeas = fit->nMeasurements( LHCb::Measurement::OT );
    float nOTBad = nMeas-nMeas*aTrack.info(25 , 0 );// info 25 : FitFracUsedOTTimes
    variables.push_back(nOTBad);
    variables.push_back(aTrack.info(LHCb::Track::FitTChi2, -999));
    variables.push_back(aTrack.info(LHCb::Track::FitTNDoF, -999));
  }
  variables.push_back(m_veloHits);
  variables.push_back(  m_ttHits);
  variables.push_back(  m_itHits);
  variables.push_back(  m_otHits);
    variables.push_back(aTrack.pt());
  //if (LHCb::Track::Long != aTrack.type() && LHCb::Track::Downstream != aTrack.type()) {
    variables.push_back(aTrack.chi2());
    if (LHCb::Track::Downstream != aTrack.type()) {
      variables.push_back(aTrack.nDoF());
    }
    variables.push_back(aTrack.chi2PerDoF());
  //}

  if (LHCb::Track::Long == aTrack.type()) {
    variables.push_back(aTrack.info(LHCb::Track::NCandCommonHits, -999));
    variables.push_back(aTrack.info(LHCb::Track::FitMatchChi2, -999));
  }
  variables.push_back(1.f);
  return variables;
}

std::vector<std::string> Run2GhostId::variableNames(LHCb::Track::Types type) const {
  return ((*this).*(m_varNameMethods[type]))();
}


std::vector<std::string> Run2GhostId::veloVars() const {
  std::vector<std::string> retval;
 retval.push_back("tracks_OldImplementation_obsVELO");
 retval.push_back("tracks_TrackExtraInfo_FitVeloChi2");
 retval.push_back("tracks_TrackExtraInfo_FitVeloNDoF");
 retval.push_back("tracks_OldImplementation_veloHits");
 retval.push_back("tracks_OldImplementation_ttHits");
 retval.push_back("tracks_OldImplementation_itHits");
 retval.push_back("tracks_OldImplementation_otHits");
 retval.push_back("tracks_PT");
 retval.push_back("tracks_TRACK_CHI2");
 retval.push_back("tracks_TRACK_NDOF");
 retval.push_back("tracks_TRACK_CHI2NDOF");

  return retval;
}


std::vector<std::string> Run2GhostId::upstreamVars() const {
  std::vector<std::string> retval;
  retval.push_back("tracks_OldImplementation_obsVELO");
  retval.push_back("tracks_TrackExtraInfo_FitVeloChi2");
  retval.push_back("tracks_TrackExtraInfo_FitVeloNDoF");
  retval.push_back("tracks_OldImplementation_obsTT");
  retval.push_back("tracks_OldImplementation_TToutlier");
  retval.push_back("tracks_OldImplementation_veloHits");
  retval.push_back("tracks_OldImplementation_ttHits");
  retval.push_back("tracks_OldImplementation_itHits");
  retval.push_back("tracks_OldImplementation_otHits");
  retval.push_back("tracks_PT");
  retval.push_back("tracks_TRACK_CHI2");
  retval.push_back("tracks_TRACK_NDOF");
  retval.push_back("tracks_TRACK_CHI2NDOF");
  return retval;
}



std::vector<std::string> Run2GhostId::downstreamVars() const {
  std::vector<std::string> retval;
  retval.push_back("tracks_OldImplementation_obsTT");
  retval.push_back("tracks_OldImplementation_TToutlier");
  retval.push_back("tracks_OldImplementation_obsIT");
  retval.push_back("tracks_OldImplementation_obsOT");
  retval.push_back("tracks_OldImplementation_IToutlier");
  retval.push_back("tracks_OldImplementation_OTunused");
  retval.push_back("tracks_TrackExtraInfo_FitTChi2");
  retval.push_back("tracks_TrackExtraInfo_FitTNDoF");
  retval.push_back("tracks_OldImplementation_veloHits");
  retval.push_back("tracks_OldImplementation_ttHits");
  retval.push_back("tracks_OldImplementation_itHits");
  retval.push_back("tracks_OldImplementation_otHits");
  retval.push_back("tracks_PT");
  retval.push_back("tracks_TRACK_CHI2");
  retval.push_back("tracks_TRACK_CHI2NDOF");
  return retval;
}



std::vector<std::string> Run2GhostId::longVars() const {
  std::vector<std::string> retval;
  retval.push_back("tracks_OldImplementation_obsVELO");
  retval.push_back("tracks_TrackExtraInfo_FitVeloChi2");
  retval.push_back("tracks_TrackExtraInfo_FitVeloNDoF");
  retval.push_back("tracks_OldImplementation_obsTT");
  retval.push_back("tracks_OldImplementation_TToutlier");
  retval.push_back("tracks_OldImplementation_obsIT");
  retval.push_back("tracks_OldImplementation_obsOT");
  retval.push_back("tracks_OldImplementation_IToutlier");
  retval.push_back("tracks_OldImplementation_OTunused");
  retval.push_back("tracks_TrackExtraInfo_FitTChi2");
  retval.push_back("tracks_TrackExtraInfo_FitTNDoF");
  retval.push_back("tracks_OldImplementation_veloHits");
  retval.push_back("tracks_OldImplementation_ttHits");
  retval.push_back("tracks_OldImplementation_itHits");
  retval.push_back("tracks_OldImplementation_otHits");
  retval.push_back("tracks_PT");
  retval.push_back("tracks_TRACK_CHI2");
  retval.push_back("tracks_TRACK_NDOF");
  retval.push_back("tracks_TRACK_CHI2NDOF");
  retval.push_back("tracks_TrackExtraInfo_NCandCommonHits");
  retval.push_back("tracks_TrackExtraInfo_FitMatchChi2");
  return retval;
}



std::vector<std::string> Run2GhostId::ttrackVars() const {
  std::vector<std::string> retval;
  retval.push_back("tracks_OldImplementation_obsIT");
  retval.push_back("tracks_OldImplementation_obsOT");
  retval.push_back("tracks_OldImplementation_IToutlier");
  retval.push_back("tracks_OldImplementation_OTunused");
  retval.push_back("tracks_TrackExtraInfo_FitTChi2");
  retval.push_back("tracks_TrackExtraInfo_FitTNDoF");
  retval.push_back("tracks_OldImplementation_veloHits");
  retval.push_back("tracks_OldImplementation_ttHits");
  retval.push_back("tracks_OldImplementation_itHits");
  retval.push_back("tracks_OldImplementation_otHits");
  retval.push_back("tracks_PT");
  retval.push_back("tracks_TRACK_CHI2");
  retval.push_back("tracks_TRACK_NDOF");
  retval.push_back("tracks_TRACK_CHI2NDOF");
  return retval;
}
