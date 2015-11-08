// Include files 

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "TrackInterfaces/ITrackSelector.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "Event/FitNode.h"
#include "Kernel/HitPattern.h"

#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToStream.h"

#include <map>

#include "Map.h"

using namespace LHCb;
using namespace Gaudi;
 

/** @class TrackMonitor TrackMonitor.h "TrackCheckers/TrackMonitor"
 * 
 * Class for track monitoring
 *  @author M. Needham.
 *  @date   6-5-2007
 */                 
                                                           
class TrackMonitorNT : public GaudiTupleAlg {
                                                                             
 public:
                                                                             
  /** Standard construtor */
  TrackMonitorNT( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~TrackMonitorNT();

  /** Algorithm execute */
  virtual StatusCode execute();

  /** Algorithm initialize */
  virtual StatusCode initialize();

 private:

  ITrackSelector* selector(LHCb::Track::Types aType) const {
    return m_selectors[ m_splitByAlgorithm ? aType : LHCb::Track::TypeUnknown ];
  }
  
  void fillNtuple(const LHCb::Track* aTrack, const std::string& type);
private:
  std::string m_tracksInContainer;    ///< Input Tracks container location
  bool m_splitByAlgorithm; 
  typedef std::map<LHCb::Track::Types,ITrackSelector*> Selectors;
  mutable Selectors m_selectors; 
  std::string m_allString;  
  
};

DECLARE_ALGORITHM_FACTORY( TrackMonitorNT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitorNT::TrackMonitorNT(const std::string& name,
                           ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator ),
  m_allString("ALL")
{
  declareProperty( "TracksInContainer", m_tracksInContainer = LHCb::TrackLocation::Default  );
  declareProperty("SplitByAlgorithm", m_splitByAlgorithm = false);
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitorNT::~TrackMonitorNT() {}

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackMonitorNT::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  if (m_splitByAlgorithm){
    for (const auto&  map : TrackMonitorMaps::typeDescription() ){
      // make a tool from this
      m_selectors[map.second] = tool<ITrackSelector>("TrackSelector",map.first+"Selector");  
    } 
  } else {
    m_selectors[LHCb::Track::TypeUnknown] = tool<ITrackSelector>("TrackSelector",m_allString+"Selector");  
  } // splitByType

   
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode TrackMonitorNT::execute()
{
  // get the input data
  LHCb::Tracks* tracks = getIfExists<LHCb::Tracks>(m_tracksInContainer);
  if ( !tracks )
    return Warning( m_tracksInContainer+" not found", StatusCode::SUCCESS, 0);

  std::map<std::string, unsigned int> tMap;

  // # number of tracks
  plot(tracks->size(),1, "# tracks", 0., 500., 50);

  // histograms per track
  
  for (const auto& iterT : *tracks ) { 
    if ( /*selector((iterT)->type())->accept(*iterT) ==*/ true){

      std::string type = ( m_splitByAlgorithm 
                                  ? Gaudi::Utils::toString(iterT->history()) 
                                  : m_allString ); 
      ++tMap[type];
      fillNtuple(iterT,type);
    }
  } // iterT

  // fill counters....
  counter("#Tracks") += tracks->size();
  for (const auto& map : tMap ) counter("#"+map.first) += map.second;

  return StatusCode::SUCCESS;
}


void TrackMonitorNT::fillNtuple(const LHCb::Track* aTrack,
			     const std::string& type){


 

  int maxVelo = 50; 
  std::vector<double> Velo_res;
  std::vector<double> Velo_pull;
  std::vector<bool>   Velo_rType;
  std::vector<int>    Velo_sensor;
 
  int maxTT = 10;
  std::vector<double> TT_res;
  std::vector<double> TT_pull;
  std::vector<int>    TT_station;
  std::vector<int>    TT_layer;
  std::vector<int>    TT_region;
  std::vector<int>    TT_sector;

  int maxOT = 50;
  std::vector<double> OT_res;
  std::vector<double> OT_pull;
  std::vector<int>    OT_station;
  std::vector<int>    OT_layer;
  std::vector<int>    OT_quarter;
  std::vector<int>    OT_module;
  
  int maxIT = 50;
  std::vector<double> IT_res;
  std::vector<double> IT_pull;
  std::vector<int>    IT_station;
  std::vector<int>    IT_layer;
  std::vector<int>    IT_box;
  std::vector<int>    IT_sector;
    
  LHCb::STChannelID   theSTID;
  LHCb::OTChannelID   theOTID;
  LHCb::STChannelID   theTTID;
  LHCb::VeloChannelID theVeloID;
  
  // Loop over the nodes to get the hits variables
  LHCb::Track::ConstNodeRange nodes = aTrack->nodes() ;
  for( LHCb::Track::ConstNodeRange::const_iterator iNodes = nodes.begin() ;
       iNodes != nodes.end() ; ++iNodes) {
    
    // Only loop on hits with measurement
    if ( !(**iNodes).hasMeasurement() ) continue;
    
    const LHCb::Node& aNode = **iNodes;
    LHCb::LHCbID nodeID = aNode.measurement().lhcbID();
    const LHCb::FitNode* fNode = dynamic_cast<const LHCb::FitNode*>(*iNodes);

    if ( aNode.measurement().type() == LHCb::Measurement::IT ){  
      theSTID = nodeID.stID();
      IT_res.push_back(fNode->unbiasedResidual());
      IT_pull.push_back(fNode->unbiasedResidual()/fNode->errUnbiasedResidual());
      IT_station.push_back(theSTID.station());
      IT_layer.push_back(theSTID.layer());
      IT_box.push_back(theSTID.detRegion());
      IT_sector.push_back(theSTID.sector());
    }
    else if ( aNode.measurement().type() == LHCb::Measurement::OT ) {
      theOTID = nodeID.otID();
      OT_res.push_back(fNode->unbiasedResidual()); 
      OT_pull.push_back(fNode->unbiasedResidual()/fNode->errUnbiasedResidual());
      OT_station.push_back(theOTID.station());
      OT_layer.push_back(theOTID.layer());
      OT_quarter.push_back(theOTID.quarter());
      OT_module.push_back(theOTID.module());
    }
    else if ( aNode.measurement().type() == LHCb::Measurement::TT ) {
      theTTID = nodeID.stID(); 
      TT_res.push_back(fNode->unbiasedResidual());
      TT_pull.push_back(fNode->unbiasedResidual()/fNode->errUnbiasedResidual());
      TT_station.push_back(theTTID.station());
      TT_layer.push_back(theTTID.layer());
      TT_region.push_back(theTTID.detRegion());
      TT_sector.push_back(theTTID.sector());
    }
     else if (aNode.measurement().type() == LHCb::Measurement::VeloR ||
	      aNode.measurement().type() == LHCb::Measurement::VeloLiteR ){
       theVeloID = nodeID.veloID();  
       Velo_res.push_back(fNode->unbiasedResidual());
       Velo_pull.push_back(fNode->unbiasedResidual()/fNode->errUnbiasedResidual());
       Velo_sensor.push_back(theVeloID.sensor());
       Velo_rType.push_back(theVeloID.isRType());
     } 
    else if (aNode.measurement().type() == LHCb::Measurement::VeloPhi ||
	     aNode.measurement().type() == LHCb::Measurement::VeloLitePhi ){
       theVeloID = nodeID.veloID();  
       Velo_res.push_back(fNode->unbiasedResidual()); 
       Velo_pull.push_back(fNode->unbiasedResidual()/fNode->errUnbiasedResidual());
       Velo_sensor.push_back(theVeloID.sensor());
       Velo_rType.push_back(theVeloID.isRType());
     }
    
  }

  Tuple theTuple = nTuple( "HitResiduals/"+type+"/" , "" , CLID_ColumnWiseTuple );

  theTuple->farray( "Velo_res"     , Velo_res     , "nVelo" , maxVelo );  
  theTuple->farray( "Velo_pull"    , Velo_pull    , "nVelo" , maxVelo );
  theTuple->farray( "Velo_sensor"  , Velo_sensor  , "nVelo" , maxVelo ); 
  theTuple->farray( "Velo_rType"   , Velo_rType   , "nVelo" , maxVelo );  
    
  theTuple->farray( "TT_res"       , TT_res       , "nTT"   , maxTT );
  theTuple->farray( "TT_pull"      , TT_pull      , "nTT"   , maxTT );
  theTuple->farray( "TT_station"   , TT_station   , "nTT"   , maxTT ); 
  theTuple->farray( "TT_layer"     , TT_layer     , "nTT"   , maxTT );  
  theTuple->farray( "TT_region"    , TT_region    , "nTT"   , maxTT ); 
  theTuple->farray( "TT_sector"    , TT_sector    , "nTT"   , maxTT );

  theTuple->farray( "OT_res"       , OT_res       , "nOT"   , maxOT );  
  theTuple->farray( "OT_pull"      , OT_pull      , "nOT"   , maxOT );
  theTuple->farray( "OT_station"   , OT_station   , "nOT"   , maxOT ); 
  theTuple->farray( "OT_layer"     , OT_layer     , "nOT"   , maxOT );  
  theTuple->farray( "OT_quarter"   , OT_quarter   , "nOT"   , maxOT ); 
  theTuple->farray( "OT_module"    , OT_module    , "nOT"   , maxOT );

  theTuple->farray( "IT_res"       , IT_res       , "nIT"   , maxIT );
  theTuple->farray( "IT_pull"      , IT_pull      , "nIT"   , maxIT );
  theTuple->farray( "IT_station"   , IT_station   , "nIT"   , maxIT ); 
  theTuple->farray( "IT_layer"     , IT_layer     , "nIT"   , maxIT );  
  theTuple->farray( "IT_box"       , IT_box       , "nIT"   , maxIT ); 
  theTuple->farray( "IT_sector"    , IT_sector    , "nIT"   , maxIT );
  
  theTuple->column("p"           , aTrack->p()/Gaudi::Units::GeV );
  theTuple->column("probChi2"    , aTrack->probChi2() );
  theTuple->column("chi2",aTrack->chi2() );
  theTuple->column("ndof",aTrack->nDoF() );
  theTuple->column("type",aTrack->type() ) ;
  theTuple->column("z", aTrack->firstState().z() ) ;
  theTuple->column("x", aTrack->firstState().x() ) ;
  theTuple->column("y", aTrack->firstState().y() ) ;
  theTuple->column("tx", aTrack->firstState().tx() ) ;
  theTuple->column("ty", aTrack->firstState().ty() ) ;
  theTuple->column("qop", aTrack->firstState().qOverP() ) ;
  theTuple->column("veloChi2", aTrack->info(LHCb::Track::FitVeloChi2,0)) ;
  theTuple->column("veloNdof", aTrack->info(LHCb::Track::FitVeloNDoF,0)) ;
  theTuple->column("TChi2", aTrack->info(LHCb::Track::FitTChi2,0)) ;
  theTuple->column("TNdof", aTrack->info(LHCb::Track::FitTNDoF,0)) ;
  
  LHCb::HitPattern hitpattern(aTrack->lhcbIDs()) ;
  theTuple->column("numVeloStations",hitpattern.numVeloStations()) ;
  theTuple->column("numVeloStationsOverlap",hitpattern.numVeloStationsOverlap()) ;
  theTuple->column("numITStationsOverlap",hitpattern.numITStationsOverlap()) ;
  theTuple->column("numITOTStationsOverlap",hitpattern.numITOTStationsOverlap()) ;
  theTuple->column("numVeloHoles",hitpattern.numVeloHoles()) ;
  theTuple->column("numTHoles",hitpattern.numTHoles()) ;
  theTuple->column("numTLayers",hitpattern.numTLayers()) ;
  theTuple->column("numVeloStations",hitpattern.numVeloStations()) ;
  theTuple->column("numVeloClusters",hitpattern.numVeloR()+hitpattern.numVeloPhi()) ;
  
  theTuple->write();
}
