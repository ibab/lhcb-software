#include "GaudiKernel/AlgFactory.h" 
//#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/CaloCluster.h"
#include "Event/CaloPosition.h"
#include "CaloDet/DeCalorimeter.h"
#include <boost/foreach.hpp>
#
class TrackCaloMatchMonitor : public GaudiHistoAlg 
{
public:
   /** Standard construtor */
  TrackCaloMatchMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackCaloMatchMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  std::string m_trackLocation;
  std::string m_caloName ;
  bool m_useClusters ;
  std::string m_clusterLocation;
  std::string m_caloDigitLocation;
  DeCalorimeter* m_caloDet ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackCaloMatchMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCaloMatchMonitor::TrackCaloMatchMonitor( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty( "CaloSystem", m_caloName = "Ecal") ;
  declareProperty( "UseClusters", m_useClusters = true ) ;
  //declareProperty( "ClusterContainer", m_clusterLocation = LHCb::CaloClusterLocation::Ecal ) ;
  //declareProperty( "CaloDigitContainer", m_caloDigitsLocation = LHCb::CaloDigitLocation::Ecal ) ;
}

//=============================================================================
// Destructor
//=============================================================================
TrackCaloMatchMonitor::~TrackCaloMatchMonitor() 
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackCaloMatchMonitor::initialize() 
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;             // error printed already by GaudiAlgorithm
  if(m_caloName == "Ecal") {
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    if( m_useClusters ) m_clusterLocation = LHCb::CaloClusterLocation::Ecal ;
    m_caloDigitLocation = LHCb::CaloDigitLocation::Ecal ;
  } else if( m_caloName == "Hcal") {
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    if( m_useClusters ) m_clusterLocation = LHCb::CaloClusterLocation::Hcal ;
    m_caloDigitLocation = LHCb::CaloDigitLocation::Hcal ;
  } else if( m_caloName == "Prs") {
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_caloDigitLocation = LHCb::CaloDigitLocation::Prs ;
  } else if( m_caloName == "Spd") {
    m_caloDigitLocation = LHCb::CaloDigitLocation::Spd ;
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  } else {
    error() << "Unknown calo system: " << m_caloName << endreq ;
    sc = StatusCode::FAILURE ; 
  }
  return sc;
}

struct MyCaloPosition
{
  MyCaloPosition( const LHCb::CaloCellID& _cell,
		  const LHCb::CaloPosition& _pos) : cell(_cell), pos(_pos) {}
  LHCb::CaloCellID cell ;
  LHCb::CaloPosition pos ;
} ;

StatusCode TrackCaloMatchMonitor::execute()
{ 
  setHistoTopDir("Track/") ;

  // make a list of calo positions, depending on the subsystem use clusters or cells
  std::vector< MyCaloPosition > calopositions ;
  if( !m_clusterLocation.empty() ) {
    const LHCb::CaloClusters* caloclusters = get<LHCb::CaloClusters>( m_clusterLocation ) ;
    calopositions.reserve( caloclusters->size()) ;
    BOOST_FOREACH( const LHCb::CaloCluster* cluster, *caloclusters) 
      calopositions.push_back( MyCaloPosition( cluster->seed(), cluster->position()) ) ;
  } else {
    const LHCb::CaloDigits* calodigits = get<LHCb::CaloDigits>( m_caloDigitLocation ) ;
    calopositions.reserve( calodigits->size()) ;
    BOOST_FOREACH( const LHCb::CaloDigit* digit, *calodigits) {
      if (! m_caloDet->isNoisy(digit->cellID()) ) {
	Gaudi::XYZPoint center = m_caloDet->cellCenter(digit->cellID()) ;
	Gaudi::Vector3 parameters ;
	parameters(0) = center.x() ;
	parameters(1) = center.y() ;
	parameters(2) = digit->e() ;
	LHCb::CaloPosition pos ;
	pos.setZ( center.z() ) ;
	pos.setParameters(parameters) ;
	calopositions.push_back( MyCaloPosition(digit->cellID(),pos) ) ;
      }
    }
  }

  const LHCb::Tracks* trackcontainer = get<LHCb::Tracks>( m_trackLocation ) ;

  BOOST_FOREACH( const LHCb::Track* track, *trackcontainer) 
    if( track->hasT() ) {
      const LHCb::State* state = track->stateAt( LHCb::State::EndRich2 ) ;
      if(state==0) {
	//info() << "strange: cannot find EndRich2 state: " << std::endl
	//      << *track << endreq ;
	state = &(track->closestState(12000)) ;
      }
      BOOST_FOREACH( const MyCaloPosition& cluster, calopositions) {
	//state = &(track->closestState(pos.z())) ;
	double dz = cluster.pos.z() -state->z() ;
	double xtrack = state->x() + state->tx() * dz ;
	double ytrack = state->y() + state->ty() * dz ;
	double dx = cluster.pos.x() - xtrack ;
	double dy = cluster.pos.y() - ytrack ;
	if( std::abs(dy)<200 && std::abs(dx)<200 ) {
	  if( cluster.cell.area()==0 ) {
	    if(std::abs(dy)<100) plot(dx,"dx_out","xCALO - xTRK",-200,200) ;
	    if(std::abs(dx)<100) plot(dy,"dy_out","yCALO - yTRK",-200,200) ;
	  } else if( cluster.cell.area()==1 ) {
	    if(std::abs(dy)<100) plot(dx,"dx_mid","xCALO - xTRK",-200,200) ;
	    if(std::abs(dx)<100) plot(dy,"dy_mid","yCALO - yTRK",-200,200) ;
	  } else if( cluster.cell.area()==2 ) {
	    if(std::abs(dy)<100) plot(dx,"dx_inn","xCALO - xTRK",-200,200) ;
	    if(std::abs(dx)<100) plot(dy,"dy_inn","yCALO - yTRK",-200,200) ;
	  } 
	  if( std::abs(dy)<200 && std::abs(dx)<100 ) {
	    profile1D( xtrack, dy, "dyVersusX","dy versus x",-3500,3500) ;
	    profile1D( ytrack, dy, "dyVersusY","dy versus y",-3500,3500) ;
	    profile1D( xtrack, dx, "dxVersusX","dx versus x",-3500,3500) ;
	    profile1D( ytrack, dx, "dxVersusY","dx versus y",-3500,3500) ;
	  }
	}
      }
    }
  return StatusCode::SUCCESS ;
}

