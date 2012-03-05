// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Math definitions
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// From LHCb
#include "Kernel/Trajectory.h"
#include "Kernel/LHCbID.h"
#include "Kernel/FastClusterContainer.h"
#include "Event/Track.h"
#include "Event/TrackTypes.h"

#include "Event/STLiteCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"
// local
#include "SeedHit.h"
#include "TrackSeedHit.h"
#include "TrackCand.h"
#include "TrackSeedFind.h"
//tools
#include "Kernel/IBIntegrator.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackSeedFind
//
// 2005-12-14  HJ Bulten
// Last update: 27 july 2006.
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackSeedFind )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackSeedFind::TrackSeedFind( const std::string& name, ISvcLocator* pSvcLocator) : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ITDetLocation",    m_itTrackerPath = DeSTDetLocation::IT); // IT detector
  declareProperty( "OTDetLocation",    m_otTrackerPath = DeOTDetectorLocation::Default); // OT detector
  declareProperty( "maxXSlope",    m_maxXSlope = 0.6 ); // maximum tangent of slope in x/z plane after magnet
  declareProperty( "sigmaOutlierX", m_sigmaOutlierX = 3. ); // standard deviations that a single hit in an x-plane may be away from the current track candidate
  declareProperty( "sigmaOutlierY", m_sigmaOutlierY = 4. ); // idem, for hits in a y-plane
  declareProperty( "YSlopeDevMax", m_YSlopDev = 0.01 ); // maximum deviation of the slope in y from a track coming from the origin
  declareProperty( "usePtKick", m_usePtKick = true ); // use magnetic field info and the PtKick tool to create a State
  declareProperty( "useSpilloverKiller", m_useSpilloverKiller = true ); // kill spill-over tracks. The Seeding finds about 30 percent spill-over tracks
  declareProperty( "OTTimeAv", m_otTimeAv = 29 ); // average value of OT times for a valid track
  declareProperty( "OTTimeWindow", m_otTimeWindow = 12 ); // width of window of acceptable OT times
  declareProperty( "inputITClusterpath", m_ITclusterPath = "Raw/IT/LiteClusters" ); // STLiteClusters
  declareProperty( "inputOTTimepath", m_OTtimePath = LHCb::OTTimeLocation::Default ); // OTTimes
  declareProperty( "outputtracklocation", m_trackLocation = LHCb::TrackLocation::Seed ); 
  declareProperty( "useIT", m_useIT=true ); // if false, only the OT is considered
  declareProperty( "useOT", m_useOT=true ); // if false, only the IT is considered
  declareProperty( "maxOThits", m_maxOTHits = 10000 ); // maximum number of hits that are allowed. 5000 is about 10 percent average occupancy 
  declareProperty( "minXplane", m_minX13=2 ); // minimum number of OT hits per X-station on straight line track candidates through station 1 and 3
  declareProperty( "minYplane", m_minY13=2 ); // same, for the Y-planes
  declareProperty( "minT1T3", m_minT1T3=5 ); // minimum number of hits in X and also in Y for a track candidate through chamber 1 and 3 (so 5 imlies minimally 5 X hits and 5 Y hits, or 3 IT X hits and 3 IT Y hits
  declareProperty( "minUV13" , m_minUV13=1 ); // minimum number of hits in u-direction and in v direction in stations 1 and 3 combined
  declareProperty( "minUV123" , m_minUV123=2 ); // same, for stations 1,2, and 3 combined
  declareProperty( "minX123", m_minX123=7); // minimum number of x-hits on the parabola through station 1,2,3
  declareProperty( "minY123", m_minY123=7); // minimum number of y-hits on the parabola through station 1,2,3
  declareProperty( "minT123", m_minT123=16); // minimum number of OT hits on the track
  declareProperty( "maxChi2", m_maxChi2=6.0 ); // maximum allowed chi2 for all fits
}


//=============================================================================
// Destructor
//=============================================================================
TrackSeedFind::~TrackSeedFind() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackSeedFind::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  
  m_debug   = msgLevel( MSG::DEBUG );   // calculate some additional statistics
  m_verbose = msgLevel( MSG::VERBOSE ); // calculate some additional statistics
  info() << "Init Seeding/TrackSeedFind " << endmsg;
  if(m_debug)
    debug() << "==> Initialize" << endmsg
            << "TrackSeedFind::initialize Start reading geometry ..." << endmsg;

  m_itTracker = getDet<DeSTDetector>( m_itTrackerPath );
  m_otTracker = getDet<DeOTDetector>( m_otTrackerPath );

  if (!m_itTracker) return StatusCode::FAILURE; // no pointer to the IT detector
  if (!m_otTracker) return StatusCode::FAILURE; // no pointer to the OT detector
  if (!(m_useIT||m_useOT)) return StatusCode::FAILURE; // try to do seeding without inner or outer tracker hits
  m_BAngleChange=0;
  m_zRotPoint = 8000;
  // find z-positions of OT stations
  std::vector<DeOTStation*> stations = m_otTracker->stations();
  if (stations.size() != 3) {
    error() << " TrackSeedFind: not 3 OT stations in detector geometry found " << endmsg;
	  return StatusCode::FAILURE;
  }

  m_zmin[0]=m_zmin[1]=m_zmin[2]=1000000;
  m_zmax[0]=m_zmax[1]=m_zmax[2]=-1000000;
  m_zMagnet=5500; // will be resetted by the call to the magnetic field calculation.

  if ( m_debug ) debug() << " OT detector: " << stations.size() << " found " << endmsg;
  m_OTWireDriftTime=m_otTracker->propagationDelay();
  m_OTDriftSpeed=1.0/m_otTracker->driftDelay();
  m_OTCellRadius=m_otTracker->param<double>("cellRadius");
  // initializations for SeedHit:
  SeedHit::OT_driftspeed = m_OTDriftSpeed;
  SeedHit::OT_wiredrifttime = m_OTWireDriftTime;
  SeedHit::OT_cellradius = m_OTCellRadius;
  SeedHit::OTResolution = m_otTracker->resolution();
  // test if double resolution this is necessary
  //   info() << "For the moment, double the OT resolution " << endmsg;
  //   SeedHit::OTResolution *= 2;
   
  m_minstereo=100.0; // stereo angle between these values
  m_maxstereo=0.0; // stereo angle between these values

  // calculate the minimum and maximum z-positions of stations 1,2,3
  if (m_useOT) for (unsigned int i=0; i<stations.size(); ++i) {
    std::vector<DeOTLayer*> lays = stations[i]->layers();
    for (unsigned int j=0; j<lays.size(); ++j) {
      Gaudi::XYZPoint pmax(0,2.5,0); // 2.5 for the maximum position in the layer 
      Gaudi::XYZPoint qmax = (lays[j])->geometry()->toGlobal(pmax);
      Gaudi::XYZPoint pmin(0,-2.5,0); // 2.5 for the maximum position in the layer 
      Gaudi::XYZPoint qmin = (lays[j])->geometry()->toGlobal(pmin);
      double min=qmin.z();
      double max=qmax.z();
      if (min>max) {
        min=max;
        max=qmin.z();
      }
      min -= m_OTCellRadius;
      max += m_OTCellRadius;
      if (max>m_zmax[i]) m_zmax[i]=max;
      if (min<m_zmin[i]) m_zmin[i]=min;
      double ang = (lays[j]->angle());
      ang=fabs(ang);
      if (ang>0.001) {
        if (ang<m_minstereo) m_minstereo=ang;
        if (ang>m_maxstereo) m_maxstereo=ang;
      }
    }
  }

  // 
  // Now, minimum and maximum z-positions for the Inner Tracker
  // unfortonately, there is no method to get all layers in a station.
  // therefore I assume geometry knowledge: the IT layers are positioned closely before the OT layers
  //
  std::vector<DeSTLayer*> ITlays =m_itTracker->layers();
  if (m_useIT) for (unsigned int j=0; j<ITlays.size(); ++j) {
    std::vector<double> stationboundaries; // separates stations 1,2,3
    if (m_useOT) for (unsigned int i=0; i< 3; ++i) stationboundaries.push_back(m_zmax[i]);
    else {
      stationboundaries.push_back(8100);
      stationboundaries.push_back(8700);
      stationboundaries.push_back(9400);
    }
    Gaudi::XYZPoint pcent(0,0,0);
    Gaudi::XYZPoint qcent = (ITlays[j])->geometry()->toGlobal(pcent);
    double min=qcent.z();
    unsigned int i=0;
    if (min<7300) {
      Warning(" IT detector: layer at odd z encountered ",StatusCode::SUCCESS).ignore();
      if(m_debug) debug() << " IT detector: layer at z= " << min << "encountered" << endmsg;
      continue; // do not use this layer.
    }
    while ((min>stationboundaries[i]) && (i<2)) ++i;
    double ang = (ITlays[j]->angle());
    if (min<m_zmin[i]) m_zmin[i]=min;
    if (min>m_zmax[i]) m_zmax[i]=min;
    ang=fabs(ang);
    if (ang>0.001) {
      if (ang<m_minstereo) m_minstereo=ang;
      if (ang>m_maxstereo) m_maxstereo=ang;
    }
  }

  if (m_usePtKick) {
    m_fCalcMomentum = tool<ITrackMomentumEstimate>("TrackPtKick");
    info() << "In init, PTKick from geometry. n stations = " <<
      m_otTracker->nStation() << " zmin = " << m_zmin[0] << " zmax= " << m_zmax [2]<< endmsg;
    // check, if plane positions are reasoneable
    // i.e. between 5 and 20 meters, and not overlapping
    bool OK = (m_zmin[0] > 5000);
    if (OK) OK = ((m_zmax[0]>m_zmin[0]) && (m_zmin[1] > m_zmax[0]));
    if (OK) OK = ((m_zmax[1]>m_zmin[1]) && (m_zmin[2] > m_zmax[1]));
    if (OK) OK = ((m_zmax[2]>m_zmin[2]) && (m_zmax[2] < 20000));
    if (!OK) {
      Warning("Failed to retrieve correct z-positions from the OT stattions in TrackSeedFind ").ignore();
      if (m_debug) debug() << "Failed to retrieve correct z-positions from the OT stattions in TrackSeedFind "
                           << " Positions : " <<
        m_zmin[0] << " " << m_zmax[0] << " " <<
        m_zmin[1] << " " << m_zmax[1] << " " <<
        m_zmin[2] << " " << m_zmax[2] << " " << endmsg;
      return StatusCode::FAILURE;
    }

    // z-positions are known now. 
    // Calculate how much Bfield integral there is between zmax and zmin and the center of station 2
    Gaudi::XYZPoint orig(0,0,0);
    double tX=0.02;
    double tY=0.1;
    Gaudi::XYZPoint pmin(0,m_zmin[0]*tY,m_zmin[0]);
    Gaudi::XYZPoint pmax(0,m_zmax[2]*tY,m_zmax[2]);
    Gaudi::XYZPoint pmid(0,(m_zmax[1]+m_zmin[1])/2.0*tY,(m_zmax[1]+m_zmin[1])/2.0);
    Gaudi::XYZVector Bdl;
    IBIntegrator* MagTool;
    StatusCode sc; 
    MagTool = tool<IBIntegrator>("BIntegrator");

    if (!(sc.isSuccess())) {
      error() << "Failed to retieve magnetic field tool in TrackSeedFind " << endmsg; 
      return sc;
    }
    sc = MagTool->calculateBdlAndCenter(orig,pmax,tX,tY,m_zMagnet,Bdl);
    if (sc.isFailure()) { 
      error() << " In TrackSeedFind Init: Failed to calculate magnetic field " << endmsg;
      return sc;
    }
    double Bmax=sqrt(Bdl.Mag2());
    sc = MagTool->calculateBdlAndCenter(orig,pmin,tX,tY,m_zMagnet,Bdl);
    if (!(sc.isSuccess())) { 
      error() << " In TrackSeedFind Init: Failed to calculate magnetic field " << endmsg;
      return sc;
    }
    double Bmin=sqrt(Bdl.Mag2());
    sc = MagTool->calculateBdlAndCenter(orig,pmid,tX,tY,m_zMagnet,Bdl);
    if (!(sc.isSuccess())) { 
      error() << " In TrackSeedFind Init: Failed to calculate magnetic field " << endmsg;
      return sc;
    }
    toolSvc()->releaseTool(MagTool);
    double Bmid=sqrt(Bdl.Mag2());
    if (Bmax*Bmin*Bmid<1e-12) {
      Warning(" Zero magnetic field encountered, but Bfield corrections are requested in Seeding! ",StatusCode::SUCCESS).ignore();
      m_BAngleChange = 0;
    } else {
      //
      // now we have the Bdl values from the origin to T1, T2, T3        
      // from this, we calculate the fractional change in bending angle between
      // T1 and T3 with respect to the total angle.
      // 
      // I calculate the track that is found between T1 and T3 and estimate where
      // it should cross in T2, assuming that the change of angle occurs close
      // to T1, namely at (Bmax-Bmid)*zmax+(Bmid-Bmin)*zmin/(Bmax-Bmin)
      //
      if ((Bmax-Bmin)/Bmax < 1e-7) {
        warning() << "In TrackSeedFind Init: no change of field between OT station 1 and 3 found!" << endmsg;
        m_BAngleChange = 0;
      } else {
        m_zRotPoint = (Bmax-Bmid)*m_zmax[2]+(Bmid-Bmin)*m_zmin[0];
        m_zRotPoint /= (Bmax-Bmin);
        m_BAngleChange = 1-Bmin/Bmax;
      } // end if Bmax-Bmin >0.001% 
      if ( m_debug )
        debug() << " Magnet info : " << Bmin << " " << Bmid << " " << Bmax << endmsg
                << " rotation point, angle change " << m_zRotPoint << " " << m_BAngleChange << endmsg;
    }  // end if Reasoneable fields found
  } // end if use PtKick

  // add the cluster position tool. This is necessary to create measurements
  m_itPositionTool = tool<ISTClusterPosition>("STOfflinePosition/ITClusterPosition");

  return StatusCode::SUCCESS;
}


//=============================================================================
//
// Main execution
//=============================================================================
StatusCode TrackSeedFind::execute() {
  //  start timers, if debug mode is on
  IChronoStatSvc* svc = chronoSvc(); 
  if (m_debug) svc->chronoStart( "all" );

  //clear all hit multimaps
  T1X.clear();
  T1Y.clear();
  T2X.clear();
  T2Y.clear();
  T3X.clear();
  T3Y.clear();

  //register tracks store
  LHCb::Tracks* mytracks;
  if (exist<LHCb::Tracks>(m_trackLocation)) {
    mytracks = get<LHCb::Tracks>(m_trackLocation);;
  } else {
    mytracks = new LHCb::Tracks();
    put ( mytracks , m_trackLocation );
  } 

  // initialize arrays with hits
  if ( m_debug ) {
    debug() << "TrackSeedFind::getData " ;
    svc->chronoStart( "getData" );
  }
  getData();
  if (m_debug) svc->chronoStop( "getData" );
  double combis=T1X.size()*T1Y.size();
  combis *= T2X.size()*T2Y.size();
  combis *= T3X.size()*T3Y.size();
  if ( m_debug ) debug() << combis << " hit combinations " << endmsg;
  if (combis<1) return StatusCode::SUCCESS; // a plane is missing

  // find the seeds
  if ( m_debug ) {
    debug() << "TrackSeedFind:: search routine" << endmsg;
    svc->chronoStart( "search" );
  }
  findSeeds();
  if (m_debug) {
    svc->chronoStop( "search" );
    debug() << "spill-over and clone killer" << endmsg;
    svc->chronoStart( "select" );
  }
   
  // spill-over killer: few ms per event
  if (m_useSpilloverKiller) pileUpKiller(); // takes microseconds, no need to time it
  // clone killer: on average about 20 clones per OT track due to acceptable ambiguity combinations
  cloneKiller();
  if (m_debug) svc->chronoStop( "select" );

  // make tracks: get the seeds in an array that can be read by the outside world
  // and add the PState
  if (m_debug) {
    debug() << "maketracks" << endmsg;
    svc->chronoStart( "maketracks" );
  }
  maketracks();
  if (m_debug) {
    svc->chronoStop( "maketracks" );
    svc->chronoStop( "all" );
    svc->chronoPrint("all");
    debug() << " delta " << (svc->chronoDelta("all",IChronoStatSvc::USER)/1e6) << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TrackSeedFind::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================



//=============================================================================
// Algorithm Methods
//=============================================================================

//=============================================================================
//
//=============================================================================

StatusCode TrackSeedFind::getData() {
  // 
  // getData: fill the It and OT clusters; for every hit fill a plane to which it belongs.
  //
  if (m_debug)
    debug() << " SeedHit speed " << SeedHit::OT_driftspeed << endmsg
            << " SeedHit wiretime " << SeedHit::OT_wiredrifttime << endmsg
            << " SeedHit cellrad " << SeedHit::OT_cellradius << endmsg
            << " SeedHit resol. " << SeedHit::OTResolution << endmsg;

  // Fill outer tracker hits
  LHCb::OTTimes::iterator It2;
  if (m_useOT) {
    SmartDataPtr<LHCb::OTTimes> OTTimes (eventSvc(), m_OTtimePath );
    if ( 0 == OTTimes) {
      Warning(" No OTTimes container! continuing without OT ",StatusCode::SUCCESS).ignore();
    } else if ((int)OTTimes->size()>m_maxOTHits) {
      Warning(" Too many OT hits, too high multiplicity",StatusCode::SUCCESS,0).ignore();
      if (m_debug) debug() << " Too many OT hits, too high multiplicity " << OTTimes->size() 
                           << " Skipping OT for this event " << endmsg;
      // return StatusCode::SUCCESS;
    }
    else for (It2 = OTTimes->begin(); It2 != OTTimes->end(); ++It2) {
      SeedHit hit(*It2, m_otTracker);
      std::pair<double,SeedHit> p = std::make_pair(hit.x(0.0),hit);
      // store hit in multimap; according to x-position of cluster
      if (hit.station() ==  1) { // station 1
        if (hit.isStereo()) T1Y.insert(p);
        else T1X.insert(p);
        /*
          if (m_debug) {
          TrackSeedHit sh(&hit);
          debug() << "hits :stereo x,y cent " << sh.isStereo() << " " << sh.x() << " " << sh.y() <<
          " ymin,max " << sh.y_min() << " " << sh.y_max() <<
          " x false,true " << sh.x(false) << " " << sh.x(true) << endmsg <<
          " x,y top left " << sh.x(false,sh.y_max()) << " " << sh.y(false,sh.y_max()) <<
          " x-diff top " << sh.x(false,sh.y_max())-sh.x(true,sh.y_max()) <<
          " x-diff bot " << sh.x(false,sh.y_min())-sh.x(true,sh.y_min()) <<
          endmsg << " Z : " << sh.z() << " " << sh.z(sh.x(),sh.y()) << " at 0,0 " << sh.z(0,0) << " at xcent,-2000 " << sh.z(sh.x(),-2000) <<
          endmsg;
          }
        */
      }
      if (hit.station() ==  2) { // station 1
        if (hit.isStereo()) T2Y.insert(p);
        else T2X.insert(p);
      }
      if (hit.station() ==  3) { // station 1
        if (hit.isStereo()) T3Y.insert(p);
        else T3X.insert(p);
      }
    } // if OTTimes present, and hits<OTMaxHits
  } // if use OT
  if (m_debug) debug() << " number of hits after OT : 1X " 
                       << T1X.size() << " 1Y " << T1Y.size() << " 2X "
                       << T2X.size() << " 2Y " << T2Y.size() << " 3X "
                       << T3X.size() << " 3Y " << T3Y.size() << endmsg;
 
  if (m_useIT) {
    typedef FastClusterContainer<LHCb::STLiteCluster, int> STLiteClusters;
    STLiteClusters* ITClus = get<STLiteClusters>( m_ITclusterPath );
    if ( m_verbose )
      verbose() << " ITClus pointer " << ITClus << endmsg;
   
    LHCb::STLiteCluster::FastContainer::iterator It;
    //LHCb::STLiteCluster* itclus;
    //
    // temporary: get maximum wire angle in getdata, from first event
    //
    if (!(ITClus))  Warning("No STLiteClusters were found for IT in Seeding",StatusCode::SUCCESS,1).ignore();
    else for (It = ITClus->begin(); It != ITClus->end(); ++It) {
      if ( m_verbose ) verbose() << " IT ";
      //itclus = (LHCb::STLiteCluster *) &(*It);
      SeedHit hit(&(*It), m_itTracker);
      // Note , that the TrackSeedHit constructor fills in all relevant information, up to the
      // full alignment offsets of the hit.
   
      // store hit in multimap; according to x-position of cluster
      std::pair<double,SeedHit> p = std::make_pair(hit.x((double) 0.0),hit);
      if (hit.station() ==  1) { // station 1
        if (hit.isStereo()) T1Y.insert(p);
        else T1X.insert(p);
      }
      if (hit.station() ==  2) { // station 1
        if (hit.isStereo()) T2Y.insert(p);
        else T2X.insert(p);
      }
      if (hit.station() ==  3) { // station 1
        if (hit.isStereo()) T3Y.insert(p);
        else T3X.insert(p);
      }
    } // if IT Clusters are present
  } // if use IT
  if (m_debug) debug() << " number of hits after IT AND OT : 1X " 
                       << T1X.size() << " 1Y " << T1Y.size() << " 2X "
                       << T2X.size() << " 2Y " << T2Y.size() << " 3X "
                       << T3X.size() << " 3Y " << T3Y.size() << endmsg;
   
  if (T1X.size()*T1Y.size()*T2X.size()*T2Y.size()*T3X.size()*T3Y.size()<1) {
    Warning(" TrackSeedFind : event without hits in one of the tracker stations encountered ",StatusCode::SUCCESS,1).ignore();
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackSeedFind::findSeeds() {
  // this method finds the seeds
  // clear tracks of previous event
  trackCands.clear();

  // define temporary maps
  std::multimap<double, SeedHit>::iterator x1It,x3It,tempIt,x2It,lowerIt,tempItY3,lowerItX3; 
  std::vector<double> xvec,zvec,error; // for the line/parabola fit
  std::vector<unsigned int> skipped; // for the line/parabola fit
  std::vector<TrackSeedHit> Xcands; // temporary candidates
  std::vector<TrackSeedHit> xhits;
  std::vector<double> y1vec,y1zvec,y1error; // station 1 candidates in Y-planes
  std::vector<unsigned int> skippedy; // for the line/parabola fit
  std::vector<TrackSeedHit> Ycands,Y1cands; // temporary candidates
  std::vector<TrackCand> tempcands; // temporary candidates
  std::vector<TrackCand> trackCandsT1T3; // temporary candidates
  // the nine multimaps below are to keep track of hit combinations that have
  // already been considered in the TrackSeedFind loops
  std::multimap<LHCb::STChannelID,LHCb::STChannelID> ITmap;
  std::multimap<LHCb::OTChannelID,LHCb::STChannelID> OTL_ITmap, OTR_ITmap;
  std::multimap<LHCb::OTChannelID,LHCb::OTChannelID> OTR_OTRmap,OTR_OTLmap, OTL_OTRmap, OTL_OTLmap;
  std::multimap<LHCb::STChannelID,LHCb::OTChannelID> IT_OTLmap,IT_OTRmap;

  // debug vector to see if the amount of loops is what I expect
  std::vector<int> debugloops;
  vector<double> fitpars; // trials in X. For speed, make the vector outside loop 9
  vector<double> Yfitpars; // for fit of Y-seeds. For speed, make the vector outside loop 9
  TrackCand newcand;
  for (int i=0; i<5; ++i) Yfitpars.push_back(i); // make size 5.

  trackCandsT1T3.clear();
  ITmap.clear();
  IT_OTLmap.clear();
  IT_OTRmap.clear();
  OTL_ITmap.clear();
  OTL_OTLmap.clear();
  OTL_OTRmap.clear();
  OTR_ITmap.clear();
  OTR_OTLmap.clear();
  OTR_OTRmap.clear();
  debugloops.clear();

  // predefine the possibly billions of sine calculations
  double sinminstereo=sin(m_minstereo);
  double sinmaxstereo=sin(m_maxstereo);

  bool ambigX1,ambigX3; // left/right amibiguity of the trial hit in station X1, X3
  double xmaxMag = m_zMagnet*0.4; // maximum position in center magnet: +/-400 mrad from IP
  if (m_debug) {
    while (debugloops.size() < 25) debugloops.push_back(0);
  }
  // start with choosing an x-hit from station 1
  for (x1It = T1X.begin(); x1It != T1X.end(); ++x1It) {
    if ( m_verbose ) verbose() << " in outer X1 loop ";
    TrackSeedHit hitX1(&x1It->second);
    int ambi=0; // loop variable, loop in case of OT over both ambiguities
    double x1,y1min,y1max,y3min,y3max,x3;
    double z1,z3;
    int enoughX1,enoughX3;
    if (hitX1.isIT()) ambi=1;
    else if (fabs(hitX1.x(false)-hitX1.x(true))<0.001) ambi=1; // OT hit drift time smaller than 0: hit is placed on wire
    // loop over both ambiguities, for IT hit, this loop is performed only once
    for (; ambi<2; ++ambi) { 
      if (m_debug) ++(debugloops[0]);
      if ( m_verbose )
        verbose() << " x1 ambig " << hitX1.x() << " " << ambi << endmsg;
      ambigX1=(ambi!=0);
      hitX1.setAmbiguity(ambigX1);
      x1=hitX1.x(ambigX1);
      z1=hitX1.z();
      y1min=hitX1.y_min(); // 10 mm allowance; the planes are at different z
      y1max=hitX1.y_max();
      //
      // loop over T3X
      // I check all T3 x hits within about 1 rad after the magnet
      // and about 0.4 rad maximum scattering angle in IT
      //
      double xmaxproj=((m_zmax[2]-z1)*xmaxMag+(m_zmax[2]-m_zMagnet)*x1)/(z1-m_zMagnet);
      double xminproj=((m_zmax[2]-z1)*-xmaxMag+(m_zmax[2]-m_zMagnet)*x1)/(z1-m_zMagnet);
      if (xmaxproj>x1+m_maxXSlope*(m_zmax[2]-z1)) xmaxproj=x1+m_maxXSlope*(m_zmax[2]-z1);
      if (xminproj<x1-m_maxXSlope*(m_zmax[2]-z1)) xminproj=x1-m_maxXSlope*(m_zmax[2]-z1);
      if (xmaxproj<xminproj) {
        lowerItX3=T3X.end();
        // this condition is possible if the X1 hit is at such a large coordinate, that the track
        // could not come from target for bending angles smaller than about 1 rad;
        // i.e. the bending angle exceeds 1 rad
      } else {
        lowerItX3=T3X.lower_bound(xminproj);
      }
      for (x3It = lowerItX3; (x3It->first < xmaxproj) && (x3It != T3X.end()) ; ++x3It) {
        TrackSeedHit hitX3(&x3It->second);
        if ( m_verbose ) verbose() << " x1,x3 " << hitX1.x() << " "
                                   << (int) hitX1.ambiguity() << " "
                                   << hitX3.x()  << endmsg;
        int ambi3=0;
        if (hitX3.isIT()) ambi3=1;
        for (; ambi3<2; ++ambi3) {
          ambigX3=(ambi3!=0);
          hitX3.setAmbiguity(ambigX3);
          if (m_debug) ++(debugloops[1]);
          // here, check whether we have already had the current combination of
          // X1 and X3 hits. If this combination is in the map, than it has already
          // been considered in a previous attempt (including a hit at lower X)
          if (hitX1.isIT()) {
            if (hitX3.isIT()) {
              typedef std::multimap<LHCb::STChannelID,LHCb::STChannelID>::iterator It;
              std::pair<It,It> p;
              p=ITmap.equal_range(hitX1.getItRef()->channelID());
              It low=p.first;
              It up=p.second;
              bool skipthishit=false;
              for (It it=low; (it != up) ; ++it)
                if ((skipthishit = (it->second == hitX3.getItRef()->channelID())) ) break; // if this statement is true, the hit is in the mask
              if (skipthishit) continue; // check next X3 ambiguity
            } else {
              typedef std::multimap<LHCb::STChannelID,LHCb::OTChannelID>::iterator It;
              std::pair<It,It> p;
              if (ambigX1) p=IT_OTRmap.equal_range(hitX1.getItRef()->channelID());
              else p=IT_OTLmap.equal_range(hitX1.getItRef()->channelID());
              It low=p.first;
              It up=p.second;
              bool skipthishit=false;
              for (It it=low; (it != up) ; ++it)
                if ((skipthishit = (it->second == hitX3.getOtRef()->channel()))) break; // if this statement is true, the hit is in the mask
              if (skipthishit) continue; // check next X3 ambiguity
            }  // endif of X3 is IT or OT
          } else { // X1 is an OT hit  
            if (hitX3.isIT()) {
              typedef std::multimap<LHCb::OTChannelID,LHCb::STChannelID>::iterator It;
              std::pair<It,It> p;
              if (ambigX1) p=OTR_ITmap.equal_range(hitX1.getOtRef()->channel());
              else p=OTL_ITmap.equal_range(hitX1.getOtRef()->channel());
              It low=p.first;
              It up=p.second;
              bool skipthishit=false;
              for (It it=low; it!=up ; ++it)
                if ((skipthishit = (it->second == hitX3.getItRef()->channelID()))) break; // if this statement is true, the hit is in the mask
              if (skipthishit) continue; // check next X3 ambiguity
            } else { // X3 is is OT
              typedef std::multimap<LHCb::OTChannelID,LHCb::OTChannelID>::iterator It;
              std::pair<It,It> p;
              if (ambigX1) {
                if (ambigX3) p=OTR_OTRmap.equal_range(hitX1.getOtRef()->channel());
                else p=OTR_OTLmap.equal_range(hitX1.getOtRef()->channel());
              } else {
                if (ambigX3) p=OTL_OTRmap.equal_range(hitX1.getOtRef()->channel());
                else p=OTL_OTLmap.equal_range(hitX1.getOtRef()->channel());
              }
              It low=p.first;
              It up=p.second;
              bool skipthishit=false;
              for (It it=low; (it != up) ; ++it)
                if ((skipthishit = (it->second == hitX3.getOtRef()->channel()))) break; // if this statement is true, the hit is in the mask
              if (skipthishit) continue;
            } // if X3 is IT or OT
          } // if X1 is IT or OT
          // we checked that this X1-X3 combination for this ambiguity has not been 
          // considered in a previous track candidate. Continue with this candidate
          if (m_debug) {
            // the difference between debugloops[1] and [2] indicates
            // how many candidates are skipped by considering the maps before
            ++(debugloops[2]);
          }
          xvec.clear();
          zvec.clear();
          error.clear();
          Xcands.clear();
          //               skipped.clear();
          enoughX1=enoughX3=1; // the trial hits that are put in.
          if (hitX1.isIT()) ++enoughX1;
          if (hitX3.isIT()) ++enoughX3;
          z3=hitX3.z();
          x3=hitX3.x();
          // check if this hit is acceptable
          y3max=z3*y1max/z1+(z3-z1)*m_YSlopDev;
          if (y3max<hitX3.y_min()) continue; // this X3 hit is not consistent
          if (y3max<hitX3.y_max()) y3max = hitX3.y_max();
          y3min=y1min*z3/z1-(z3-z1)*m_YSlopDev;
          if (y3min> hitX3.y_max()) continue; // this X3 hit is not consistent
          if (y3min>hitX3.y_min()) y3min=hitX3.y_min();
          // for the current X1,X3 pair, try to find enough hits around the track that
          // match
          double y1mintemp = y3min*z1/z3-(z3-z1)*m_YSlopDev;
          if (y1mintemp<y1min) y1mintemp=y1min;
          double y1maxtemp = y3max*z1/z3+(z3-z1)*m_YSlopDev;
          if (y1maxtemp>y1max) y1maxtemp=y1max;
          // run a track through the middle of the defined range; search for x hits in
          // station 1 and three. If there is an IT hit in station 1 or 3, ymin and ymax
          // are close to the central y of that station and a central track should go
          // through the correct silicon sectors
          double y1=(y1mintemp+y1maxtemp)/2;
          double y3=(y3min+y3max)/2;
          double x1temp=hitX1.x(ambigX1,y1);
          double z1temp=hitX1.z(ambigX1,y1);
          z3=hitX3.z(ambigX3,y3);
          x3=hitX3.x(ambigX3,y3);
          // check if OT hit is on wire:
          if ((fabs(hitX3.x(!ambigX3,y3)-x3)<0.01) && (ambi3<1)) ++ambi3;

          // push back the trial hits
          xvec.push_back(x1temp);
          zvec.push_back(z1temp);
          if (hitX1.isOT()) error.push_back(1.2); // 1.2 mm so long y position is not known
          else error.push_back(0.3); // so long y position is not known
          Xcands.push_back(hitX1);
          xvec.push_back(x3);
          zvec.push_back(z3);
          if (hitX3.isIT()) error.push_back(0.3);
          else error.push_back(1.2); // large errors when y is not known yet
          Xcands.push_back(hitX3);

          double tolerance=z1temp-m_zmin[0];
          if (m_zmax[0]-z1temp > tolerance) tolerance=m_zmax[0]-z1temp;
          double upbound = x1temp+ fabs(x3-x1temp)*tolerance/(z3-z1temp)+10; // hits within 10 mm
          lowerIt=x1It;
          ++lowerIt;
          // y-spread due to spread in plane positions
          tolerance *= fabs((y3-y1)/(z3-z1temp));
          tolerance += 5;
          for (tempIt=lowerIt; (tempIt->first<upbound) && (tempIt != T1X.end()) ; ++tempIt) {
            if (m_debug) ++debugloops[3];
            TrackSeedHit hit(&tempIt->second);
            // require, that the plane covers the MID of the plane in the same station of the trial hit
            if (hit.y_min() > y1 + tolerance) continue; 
            if (hit.y_max() < y1 - tolerance) continue; 
            double z=hit.z(x1temp,y1);
            double x= (x1temp*(z3-z)+x3*(z-z1temp))/(z3-z1temp);
            double y=(y1*(z3-z)+y3*(z-z1temp))/(z3-z1temp);
            if (y<hit.y_min()) y=hit.y_min();
            if (y>hit.y_max()) y=hit.y_max();
            double errortemp = 0.7; // 0.7 mm for OT when y is not known yet
            if (hit.isIT()) errortemp = 0.2; // 200 microns for IT when y is not known
            // Note: the errors are given by the OTDetector class in refitting as soon as the y-information
            // is available
            bool ambiguity=true;
            if (hit.isOT()) ambiguity = (fabs(x-hit.x(true,y))<fabs(x-hit.x(false,y)));
            if (fabs(x-hit.x(ambiguity,y))<errortemp*m_sigmaOutlierX) {
              hit.setAmbiguity(ambiguity);
              xvec.push_back(hit.x(ambiguity,y));
              zvec.push_back(hit.z(x,y));
              error.push_back(errortemp);
              Xcands.push_back(hit);
              if (hit.isIT()) ++enoughX1;
              ++enoughX1;
              if (m_debug) ++debugloops[4];
            }
          } // end loop find extra hits in station 1X
          if (enoughX1 < m_minX13) {
            // Not enough hits in station 1 found (less than 2 OT or 1 IT hit)
            // try a next trial hit in station 3.
            // this is done by jumping to the end of the ambiguity loop over the X3 hit ambiguity
            ambi3=2; 
            continue; 
          }

          if (m_debug) ++debugloops[5]; // the difference with debugloops[4] indicates how often there were not enough hits in station 1X
            
          //
          // now, check if there are hits in station 3 that are close enough to the line
          //
          tolerance = z3-m_zmin[2];
          if (tolerance  < m_zmax[2]-z3) tolerance = m_zmax[2]-z3;
          upbound = (x3+fabs(x3-x1temp)*tolerance/(z3-z1temp)+m_OTCellRadius*2.2); // possible hits
          tolerance *= fabs((y3-y1)/(z3-z1));
          tolerance += 5;
          tempIt=x3It;
          ++tempIt;
          for (; (tempIt->first < upbound) && (tempIt != T3X.end()) ; ++tempIt) {
            if (m_debug) ++debugloops[6];
            TrackSeedHit hit(&tempIt->second);
            if (hit.y_min() > y3 + tolerance) continue; 
            if (hit.y_max() < y3 - tolerance) continue; 
            double z=hit.z(x3,y3);
            double x=x3+(z-z3)/(z3-z1temp)*(x3-x1temp);
            double y=y3+(z-z3)/(z3-z1temp)*(y3-y1);
            if (y<hit.y_min()) y=hit.y_min();
            if (y>hit.y_max()) y=hit.y_max();
            double errortemp = 0.7; // 0.7 mm for OT-UV planes when y is not known yet
            if (hit.isIT()) errortemp = 0.2; // 200 microns for IT when y is not known
            bool ambiguity=true;
            if (hit.isOT()) ambiguity = (fabs(x-hit.x(true,y))<fabs(x-hit.x(false,y)));
            if (fabs(x-hit.x(ambiguity,y))<errortemp*m_sigmaOutlierX) {
              hit.setAmbiguity(ambiguity);
              xvec.push_back(hit.x(ambiguity,y));
              zvec.push_back(hit.z(x,y));
              error.push_back(errortemp);
              Xcands.push_back(hit);
              if (hit.isIT()) ++enoughX3;
              ++enoughX3; // OT hit or IT hit
              if (m_debug) ++debugloops[7];
            }
          } // end check of extra hits in station 3
          if (enoughX3<m_minX13) continue; // not enough X-hits found in station 3, try next ambiguity of trial hit in station 3
          if (enoughX3+enoughX1 < m_minT1T3) continue;
          if (m_debug)  ++debugloops[8];
          // obtain the parameters of the fit through the trial hits
          double chi2 = 1;
          int skippedhits = 0;
          int skippedoldhits = -1;
          skipped.clear();
          while ((skippedhits != skippedoldhits) && (chi2>0.0)) {
            skippedoldhits = skippedhits;
            chi2=TrackSeedHit::line_fit(zvec,xvec,error,fitpars,skipped);
            skippedhits = (int) skipped.size();
            if (m_debug) {
              if (skippedhits > skippedoldhits) {
                if ( m_verbose ) {
                  verbose() << " X-hit removal : " << debugloops[8] << " ";
                  for (unsigned int i=0; i<xvec.size(); ++i ) verbose() << zvec[i] << " " << xvec[i] << " ";
                  verbose() << " removed: " ;
                  for (unsigned int j=0; j<skipped.size(); ++j) verbose() << skipped[j] << " " << zvec[skipped[j]] << " " << xvec[skipped[j]] << " ";
                  verbose() << " chi2 " << chi2 << endmsg;
                } 
              }
            }
          }
          //
          // a line has been fitted through the trial hits. All hits that are more than
          // 3 sigma away have been removed. Check, if the candidate is still valid
          //
          if (chi2<0) continue; // next trial hit; this one did not satisfy the line fit
          if (chi2>m_maxChi2) continue; // next trial hit; this one did not satisfy the line fit

          if (skippedhits>0) {
            // hits have been removed. Do we still have enough?                    
            enoughX1=enoughX3=0;
            for (unsigned int i=0; i<Xcands.size(); ++i) {
              bool add=true;
              for (unsigned int j=0; j<skipped.size(); ++j) if (i==skipped[j]) add=false;
              if (add) {
                // this hit was not skipped
                if (Xcands[i].station() == 1) {
                  if (Xcands[i].isIT()) ++enoughX1;
                  ++enoughX1;
                } else {
                  if (Xcands[i].isIT()) ++enoughX3;
                  ++enoughX3;
                }
              }
            }
          }
          if ((enoughX1<m_minX13) || (enoughX3<m_minX13)) continue; // fit was not OK, try next X3hit ambiguity 
          if (enoughX1+enoughX3<m_minT1T3)  continue;

          if (m_debug) ++debugloops[9];
          //
          //
          // we now have at least minT1T3 aligned OT hits or m_minT1T3/2 aligned IT hits in X
          // We could be more restrictive, but there are many dead area's that lower the
          // efficiency. Therefore it is better to check in Y direction first, since
          // in that direction we can require a STRAIGHT track
          // also, as soon as the y-position is known, the full hit resolution can be obtained
          // including wire drift time and tilts of the planes
          //

          //
          // update the maps: all combinations of X1-X3 hits that made it up to here will not be checked again.
          // this may mean some loss, since "wrong" hits may be included in this track candidate
          //
          for (unsigned int i=0; i<Xcands.size(); ++i) {
            bool skip=false;
            if (Xcands[i].station() != 1) continue;
            for (unsigned int k=0; k<skipped.size(); ++k) {
              skip=(i==skipped[k]); 
              if (skip) break;
            }
            if (skip) continue;
            for (unsigned int j=0; j<Xcands.size(); ++j) {
              if (Xcands[j].station() != 3) continue;
              for (unsigned int k=0; k<skipped.size(); ++k) {
		            skip=(j==skipped[k]); 
                if (skip) break;
              }
              if (skip) continue;
                        
              if (Xcands[i].isIT()) {
                if (Xcands[j].isIT()) {
                  std::pair<LHCb::STChannelID,LHCb::STChannelID> p = std::make_pair(Xcands[i].getItRef()->channelID(),Xcands[j].getItRef()->channelID());
                  ITmap.insert(p);
                } else {
                  std::pair<LHCb::STChannelID,LHCb::OTChannelID> p = std::make_pair(Xcands[i].getItRef()->channelID(),Xcands[j].getOtRef()->channel());
                  if (Xcands[j].ambiguity()) IT_OTRmap.insert(p);
                  else IT_OTLmap.insert(p);
                }
              } else { 
                if (Xcands[j].isIT()) {
                  std::pair<LHCb::OTChannelID,LHCb::STChannelID> p = std::make_pair(Xcands[i].getOtRef()->channel(),Xcands[j].getItRef()->channelID());
                  if (Xcands[i].ambiguity()) OTR_ITmap.insert(p);
                  else OTL_ITmap.insert(p);
                } else {
                  std::pair<LHCb::OTChannelID,LHCb::OTChannelID> p = std::make_pair(Xcands[i].getOtRef()->channel(),Xcands[j].getOtRef()->channel());
                  if (Xcands[i].ambiguity()) {
                    if (Xcands[j].ambiguity()) OTR_OTRmap.insert(p);
                    else OTR_OTLmap.insert(p);

                  } else {
                    if (Xcands[j].ambiguity()) OTL_OTRmap.insert(p);
                    else OTL_OTLmap.insert(p);
                  }
                }
              } // X1 cand is IT or OT
            } // loop over all X3 hits in the track
          } // loop over all X1 hits in the track
	          //verbose() << "maps updated, " << debugloops[1] << " " << debugloops[9] << " " << OTL_OTLmap.size() << " " << OTL_OTRmap.size() << " " << OTR_OTLmap.size() << " " << OTR_OTRmap.size() << endmsg;

          //
          // store X-hits in temporary track candidate
          //
          tempcands.clear();
          TrackCand candidate; // The candidate with these X-hits in T1/T3
          candidate.setXFitParams(fitpars);
          for (unsigned int i=0; i<xvec.size(); ++i) {
            bool add=true;
            for (unsigned int j=0; j<skipped.size(); ++j) if (skipped[j]==i) { add=false; break; }
            if (add) {
              candidate.addXHit(Xcands[i],xvec[i],zvec[i],error[i]);
            }
          }
          //
          // find hits in Y-layers of station 1
          //
          double xmax,xmin;
          xmin=xmax=fitpars[1];
          if (fitpars[3]<0) { // fitted slope is negative: smallest x at largest z
            xmin+=(m_zmax[0]-fitpars[0])*fitpars[3];
            xmax+=(m_zmin[0]-fitpars[0])*fitpars[3];
          } else {
            xmin+=(m_zmin[0]-fitpars[0])*fitpars[3];
            xmax+=(m_zmax[0]-fitpars[0])*fitpars[3];
          }
          double spread=(fitpars[0]-m_zmin[0])*fitpars[4]+fitpars[2]+m_OTCellRadius*2.2;
          xmin -=spread;
          xmax +=spread;
          // in the maps, I store the x-coordinate of the wire at 0
          double ylarge=fabs(y1maxtemp);
          double ysmall=0;
          if (y1maxtemp*y1mintemp>0) ysmall=fabs(y1mintemp);
          if (ylarge<ysmall) {
            ysmall=ylarge;
            ylarge=fabs(y1mintemp);
          }
          ysmall *= z1temp/m_zmax[0]; // the position closest possible to the beam line in station 1
          ylarge *= m_zmax[0]/z1temp; // the position furthest possible to the beam line in station 1

          // maximum x- range in station 1 at y=0
          double uvmin=xmin-sinmaxstereo*ylarge;
          double uvmax=xmax+sinmaxstereo*ylarge;
          double uvmidlow=xmax-ysmall*sinminstereo;
          double uvmidhigh=xmin+ysmall*sinminstereo;
          // verbose() << " xmin, xmax, uvmin, uvmax, uvmidlow, uvmidhigh " << xmin << " " << xmax << " " << uvmin << " " << uvmax << " " << uvmidlow << " " << uvmidhigh <<  " Ysmall " << ysmall << endmsg;
               
          lowerIt=T1Y.lower_bound(uvmin);
          // zero Ycandidate arrays
          Ycands.clear();
          y1vec.clear();
          y1zvec.clear();
          y1error.clear();
          Y1cands.clear();
          skippedy.clear();

          for (tempIt=lowerIt; tempIt != T1Y.end() ; ++tempIt) {
            if (tempIt->first>uvmax) break;
            if ((tempIt->first>uvmidlow) && (tempIt->first<uvmidhigh)) continue; // this region of wires does not cover the target area
            TrackSeedHit yhit(&tempIt->second);
            double yminloc = yhit.y_min();
            if (m_debug) ++debugloops[10];
            if (yminloc > y1maxtemp) continue; // next Y hit to consider
            if (yminloc<y1mintemp-10) yminloc = y1mintemp-10;
            double ymaxloc = yhit.y_max();
            if (ymaxloc>y1maxtemp+10) ymaxloc = y1maxtemp+10;
            if (ymaxloc<yminloc) continue;
            double zlocal = yhit.z((xmin+xmax)/2.0,(yminloc+ymaxloc)/2.0);
            double xlocal = fitpars[1]+fitpars[3]*(zlocal-fitpars[0]);
            double ylocal=yhit.y(false,xlocal);
            double ylocal2 = 0.;
            for (int yambi=0; yambi<2; ++yambi) {
              if (yhit.isIT()) ++yambi;
              else {
                if (yambi==0) {

                  ylocal2 = yhit.y(true,xlocal);
                  if (fabs(ylocal-ylocal2)<0.01) ++yambi;
                } else ylocal=ylocal2;
              }
              // note that the yambi==0 statement improves the speed of this loop
              if ((ylocal<ymaxloc) && (ylocal>yminloc)) {
                     
                // found a correct Y-hit 
                yhit.setAmbiguity(yambi!=0);
                y1vec.push_back(ylocal);
                y1zvec.push_back(zlocal);
                y1error.push_back((fitpars[2]+yhit.error())/sinminstereo); // as error, I add linearly the uncertainty in the x-fit.
                Y1cands.push_back(yhit);
                if (m_debug) ++debugloops[11];
              }
            }
          } // for all Y-hits in station 1
          // check first now if there are enough Y-hits in station 1
          if (y1vec.size()<1) continue; // next IT3 x trial hit, no Y-hit found 
          if ((y1vec.size()<2) && (Y1cands[0].isOT()) ) continue; // station 1: only 1 OT hit found
          //
          //
          // for all Y1 candidates, check if there are hits in Y3
          // calculate boundaries in station 3
          //
          xmin=xmax=fitpars[1];
          if (fitpars[3]<0) {
	          xmin+=fitpars[3]*(m_zmax[2]-fitpars[0]);
            xmax+=fitpars[3]*(m_zmin[2]-fitpars[0]);
          } else {
	          xmin+=fitpars[3]*(m_zmin[2]-fitpars[0]);
            xmax+=fitpars[3]*(m_zmax[2]-fitpars[0]);
          }
          spread=fitpars[2]+fitpars[4]*(m_zmax[2]-fitpars[0])+m_OTCellRadius*2.2;
          xmin-=spread;
          xmax+=spread;
		  
          spread=(m_zmax[2]-m_zmin[0])*m_YSlopDev; // y3-spread 
          bool xFitNotDone=true; // set it false if this X-candidate gives a valid fit for 1 y-track

          // loop over all Y1-hits
          for (unsigned int indexY1=0; indexY1<y1vec.size(); ++indexY1) {
            if (m_debug) ++debugloops[12];
            // first, determine minimum and maximum U,V coordinates in station 3 for this Y1 hit
            double zstart=y1zvec[indexY1];
            double ystart=y1vec[indexY1];
            double ymax=ystart*m_zmax[2]/zstart; // extrapolate from origin to maximum possible z-position in station 3
            double ymin=ystart*m_zmin[2]/zstart;
            if (ymax<ymin) { // this is for negative y-start; phi<0
              double temp=ymin;
              ymin=ymax;
              ymax=temp;
            }
            ymin-=spread;
            ymax+=spread;
            if (fabs(ymin)>fabs(ymax)) {
              ylarge=fabs(ymin);
              if (ymin*ymax<0) ysmall=0;
              else ysmall=fabs(ymax);
            } else {
              ylarge=fabs(ymax);
              if (ymin*ymax<0) ysmall=0;
              else ysmall=fabs(ymin);
            }
            // test separately for u and for v wires, first test here
            uvmin = xmin - sinmaxstereo * ylarge;
            uvmax = xmax + sinmaxstereo * ylarge;
            double uvmidlow = xmax-sinminstereo* ysmall;
            double uvmidhigh = xmin+sinminstereo* ysmall;
            lowerIt=T3Y.lower_bound(uvmin);
            //
            // between lowerIt and upperIt we have all elements in the range [uvmin,uvmax>
            // i.e. if there are NO such elements, lowerIt=upperIt is the first element 
            // larger than uvmax, or end()
            //
            for (tempIt=lowerIt; tempIt!=T3Y.end(); ++tempIt) {
              // this loop is over all u and v wires that migth coincide. The "mid" section of these wires miss the target area.
              if ((tempIt->first>uvmidlow) && (tempIt->first<uvmidhigh)) {
                tempIt=T3Y.lower_bound(uvmidhigh); // skip all intermediate wires
                if (tempIt == T3Y.end()) break; // protect against this condition. It may occur at the edges of the station
              }
              ; // this set of wires misses the target area
              if (tempIt->first>uvmax) break; // all other hits in station 3 have a too large x-value
              TrackSeedHit yhit(&tempIt->second);
              double yminloc = yhit.y_min();
              if (yminloc > ymax)  continue; // the track misses the active plane of this hit
              double ymaxloc = yhit.y_max();
              if (ymaxloc<ymin) continue; // the track misses the active plane of this hit
              if (m_debug) ++debugloops[13];
              double zlocal = yhit.z((xmin+xmax)/2.0,(yminloc+ymaxloc)/2.0);
              double xlocal = fitpars[1]+fitpars[3]*(zlocal-fitpars[0]);
              // test outside ambiguity loop if the center of the wire is somewhere near. This seems to speed up
              // the code by about 10 percent
              double ylocal = yhit.y(xlocal);
              if (ylocal<ymin-40) continue; // 40 >> Rcell/sin(minimum stereo angle)
              if (ylocal>ymax+40) continue; // 40 >> Rcell/sin(minimum stereo angle)
              double ylocal2 = 0.;
              for (unsigned int yambi=0; yambi<2; ++yambi) {
                // this loop is executed the most by far. Optimize number of calls to
                // yhit.x, yhit.y, yhit.z
                if (yhit.isIT()) yambi=1;
                else if (yambi==0) {
                  ylocal=yhit.y(false,xlocal);
                  ylocal2=yhit.y(true,xlocal);
                  if (fabs(ylocal-ylocal2)<0.001) yambi=1;
                } else ylocal=ylocal2;
                if (ylocal<ymin) continue;
                if (ylocal>ymax) continue;
                if ((ylocal-ystart)/(zlocal-zstart)-ystart/zstart > m_YSlopDev + 0.005) continue; // the extra 5 millirad is for the resolution in the hit: a hit resolution of 0.5 mm gives about 5 mrad uncertainty in y 
                if (m_debug) ++debugloops[14];
                yhit.setAmbiguity(yambi!=0);
                int addY1=1;
                if (Y1cands[indexY1].isIT()) ++addY1;
                int addY3=1;
                if (yhit.isIT()) ++addY3;
                // copying the Track candidate and adding the Y hits here takes more than 50 % of the TrackSeedFind::execute time.
                // in order to increase speed, I make the track candidate later, when extra Y1 hits are found.
                bool candidatecopied = false;
                //TrackCand newcand = candidate; // copy X-hits
                //newcand.addYHit(Y1cands[indexY1],y1vec[indexY1],y1zvec[indexY1],y1error[indexY1]);
                //newcand.addYHit(yhit,ylocal,zlocal,yhit.error()/sinminstereo);
                //newcand.refitYHits();
                //newcand.yParams(Yfitpars);
                Yfitpars[0]=y1zvec[indexY1];
                Yfitpars[1]=y1vec[indexY1];
                Yfitpars[2]=yhit.error()+fitpars[2];
                Yfitpars[2]/=sinminstereo;
                Yfitpars[2]*=Yfitpars[2];
                Yfitpars[2]=1/Yfitpars[2]+1/y1error[indexY1]/y1error[indexY1];
                Yfitpars[2]=1/sqrt(Yfitpars[2]);
                Yfitpars[3]=(ylocal-y1vec[indexY1])/(zlocal-y1zvec[indexY1]);
                Yfitpars[4]=Yfitpars[2]/(zlocal-y1zvec[indexY1]); // note, all these parameters will be refitted later.
			
                for (unsigned int j=y1vec.size()-1; j>indexY1; j--) { 
                  bool samehit= ((Y1cands[j].getOtRef() == Y1cands[j-1].getOtRef()) && Y1cands[j].isOT());
                  if (samehit && (j==indexY1+1)) continue; // always choose trial hit at indexY1 in fit
                  // if ((debugloops[14]<100) && m_debug) verbose() << " loop 14; " << samehit << " " <<  indexY1 << " " << j << " " << y1vec[j] << " " << y1zvec[j] << endmsg;
                  double dist = Yfitpars[1]+(y1zvec[j]-Yfitpars[0])*Yfitpars[3];
                  if (samehit) {
                    double dist2= fabs(y1vec[j-1]-dist);
                    dist = fabs(y1vec[j]-dist);
                    if (dist2<dist) {
                      j--;
                      dist=dist2;
                      samehit=false; // decremented j, continue normally with the hit at j--
                    }
                  } else dist=fabs(y1vec[j]-dist); // end check, which of the two consecutive same hits in y1vec (with only different ambiguity) is closer to the track; store the closests 
                  double uncert= Yfitpars[2]+Yfitpars[4]*(y1zvec[j]-Yfitpars[0])+y1error[j];
                  if (dist<uncert*m_sigmaOutlierY) {
                    if (!candidatecopied) {
                      candidatecopied=true;
                      newcand = candidate; // copy X-hits
                      newcand.addYHit(Y1cands[indexY1],y1vec[indexY1],y1zvec[indexY1],y1error[indexY1]);
                      newcand.addYHit(yhit,ylocal,zlocal,(yhit.error()+fitpars[2])/sinminstereo);
                    }
                    newcand.addYHit(Y1cands[j],y1vec[j],y1zvec[j],y1error[j]);
                    if (Y1cands[j].isIT()) ++addY1;
                    ++addY1;
                  }
                  if (samehit) j--; // if samehit is true, the hit at j-- is further from the track. It should not be considered for storage in newcand
                } 
                if (addY1<m_minY13) continue;
                // check Y3 hits that have not yet been tested before
                // to see if they are also part of this candidate
                tempItY3=tempIt;
                ++tempItY3;
                for ( ; tempItY3 != T3Y.end(); ++tempItY3 ) {
                  if ((tempItY3->first > uvmidlow) && (tempItY3->first <uvmidhigh)) {
                    tempItY3 = T3Y.lower_bound(uvmidhigh);
                    if (tempItY3 == T3Y.end()) break;
                  }
                  if (tempItY3->first > uvmax) break;
                  TrackSeedHit  y3testhit(&tempItY3->second); 
                  if (y3testhit.y_max()<ymin) continue;
                  if (y3testhit.y_min()>ymax) continue;
                  double tempz=y3testhit.z((xmin+xmax)/2,(yminloc+ymaxloc)/2);
                  double tempx = fitpars[1]+fitpars[3]*(tempz-fitpars[0]);
                  double tempy = y3testhit.y(tempx); // first, try center of hit. This speeds up the code
                  double dist = Yfitpars[1]+(tempz-Yfitpars[0])*Yfitpars[3];
                  if (fabs(dist-tempy)>40) continue;
                  double temperror=(y3testhit.error()+fitpars[2])/sinminstereo;
                  double uncert= Yfitpars[2]+Yfitpars[4]*(tempz-Yfitpars[0])+temperror; // Yfitpars[0] is garauntueed to be smaller than tempz in this piece of code 
                  // if ((debugloops[15]<10) && m_debug) debug() << " addhit: uncer " << uncert << " Yfitpars2,4 " << Yfitpars[2] << " " << Yfitpars[4] << " fitpars2 " << fitpars[2] << " yhit " << temperror << " " << y3testhit.error() << endmsg;
                  if (y3testhit.isOT()) {
                    tempy=y3testhit.y(false,tempx);
                    double tempy2 = y3testhit.y(true,tempx);
                    if (fabs(tempy2-dist)<fabs(tempy-dist)) {
                      y3testhit.setAmbiguity(true);
                      tempy=tempy2;
                    } else y3testhit.setAmbiguity(false);
                  } 
                  dist = fabs(tempy-dist);
                  if (dist<m_sigmaOutlierY*uncert) {
                    if (!candidatecopied) {
                      candidatecopied=true;
                      newcand = candidate; // copy X-hits
                      newcand.addYHit(Y1cands[indexY1],y1vec[indexY1],y1zvec[indexY1],y1error[indexY1]);
                      newcand.addYHit(yhit,ylocal,zlocal,yhit.error()/sinminstereo);
                    }
                    newcand.addYHit(y3testhit,tempy,tempz,temperror);
                    //if ((debugloops[14]<100) && m_debug) verbose() << " loop 14; addhit T3 " << " " << tempy << " " << tempz << " " << tempy/tempz -ystart/zstart << endmsg;
                    if (y3testhit.isIT()) ++addY3;
                    ++addY3;
                  }
                } // loop over extra T3 hits
                // if (m_debug) debug() << "add Y1 etc " << addY1 << " " << addY3 << endmsg;
                if ((addY3<m_minY13) || (addY1+addY3<m_minT1T3)) continue;
                // if (m_debug) debug() << "loop 13, 15: " << debugloops[13] << " " << debugloops[15] << " fitdone " << xFitNotDone << endmsg;
                if (!candidatecopied) {
                  candidatecopied=true;
                  newcand = candidate; // copy X-hits
                  newcand.addYHit(Y1cands[indexY1],y1vec[indexY1],y1zvec[indexY1],y1error[indexY1]);
                  newcand.addYHit(yhit,ylocal,zlocal,yhit.error()/sinminstereo);
                }
                if (xFitNotDone) {
                  double chi2=newcand.refitXHits();
                  if ((chi2>m_maxChi2) || (chi2<0)) {
                    tempIt=T3Y.end(); // for this y1 position, the X-fit fails. skip all y3 hits
                    ++yambi;
                    if (Xcands[0].isOT() && Xcands[1].isOT()) {
                      // if the X-candidate contains OT hits in T1 and T3, then there is no use to retry the y fit
                      // unless the y1 candidate differs substantially in Y from the present trial.
                      // this cut has to be tested
                      unsigned int i=indexY1+1;
                      while (((fabs(y1vec[indexY1]-y1vec[i])<500) || (y1vec[indexY1]*y1vec[i]<0)) && (i<y1vec.size())) ++i; // test for y1vec.size() should be right-most 
                      indexY1=--i; // indexY1 will be incremented again at the end of the loop
                    } // for IT hits, test all y's since for this track candidate we may have missed a sector
                    continue; // ends both the yambi loop and the T3Y loop, goes to next y1 hit if it differs at least 50 cm from this candidate. If not, we keep 
                  } else { // chi2 is OK;
                    // check, if there are sufficient x-hits on the track
                    xFitNotDone = ((newcand.xSkipped()-skipped.size())!=0); // if this is 0, then no hits are removed and fit is done for this X1-X3 combination. Accept all y-candidates for this X1X3 combination hereon
                    if (xFitNotDone) { // hits removed
                      newcand.xHits(xhits);
                      int nX1=0;
                      int nX3=0;
                      for (unsigned int i=0; i<xhits.size(); ++i) {
                        int add=1;
                        if (xhits[i].isIT()) ++add;
                        if (xhits[i].station()==1) nX1+=add;
                        else nX3+=add;
                      }
                      xFitNotDone=((nX1<m_minX13)||(nX3<m_minX13)||((nX1+nX3)<m_minT1T3));
                    } 
                    if (xFitNotDone) { // fit failed minimum X-hit criterion.
                      ++yambi; // abort y3-ambiguity loop
                      tempIt=T3Y.end(); // for this y1 position, the X-fit fails, due to skipped hits. abort Y3 loop
                      if (Xcands[0].isOT() && Xcands[1].isOT()) { // OT trial hits: skip all Y1 that are so close to the current y1 candidate that the x-fit is bound to fail again
                        unsigned int i=indexY1+1;
                        while (((fabs(y1vec[indexY1]-y1vec[i])<500) || (y1vec[indexY1]*y1vec[i]<0)) && (i<y1vec.size())) ++i; // test for y1vec.size() should be right-most 
                        indexY1=--i;
                      }
                      continue; // with the next reasoneable y-1 hit (skip innoermost 2 loops over Y3)
                    } // if here, then xFit is set to "DONE"
                  } // chi2 OK, xFitDone
                } // if xfitNotDone  
                double chi2y=newcand.refitYHits(); // may already remove some hits
                if (chi2y>m_maxChi2) continue; 
                tempcands.push_back(newcand);
              } // loop over ambiguity of T3-Y candidate hits
              // the following statement should not be necessary in the standard library; however the LHCb compiler
              // screws up
              if (tempIt==T3Y.end()) break;
            } // loop over T3 Y hits   
            // debug() << "end y1 loop " << indexY1 << endmsg;
          } // loop over possible T1 Y hits
          //
          // here, inside the loop over all X1,X3 combinations, we have a set of candidate
          // tracks in tempcands. Now, we check if these candidates have enough hits.
          //
          if (m_debug) debugloops[15]+=tempcands.size();
          for (unsigned int candIndex=0; candIndex<tempcands.size(); ++candIndex) {
            int totx1,totx3,toty1,toty3,totu,totv,totIT;
            double chi2=tempcands[candIndex].refitXHits(); // maybe, some x-hits are tossed in the re-fit of the track
            if (chi2>m_maxChi2) continue;
            if (m_debug) ++debugloops[17]; 
            tempcands[candIndex].xHits(xhits);
            totx1=totx3=toty1=toty3=totu=totv=totIT=0;
            for (unsigned int j=0; j<xhits.size(); ++j) {
              int add=1;
              if (xhits[j].isIT()) {
                add=2;
                ++totIT;
              }
              if (xhits[j].station() == 1) totx1+=add;
              else totx3+=add;
            }
            if ((totx1<m_minX13) || (totx3<m_minX13)) continue; // less than 1 IT/ 2 OT hits 
            if (totx1+totx3<m_minT1T3) continue; // less than 1 IT/ 2 OT hits 
            if (m_debug) ++debugloops[18]; 
            double chi2y=tempcands[candIndex].refitYHits();
            if (chi2y>m_maxChi2) continue;
            tempcands[candIndex].yHits(Ycands);
            for (unsigned int j=0; j<Ycands.size(); ++j) {
              int add=1;
              if (Ycands[j].isIT()) {
                add=2;
                ++totIT;
              }
              double uv=Ycands[j].x()-Ycands[j].x(Ycands[j].y()-1000.0);
              if (Ycands[j].station() == 1) toty1+=add;
              else toty3+=add;
              if (uv>0) totu+=add;
              else totv+=add;
            }
            // verbose() << " tempcand " << totx1 << " " << totx3 << " " << toty1 << " " << toty3 << " " << totu << " " << totv << endmsg;
            if (totu<m_minUV13) continue; // this track has no U or V plane in both station 1 and station 3. I skip it 
            if (totv<m_minUV13) continue; // this track has no U or V plane in both station 1 and station 3. I skip it 
            if (m_debug) ++debugloops[19];
            if ((toty1<m_minY13) || (toty3<m_minY13)) continue;
            if (toty1+toty3<m_minT1T3) continue;
            if (m_debug) ++debugloops[20];
            if (m_debug) debug() << " tempcand, loop " << debugloops[1] << " " << debugloops[9] << " " << totx1 << " " << totx3 << " " << toty1 << " " << toty3 << " " << totu << " " << totv;
            // if here, store the track 
            trackCandsT1T3.push_back(tempcands[candIndex]);
            if (m_debug) ++debugloops[22];
          } // loop over all tempcands
        } // loop over T3X ambiguity
      } // loop over T3X hits
    } // loop over T1X ambiguity
  } // loop over T1X hits
  if (m_debug) {
    debug() << " in TrackSeedFind: the sizes of the skipped maps are : " << endmsg
            << " IT IT  " << ITmap.size() << " ;   IT OTL " << IT_OTLmap.size()
            << " ;    IT OTR " << IT_OTRmap.size() << endmsg
            << " OTR OTR " << OTR_OTRmap.size() << " ;   OTR OTL " << OTR_OTLmap.size()  
            << " ; OTL OTR " << OTL_OTRmap.size() << " ; OTL OTL " << OTL_OTLmap.size() << endmsg;
    for (unsigned int i=0; i<debugloops.size(); ++i)
      debug() << " Loop " << i << " : "<<debugloops[i] << endmsg;
    debug() << trackCandsT1T3.size() << " track candidates in T1 T3 found " << endmsg;
  }
   
  // Now, we have straight tracks in station 1 and 3 with decent chi2 and enough hits.
  // proceed with station 2

  //
  // first, calculate the extra shift in x that one gets from a given shift in
  // angle due to the field integrals to m_zmin[0], m_zmid[1], and m_zmax[2]
  // this is the distance between T1 and T2 or T3 and T2 times the extra angle
  // obtained from the difference in field integral
  // the factor that is not depending on track is calculated here :
  //
  double x2extra_factor = 0; 
  if (m_zRotPoint < m_zmin[1]) {
    // this should be the case
    // angle change between m_zmin[0] and m_zRotPoint is about this factor:
    x2extra_factor = (m_zmax[2]-m_zRotPoint)/(m_zmax[2]-m_zmin[0])*m_BAngleChange;
    x2extra_factor /=-2; // ( negative displacement in z-rotpoint for a circular trajectory from m_zmin[0] , gives factor 2
    x2extra_factor *= (m_zRotPoint-m_zmin[0]); // length of trajectory
    x2extra_factor *= (m_zmax[2]- (m_zmin[1]+m_zmax[1])/2 );
    x2extra_factor /= (m_zmax[2]- m_zRotPoint ); // displacement in center of T2 is fraction of displacement in m_zRotPoint
  } else {
    x2extra_factor = (m_zmin[0]-m_zRotPoint)/(m_zmax[2]-m_zmin[0])*m_BAngleChange; // angle change between m_zRotpoint and m_zmax[2]
    x2extra_factor /=2; // (displacement in z-rotpoint for a circular trajectory from m_zmax[2]
    x2extra_factor *= (m_zmax[2]-m_zRotPoint); // length of trajectory
    x2extra_factor *= (m_zmin[0]- (m_zmin[1]+m_zmax[1])/2 );
    x2extra_factor /= (m_zmin[0]- m_zRotPoint ); // displacement in center of T2 is fraction of displacement in m_zRotPoint
  }
      
  if ( m_verbose ) verbose() << "x2 extra factor : " << x2extra_factor << endmsg;
  std::vector<double> xpars,ypars,pars;
  std::vector<TrackSeedHit> hits;
  if (m_debug) debug() << "T1T3 candidates: " << trackCandsT1T3.size() << endmsg;
  std::vector<TrackCand> candidate;
  for (unsigned int T1T3index = 0; T1T3index<trackCandsT1T3.size(); ++T1T3index) {
    // calculate which range of X and Y is acceptable for each track in T2
    trackCandsT1T3[T1T3index].xParams(xpars); // line fit parameters
    trackCandsT1T3[T1T3index].yParams(ypars);
    candidate.clear();
    int bestcand=-1; // index of best candidate on temporary vector of candidates
    double vertexslope = (xpars[1] + (m_zMagnet-xpars[0])*xpars[3])/m_zMagnet;
    double extraX2 = (xpars[3]-vertexslope)*x2extra_factor;
    // if the track bends towards higher slope, the points in T2 will seem to lay
    // at LOWER x than the straight line from the T1- hits to the T3 - hits
    double spread=fabs(extraX2)*0.5+m_OTCellRadius+2; // add extra tolerance for OT
    double xmin=xpars[1]+extraX2; 
    double xmax=xmin;
    xmin-=spread;
    xmax+=spread;
    if (xpars[3]<0) {
      xmin += xpars[3]*(m_zmax[1]-xpars[0]);
      xmax += xpars[3]*(m_zmin[1]-xpars[0]);
    } else {
      xmax += xpars[3]*(m_zmax[1]-xpars[0]);
      xmin += xpars[3]*(m_zmin[1]-xpars[0]);
    }
    double ymin=ypars[1]-5.0-ypars[2]-ypars[4]*300; // 5 mm extra tolerance, plus 30cm*uncertainty in y-angle
    double ymax=ypars[1]+5.0+ypars[2]+ypars[4]*300;
    if (ypars[3]>0) {
      ymin += ypars[3]*(m_zmin[1]-ypars[0]);
      ymax += ypars[3]*(m_zmax[1]-ypars[0]);
    } else {
      ymax += ypars[3]*(m_zmin[1]-ypars[0]);
      ymin += ypars[3]*(m_zmax[1]-ypars[0]);
    } 
    lowerIt = T2X.lower_bound(xmin); 
    unsigned int candIndex;
    for (tempIt=lowerIt; tempIt != T2X.end(); ++tempIt) {
      if (tempIt->first > xmax) break;
      TrackSeedHit hit(&tempIt->second);
      if ((hit.y_min() > ymax) || (hit.y_max() < ymin)) continue;
      double z=hit.z((xmin+xmax)/2.0,(ymin+ymax)/2.0);
      double y=ypars[1]+ypars[3]*(z-ypars[0]);
      for (int ambi=0; ambi<2; ++ambi) {
        if (hit.isIT()) ++ambi;
        bool hitambig=(ambi!=0);
        hit.setAmbiguity(hitambig);
        double x=hit.x(hitambig,y);
        double xproj = xpars[1]+(z-xpars[0])*xpars[3]+extraX2;
        double tol=hit.error()+xpars[2]+fabs(z-xpars[0])*xpars[4];
        tol *=m_sigmaOutlierX; // 3 sigma window
        tol += 0.3*fabs(extraX2);
        if (m_usePtKick) tol += 1; // temporary : add extra 1 mm for miscalculation of curvature of track
        if (fabs(x-xproj) < tol) {
          candidate.push_back(trackCandsT1T3[T1T3index]);
          candIndex=candidate.size()-1;
          candidate[candIndex].addXHit(hit,x,z,hit.error());
          candidate[candIndex].refitXHits(false); // parabola fit through this hit
          candidate[candIndex].xParams(pars); // parabola fit through this hit
          x2It=tempIt;
          ++x2It;
          for (; x2It != T2X.end(); ++x2It) { // pick up all extra hits
	          if (x2It->first > xmax) break;
            TrackSeedHit temphit(&x2It->second);
            double tempz=temphit.z(x,y);
            double tempy=ypars[1]+(tempz-ypars[0])*ypars[3];
            if (temphit.y_min()>tempy) continue;
            if (temphit.y_max()<tempy) continue;
            double tempx=pars[0]+(pars[2]*tempz+pars[1])*tempz;
            double x1=temphit.x(false,tempy);
            double x2=x1;
            if (temphit.isOT()) x2=temphit.x(true,tempy);
            double dist = fabs(x1-tempx);
            double dist2 = fabs(x2-tempx);
            if (dist2<dist) {
              dist=dist2;
              x1=x2;
              temphit.setAmbiguity(true);
            } else temphit.setAmbiguity(false);
            if ( m_verbose ) verbose() << "distances " << x1 << " " << x2
                                       << " xmin,xmax " << xmin << " " << xmax <<
              " " << dist << " "<<dist2 << endmsg;
            if (dist<temphit.error()*m_sigmaOutlierX) candidate[candIndex].addXHit(temphit,x1,tempz,temphit.error());
          } // add extra T2X hits
        } // if this hit is OK
      } // for T2X ambiguity
    } // for T2X hits in window
    if (candidate.size()<1) continue;
    if ( m_verbose ) verbose() << "T1-T3 " <<T1T3index << " candidates " << candidate.size() << endmsg;
    double maxquality=100;
    for (unsigned int i=0; i<candidate.size(); ++i) {
      int addX2=0;
      double quality=candidate[i].refitXHits(false); // reduced chi2
      if (quality>3) continue; // bad fit
      candidate[i].xHits(hits);
      if ( m_verbose ) verbose() << " quality " << i << " " << quality;
      quality *= (hits.size()-3); // go back to chi-2
      quality -= 3*hits.size(); // missing hit: penalty 3 in chi-squared
      for (unsigned int j=0; j<hits.size(); ++j) if (hits[j].station()==2) {
        if (hits[j].isIT()) ++addX2;
        ++addX2;
      }
      quality-=addX2; // prefer the candidate with the most added X2 hits preferably
      if ( m_verbose ) verbose() << " addX2 " << addX2 << endmsg;
      if ((addX2>1) && (quality<maxquality)) {
        bestcand=i;
        maxquality=quality;
      }
    }
    if (bestcand<0) continue; // no good X2 candidates found
    if ( m_verbose ) verbose() << " candidate " << T1T3index << " " << bestcand << endmsg;
    // refit Y
    double chi2=candidate[bestcand].refitYHits();
    if ( m_verbose ) verbose() << " chi-y " << chi2;
    if (chi2>m_maxChi2) continue; // bad y-fit
    candidate[bestcand].yParams(ypars);
    candidate[bestcand].xParams(xpars); // parabola

    //
    // find extra y-hits. We search separately for u = x cos(alpha) + y sin(alpha)
    // and v = x cos(alpha) - y sin(alpha)
    // in this manner, the search windows are only a few cm large and the loop 
    // should be very fast (submicroseconds)
    xmin=xpars[0]+(xpars[2]*m_zmin[1]+xpars[1])*m_zmin[1];
    xmax=xpars[0]+(xpars[2]*m_zmax[1]+xpars[1])*m_zmax[1];
    if (xmax<xmin) {
      double tmp =xmin;
      xmin=xmax;
      xmax=tmp;
    }
    xmin-=m_OTCellRadius*2.2; // for OT
    xmax+=m_OTCellRadius*2.2; // for OT
    double ylarge = fabs(ymax);
    double ysmall = 0;
    if (ymin*ymax >0) ysmall = fabs(ymin);
    if (ylarge<ysmall) {
      ylarge=ysmall;
      ysmall= fabs(ymax);
    }
    double umin=xmin-sinmaxstereo*ylarge;
    double umax=xmax+sinmaxstereo*ylarge;
    double umidlow=xmax-sinminstereo*ysmall;
    double umidhigh=xmin+sinminstereo*ysmall;
    int Yadded = 0;
    lowerIt = T2Y.lower_bound(umin);
    for (tempIt = lowerIt; tempIt != T2Y.end() ; ++tempIt) {
      if ((tempIt->first>umidlow) && (tempIt->first<umidhigh)) {
        tempIt=T2Y.lower_bound(umidhigh);
        if (tempIt==T2Y.end()) break;
      };
      if (tempIt->first > umax) break;
      TrackSeedHit hit(&tempIt->second);
      if (hit.y_min() > ymax) continue;
      if (hit.y_max() < ymin) continue;
      hit.setAmbiguity(false);
      double z = hit.z((xmin+xmax)/2,(ymin+ymax)/2);
      double x = xpars[0]+xpars[1]*z + xpars[2]*z*z;
      double y = hit.y(false,x);
      double yproj = ypars[1] + (z-ypars[0])*ypars[3];
      if (hit.isOT()) {
        double y2=hit.y(true,x);
        if (fabs(y-yproj) > fabs(y2-yproj)) {
          hit.setAmbiguity(true);
          y=y2;
        }
      }
      double error = hit.error();
      // error += xpars[3];
      error /= sinminstereo;
      double uncert =  error + ypars[2]+ypars[4]*fabs(ypars[0]-z);
      uncert *= m_sigmaOutlierY;
      if (fabs(y-yproj) < uncert) {
        candidate[bestcand].addYHit(hit,y,z,error);
        if (hit.isIT()) ++Yadded;  // IT : add weight 2, OT weight 1
        ++Yadded;
      }
    }
    if ( m_verbose ) verbose() << " Y-search " << Yadded;
    if (Yadded<m_minY13) continue; // not enough Yplanes added
    //
    // now candidate should be an OK track candidate with at least 2OT or 1IT hit
    // in station 1,2,3 X-planes and UV-planes
    // add some extra requirements
    //
    chi2 = candidate[bestcand].refitYHits();
    if ( m_verbose ) verbose() << " chi2 " << chi2 ;
    if (chi2<1e-18) continue; // (smaller than 3 valid y-hits, or wrong errors)
    if (chi2 > m_maxChi2) continue; // something wrong in this track
    chi2 = candidate[bestcand].refitXHits(false);
    if ((chi2 < 1e-18) || (chi2>m_maxChi2)) continue;
    candidate[bestcand].xHits(hits);
    int totX = 0;
    int totU = 0;
    int totV = 0;
    for (unsigned int i=0; i<hits.size(); ++i) {
      if (hits[i].isIT()) ++totX;
      ++totX;
    }
    if ( m_verbose ) verbose() << " totX " << totX;
    if (totX<m_minX123) continue; // at least 4 IT, 8 OT hits
    candidate[bestcand].yHits(hits);
    for (unsigned int i=0; i<hits.size(); ++i) {
      int add = 1;
      double uv=hits[i].x()-hits[i].x(hits[i].y()-200.0);
      if (hits[i].isIT()) add=2;
      if (uv>0) totU += add;
      else totV +=add;
    }
    if ( m_verbose ) verbose() << " totU " << totU << " totV " << totV << endmsg;
    if ((totU<m_minUV123) || (totV<m_minUV123)) continue; // not enough hits in U/V planes 
    if (totU+totV<m_minY123) continue; // 
    if (totU+totV+totX < m_minT123) continue;  // less than 9 IT hits/ 17 OT hits
    trackCands.push_back(candidate[bestcand]);
    //
    // check performance of the extra_x2 factor
    //
    if (m_debug) {
      debug() << " trackcandidate : displacement at mid and extra factor : " ;
      double z1=(m_zmin[0]+m_zmax[0])/2.0;
      double z2=(m_zmin[1]+m_zmax[1])/2.0;
      double z3=(m_zmin[2]+m_zmax[2])/2.0;
      double x1=xpars[0]+xpars[1]*z1+xpars[2]*z1*z1;
      double x2=xpars[0]+xpars[1]*z2+xpars[2]*z2*z2;
      double x3=xpars[0]+xpars[1]*z3+xpars[2]*z3*z3;
      double diff=x1+(x3-x1)/(z3-z1)*(z2-z1)-x2;
      debug() << diff << " " << diff-extraX2 << " x2-pos " << x2 << " angle " << (2*z2*xpars[2]+xpars[1]) << " vertex angle " << vertexslope <<  " phi " << ypars[3] << endmsg;
    }
  }
  // list with trackcands is delivered. Further clone killing is performed in the main routine
  if (m_debug) debug() << trackCands.size() << " T1-T2-T3 candidates found " << endmsg;
  return StatusCode::SUCCESS;
}

//===========================================================================
//
//===========================================================================

StatusCode TrackSeedFind::pileUpKiller() {
  std::list<TrackCand>::iterator it,it2;
  std::vector<TrackSeedHit> vec;
  int otChecks=0;
  int itChecks=0;
  double ottimav = 0.;
  if (m_debug) debug() << "pileUpKiller : "
                       << trackCands.size() << "Candidates" << endmsg;
  for (it=trackCands.begin(); it != trackCands.end(); ++it) {
    int otHits=0;
    int itHits=0;
    int itHigh=0;
    double otTime=0;
    vec.clear();
    it->xHits(vec);
    for (unsigned int i=0; i<vec.size(); ++i) {
      if (vec[i].isOT()) {
        ++otHits;
        otTime+=vec[i].getOtRef()->calibratedTime();
      } else {
        ++itHits;
        if (vec[i].getItRef()->highThreshold()) ++itHigh;
      }
    }
    vec.clear();
    it->yHits(vec);
    for (unsigned int i=0; i<vec.size(); ++i) {
      if (vec[i].isOT()) {
        ++otHits;
        otTime+=vec[i].getOtRef()->calibratedTime();
      } else {
        ++itHits;
        if (vec[i].getItRef()->highThreshold()) ++itHigh;
      }
    }

    bool erased = false;
    // this may be made neater: fit position in OT and subtract the wiredrifttime from the average
    if (otHits>10) { 
      ++otChecks;
      ottimav+=otTime/otHits;
      if (otTime/otHits > m_otTimeAv+m_otTimeWindow) erased=true;
      if (otTime/otHits < m_otTimeAv-m_otTimeWindow) erased=true;
      if ( m_verbose ) verbose() << "pileupkiller: " << otHits << " " << otTime/otHits << endmsg;
      if ( m_debug && erased) debug() << "erase OT " << otTime/otHits << " av " << ottimav/otChecks << " checks " << itChecks << " " << otChecks << endmsg;
    }
    // if less than 1 out of 10..12 clusters have a high threshold in IT, it is
    // probably a spill-over. high threshold should fire roughly 50% for correct
    // tracks and maybe 10% for spill-over tracks.
    // if no it Hit fires the high threshold, then for efficient tracks inside the
    // IT, having typically at least 10 planes, the probability is 1 permille 
    // assuming that the high threshold bit fires at 50% of the normal clusters.
    // if there are less hits, then this may be due true spill-over anyways. So a track// with only 7 itHits has (0.5)^7 = 1% chance to fail the criterion below (for
    // good tracks) but it has already a high probability to be a spill-over track
    // because not so many planes were found
    // if necessary, I'll make a more fancy likelihood of this later
    if (itHits>6) {
      ++itChecks;
      if ( m_verbose ) verbose() << " IT checks " << itChecks << " " << itHits << " " << itHigh << endmsg;
      if (itHigh<1) erased=true;
      if ( m_debug && erased) debug() << "erase IT " << itHits << " checks " << itChecks << " " << otChecks << endmsg;
    }
    if (erased){
      if ( m_verbose ) verbose() << " before erase : " << trackCands.size() << " IT/OT checks " << itChecks << " " << otChecks << endmsg;
      it2=it;
      it--; // point to previous hit in list, else it is dangling after erase
      trackCands.erase(it2);
    }
  }
  if (m_debug) debug() << "IT checks: " << itChecks << " OT checks: " << otChecks << " average time " << ottimav/otChecks << " candidates left: " << trackCands.size() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TrackSeedFind::cloneKiller()
{
  std::vector<int> debugloops;
  std::vector<double> xpar1,ypar1;
  std::vector<double> xpar2,ypar2;
  std::vector<TrackSeedHit> hits1,hits1y;
  std::vector<TrackSeedHit> hits2,hits2y;
  std::list<TrackCand>::iterator it, it2, it3;
  double chi1,chi2,chi1y,chi2y;
  double max_xdiff=0;
  double max_angdiff=0;
  double xdiff,angdiff;

  if (m_debug) debug() << "TrackSeedFind: clone killer: " << trackCands.size()
                       << " candidates before clone killing" << endmsg; 
  for (it = trackCands.begin(); it != trackCands.end() ; ) {
    bool enhanced=false;
    chi1 = it->refitXHits(false);
    it->xParams(xpar1);
    if ( m_verbose ) verbose() << "slope 8000 " << (xpar1[1]+16000*xpar1[2])
                               << " pos 8000 " << (xpar1[0]+8000*xpar1[1]+6.4e7*xpar1[2])
                               << " slope 9400 " << xpar1[1]+9400*xpar1[2] << endmsg;
    it2=it;
    ++it2;
    for (; it2 != trackCands.end() ; ++it2) {
      chi2 = it2->refitXHits(false);
      it2->xParams(xpar2);
      if (m_debug) {
        while (debugloops.size() < 10) debugloops.push_back(0);
        ++debugloops[0];
      }
      xdiff= fabs(xpar1[0]-xpar2[0]+m_zmin[1]* ( xpar1[1]-xpar2[1] + m_zmin[1] * (xpar1[2]-xpar2[2])) );
      if ( xdiff > 10) continue;
      if (m_debug) ++debugloops[1];
      angdiff = fabs(xpar1[1]-xpar2[1]+2*m_zmin[1]*(xpar1[2]-xpar2[2]));
      if (angdiff > 0.01) continue; // here, the slope in begin of T2 chamber differs by more than 10 mrad. It is hard to imagine that clones can have such different x-slopes.
      if (m_debug) ++debugloops[2];
      it->xHits(hits1);
      it2->xHits(hits2);
      unsigned int match  = 0;
      unsigned int nhits1 = 0;
      unsigned int nhits2 = 0;
      for (unsigned int i = 0; i<hits1.size(); ++i) {
        ++nhits1;
        if (hits1[i].isIT()) ++nhits1;
        for (unsigned int j = 0; j<hits2.size(); ++j) {
	        if (i==0) {
            if (hits2[j].isIT()) ++nhits2;
            ++nhits2;
          }
          if (hits1[i].isIT()) {
            if (hits1[i].getItRef() == hits2[j].getItRef()) match+=2;
          } else if (hits1[i].getOtRef() == hits2[j].getOtRef()) ++match;
        } 
      }
      if (m_debug) {
        debugloops[3]+=nhits1;
        debugloops[4]+=nhits2;
        debugloops[5]+=match;
        if ( m_verbose ) {
          verbose() << "debugloops clonekiller: ";
          for (unsigned int k=0; k<debugloops.size(); ++k) verbose() << " " << debugloops[k];
          verbose()<< endmsg;
        }
           
        if (m_verbose && match<(nhits1+nhits2)/5)
          verbose() << " hits " << nhits1 << " " << nhits2 << " match " << match
                    << " displacement "
                    << (xpar1[0]-xpar2[0]+8500*(xpar1[1]-xpar2[1]+8500*(xpar1[2]-xpar2[2])))
                    << " angle " << (xpar1[1]-xpar2[1]+17000*(xpar1[2]-xpar2[2]))
                    << (bool) (match>(nhits2+nhits1)/6) << endmsg;
      }
      if (match>(nhits2 + nhits1)/6) {
        if (m_debug) ++debugloops[6];
        if (xdiff>max_xdiff) max_xdiff=xdiff;
        if (angdiff>max_angdiff) max_angdiff=angdiff;
        //
        // more than  about 30 % of the x-hits are shared
        // one of the two will be tossed here
        //
        chi1y=it->refitYHits();
        chi2y=it2->refitYHits();
        it->yHits(hits1y);
        it2->yHits(hits2y);
        for (unsigned int k=0; k<hits1y.size(); ++k) {
          if (hits1y[k].isIT()) nhits1+=2;
          else ++nhits1;
        }
        for (unsigned int k=0; k<hits2y.size(); ++k) {
          if (hits2y[k].isIT()) nhits2+=2;
          else ++nhits2;
        }
        int difnum = nhits1-nhits2;
        if ( m_verbose ) verbose() << " before erase ";
        if (difnum < -2) {
          // hits2 is the array that I would like to keep; it has at least 3 hits more
          it3=it;
          ++it;
          trackCands.erase(it3);
          if ( m_verbose ) verbose() << " after erase " << difnum << endmsg;
          enhanced=true;
          break; // break out of the loop over it2, since it has been removed
        } 
        if (difnum>2) {
          it3=it2;
          it2--;
          trackCands.erase(it3);
          if ( m_verbose ) verbose() << " after erase " << difnum << endmsg;
          continue; // continue, it2 will be the next element after the increment at the end of the loop
        }
        //
        // maybe same x-track, but different y-tracks
        // I think that the correct track is likely to have the best (straight) fit in y
        // which track to toss? the relative difference between chi1y and chi2y is 
        // probably more sensitive than the difference between chi1 and chi2, since an
        // incorrect track will screw up in y earlier than in x.
        // currently: missing a hit on a track will give you an additional penalty in
        // chi2 of m_maxOutlierX*3
        //
        double quality=3.0*m_sigmaOutlierX*(nhits1-nhits2)+(chi2+chi2y)*(nhits2-5)-(chi1+chi1y)*(nhits1-5);
        // missing OT hit: penalty 2 in chi2.  Missing IT : penalty 4 in chi2. The chi2 values are however reuduced chi2s.
        // for low number of hits, the penalty for missing hits is higher than for high number of hits, as it should.
        // high quality: track 1 is better than track 2
        if ( m_verbose ) verbose() << " qualities " << quality << " " << chi1 << " " << chi1y << " " << chi2 << " " << chi2y << endmsg;
        if (quality<0) {
          it3=it;
          ++it; // increment it and...
          trackCands.erase(it3);
          if ( m_verbose ) verbose() << " after erase y it" << endmsg;
          enhanced=true;
          break; // continue with the loop over it
        } else {
          it3=it2;
          it2--;
          trackCands.erase(it3);
          if ( m_verbose ) verbose() << " after erase y it2" << endmsg;
          continue;
        }
      } // if more than 30 % of x-hits match
    } // loop over trackcands, it2 iterator
    if (!enhanced) ++it; // enhance it when trackCands(it) has not been removed and the pointer is still to the old candidate
  }
  if (m_debug) {     
    debug() << "TrackSeedFind: clone killer: end " << trackCands.size()
            << " candidates left " << endmsg
            << "TrackSeedFind: maximum x-difference of clones : "
            << max_xdiff << " and angle " << max_angdiff << endmsg;
    for (it = trackCands.begin(); it != trackCands.end() ; ++it) {
      it->xParams(xpar1);
      it->yParams(ypar1);
      double chi2 = it->refitXHits(false);
      double chi2y = it->refitYHits();
      debug() << "slope 8000 " << (xpar1[1]+16000*xpar1[2]) << "pos 8000 " << (xpar1[0]+8000*xpar1[1]+6.4e7*xpar1[2]) <<  "slope y" << ypar1[3] << " chi2 " << chi2 << " chi-y " << chi2y << endmsg;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrackSeedFind::maketracks() {
  // check if there is any track to register
  if (trackCands.size()<1) return StatusCode::SUCCESS;

  //m_measProvider->load();
  //   LHCb::STClusters* itClusters = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
  LHCb::Tracks* trSeedTracksCont;

  if (exist <LHCb::Tracks>(m_trackLocation)) trSeedTracksCont = get<LHCb::Tracks>(m_trackLocation);  
  else {
    if (m_debug) debug() << " in if: container does not exist yet " << endmsg;
    trSeedTracksCont = new LHCb::Tracks();
    put ( trSeedTracksCont , m_trackLocation);
  }
  if ( 0 == trSeedTracksCont ){
    return Error("TrackSeedFind: new returned NULL pointer",StatusCode::FAILURE,1);
  }
  if (m_debug) debug() << " before maketracks: container size " << trSeedTracksCont->size() << endmsg;
  if (trSeedTracksCont->size() >0) 
  {
    Warning(" track container is already filled! ",StatusCode::SUCCESS).ignore();
    if (m_debug) debug() << " track container " << m_trackLocation << " is already filled! " << endmsg;
  }
      
   
  if (m_debug) debug() << " makeTracks: Registering seed tracks in "
                       << m_trackLocation << endmsg
                       << "Processing " << trackCands.size()
                       << " candidates" << endmsg;

  std::list<TrackCand>::iterator iCandidate;
  std::vector<double> xpars,ypars;
  for(iCandidate = trackCands.begin(); iCandidate != trackCands.end(); ++iCandidate) {
    //fill track parameters
    LHCb::Track* tTrack = new LHCb::Track();
    LHCb::State seedState;
    Gaudi::TrackSymMatrix stateCov;
    //debug() << " after state and track" << endmsg;

    // retrieve x and y parameters
    iCandidate->xParams(xpars);
    iCandidate->yParams(ypars);
    double z=ypars[0];
    double y=ypars[1];
    double x=xpars[0]+z*(xpars[1]+z*xpars[2]);
    double tx=xpars[1]+2*z*xpars[2];
    double ty=ypars[3];
    seedState.setState(x,y,z,tx,ty,0);

    stateCov(0,0)=xpars[3];
    stateCov(1,1)=ypars[2]*ypars[2];
    stateCov(2,2)=xpars[4]; // this may be an underestimate; there is also the parabolic contribution to the error
    stateCov(3,3)=ypars[4]*ypars[4];
    stateCov(4,4)=0; // will be filled by PTKick
    seedState.setCovariance(stateCov);
    seedState.setLocation(LHCb::State::AtT);
    double qOverP, sigmaQOverP;
    m_fCalcMomentum->calculate(&seedState, qOverP, sigmaQOverP);
    seedState.setQOverP(qOverP);
    seedState.setErrQOverP2(sigmaQOverP*sigmaQOverP);

    // set new state as current track state
    tTrack->addToStates(seedState);

    // add all hits
    std::vector<TrackSeedHit> hits;
    LHCb::LHCbID id;
    iCandidate->xHits(hits);
    for (unsigned int i=0; i<hits.size(); ++i) {
      if (hits[i].isIT()) {
        id = LHCb::LHCbID(hits[i].getItRef()->channelID());
        tTrack->addToLhcbIDs(id);
      } else {
        id = LHCb::LHCbID( hits[i].getOtRef()->channel());
        tTrack->addToLhcbIDs(id);
      }
    }
    tTrack->setType(LHCb::Track::Ttrack);
    tTrack->setPatRecStatus(LHCb::Track::PatRecMeas);
    tTrack->setHistory( LHCb::Track::TrackSeeding );
    trSeedTracksCont->add(tTrack);
  } //loop over tracks
  if (m_debug)
    debug() << "Stored " << trSeedTracksCont->size()
            << " good seed tracks in " << m_trackLocation <<  endmsg;

  return StatusCode::SUCCESS;
}
