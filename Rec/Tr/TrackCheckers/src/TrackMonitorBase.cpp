// $Id: TrackMonitorBase.cpp,v 1.2 2007-06-27 15:05:06 mneedham Exp $
// Include files 
#include "TrackMonitorBase.h"
#include "Event/Track.h"
#include "Map.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitorBase::TrackMonitorBase( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ),
  m_allString("ALL"){

  declareProperty( "TracksInContainer",
                   m_tracksInContainer = LHCb::TrackLocation::Default  );
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackMasterExtrapolator" );
  declareProperty("SplitByAlgorithm", m_splitByAlgorithm = false);
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitorBase::~TrackMonitorBase() {}; 

//=============================================================================
// Initialization. Check parameters
//=============================================================================
StatusCode TrackMonitorBase::initialize()
{

  static const std::string histoDir = "Track/" ;
  if ( "" == histoTopDir() ) setHistoTopDir(histoDir);

  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  //  m_selector = tool<ITrackSelector>( m_selectorName,
  //                                        "TrackSelector", this );
  
  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName);
  
  // Retrieve the magnetic field and the poca tool
  m_poca = tool<ITrajPoca>("TrajPoca");
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  if (m_splitByAlgorithm == true){
    const TrackMaps::TypeMap& tMap = TrackMaps::typeDescription();
    TrackMaps::TypeMap::const_iterator iterM = tMap.begin();
    for (; iterM != tMap.end(); ++iterM ){
      // make a tool from this
      std::string name = iterM->first+"Selector";
      ITrackSelector* selector = tool<ITrackSelector>("TrackSelector",name);  
      m_selectors[iterM->second] = selector;  
    } // iterM
  }
  else {
    std::string name = m_allString+"Selector";
    ITrackSelector* selector = tool<ITrackSelector>("TrackSelector",name);  
    m_selectors[LHCb::Track::TypeUnknown] = selector;  
  } // splitByType

  return StatusCode::SUCCESS;
};
