#include "GaudiAlg/GaudiHistoAlg.h"

#include "GaudiKernel/ToolHandle.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include "Kernel/ITrajPoca.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "TrackInterfaces/IPitchResTool.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <map>

const double cellRadius = 2.5;



class OTTrackMonitor: public GaudiHistoAlg
{
  static const int HIST_DRIFTTIME = 0;
  static const int HIST_DRIFTRADIUS = 1;
  static const int HIST_TRACK_DISTANCE = 2;
  static const int HIST_DRIFTTIME_RESIDUAL = 3;
  static const int HIST_RESIDUAL = 4;
  static const int HIST_RESIDUAL_PULL = 5;
  static const int HIST_GOOD_DRIFTTIME = 8 + 0;
  static const int HIST_GOOD_DRIFTRADIUS = 8 + 1;
  static const int HIST_GOOD_TRACK_DISTANCE = 8 + 2;
  static const int HIST_GOOD_DRIFTTIME_RESIDUAL = 8 + 3;
  static const int HIST_GOOD_RESIDUAL = 8 + 4;
  static const int HIST_GOOD_RESIDUAL_PULL = 8 + 5;


private:
  AIDA::IHistogram1D* hists[12][16];
  AIDA::IHistogram2D* histXY[12];
  AIDA::IHistogram2D* histRT[12];
  AIDA::IProfile1D* histAverageTimeResidualVsY[12] ;

  AIDA::IProfile1D* profileTimeResidualVsModule;
  AIDA::IProfile1D* profileResidualVsModule;
  AIDA::IProfile1D* profileResidualPullVsModule;

  AIDA::IProfile1D* profileTimeResidualVsDistance;
  AIDA::IProfile1D* profileResidualVsDistance;
  AIDA::IProfile1D* profileResidualPullVsDistance;

  AIDA::IProfile1D* profileTimeResidualVsDistanceGood;
  AIDA::IProfile1D* profileTimeResidualVsModuleGood;
  AIDA::IProfile1D* profileResidualVsDistanceGood;
  AIDA::IProfile1D* profileResidualPullVsDistanceGood;

  AIDA::IHistogram1D* histModuleHitOccupancy;
  AIDA::IHistogram1D* histModuleHotOccupancy;
  AIDA::IHistogram1D* histModuleOutlierOccupancy;
  AIDA::IHistogram1D* histOtisHitOccupancy;
  AIDA::IHistogram1D* histOtisHotOccupancy;

  AIDA::IHistogram1D* histAverageTimeResidual;
  AIDA::IHistogram2D* histAverageTimeResidualVsMomentum;
  AIDA::IHistogram1D* histDeltaToF;
  AIDA::IHistogram1D* m_hotMultiplicity ;
  AIDA::IHistogram1D* m_hitMultiplicity ;
  AIDA::IHistogram1D* m_driftTimeUse ;

  AIDA::IHistogram1D* histEventAverageTimeResidual;


 

public:
  /** Standard construtor */
  OTTrackMonitor(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~OTTrackMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();
private:
  void bookHists(int index, const std::string& prefix);
  void plotHist1D( AIDA::IHistogram1D* hist, double value, double weight);
  void plotHist2D( AIDA::IHistogram2D* hist, double x, double y, double weight);
  void plotProf1D(AIDA::IProfile1D* hist, double x, double y, double weight);
  void setNormalization(AIDA::IHistogram1D* hist);
  void initPointers(int index);
  enum
  {
    NumUniqueStation = 3,
    NumUniqueLayer = 12,
    NumUniqueModule = 432,
    NumUniqueOtis = 432 * 4
  };

  // this should be moved to OTChannelID
  inline int uniqueLayer(const LHCb::OTChannelID& channelID)
  {
    return (channelID.station() - 1) * 4 + channelID.layer();
  }

  inline int uniqueQuarter(const LHCb::OTChannelID& channelID)
  {
    return uniqueLayer(channelID) * 4 + channelID.quarter();
  }

  inline int uniqueModule(const LHCb::OTChannelID& channelID)
  {
    return uniqueQuarter(channelID) * 9 + channelID.module() - 1;
  }

  inline int uniqueOtis( const LHCb::OTChannelID& channelID)
  {
    return uniqueModule(channelID) * 4 + (channelID.straw() - 1) / 32;
  }

private:
  const ITrajPoca* m_pocatool;
  ToolHandle<ITrackProjector> m_projector;
  ToolHandle<IOTRawBankDecoder> m_decoder;
  const IPitchResTool* m_pitchtool;
  const DeOTDetector* m_otdet;
  std::string m_trackLocation;
  double m_maxUnbiasedChisqPerDofGoodTracks;
  int m_granularity;
  size_t m_numEvents;
  bool m_isOnline;
 

};

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTTrackMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTTrackMonitor::OTTrackMonitor(const std::string& name, ISvcLocator* pSvcLocator): 
  GaudiHistoAlg( name , pSvcLocator ),
  profileResidualPullVsModule(NULL),
  profileTimeResidualVsDistance(NULL),
  profileResidualVsDistance(NULL),
  profileResidualPullVsDistance(NULL),
  profileTimeResidualVsDistanceGood(NULL),
  profileTimeResidualVsModuleGood(NULL),
  profileResidualVsDistanceGood(NULL),
  profileResidualPullVsDistanceGood(NULL),
  histModuleOutlierOccupancy(NULL),
  histOtisHitOccupancy(NULL),
  histOtisHotOccupancy(NULL),
  histAverageTimeResidual(NULL),
  histAverageTimeResidualVsMomentum(NULL),
  histDeltaToF(NULL),
  m_driftTimeUse(NULL),
  histEventAverageTimeResidual(NULL),
  m_projector("TrajOTProjector"),
  m_decoder("OTRawBankDecoder"),
  m_numEvents(0)
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default );
  declareProperty( "Projector", m_projector ) ;
  declareProperty( "MaxUnbiasedChisqPerDofGoodTracks", m_maxUnbiasedChisqPerDofGoodTracks = 2 ) ;
  declareProperty( "Granularity", m_granularity = 1 ) ;
  declareProperty( "RawBankDecoder", m_decoder ) ;
  declareProperty( "Online", m_isOnline = false );
  
}

//=============================================================================
// Destructor
//=============================================================================
OTTrackMonitor::~OTTrackMonitor()
{
}

void OTTrackMonitor::bookHists(int index, const std::string& prefix)
{
 
  hists[index][HIST_DRIFTTIME] = book(prefix + "drifttime", "drifttime", -25, 75);
  hists[index][HIST_RESIDUAL] = book(prefix + "residual", "residual (rms unbiased)", -2, 2);
  histXY[index] = book2D(prefix + "xyh2", "position of track at hit",
                         -3000, 3000, 20, -3000, 3000, 20);
  histRT[index] = book2D(prefix + "rt", "drifttime versus unbiased distance",
                         -cellRadius, cellRadius, 50, -25, 75, 50);
  hists[index][HIST_GOOD_DRIFTTIME_RESIDUAL] = book(prefix +
                                                    "drifttimeresidualgood",
                                                    "drifttime residual (rms unbiased, good tracks)",
                                                    -20, 20);
  
  if(!m_isOnline){
    hists[index][HIST_DRIFTRADIUS] = book(prefix + "driftradius", "driftradius", 0, 5);
    hists[index][HIST_TRACK_DISTANCE] = book(prefix + "trkdist", "unbiased distance", -5, 5);
    hists[index][HIST_DRIFTTIME_RESIDUAL] = book(prefix + "drifttimeresidual", "drifttime residual (rms unbiased)", -20, 20);
    
    hists[index][HIST_RESIDUAL_PULL] = book(prefix + "residualpull", "residual pull", -5, 5);
    
    hists[index][HIST_GOOD_DRIFTTIME] = book(prefix + "drifttimegood", "drifttime (good tracks)", -25, 75);
    hists[index][HIST_GOOD_DRIFTRADIUS] = book(prefix + "driftradiusgood", "driftradius (good tracks)", 0, 5);
    hists[index][HIST_GOOD_TRACK_DISTANCE] = book(prefix + "trkdistgood", "unbiased distance (good tracks)", -5, 5);
    hists[index][HIST_GOOD_RESIDUAL] = book(prefix + "residualgood", "residual (rms unbiased, good tracks)", -2, 2);
    hists[index][HIST_GOOD_RESIDUAL_PULL] = book(prefix + "residualpullgood", "residual pull (good tracks)", -5, 5);
    
    
    
    histAverageTimeResidualVsY[index] = bookProfile1D(prefix + "avtimeresvsy", "average time residual versus y", 
                                                      0, 2000, 40) ;
  }
  
   
}

void OTTrackMonitor::initPointers(int index) {
  hists[index][HIST_DRIFTTIME] = NULL;
  hists[index][HIST_RESIDUAL] = NULL;
  histXY[index] = NULL;
  histRT[index] = NULL;
  
  hists[index][HIST_DRIFTRADIUS] = NULL;
  hists[index][HIST_TRACK_DISTANCE] = NULL;
  hists[index][HIST_DRIFTTIME_RESIDUAL] = NULL;
  
  hists[index][HIST_RESIDUAL_PULL] = NULL;
  
  hists[index][HIST_GOOD_DRIFTTIME] = NULL;
  hists[index][HIST_GOOD_DRIFTRADIUS] = NULL;
  hists[index][HIST_GOOD_TRACK_DISTANCE] = NULL;
  hists[index][HIST_GOOD_DRIFTTIME_RESIDUAL] = NULL;
  hists[index][HIST_GOOD_RESIDUAL] = NULL;
  hists[index][HIST_GOOD_RESIDUAL_PULL] = NULL;
  histAverageTimeResidualVsY[index] = NULL;

}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTTrackMonitor::initialize()
{
  if(msgLevel(MSG::DEBUG)) debug() << "------------ TrackMonitor::initialize() / start -------" << endmsg;

  StatusCode statusCode = GaudiHistoAlg::initialize(); // must be executed first

  m_pocatool = tool<ITrajPoca>("TrajPoca");
  m_projector.retrieve().ignore();
  m_decoder.retrieve().ignore();
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  m_pitchtool = tool<IPitchResTool>("PitchResTool");

  // initialize *all* pointers to NULL:
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++)
    initPointers(4 * s + l);
  
  setHistoTopDir("OT/");

  switch(m_granularity)
  {
  case 1:
    for(int s = 0; s < 3; s++)
    {
      bookHists(s, "station" + boost::lexical_cast<std::string>(s + 1) + "/");
    }
    break;
  case 2:
    for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++)
    {
      bookHists(4 * s + l, "layer" + boost::lexical_cast<std::string>(4 * s + l) + "/");
    }
    break;
  }

  
  const int numDistBins = 25;
 
  profileTimeResidualVsModule = bookProfile1D("timeresvsmodule", "unbiased drifttime residual versus module",
                                              -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);
  profileResidualVsModule = bookProfile1D("resvsmodule", "unbiased residual versus module",
                                          -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);
  m_hitMultiplicity = book("hitmultiplicity","hit multiplicity", 0, 15000, 400) ;
  m_hotMultiplicity = book("hotmultiplicity","hot multiplicity", 0, 15000, 400) ; 
  histModuleHitOccupancy = book("moduleHitOccupancy", "number of hits per module",
                                -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);
  histModuleHotOccupancy = book("moduleHotOccupancy", "number of HOTs per module",
                                -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);
  
  if(!m_isOnline){
    profileTimeResidualVsModuleGood = bookProfile1D("timeresvsmodulegood", "unbiased drifttime resid// ual versus module",
                                                    -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);
    
    profileResidualPullVsModule = bookProfile1D("respullvsmodule", "residual pull versus module",
                                                -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);
    
    profileTimeResidualVsDistance = bookProfile1D("timeresvsdistance", "unbiased drifttime residual versus distance",
                                                  0, cellRadius, numDistBins);
    profileResidualVsDistance = bookProfile1D("resvsdistance", "unbiased residual versus distance",
                                              0, cellRadius, numDistBins);
    profileResidualPullVsDistance = bookProfile1D("respullvsdistance", "residual pull versus distance",
                                                  0, cellRadius, numDistBins);
    
    profileTimeResidualVsDistanceGood = bookProfile1D("timeresvsdistancegood", 
                                                      "unbiased drifttime residual versus distance (good tracks)",
                                                      0, cellRadius, numDistBins);
  profileResidualVsDistanceGood = bookProfile1D("resvsdistancegood", "unbiased residual versus distance (good tracks)",
                                                  0, cellRadius, numDistBins);
    profileResidualPullVsDistanceGood = bookProfile1D("respullvsdistancegood", "residual pull versus distance (good tracks)",
                                                      0, cellRadius, numDistBins);

    histModuleOutlierOccupancy = book("moduleOutlierOccupancy", "number of outliers per module",
                                      -0.5, 1.0 * NumUniqueModule - 0.5, NumUniqueModule);

    histOtisHitOccupancy = book("otisHitOccupancy", "number of hits per otis",
                                -0.5, 1.0 * NumUniqueOtis - 0.5, NumUniqueOtis);

    histOtisHotOccupancy = book("otisHotOccupancy", "number of HOTs per otis",
                                -0.5, 1.0 * NumUniqueOtis - 0.5, NumUniqueOtis);
    histAverageTimeResidual = book("avtimeres", "track t0", -10, 10);
    histAverageTimeResidualVsMomentum = book2D("avtimeresvsmom", "average time residual versus momentum",
                                               0, 20, 40, -5, 5, 50) ;
    histDeltaToF = book("deltaToF", "delta time of flight",
                        -25, 25);
    
    m_driftTimeUse    = book("drifttimeuse","drift time strategy flag", -0.5,5.5,6) ;
    
    histEventAverageTimeResidual = book("eventavtimeres", "event average time residual",
                                        -10, 10);
  }

  if(msgLevel(MSG::DEBUG)) debug() << "------------ TrackMonitor::initialize() / end ---------" << endmsg;

  return statusCode;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTTrackMonitor::finalize()
{
  // rescale the occupancy histogram
  m_projector.release().ignore();
  m_decoder.release().ignore();
  return GaudiHistoAlg::finalize();
}


void OTTrackMonitor::plotHist1D(AIDA::IHistogram1D* hist, double value, double weight)
{
  if(hist)
    fill(hist, value, weight);
} 

void OTTrackMonitor::plotHist2D(AIDA::IHistogram2D* hist, double x, double y, double weight)
{
  if(hist)
  	fill(hist, x, y, weight);
} 

void OTTrackMonitor::plotProf1D(AIDA::IProfile1D* hist, double x, double y, double weight)
{
  if(hist)
  	fill(hist, x, y, weight);
} 
//=========================================================================
// 
//=========================================================================
StatusCode OTTrackMonitor::execute()
{
  if(msgLevel(MSG::DEBUG)) debug() << "------------ TrackMonitor::execute() / start ----------" << endmsg;

  ++m_numEvents;

  // may be we don't need it
  setHistoTopDir("OT/");

  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackLocation) ;

  double eventAverageTimeResidual = 0;
  int eventAverageTimeResidualN = 0;

  // iterate over all tracks
  size_t numHots(0);
  BOOST_FOREACH(const LHCb::Track* track,tracks)
  {
    int timeResidualSumN = 0;
    double timeResidualSum = 0;

    // process only fitted tracks with nDoF >= 2
    if(track->fitStatus() != LHCb::Track::Fitted || track->nDoF() < 2) continue;

    if(fullDetail())
    {
      std::vector< std::pair<LHCb::OTChannelID, double> > pitchRes = m_pitchtool->calcPitchResiduals(track);
      for(unsigned int i = 0; i < pitchRes.size(); ++i)
      {
        // directory name
        char tmpprefix[256] = "" ;
        switch( m_granularity )
        {
        case 1: sprintf(tmpprefix,"station%d/PitchRes/",pitchRes[i].first.station()) ; break ;
        case 2: sprintf(tmpprefix,"layer%d/PitchRes/",uniqueLayer(pitchRes[i].first)) ; break ;
        default: break;
        }
        std::string nameprefix(tmpprefix);
        std::ostringstream os;
        os << "Layer " << uniqueLayer(pitchRes[i].first) << " pitch residuals";
        plot(pitchRes[i].second, nameprefix + os.str(), os.str(), -3., 3., 120);
        std::ostringstream os1;
        os1 << "Module " << uniqueModule(pitchRes[i].first) << " pitch residuals";

        plot(pitchRes[i].second, nameprefix + os1.str(), os1.str(), -10., 10., 120);
        profile1D( uniqueModule(pitchRes[i].first),pitchRes[i].second,"Pitchresvsmodule","Pitchresidual vs. module nr.",
          -0.5,NumUniqueModule-0.5,NumUniqueModule);
      }
    }

    // iterate over all track nodes
    BOOST_FOREACH(const LHCb::Node* node, track->nodes())
    {
      // process only OT nodes which are HitOnTrack or Outlier and with OT measurement
      if((node->type() != LHCb::Node::HitOnTrack && node->type() && node->type() != LHCb::Node::Outlier) || node->measurement().type() != LHCb::Measurement::OT) continue;

      //sanity check of measurement, check detectorElement is OK
      if((node->type() != LHCb::Node::HitOnTrack && node->type() 
          && node->type() != LHCb::Node::Outlier) 
         || !node->measurement().detectorElement() || node->measurement().type() != LHCb::Measurement::OT) continue;

      ++numHots;

      // get fit node
      const LHCb::FitNode* fitnode = dynamic_cast<const LHCb::FitNode*>(node);
      if(fitnode == 0) continue;

      // get measurement and cast to OT measurement
//      const LHCb::OTMeasurement* measurement = static_cast<const LHCb::OTMeasurement*>(&node->measurement());
      const LHCb::OTMeasurement* measurement = dynamic_cast<const LHCb::OTMeasurement*>(&node->measurement());
      if(measurement == 0) continue;

      plotHist1D(m_driftTimeUse, measurement->driftTimeStrategy(), 1.0 ) ;

      LHCb::OTChannelID channel = measurement->channel();

      bool isOutlier = (node->type() == LHCb::Node::Outlier);
      bool drifttimeWasUsed = 
        measurement->driftTimeStrategy() == LHCb::OTMeasurement::FitTime ||
        measurement->driftTimeStrategy() == LHCb::OTMeasurement::FitDistance ;

      LHCb::State unbiasedState = isOutlier ? fitnode->state() : fitnode->unbiasedState();

      double residual = fitnode->unbiasedResidual();

      // project it:
      LHCb::FitNode unbiasedNode(*fitnode);
      try
      {
        unbiasedNode.setState( unbiasedState );
        unbiasedNode.setRefVector( unbiasedState.stateVector() );
        if(m_projector->projectReference(unbiasedNode) != StatusCode::SUCCESS)
        {
          warning() << "Failed to project reference." << endmsg;
          continue;
        }
      }
      catch(...)
      {
        warning() << "Failed to create unbiased node or project reference." << endmsg;
        continue;
      }

      // and only now get the time-of-flight
      double drifttime = measurement->driftTimeFromY(unbiasedState.y());
      double radius = measurement->driftRadiusWithErrorFromY(unbiasedState.y()).val;
      double trackDistance = unbiasedNode.pocaVector().Dot(
                                                           unbiasedState.position() - 
                                                           fitnode->measurement().trajectory().beginPoint());

      double trackTime = measurement->module().driftTimeWithError(std::abs(trackDistance)).val;
      double drifttimeResidual = drifttime - trackTime;
      double residualScaleFactor = std::sqrt(fitnode->errMeasure() / fitnode->errUnbiasedResidual());
      double residualPull = fitnode->residual() / fitnode->errResidual();

      // make sure all residuals have the same sign in global coordinates
      if( measurement->trajectory().direction(0).y() < 0 )
      {
        residual *= -1.0;
        residualPull *= -1.0;
      }

      int uniquelayer = uniqueLayer(channel);
//      int uniquequarter = uniqueQuarter(channel);
      int uniquemodule = uniqueModule(channel);

      if(drifttimeWasUsed)
      {
        if(std::abs( drifttimeResidual ) < 12)
        {
          plotProf1D(profileTimeResidualVsModule, uniquemodule, drifttimeResidual, 1.0);
        }
        plotProf1D(profileResidualVsModule, uniquemodule, residual, 1.0);
        plotProf1D(profileResidualPullVsModule, uniquemodule, residualPull, 1.0);
      }

      plotHist1D(histModuleHotOccupancy, uniquemodule, 1.0);
      if(isOutlier) plotHist1D(histModuleOutlierOccupancy, uniquemodule, 1.0);

      plotHist1D(histOtisHotOccupancy, uniqueOtis(channel), 1.0);

      // get index of hist - this depends on granularity (plots per stations or per layer)
      int histIndex = 0;
      switch(m_granularity)
      {
      case 1: histIndex = channel.station() - 1; break;
      case 2: histIndex = uniquelayer; break;
      }
      plotHist1D(hists[histIndex][HIST_DRIFTTIME], drifttime, 1.0);
      plotHist1D(hists[histIndex][HIST_DRIFTRADIUS], radius, 1.0);
      plotHist1D(hists[histIndex][HIST_TRACK_DISTANCE], trackDistance, 1.0);
      if( drifttimeWasUsed )
      {
        plotHist1D(hists[histIndex][HIST_DRIFTTIME_RESIDUAL], drifttimeResidual * residualScaleFactor, 1.0);
        plotHist1D(hists[histIndex][HIST_RESIDUAL], residual * residualScaleFactor, 1.0);
        plotHist1D(hists[histIndex][HIST_RESIDUAL_PULL], residualPull, 1.0);

        plotProf1D(profileTimeResidualVsDistance, std::abs(trackDistance), drifttimeResidual, 1.0);
        plotProf1D(profileResidualVsDistance, std::abs(trackDistance), residual, 1.0);
        plotProf1D(profileResidualPullVsDistance, std::abs(trackDistance), residualPull, 1.0);
        double y = unbiasedState.y() - fitnode->measurement().trajectory().beginPoint().y() ;
        plotProf1D(histAverageTimeResidualVsY[histIndex], y, drifttimeResidual, 1.0) ;
      }

      plotHist1D(histDeltaToF, measurement->deltaTimeOfFlight(), 1.0);

      // same for 'good' tracks
      double unbiasedChi2 = (track->chi2() - residualPull * residualPull) / (track->nDoF() - 1);
      bool isGoodTrack = unbiasedChi2 < m_maxUnbiasedChisqPerDofGoodTracks;

      if(isGoodTrack && drifttimeWasUsed)
      {
        if(std::abs( drifttimeResidual ) < 12)
        {
          plotProf1D(profileTimeResidualVsModuleGood, uniquemodule, drifttimeResidual, 1.0);
        }

        plotHist1D(hists[histIndex][HIST_GOOD_DRIFTTIME], drifttime, 1.0);
        plotHist1D(hists[histIndex][HIST_GOOD_DRIFTRADIUS], radius, 1.0);
        plotHist1D(hists[histIndex][HIST_GOOD_TRACK_DISTANCE], trackDistance, 1.0);

        // remove hits outside cell radius
        // NOTE: actualy it is qute strange: the hits outside of cell radius also can be good
        if(std::abs(trackDistance) < cellRadius)
        {
          plotHist1D(hists[histIndex][HIST_GOOD_DRIFTTIME_RESIDUAL], drifttimeResidual * residualScaleFactor, 1.0);
          plotHist1D(hists[histIndex][HIST_GOOD_RESIDUAL], residual * residualScaleFactor, 1.0);
          plotHist1D(hists[histIndex][HIST_GOOD_RESIDUAL_PULL], residualPull, 1.0);

          plotProf1D(profileTimeResidualVsDistanceGood, std::abs(trackDistance), drifttimeResidual, 1.0);
          plotProf1D(profileResidualVsDistanceGood, std::abs(trackDistance), residual, 1.0);
          plotProf1D(profileResidualPullVsDistanceGood, std::abs(trackDistance), residualPull, 1.0);

          if(!isOutlier && drifttimeWasUsed)
          {
            ++timeResidualSumN;
            timeResidualSum += drifttimeResidual;
            ++eventAverageTimeResidualN;
            eventAverageTimeResidual += drifttimeResidual;
          }
        }
      } // if(isGoodTrack)

      // 2D occupancy plot, quite a beast. should be forbidden in monitoring!
      plotHist2D(histXY[histIndex], node->state().x(), node->state().y(), 1.0);
      plotHist2D(histRT[histIndex], trackDistance, drifttime, 1.0);
    } // BOOST_FOREACH(const LHCb::Node* node, track->nodes())

    if(timeResidualSumN > 5)
    {
      plotHist1D(histAverageTimeResidual, timeResidualSum / timeResidualSumN, 1.0);
      plotHist2D(histAverageTimeResidualVsMomentum, track->p() / 1000.0, timeResidualSum / timeResidualSumN, 1.0);
    } // if(timeResidualSumN > 5)
  } // BOOST_FOREACH(const LHCb::Track* track,tracks)

  if(eventAverageTimeResidualN > 9)
  {
    plotHist1D(histEventAverageTimeResidual, eventAverageTimeResidual / eventAverageTimeResidualN, 1.0);
  }

  BOOST_FOREACH(const DeOTModule* module, m_otdet->modules())
  {
    LHCb::OTChannelID modid = module->elementID();
    LHCb::OTLiteTimeRange liteTimes = m_decoder->decodeModule(modid);
    size_t numhits = liteTimes.size();
    plotHist1D(histModuleHitOccupancy, uniqueModule(modid), numhits);
    BOOST_FOREACH(const LHCb::OTLiteTime& liteTime, liteTimes)
    {
      plotHist1D(histOtisHitOccupancy, uniqueOtis(liteTime.channel()), 1.0);
    }
  } // BOOST_FOREACH(const DeOTModule* module, m_otdet->modules())

  plotHist1D(m_hitMultiplicity, m_decoder->totalNumberOfHits(), 1.0 );
  plotHist1D(m_hotMultiplicity, numHots , 1.0);

  // set this on every event
  setNormalization(histModuleHitOccupancy);
  setNormalization(histModuleHotOccupancy);
  if(!m_isOnline)
    setNormalization(histModuleOutlierOccupancy);

  if(msgLevel(MSG::DEBUG)) debug() << "------------ TrackMonitor::execute() / end ------------" << endmsg;

  return StatusCode::SUCCESS;
}

void OTTrackMonitor::setNormalization(AIDA::IHistogram1D* hist) 
{
  if(!hist) Exception( "You have passed a null histogram!");
  
  TH1* h1 = Gaudi::Utils::Aida2ROOT::aida2root(hist);
  if(h1) h1->SetEntries(m_numEvents);
}


