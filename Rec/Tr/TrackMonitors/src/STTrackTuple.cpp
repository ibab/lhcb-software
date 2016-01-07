// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "STTrackTuple.h"

// track interfaces
#include "TrackInterfaces/IHitExpectation.h"

// kernel
#include "Kernel/ISTChannelIDSelector.h"
#include "Kernel/STLexicalCaster.h"
#include "Kernel/ITDetectorPlot.h"
#include "Kernel/TTDetectorPlot.h"
#include "Kernel/HitPattern.h"
#include "Kernel/Trajectory.h"

// detector
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeSTSensor.h"

// Event
#include "Event/STCluster.h"
#include "Event/Measurement.h"
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "Event/VPMeasurement.h"

// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"

#include "Event/ODIN.h"

#include <sstream>
#include <cmath>
#include <iostream>

// Namespaces
using namespace std;
using namespace LHCb;
using namespace ST;
using namespace AIDA;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : STTrackTuple
//
// 2009-06-16 : Johan Luisier
// 2010-07-27 : Frederic Dupertuis
// 2015-04-15 : Ilya Komarov and Elena Graverini
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STTrackTuple )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STTrackTuple::STTrackTuple( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : TrackMonitorTupleBase ( name , pSvcLocator ),
    m_totalExpected( 0u ),
    m_totalFound( 0u ),
    m_binSize( 10000. )
{
  declareProperty( "DetType"            , m_detType = "IT" );
  declareProperty( "ClusterCollector"   , m_clustercollectorName = m_detType+"ClusterCollector");  
  declareProperty( "HitExpectation"     , m_expectedHitsToolName = m_detType+"HitExpectation");  
  declareProperty( "Cuts"               , m_spacialCuts = { 300. * Gaudi::Units::um } );
  declareProperty( "XLayerCut"          , m_xCut = 300. * Gaudi::Units::um );
  declareProperty( "StereoLayerCut"     , m_stereoCut = 300. * Gaudi::Units::um );
  declareProperty( "defaultCut"         , m_defaultCut = 300. * Gaudi::Units::um );
  declareProperty( "MinExpectedSectors" , m_minExpSectors = 1 );
  declareProperty( "MinNbResSectors"    , m_minNbResSectors = -10 );
  declareProperty( "MaxNbResSectors"    , m_maxNbResSectors = 10 );
  declareProperty( "MinDistToEdgeX"     , m_minDistToEdgeX = 2. );
  declareProperty( "MinDistToEdgeY"     , m_minDistToEdgeY = 2. );
  declareProperty( "MinStationPassed"   , m_minStationPassed = 1 );
  declareProperty( "MinCharge"          , m_minCharge = 0. );
  declareProperty( "EfficiencyPlot"     , m_effPlot = true );
  declareProperty( "ResidualsPlot"      , m_resPlot = false );
  declareProperty( "TakeEveryHit"       , m_everyHit = false);
  declareProperty( "HitsOnTrack"        , m_hitsOnTrack = true);
  declareProperty( "SingleHitPerSector" , m_singlehitpersector = false );
  declareProperty( "BranchBySector"     , m_branchBySector = false);
  declareProperty( "BranchByTrack"      , m_branchByTrack = true);
  declareProperty( "SkipEdges"          , m_skipEdges = true );
  declareProperty( "SaveSectorPositions", m_savePositions = false );
}

//=============================================================================
// Destructor
//=============================================================================
STTrackTuple::~STTrackTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STTrackTuple::initialize()
{
  StatusCode sc( TrackMonitorTupleBase::initialize() ); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by TrackMonitorTupleBase
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_clustercollectorName = m_detType+"ClusterCollector";
  m_expectedHitsToolName = m_detType+"HitExpectation";
  
  m_clustercollector = tool< ISTClusterCollector >( "STClusterCollector", m_clustercollectorName );
  m_expectedHits  = tool< IHitExpectation >( m_detType+"HitExpectation", m_expectedHitsToolName );
  
  m_tracker = getDet< DeSTDetector >( DeSTDetLocation::location( m_detType ) );

  m_NbrOfCuts = m_spacialCuts.size();
  m_foundSector.resize(m_NbrOfCuts);
  
  const DeSTDetector::Sectors& sectors = m_tracker->sectors();

  // Retrieve and print sector positions 
  // (Done only once per job)
  if (m_savePositions)
  {
    Tuple positions = nTuple("SectorPositions");
    std::vector<DeSTSensor*> sensorsInSector;
    for(DeSTSector * sector: sectors)
    {
      // Printouts
      //std::cout << sector->nickname() << "\t" << int( sector->elementID() ) << "\t\t" ;
      //Gaudi::XYZPoint pos = sector->toGlobal( Gaudi::XYZPoint(0.,0.,0.) );
      // std::cout << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;

      positions -> column( "sectorID", int( sector->elementID() ) );
      sensorsInSector = sector -> sensors();
      std::vector<float> sensor_x;     sensor_x.reserve( sensorsInSector.size() );
      std::vector<float> sensor_y;     sensor_y.reserve( sensorsInSector.size() );
      std::vector<float> sensor_z;     sensor_z.reserve( sensorsInSector.size() );
      std::vector<float> corner1_x;    corner1_x.reserve( sensorsInSector.size() );
      std::vector<float> corner1_y;    corner1_y.reserve( sensorsInSector.size() );
      std::vector<float> corner1_z;    corner1_z.reserve( sensorsInSector.size() );
      std::vector<float> corner2_x;    corner2_x.reserve( sensorsInSector.size() );
      std::vector<float> corner2_y;    corner2_y.reserve( sensorsInSector.size() );
      std::vector<float> corner2_z;    corner2_z.reserve( sensorsInSector.size() );
      std::vector<float> corner3_x;    corner3_x.reserve( sensorsInSector.size() );
      std::vector<float> corner3_y;    corner3_y.reserve( sensorsInSector.size() );
      std::vector<float> corner3_z;    corner3_z.reserve( sensorsInSector.size() );
      std::vector<float> corner4_x;    corner4_x.reserve( sensorsInSector.size() );
      std::vector<float> corner4_y;    corner4_y.reserve( sensorsInSector.size() );
      std::vector<float> corner4_z;    corner4_z.reserve( sensorsInSector.size() );
      for(auto sensor = sensorsInSector.begin(); sensor != sensorsInSector.end(); ++sensor)
      {
        // Sensor centre
        Gaudi::XYZPoint pos = (*sensor)->globalCentre();//toGlobal( Gaudi::XYZPoint(0.,0.,0.) );
        sensor_x.push_back( pos.x() );
        sensor_y.push_back( pos.y() );
        sensor_z.push_back( pos.z() );
        // Sensor corners
        auto firsttraj = (*sensor)->trajectory(1, 0.);
        Gaudi::XYZPoint corner1 = firsttraj->beginPoint();
        Gaudi::XYZPoint corner2 = firsttraj->endPoint();
        auto lasttraj = (*sensor)->trajectory(512, 0.);
        Gaudi::XYZPoint corner3 = lasttraj->beginPoint();
        Gaudi::XYZPoint corner4 = lasttraj->endPoint();
        corner1_x.push_back( corner1.x() );
        corner1_y.push_back( corner1.y() );
        corner1_z.push_back( corner1.z() );
        corner2_x.push_back( corner2.x() );
        corner2_y.push_back( corner2.y() );
        corner2_z.push_back( corner2.z() );
        corner3_x.push_back( corner3.x() );
        corner3_y.push_back( corner3.y() );
        corner3_z.push_back( corner3.z() );
        corner4_x.push_back( corner4.x() );
        corner4_y.push_back( corner4.y() );
        corner4_z.push_back( corner4.z() );
      }
      positions -> farray( "sensor_x",  sensor_x, "len",  sensorsInSector.size() );
      positions -> farray( "sensor_y",  sensor_y, "len",  sensorsInSector.size() );
      positions -> farray( "sensor_z",  sensor_z, "len",  sensorsInSector.size() );
      positions -> farray( "corner1_x", corner1_x, "len",  sensorsInSector.size() );
      positions -> farray( "corner1_y", corner1_y, "len",  sensorsInSector.size() );
      positions -> farray( "corner1_z", corner1_z, "len",  sensorsInSector.size() );
      positions -> farray( "corner2_x", corner2_x, "len",  sensorsInSector.size() );
      positions -> farray( "corner2_y", corner2_y, "len",  sensorsInSector.size() );
      positions -> farray( "corner2_z", corner2_z, "len",  sensorsInSector.size() );
      positions -> farray( "corner3_x", corner3_x, "len",  sensorsInSector.size() );
      positions -> farray( "corner3_y", corner3_y, "len",  sensorsInSector.size() );
      positions -> farray( "corner3_z", corner3_z, "len",  sensorsInSector.size() );
      positions -> farray( "corner4_x", corner4_x, "len",  sensorsInSector.size() );
      positions -> farray( "corner4_y", corner4_y, "len",  sensorsInSector.size() );
      positions -> farray( "corner4_z", corner4_z, "len",  sensorsInSector.size() );
      positions -> write();
    } // end loop (sectors)
  }

  for (auto iterS = sectors.begin() ; iterS != sectors.end(); ++iterS)
  {
    STChannelID elemID = (*iterS)->elementID();
    m_expectedSector[elemID.uniqueSector()] = 0u;
    m_nameMapSector[elemID.uniqueSector()] = *iterS;
    for (unsigned int i=0 ; i < m_NbrOfCuts; ++i )
    {
      m_foundSector[i][elemID.uniqueSector()] = 0u; 
    }
  } // end loop (iterS)

  for (unsigned iCut = 0; iCut < m_NbrOfCuts; ++iCut)
  {
    if ( std::abs( m_xCut - m_spacialCuts[iCut] ) < .005 )
      m_whichCut[0] = iCut;
    if ( std::abs( m_stereoCut -  m_spacialCuts[iCut]) < .005 )
      m_whichCut[1] = iCut;
  } // end loop (iCut)

  for(unsigned iCut = 1; iCut < m_NbrOfCuts; iCut++)
  {
    m_binSize = std::min( m_binSize,
                          m_spacialCuts[ iCut ] - m_spacialCuts[ iCut - 1 ] );
  }
  //m_binSize = std::min( m_binSize, .01 );
  //m_binSize = std::max( m_binSize, .01 );
  m_binNumber = static_cast<unsigned int>( (m_spacialCuts.back() -
                                            m_spacialCuts.front()) / m_binSize );
  m_binNumber++;
  
  return StatusCode::SUCCESS;
} // STTrackTuple::initialize()




//=============================================================================
// Main execution
//=============================================================================
StatusCode STTrackTuple::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  if (m_branchBySector && !m_everyHit)
  {
    error() << "STTrackTuple ERROR: trying to compute hit "
            << "efficiency using only hits on tracks! Set "
            << "TakeEveryHit and unset HitsOnTracks to avoid!\n";
    return StatusCode::FAILURE;
  }

  if (m_branchBySector && m_branchByTrack)
  {
    warning() << "STTrackTuple WARNING: I'm producing an ntuple "
              << "branched by sector AND by track, possible "
              << "ntuple clash. To avoid, select either "
              << "BranchByTrack or BranchBySector.\n";
  }


  const DeSTDetector::Sectors& sectors = m_tracker->sectors();

  // Book ntuple
  Tuple tup_test = nTuple ("TrackMonTuple") ;

  // Read tracks
  Tracks* tracks = get< LHCb::Tracks >( inputContainer() );
  Tracks::const_iterator It, Begin( tracks -> begin() ),
    End( tracks -> end() );

  std::vector<LHCbID> expectedHits;
  std::vector<unsigned int> expectedSectors;
  std::vector<unsigned int> existingSectors;
  std::vector<unsigned int> foundSectors;
  std::vector<unsigned int>::iterator itFoundSectors;
  ISTClusterCollector::Hits prefoundHits;
  ISTClusterCollector::Hits foundHits;
  StatusCode inSensor;
  bool inActiveSensor;
  unsigned int nbFoundHits,nbExpHits,nbExpSectors,nbFoundSectors;
  int nbResHits,nbResSectors;
  State trackState;
  // if computing hit efficiency use only active regions
  Gaudi::XYZPoint toleranceOnEdge(- std::max(0.,m_minDistToEdgeX), - std::max(0.,m_minDistToEdgeY), 0.);
  // otherwise keep the whole sensor surface
  if (!m_skipEdges)
  {
    toleranceOnEdge = Gaudi::XYZPoint(0., 0., 0.);
  }

  DeSTSector* sector = 0;
  std::vector<DeSTSensor*> sensors;

  // Loop on tracks
  for (It = Begin; It != End; It++)
  {
    expectedHits.clear();
    prefoundHits.clear();
    foundHits.clear();
    expectedSectors.clear();
    foundSectors.clear();

    // ask for tracks crossing enough stations
    if( ! hasMinStationPassed( (*It) ) )
      continue;
        
    // collect the list of expected hits
    m_expectedHits -> collect( **It, expectedHits );

    // remove tracks with too few expected hits
    nbExpHits = expectedHits.size();
    if ( nbExpHits < m_minExpSectors ) 
      continue; 

    // collect the list of found hits
    m_clustercollector -> execute( **It, prefoundHits );
    nbFoundHits = prefoundHits.size();
    
    // Number of hits not found
    nbResHits = (int)(nbFoundHits) - (int)(nbExpHits);
    
    if(fullDetail()){
      plot(nbFoundHits, "Control/NbFoundHits","Number of found hits",-0.5,24.5,25);
      plot(nbExpHits,   "Control/NbExpHits",  "Number of expected hits",-0.5,24.5,25);
      plot(nbResHits,   "Control/NbResHits",  "Number of found - expected hits",-5.5,7.5,13);
    }
        
    // convert the expected hits into expected sectors, cutting, sensor edges if m_skipEdges=true
    for(const auto& id: expectedHits)
    {
      if( m_minDistToEdgeX > 0. || m_minDistToEdgeY > 0.){
        sector = m_tracker -> findSector( id.stID() );
        //info()<<"Sector is "<<sector<<endl;
        if ( sector == 0 ){
          info() << "Should not happen !" << endmsg;
          continue;
        }
        sensors = sector -> sensors();
        inActiveSensor = false;
        for(const auto& sensor: sensors)
        {
          inSensor = extrapolator() -> propagate( (**It), sensor -> plane(), trackState, sensor -> thickness() );
          if( inSensor.isSuccess() ){
            inActiveSensor = sensor -> globalInActive(trackState.position(), toleranceOnEdge) ;
            if( inActiveSensor ) break;
          }
        } // end loop (find active sensor)
        if( inActiveSensor )
        {
          expectedSectors.push_back(id.stID().uniqueSector());
        }
      }// end if (m_minDistToEdgeX > 0. || m_minDistToEdgeY > 0.)
    }// end loop (convert the expected hits into expected sectors, cutting sensor edges)
    nbExpSectors = expectedSectors.size();

    // remove tracks with too few expected hits
    if ( nbExpSectors < m_minExpSectors ) 
      continue; 
        
    // Fill foundHits and foundSectors
    for(const auto& hit: prefoundHits)
    {
      // if the hit sector is in the list of expected sectors
      if(find( expectedSectors.begin(), expectedSectors.end(), hit.cluster->channelID().uniqueSector() ) != expectedSectors.end()){
        // push hit in foundHits and sector in foundSectors, if not already there
        if(find(foundSectors.begin(), foundSectors.end(), hit.cluster->channelID().uniqueSector()) == foundSectors.end()){
          foundHits.push_back(hit);
          foundSectors.push_back(hit.cluster->channelID().uniqueSector());
        }else if( !m_singlehitpersector )
          {foundHits.push_back(hit);} // push hit in foundHits anyways
        else
          continue;
      }
    }

    nbExpHits = nbExpSectors;
    nbFoundHits = foundHits.size();
    nbResHits = (int)(nbFoundHits) - (int)(nbExpHits);
    
    nbFoundSectors = foundSectors.size();
    nbResSectors = (int)(nbFoundSectors) - (int)(nbExpSectors);
    
    // remove tracks with too many (found - expected) hits
    if ( nbResHits > m_maxNbResSectors ) {
      if (msgLevel(MSG::DEBUG)) debug() << "Possible track expectation mismatch";
      continue;
    }
    
    // remove tracks with too few (found - expected) sectors
    if ( nbResSectors < m_minNbResSectors ) {
      if (msgLevel(MSG::DEBUG)) debug() << "Possible track expectation mismatch";
      continue;
    }
    
    if(fullDetail()){
      plot(nbFoundHits, "Control/NbFoundHitsFinal","Number of found hits after full selection",-0.5,24.5,25);
      plot(nbExpHits, "Control/NbExpHitsFinal","Number of expected hits after full selection",-0.5,24.5,25);
      plot(nbResHits, "Control/NbResHitsFinal","Number of found - expected hits after full selection",-5.5,7.5,13);
      plot(nbFoundSectors, "Control/NbFoundSectorsFinal","Number of found sectors after full selection",-0.5,24.5,25);
      plot(nbExpSectors, "Control/NbExpSectorsFinal","Number of expected sectors after full selection",-0.5,24.5,25);
      plot(nbResSectors, "Control/NbResSectorsFinal","Number of found - expected sectors after full selection",-5.5,7.5,13);
    }
        
    std::vector<unsigned int>::iterator iterExp = expectedSectors.begin();

    // Count tracks
    tup_test -> column ("TrackCounter", 1);
    // Run number
    if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) )
    {
      LHCb::ODIN* report = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
      if ( ! tup_test->column( "RunNumber", report->runNumber() ) ) return StatusCode::FAILURE;
      if (msgLevel(MSG::DEBUG)) debug() << "Run number:  " << report->runNumber() << endmsg;
    }

    if (m_branchByTrack)
    {
      // Add track information to the tuple
      tup_test->column("p",        (*It)->p());
      tup_test->column("pt",       (*It)->pt());
      tup_test->column("phi",      (*It)->phi());
      tup_test->column("eta",      (*It)->pseudoRapidity());
      tup_test->column("charge",   (*It)->charge());
      tup_test->column("nDoF",     (*It)->nDoF());
      tup_test->column("chi2",     (*It)->chi2());
      tup_test->column("probChi2", (*It)->probChi2());

      // Number of ST hits on track
      unsigned int numTTHits = 0;
      unsigned int numITHits = 0;

      // Add vectors of clusters information to the ntuple
      unsigned int maxhits = 30; // default length of vectors
      std::vector<unsigned int> clusterSize;   clusterSize.reserve(maxhits);
      std::vector<int> clusterStrip;           clusterStrip.reserve(maxhits);
      std::vector<double> clusterCharge;       clusterCharge.reserve(maxhits);
      std::vector<double> clusterNoise;        clusterNoise.reserve(maxhits);
      std::vector<double> clusterISfrac;       clusterISfrac.reserve(maxhits);
      std::vector<float> cluster_x;            cluster_x.reserve(maxhits);
      std::vector<float> cluster_y;            cluster_y.reserve(maxhits);
      std::vector<float> cluster_z;            cluster_z.reserve(maxhits);
      std::vector<float> trackState_x;         trackState_x.reserve(maxhits);
      std::vector<float> trackState_y;         trackState_y.reserve(maxhits);
      std::vector<float> trackState_z;         trackState_z.reserve(maxhits);
      std::vector<int> clusterSTchanID;        clusterSTchanID.reserve(maxhits);
      std::vector<double> hit_residual;        hit_residual.reserve(maxhits);
      std::vector<double> traj_mu;             traj_mu.reserve(maxhits);

      // Use only hits from tracks
      if (m_hitsOnTrack && !m_everyHit)
      {
        // Add: residual error, measurement error
        std::vector<double> hit_errMeasure;      hit_errMeasure.reserve(maxhits);
        std::vector<double> hit_errResidual;     hit_errResidual.reserve(maxhits);
        // Vector of measurements on the track
        std::vector<const LHCb::STMeasurement*> measVector; measVector.reserve(maxhits);
        // Read the nodes on the track
        for (const auto& node: (*It) -> nodes() ) 
        {
          const LHCb::FitNode* fNode = dynamic_cast<const LHCb::FitNode*>(node);
          if ( fNode->hasMeasurement() == false ) continue;
          if ( m_hitsOnTrack && ( node->type() != LHCb::Node::HitOnTrack ) ) continue;
          if ( node->errResidual2() <= TrackParameters::lowTolerance ) continue;
          // Count number of TT and IT hits
          if ( fNode->measurement().type() != LHCb::Measurement::TT ) numTTHits++;
          if ( fNode->measurement().type() != LHCb::Measurement::IT ) numITHits++;
          // One detector at a time
          //if ( m_detType == "TT" && fNode->measurement().type() != LHCb::Measurement::TT ) continue;
          //if ( m_detType == "IT" && fNode->measurement().type() != LHCb::Measurement::IT ) continue;
          if ( m_detType == "TT" && !(fNode->measurement().type() == LHCb::Measurement::TT  || fNode->measurement().type() == LHCb::Measurement::TTLite)) continue;
          if ( m_detType == "IT" && !(fNode->measurement().type() == LHCb::Measurement::IT  || fNode->measurement().type() == LHCb::Measurement::ITLite)) continue;
          // Retrieve the measurement from the track node
          const STMeasurement* aHit = dynamic_cast<const STMeasurement*>(&fNode->measurement());
          measVector.push_back(aHit);
          // Fill residual and residual error
          hit_residual.push_back( node->residual() );
          hit_errResidual.push_back( node->errResidual() );
          // Fill position of the track state
          const LHCb::State & aState = node->state();
          trackState_x.push_back( (float)(aState.x()) );
          trackState_y.push_back( (float)(aState.y()) );
          trackState_z.push_back( (float)(aState.z()) );
          // Retrieve the cluster from the measurement
          const LHCb::STCluster * aCluster = aHit->cluster();
          // Fill cluster ID, size, strip, charge, interstrip fraction, noise
          clusterSTchanID.push_back( (int)(aCluster->channelID()) );
          clusterSize.push_back(   aCluster->size() );
          clusterStrip.push_back(  aCluster->strip() );
          clusterCharge.push_back( aCluster->totalCharge() );
          double interStripFrac = (aHit->cluster())->interStripFraction();
          clusterISfrac.push_back( interStripFrac );
          const DeSTSector* sector = m_tracker->findSector(aCluster->channelID());
          double noise = sector->noise(aCluster->channelID()); 
          clusterNoise.push_back( noise );
          // Find the position of the charge deposit on the hit strip
          auto traj = m_tracker -> trajectory( aCluster->channelID(), interStripFrac );
          double mu = traj.get()->muEstimate(aState.position());
          LHCb::Trajectory::Point midStripPosition = traj.get()->position(mu);
          traj_mu.push_back(mu);
          //std::cout << "at mu = " << mu << ": " << midStripPosition.z() << ", state is at " << aState.z() << std::endl;
          // Fill position of the charge deposit
          cluster_x.push_back( (float)(midStripPosition.x()) );
          cluster_y.push_back( (float)(midStripPosition.y()) );
          cluster_z.push_back( (float)(midStripPosition.z()) );
          // Fill measurement error
          hit_errMeasure.push_back( aHit->errMeasure() );
        } // end loop (nndes)

        // Count the number of hits on each ST layer
        for(const auto& layer: m_tracker->layers())
        {
          unsigned int nHitsOnLayer = hitsOnLayer( measVector, layer );
          tup_test -> column(layer->nickname()+"_nHits", nHitsOnLayer);
        } // end loop (layers)

        // Fill information available only in "hits on track" mode
        tup_test -> farray( "trackState_x",    trackState_x, "len",    maxhits );
        tup_test -> farray( "trackState_y",    trackState_y, "len",    maxhits );
        tup_test -> farray( "trackState_z",    trackState_z, "len",    maxhits );
        tup_test -> farray( "hit_errMeasure",  hit_errMeasure, "len",  maxhits );
        tup_test -> farray( "hit_errResidual", hit_errResidual, "len", maxhits );
      }// end if (hits on track)

      // Use hits found by the cluster collector
      else if (m_everyHit && !m_hitsOnTrack)
      {
        // Count the number of hits on each ST layer
        const DeSTDetector::Layers& layers = m_tracker->layers();
        for(const auto& layer: layers)
        {
          unsigned int nHitsOnLayer = hitsOnLayer( foundHits, layer );
          tup_test -> column(layer->nickname()+"_nHits", nHitsOnLayer);
        } // end loop (layers)

        // Loop on the found hits
        for(const auto& aHit: foundHits)
        {
          // Count the number of TT and IT hits
          if ( aHit.cluster->isTT() ) numTTHits++;
          if ( aHit.cluster->isIT() ) numITHits++;
          // Fill vectors of cluster information
          if ( (aHit.cluster->isTT() && m_detType == "TT") || (aHit.cluster->isIT() && m_detType == "IT") )
          {
            clusterSTchanID.push_back( (int)(aHit.cluster->channelID()) );
            clusterSize.push_back( aHit.cluster->size() );
            clusterStrip.push_back( aHit.cluster->strip() );
            clusterCharge.push_back( aHit.cluster->totalCharge() );
            double interStripFrac = aHit.cluster->interStripFraction();
            clusterISfrac.push_back( interStripFrac );
            const DeSTSector* sector = m_tracker->findSector(aHit.cluster->channelID());
            double noise = sector->noise(aHit.cluster->channelID()); 
            clusterNoise.push_back( noise );
            // Find the closest state on the track
            DeSTSensor* sensor = findSensor(sector, aHit.cluster->channelID());
            const LHCb::State & aState = (*It)->closestState( sensor->plane() ); // closestState works only with planes!
            // Fill the position of the found state
            trackState_x.push_back( (float)(aState.x()) );
            trackState_y.push_back( (float)(aState.y()) );
            trackState_z.push_back( (float)(aState.z()) );
            // Find the position of the charge deposit on the hit strip
            auto traj = m_tracker -> trajectory( aHit.cluster->channelID(), interStripFrac );
            double mu = traj.get()->muEstimate(aState.position());
            traj_mu.push_back(mu);
            LHCb::Trajectory::Point hitPosition = traj.get()->position(mu);
            cluster_x.push_back( (float)(hitPosition.x()) );
            cluster_y.push_back( (float)(hitPosition.y()) );
            cluster_z.push_back( (float)(hitPosition.z()) );
            hit_residual.push_back( aHit.residual );
          } // end if (one detector at a time)
        } // end loop (foundHits)

        // Fill information available only in "every hit" mode
        tup_test -> farray( "closestTrackState_x",    trackState_x, "len", maxhits );
        tup_test -> farray( "closestTrackState_y",    trackState_y, "len", maxhits );
        tup_test -> farray( "closestTrackState_z",    trackState_z, "len", maxhits );
      } // end if (every hit)

      else
      {
        error() << "STTrackTuple ERROR: cannot select both TakeEveryHits and HitsOnTracks!\n";
        return StatusCode::FAILURE;
      } 

      // Fill information  
      tup_test -> farray( "clusterSTchanID", clusterSTchanID, "len", maxhits );
      tup_test -> farray( "clusterSize",     clusterSize, "len",     maxhits );
      tup_test -> farray( "clusterStrip",    clusterStrip, "len",    maxhits );
      tup_test -> farray( "clusterCharge",   clusterCharge, "len",   maxhits );
      tup_test -> farray( "clusterNoise",    clusterNoise, "len",    maxhits );
      tup_test -> farray( "clusterISfrac",   clusterISfrac, "len",   maxhits );
      tup_test -> farray( "traj_mu",         traj_mu, "len",         maxhits );
      tup_test -> farray( "cluster_x",       cluster_x, "len",       maxhits );
      tup_test -> farray( "cluster_y",       cluster_y, "len",       maxhits );
      tup_test -> farray( "cluster_z",       cluster_z, "len",       maxhits );
      tup_test -> farray( "hit_residual",    hit_residual, "len",    maxhits );

      tup_test->column("numIThits", numITHits);
      tup_test->column("numTThits", numTTHits);

    } // end if (m_branchByTrack)


    // Store residual information sector by sector
    if (m_branchBySector)
    {
      bool foundHit = false;
      for(const auto& sector: sectors)
      {
        tup_test -> column (sector->nickname(), -1000000 * Gaudi::Units::um);
      }
      for ( ; iterExp != expectedSectors.end(); ++iterExp)
      {
        ++m_expectedSector[*iterExp];
        ++m_totalExpected;
        foundHit = false;
        double min_cut = m_whichCut[0];
        for (unsigned int i = 0; i < m_NbrOfCuts; ++i)
        {
          foundHit = foundHitInSector(foundHits, *It, *iterExp, m_spacialCuts[i], (sector ->isStereo() && i == m_whichCut[1])||(! sector -> isStereo() &&  i == m_whichCut[0]));
          if (foundHit)
          {
            if ( sector ->isStereo() && i == m_whichCut[1] )
              ++m_totalFound;
            else if ( ! sector -> isStereo() &&  i == m_whichCut[0] )
              ++m_totalFound;
            if (m_spacialCuts[i] < min_cut)
              min_cut = m_spacialCuts[i];
            ++m_foundSector[i][*iterExp]; 
          }
        } // end loop (cuts)
        double res = foundResInSector(foundHits, *It, *iterExp, min_cut);
        tup_test -> column (m_nameMapSector[*iterExp]->nickname(), res);
      } // end loop (expected sectors)
    } // end if (branchBySector)

    // Write the ntuple
    tup_test -> write () ;
  } // end loop (tracks)

  return StatusCode::SUCCESS;
} // STTrackTuple::execute()


/**
 * Checks whether there is a hit in the given sector.
 *
 * @param hits list of clusters.
 * @param testsector uniqueID of the wanted sector
 * @param resCut window size
 *
 * @return \e true if yes.
 */
bool STTrackTuple::foundHitInSector( const ISTClusterCollector::Hits& hits,
                                     Track* const& track,
                                     const unsigned int testsector,
                                     const double resCut,
                                     const bool toplot ) const
{
  for(const auto& aHit: hits)
    {
      if ( aHit.cluster->channelID().uniqueSector() == testsector ){
        if(fullDetail()&&toplot){
          plot(aHit.residual, "Control/HitResidual","Hit residual",-resCut,resCut,static_cast<unsigned int>(resCut*100+0.01)*2);
          plot(aHit.cluster->totalCharge(), "Control/HitCharge","Hit charge",0.,200.,200);
        }
        if(m_resPlot&&toplot){
          std::string name("perSector/Residuals_");
          name += aHit.cluster->sectorName();
          plot(aHit.residual, name,"Hit residual",-resCut,resCut,static_cast<unsigned int>(resCut*100+0.01)*2);
        }
        if( fabs(aHit.residual) < resCut ){
          if ( !( m_everyHit || track -> isOnTrack( LHCbID( aHit.cluster -> channelID() ) ) ) )
            continue;
          if( aHit.cluster->totalCharge() < m_minCharge )
            continue;
          return true;
        }
      }
    } // end loop (hits)
  return false;
} // STTrackTuple::foundHitInSector()


/**
 * Checks whether there is a hit in the given layer.
 *
 * @param hits list of clusters.
 * @param testlayer uniqueID of the wanted layer
 * @param resCut window size
 *
 * @return \e true if yes.
 */
bool STTrackTuple::foundHitInLayer( const ISTClusterCollector::Hits& hits,
                                     Track* const& track,
                                     const unsigned int testlayer,
                                     const double resCut ) const
{
  for(const auto& aHit: hits)
    {
      if ( aHit.cluster->channelID().uniqueLayer() == testlayer ){
        if( fabs(aHit.residual) < resCut ){
          if ( !( m_everyHit || track -> isOnTrack( LHCbID( aHit.cluster -> channelID() ) ) ) )
            continue;
          if( aHit.cluster->totalCharge() < m_minCharge )
            continue;
          return true;
        }
      }
    } // end loop (hits)
  return false;
} // STTrackTuple::foundHitInLayer


/**
 * Returns the residual if there was a hit in the given sector.
 *
 * @param hits list of clusters.
 * @param testsector uniqueID of the wanted sector
 * @param resCut window size
 *
 * @return \e residual or -500000 um if no hits are found.
 */
double STTrackTuple::foundResInSector( const ISTClusterCollector::Hits& hits,
                                     Track* const& track,
                                     const unsigned int testsector,
                                     const double resCut ) const
{
  for(const auto& aHit: hits)
    {
      if ( aHit.cluster->channelID().uniqueSector() == testsector ){
        if( fabs(aHit.residual) < resCut ){
          if ( !( m_everyHit || track -> isOnTrack( LHCbID( aHit.cluster -> channelID() ) ) ) )
            continue;
          if( aHit.cluster->totalCharge() < m_minCharge )
            continue;
          return aHit.residual;
        }
      }
    } // end loop (hits)
  return -500000.* Gaudi::Units::um;
} // STTrackTuple::foundResInSector


//=============================================================================
//  Finalize
//=============================================================================
StatusCode STTrackTuple::finalize()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  if ( (m_totalExpected == 0) && (m_branchBySector) )
  {
    warning() << "I'm sorry, but no track was found, please check that"
              << " you selected the right track type." << std::endl
              << "Your selection was : ";
    for (unsigned int i( 0 ); i < m_wantedTypes.size(); i++)
      warning() << m_wantedTypes[ i ] << ' ';
    warning() << endmsg;
    return TrackMonitorTupleBase::finalize();
  }

  std::map<unsigned int, DeSTSector*>::const_iterator iterS = m_nameMapSector.begin();
 
  std::vector<double>::const_iterator cutBegin( m_spacialCuts.begin() ),
    cutEnd( m_spacialCuts.end() ), cutIt;

  STChannelID channelID;
  std::string nick, root;

  unsigned int i, presentCut( 0u );

  double nExpected, nFound, err;
  double eff  = 999.0;
  
  Histo2DProperties* prop;
  Histo2DProperties* propSectorStatus;
  Histo2DProperties* propNbHits;
  Histo2DProperties* propNbFoundHits;
  
  if(m_detType=="IT"){
    prop = new ST::ITDetectorPlot( "EfficiencyPlot", "Efficiency Plot" );
    propSectorStatus = new ST::ITDetectorPlot( "SectorStatus", "Sector Status Plot" );
    propNbHits = new ST::ITDetectorPlot( "NbExpHits", "Number of Expected Hits Plot" );
    propNbFoundHits = new ST::ITDetectorPlot( "NbFoundHits", "Number of Found Hits Plot" );
  }else{
    prop = new ST::TTDetectorPlot( "EfficiencyPlot", "Efficiency Plot" );
    propSectorStatus = new ST::TTDetectorPlot( "SectorStatus", "Sector Status Plot" );
    propNbHits = new ST::TTDetectorPlot( "NbExpHits", "Number of Expected Hits Plot" );
    propNbFoundHits = new ST::TTDetectorPlot( "NbFoundHits", "Number of Found Hits Plot" );
  }
  
  IHistogram2D* histoEff = 0;
  if(m_effPlot){
    histoEff = book2D( prop->name() , prop->title(),
                       prop->minBinX(), prop->maxBinX(), prop->nBinX(),
                       prop->minBinY(), prop->maxBinY(), prop->nBinY());
  }
  
  IProfile2D* histoSectorStatus = bookProfile2D( propSectorStatus->name() , propSectorStatus->title(),
                                                 propSectorStatus->minBinX(), propSectorStatus->maxBinX(), propSectorStatus->nBinX(),
                                                 propSectorStatus->minBinY(), propSectorStatus->maxBinY(), propSectorStatus->nBinY());
  
  IHistogram2D* histoNbHits = book2D( propNbHits->name() , propNbHits->title(),
                                      propNbHits->minBinX(), propNbHits->maxBinX(), propNbHits->nBinX(),
                                      propNbHits->minBinY(), propNbHits->maxBinY(), propNbHits->nBinY());
  
  IHistogram2D* histoNbFoundHits = book2D( propNbFoundHits->name() , propNbFoundHits->title(),
                                           propNbFoundHits->minBinX(), propNbFoundHits->maxBinX(), propNbFoundHits->nBinX(),
                                           propNbFoundHits->minBinY(), propNbFoundHits->maxBinY(), propNbFoundHits->nBinY());

  for (; iterS != m_nameMapSector.end(); ++iterS )
  {
    channelID = iterS -> second -> elementID();
    nick   = iterS -> second -> nickname();
    presentCut = m_whichCut[ iterS->second->isStereo() ? 1 : 0 ];
      
    root = "perSector";
    
    // how many were expected ?
    nExpected = static_cast<double>( m_expectedSector[ iterS -> first ] );
    
    if (nExpected >= 1)
    {
      if(fullDetail())
        plot(-10., root + "/NbFoundHits_" + nick,
             "Number of Found Hits vs Cut",
             m_spacialCuts.front() - m_binSize / 2.,
             m_spacialCuts.back() + m_binSize / 2.,
             m_binNumber, nExpected);
      if(fullDetail())
        plot(-10., root + "/NbExpHits_" + nick,
             "Number of Found Hits vs Cut",
             m_spacialCuts.front() - m_binSize / 2.,
             m_spacialCuts.back() + m_binSize / 2.,
             m_binNumber, nExpected);
      if(m_effPlot){
        if(fullDetail())
          profile1D(-10., nExpected, root + "/Eff_" + nick,
                    "Efficiency vs Cut",
                    m_spacialCuts.front() - m_binSize / 2.,
                    m_spacialCuts.back() + m_binSize / 2.,
                    m_binNumber);
      }
      if(m_detType=="IT"){
        ST::ITDetectorPlot::Bins theBins = static_cast<ST::ITDetectorPlot*>(propSectorStatus)->toBins(channelID); 
        histoSectorStatus -> fill( theBins.xBin, theBins.yBin, 1 );
      }else{
        ST::TTDetectorPlot::Bins theBins = 
          static_cast<ST::TTDetectorPlot*>(propSectorStatus)->toBins(static_cast<DeTTSector*>(iterS->second)); 
        for(int ybin(theBins.beginBinY);ybin<theBins.endBinY;ybin++){
          histoSectorStatus -> fill( theBins.xBin, ybin, 1 );
        }
      }
      
      for (i = 0; i < m_NbrOfCuts; ++i)
	    {
	      nFound = static_cast<double>( m_foundSector[i][iterS->first] );
	      eff = 100. * nFound / nExpected;
	      err = sqrt( eff * (100. - eff) / nExpected );
       
        if(fullDetail())
          plot(m_spacialCuts[i], root + "/NbFoundHits_" + nick,
               "Number of Found Hits vs Cut",
               m_spacialCuts.front() - m_binSize / 2.,
               m_spacialCuts.back() + m_binSize / 2.,
               m_binNumber, nFound);
        if(fullDetail())
          plot(m_spacialCuts[i], root + "/NbExpHits_" + nick,
               "Number of Exp Hits vs Cut",
               m_spacialCuts.front() - m_binSize / 2.,
               m_spacialCuts.back() + m_binSize / 2.,
               m_binNumber, nExpected);
        if(m_effPlot){
          if(fullDetail())
            profile1D(m_spacialCuts[i], eff, root + "/Eff_" + nick,
                      "Efficiency vs cut",
                      m_spacialCuts.front() - m_binSize / 2.,
                      m_spacialCuts.back() + m_binSize / 2.,
                      m_binNumber);
        }
        if ( i == presentCut )
        {
          verbose() << nick << ' ' << eff
                    << " +/- " << err << " (found " << nFound
                    << " for cut " << m_spacialCuts[i] << ")" << endmsg;
          
          if(m_detType=="IT"){
            ST::ITDetectorPlot::Bins theBins = static_cast<ST::ITDetectorPlot*>(propSectorStatus)->toBins(channelID); 
            if(m_effPlot)
              histoEff -> fill( theBins.xBin, theBins.yBin, eff );
            histoNbHits -> fill( theBins.xBin, theBins.yBin, nExpected );
            histoNbFoundHits -> fill( theBins.xBin, theBins.yBin, nFound );
          }else{
            ST::TTDetectorPlot::Bins theBins = 
              static_cast<ST::TTDetectorPlot*>(propSectorStatus)->toBins(static_cast<DeTTSector*>(iterS->second)); 
            for(int ybin(theBins.beginBinY);ybin<theBins.endBinY;ybin++){
              if(m_effPlot)
                histoEff -> fill( theBins.xBin, ybin, eff );
              histoNbHits -> fill( theBins.xBin, ybin, nExpected );
              histoNbFoundHits -> fill( theBins.xBin, ybin, nFound );
            }
          }
        }
	    } // i
    } //nExpected
    else
    {
      if(m_detType=="IT"){
        ST::ITDetectorPlot::Bins theBins = static_cast<ST::ITDetectorPlot*>(propSectorStatus)->toBins(channelID); 
        if(m_effPlot)
          histoEff -> fill( theBins.xBin, theBins.yBin, -1 );
        histoSectorStatus -> fill( theBins.xBin, theBins.yBin, -1 );
        histoNbHits -> fill( theBins.xBin, theBins.yBin, 0 );
        histoNbFoundHits -> fill( theBins.xBin, theBins.yBin, 0 );
      }else{
        ST::TTDetectorPlot::Bins theBins = 
          static_cast<ST::TTDetectorPlot*>(propSectorStatus)->toBins(static_cast<DeTTSector*>(iterS->second));
        for(int ybin(theBins.beginBinY);ybin<theBins.endBinY;ybin++){
          if(m_effPlot)
            histoEff -> fill( theBins.xBin, ybin, -1 );
          histoSectorStatus -> fill( theBins.xBin, ybin, -1 );
          histoNbHits -> fill( theBins.xBin, ybin, 0 );
          histoNbFoundHits -> fill( theBins.xBin, ybin, 0 );
        }
      }
    }
  } // iterS

  delete prop;
  delete propSectorStatus;
  delete propNbHits;
  delete propNbFoundHits;
  
  return TrackMonitorTupleBase::finalize();  // must be called after all other actions
} // STTrackTuple::finalize()


/**
 * Stupid function that allows to print the number with 2 decimals,
 * since using std::setprecision and std::fixed directly in the info()
 * screws everythings up.
 *
 * @param nbr number (double) to print
 * @param digits number of wanted decimal (2 is the default)
 *
 * @return a string with the correct number of decimals.
 */
std::string STTrackTuple::formatNumber(const double& nbr,
                                       const unsigned int& digits ) const
{
  std::stringstream ss;
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss << std::setprecision( digits ) << nbr;
  return ss.str();
} // STTrackTuple::formatNumber()


/**
 * Checks whether a track crosses enough stations.
 *
 * @return \e true if yes.
 */
bool STTrackTuple::hasMinStationPassed(const LHCb::Track* aTrack) const
{
  std::set<unsigned int> stations;
  auto fun = ( m_detType == "IT" ? &LHCb::LHCbID::isIT : &LHCb::LHCbID::isTT );
  for (const auto& id : aTrack -> lhcbIDs() ) {
    if ( (id.*fun)()) stations.insert( id.stID().station() );
  }
  if(fullDetail())
    plot(stations.size(), "Control/NbCrossedStation","Number of stations crossed by the track",-0.5,3.5,4);
  return stations.size() >= m_minStationPassed;
} // STTrackTuple::hasMinStationPassed()


/**
 * Find to what sensor of the given sector a strip belongs.
 *
 * @param sector sector to which the strip belongs
 * @param id STChannelID of the strip
 *
 * @return the sensor.
 */
DeSTSensor* STTrackTuple::findSensor(const DeSTSector* sector,
                                    STChannelID id)
{
  DeSTSensor* foundSensor = new DeSTSensor();
  for (const auto& sensor : sector->sensors() ) {
    if ( sensor->contains(id) ) foundSensor = sensor;
  }
  return foundSensor;
} // STTrackTuple::findSensor()


/**
 * Check if a layer was crossed by a track
 *
 * @param aLayer handle to the DeSTlayer
 *
 * @return true if yes.
 */
bool STTrackTuple::crossedLayer(const LHCb::Track* aTrack,
                                DeSTLayer * &aLayer) const
{
  const auto& ids = aTrack -> lhcbIDs();
  auto i = std::find_if( ids.begin(), ids.end(), 
                         [&](const LHCb::LHCbID& id) { return aLayer->contains( id.stID() ); } );
  return i!=ids.end();
} // STTrackTuple::crossedLayer()


/**
 * Counts the number of hits on a layer
 *
 * @param measVector vector of measurements on the track
 * @param aLayer handle to the DeSTlayer
 *
 * @return number of hits on the layer.
 */
unsigned int STTrackTuple::hitsOnLayer(const std::vector<const LHCb::STMeasurement*>& measVector,
                                     DeSTLayer * aLayer) const
{
  return std::count_if( measVector.begin(), measVector.end(), 
                        [&](const LHCb::STMeasurement* m) { 
      return aLayer->contains( m->channelID() );
  });
} // STTrackTuple::hitsOnLayer()


/**
 * Counts the number of hits on a layer
 *
 * @param hits hits found by STClusterCollector
 * @param aLayer handle to the DeSTlayer
 *
 * @return number of hits on the layer.
 */
unsigned int STTrackTuple::hitsOnLayer(const ISTClusterCollector::Hits& hits,
                                    DeSTLayer *aLayer) const
{
  return std::count_if( hits.begin(), hits.end(), [&](const ISTClusterCollector::Hit& hit) {
    return aLayer->contains( hit.cluster->channelID() ); 
  });
} // STTrackTuple::hitsOnLayer()


/**
 * Find the LHCbID of an STMeasurement
 *
 * @param aTrack handle to the track
 * @param aHit an STMeasurement
 *
 * @return the LHCbID.
 */
LHCb::LHCbID STTrackTuple::findHitId(const LHCb::Track* aTrack,
                                     const LHCb::STMeasurement* aHit) const
{
  LHCb::LHCbID hitId(0);
  const std::vector<LHCb::LHCbID>& ids = aTrack -> lhcbIDs();
  for (auto iter = ids.begin(); iter != ids.end(); ++iter)
  {
    if ( (aHit->cluster())->channelID() == iter->stID() )
      hitId = (*iter);
  }
  return hitId;
} // STTrackTuple::findHitId()


// Hit type namespace
namespace {
  enum HitType {VeloR=0, VeloPhi, VPX, VPY, TT, IT, OT, Muon, HitTypeUnknown} ;
  const std::string HitTypeName[] = {"VeloR","VeloPhi","VPX","VPY","TT","IT","OT","Muon"} ;
  //double      HitTypeMaxRes[] = {0.1,0.1,0.1,0.1,0.5,0.5,2.0,10} ;
  
  inline HitType hittypemap( const LHCb::Measurement& meas ) {
    LHCb::Measurement::Type type = meas.type() ;

    HitType rc = HitTypeUnknown ;
    switch( type ) {
    case LHCb::Measurement::Calo:
    case LHCb::Measurement::Origin :
    case LHCb::Measurement::Unknown: rc = HitTypeUnknown ; break ;
      
    case LHCb::Measurement::VP:
      {
        const LHCb::VPMeasurement* vpmeas = dynamic_cast<const LHCb::VPMeasurement*>(&meas);
        rc = vpmeas && vpmeas->projection() == LHCb::VPMeasurement::X ? VPX : VPY;
      }
      break;
    case LHCb::Measurement::VeloLiteR:
    case LHCb::Measurement::VeloR:   rc = VeloR ; break ;

    case LHCb::Measurement::VeloLitePhi:
    case LHCb::Measurement::VeloPhi: rc = VeloPhi ; break ;

    case LHCb::Measurement::UT :
    case LHCb::Measurement::UTLite :
    case LHCb::Measurement::TTLite:
    case LHCb::Measurement::TT: rc = TT; break;

    case LHCb::Measurement::FT :
    case LHCb::Measurement::ITLite:
    case LHCb::Measurement::IT: rc = IT; break;

    case LHCb::Measurement::OT: rc = OT; break;

    case LHCb::Measurement::Muon: rc = Muon; break;
    }
    return rc;
  }
} // end (namespace)


/**
 * Find the error on the residual of a hit
 *
 * @param aTrack handle to the track
 * @param aID the LHCbID of the hit strip
 *
 * @return the square of the residual error.
 */
double STTrackTuple::retrieveErrResidual2(const LHCb::Track* aTrack,
                                        LHCb::LHCbID aID) const
{
  double errResidual2(0);
  HitType mtype = VeloR; // initialize to avoid compiler warning
  for( const auto& node: aTrack -> nodes() ) 
  {
    if( node->type() == LHCb::Node::HitOnTrack 
        // discard extremely small fraction of hits with zero error
        // on residual. (e.g. a downstream track with only one
        // active TT hit)
        && node->errResidual2() > TrackParameters::lowTolerance 
        && (mtype = hittypemap( node->measurement() ) )!=HitTypeUnknown )
    {
      if ( node->measurement().lhcbID() == aID )
      {
        errResidual2 = node->errResidual2();
      }// end if (node is associated to the current measurement)
    }// end if (node not discarded)
  }// end loop on nodes
  return errResidual2;
} // STTrackTuple::retrieveErrResidual2

//=============================================================================
