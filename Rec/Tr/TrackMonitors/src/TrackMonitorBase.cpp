// Include files 
#include "TrackMonitorBase.h"
#include "Event/Track.h"
#include "Map.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackMonitorBase::TrackMonitorBase( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ), m_poca(0), m_pIMF(0), m_extrapolator(0),
  m_allString("ALL"){

  declareProperty( "TracksInContainer",
                   m_tracksInContainer = LHCb::TrackLocation::Default  );
  declareProperty( "Extrapolator",
                   m_extrapolatorName = "TrackMasterExtrapolator" );
  declareProperty("SplitByAlgorithm", m_splitByAlgorithm = false);
  declareProperty("SplitByType", m_splitByType = true);
}

//=============================================================================
// Destructor
//=============================================================================
TrackMonitorBase::~TrackMonitorBase() {}

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

  m_extrapolator = tool<ITrackExtrapolator>(m_extrapolatorName,this);
  
  // Retrieve the magnetic field and the poca tool
  m_poca = tool<ITrajPoca>("TrajPoca");
  m_pIMF = svc<IMagneticFieldSvc>( "MagneticFieldSvc",true );

  return StatusCode::SUCCESS;
}

std::string TrackMonitorBase::histoDirName(const LHCb::Track& track) const
{
  std::string type = "" ;
  if( splitByType() ) {
    type = Gaudi::Utils::toString(track.type()) ;
    if( track.checkFlag( LHCb::Track::Backward ) ) type += "Backward" ;
  } else if( splitByAlgorithm() ) {
    type = Gaudi::Utils::toString(track.history()) ;
  } 
  return type ;
}
