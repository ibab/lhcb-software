#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/StateParameters.h"
#include "Event/State.h"
#include "Event/CaloCluster.h"
#include "Event/CaloPosition.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "CaloDet/DeCalorimeter.h"

#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"


#ifdef _WIN32
#pragma warning ( disable : 4355 ) // This used in initializer list, needed for ToolHandles
#endif


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
  bool m_useGeometricZ ;
  bool m_requireTHits ;
  double m_clusterZCorrection ;
  std::string m_clusterLocation;
  std::string m_caloDigitLocation;
  DeCalorimeter* m_caloDet ;
  double m_geometricZ ;
  ToolHandle<ITrackExtrapolator> m_extrapolator ;

  IHistogram1D* m_dxASideH1[3] ;
  IHistogram1D* m_dyASideH1[3] ;
  IHistogram1D* m_dxCSideH1[3] ;
  IHistogram1D* m_dyCSideH1[3] ;
  IHistogram1D* m_dyVeloASideH1[3] ;
  IHistogram1D* m_dyVeloCSideH1[3] ;
  IHistogram1D* m_zH1[3] ;
  IHistogram1D* m_eOverPH1[3] ;
  IProfile1D* m_dyVsYPr ;
  IProfile1D* m_dyVsXPr ;
  IProfile1D* m_dyVsTyPr ;
  IProfile1D* m_dyVsTxPr ;
  IProfile1D* m_dxVsYPr ;
  IProfile1D* m_dxVsXPr ;
  IProfile1D* m_dxVsTyPr ;
  IProfile1D* m_dxVsTxPr ;

} ;
 
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackCaloMatchMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackCaloMatchMonitor::TrackCaloMatchMonitor( const std::string& name,
					ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_extrapolator("TrackMasterExtrapolator",this)
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty( "CaloSystem", m_caloName = "Ecal") ;
  declareProperty( "UseClusters", m_useClusters = true ) ;
  declareProperty( "UseGeometricZ", m_useGeometricZ = true ) ; // this you need for cosmics (MIPs)
  declareProperty( "ClusterZCorrection", m_clusterZCorrection = 0) ;
  declareProperty( "RequireTHits",m_requireTHits = true ) ; // this you need for cosmics (MIPs)
  declareProperty( "Extrapolator", m_extrapolator) ;
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
  sc = m_extrapolator.retrieve() ;
  if ( sc.isFailure() ) {
    error() << "Could not retrieve extrapolator" << endmsg ;
    return sc;
  }

  if(m_caloName == "Ecal") {
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    if( m_useClusters ) m_clusterLocation = LHCb::CaloClusterLocation::Ecal ;
    m_caloDigitLocation = LHCb::CaloDigitLocation::Ecal ;
    m_geometricZ = m_caloDet->geometry()->toGlobal(Gaudi::XYZPoint()).z() + m_caloDet->zOffset() ;
  } else if( m_caloName == "Hcal") {
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    if( m_useClusters ) m_clusterLocation = LHCb::CaloClusterLocation::Hcal ;
    m_caloDigitLocation = LHCb::CaloDigitLocation::Hcal ;
    m_geometricZ = m_caloDet->geometry()->toGlobal(Gaudi::XYZPoint()).z() + m_caloDet->zOffset() ;
  } else if( m_caloName == "Prs") {
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_caloDigitLocation = LHCb::CaloDigitLocation::Prs ;
    m_geometricZ = m_caloDet->geometry()->toGlobal(Gaudi::XYZPoint()).z() ;
  } else if( m_caloName == "Spd") {
    m_caloDigitLocation = LHCb::CaloDigitLocation::Spd ;
    m_caloDet = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
    m_geometricZ = m_caloDet->geometry()->toGlobal(Gaudi::XYZPoint()).z() ;
  } else {
    error() << "Unknown calo system: " << m_caloName << endmsg ;
    sc = StatusCode::FAILURE ; 
  }

  setHistoTopDir("Track/") ;
  char systitle[3][128] = { "outer","middle","inner"} ;
  char histitle[128] ;
  for(int i=0; i<3; ++i) {
    sprintf(histitle,"x%s - xTRK (%s A-side)",m_caloName.c_str(),systitle[i]) ;
    m_dxASideH1[i] = book1D(histitle,-200,200) ;
    sprintf(histitle,"y%s - yTRK (%s A-side)",m_caloName.c_str(),systitle[i]) ;
    m_dyASideH1[i] = book1D(histitle,-200,200) ;
    sprintf(histitle,"x%s - xTRK (%s C-side)",m_caloName.c_str(),systitle[i]) ;
    m_dxCSideH1[i] = book1D(histitle,-200,200) ;
    sprintf(histitle,"y%s - yTRK (%s C-side)",m_caloName.c_str(),systitle[i]) ;
    m_dyCSideH1[i] = book1D(histitle,-200,200) ;
    sprintf(histitle,"y%s - yVELO (%s A-side)",m_caloName.c_str(),systitle[i]) ;
    m_dyVeloASideH1[i] = book1D(histitle,-200,200) ;
    sprintf(histitle,"y%s - yVELO (%s C-side)",m_caloName.c_str(),systitle[i]) ;
    m_dyVeloCSideH1[i] = book1D(histitle,-200,200) ;
    sprintf(histitle,"zMatch (%s)",systitle[i]) ;
    m_zH1[i] = book1D(histitle,m_geometricZ - 400, m_geometricZ + 400 ) ;
    sprintf(histitle,"E over P (%s)",systitle[i]) ;
    m_eOverPH1[i] = book1D(histitle,-2,2) ;
  }

  m_dxVsXPr = bookProfile1D("dxVsX","dx versus x",-3500,3500) ;
  m_dxVsYPr = bookProfile1D("dxVsY","dx versus y",-3500,3500) ;
  m_dxVsTxPr = bookProfile1D("dxVsTx","dx versus tx",-0.6,0.6) ;
  m_dxVsTyPr = bookProfile1D("dxVsTy","dx versus ty",-0.3,0.3) ;
  
  m_dyVsXPr = bookProfile1D("dyVsX","dy versus x",-3500,3500) ;
  m_dyVsYPr = bookProfile1D("dyVsY","dy versus y",-3500,3500) ;
  m_dyVsTxPr = bookProfile1D("dyVsTx","dy versus tx",-0.6,0.6) ;
  m_dyVsTyPr = bookProfile1D("dyVsTy","dy versus ty",-0.3,0.3) ;
  
  if(msgLevel(MSG::DEBUG)) debug() << "CaloDet: center = " 
	  << m_caloDet->geometry()->toGlobal(Gaudi::XYZPoint()) 
	  << " zoffset: " << m_caloDet->zOffset()
	  << " geometric Z: " << m_geometricZ
	  << " zsize:   " << m_caloDet->zSize() << endmsg ;
  
  return sc;
}

namespace {

  struct MyCaloPosition
  {
    MyCaloPosition( const LHCb::CaloCellID& _cell,
		    const LHCb::CaloPosition& _pos) : cell(_cell), pos(_pos) {}
    LHCb::CaloCellID cell ;
    LHCb::CaloPosition pos ;
  } ;
  
  const LHCb::State* unbiasedVeloState( const LHCb::Track& track )
  {
    const LHCb::State* rc(0) ;
    LHCb::Track::ConstNodeRange nodes = track.nodes() ;
    if( track.hasVelo() && nodes.size()>0 ) {
      // find the last node with a velo measurement
      const LHCb::Node* node(0) ;
      for( const LHCb::Node* inode: nodes ) {
	if( inode->z() < StateParameters::ZEndVelo &&
	    (node==0 || (node->z() < inode->z()) ) )
	  node = inode ;
      }
      const LHCb::FitNode* fitnode = static_cast<const LHCb::FitNode*>(node) ;
      bool upstream = nodes.front()->z() > nodes.back()->z() ;
      rc = upstream ? 
	&(fitnode->filteredStateBackward()) : 
	&(fitnode->filteredStateForward()) ;
    }
    return rc ;
  }
}

StatusCode TrackCaloMatchMonitor::execute()
{ 
  // make a list of calo positions, depending on the subsystem use clusters or cells
  std::vector< MyCaloPosition > calopositions ;

  if( !m_clusterLocation.empty() ) {
    const LHCb::CaloClusters* caloclusters = get<LHCb::CaloClusters>( m_clusterLocation ) ;
    calopositions.reserve( caloclusters->size()) ;
    for( const LHCb::CaloCluster* cluster: *caloclusters) {
      calopositions.push_back( MyCaloPosition( cluster->seed(), cluster->position()) ) ;
    }
  } else {
    const LHCb::CaloDigits* calodigits = get<LHCb::CaloDigits>( m_caloDigitLocation ) ;
    calopositions.reserve( calodigits->size()) ;
    for( const LHCb::CaloDigit* digit: *calodigits) {
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

  if(m_useGeometricZ) {
    for( MyCaloPosition& cluster: calopositions) 
      cluster.pos.setZ( m_geometricZ ) ;
  }

  LHCb::Track::Range trackcontainer = get<LHCb::Track::Range>( m_trackLocation ) ;
  for( const LHCb::Track* track: trackcontainer) 
    if( !m_requireTHits || track->hasT() ) {
      const LHCb::State& closest = track->closestState( m_geometricZ  );
      LHCb::StateVector state = LHCb::StateVector( closest.stateVector(), closest.z()) ;
      m_extrapolator->propagate( state, m_geometricZ ) ;

      const LHCb::State* fullvelostate = unbiasedVeloState( *track ) ;
      LHCb::StateVector velostate ;
      if(fullvelostate) {
	velostate = LHCb::StateVector( fullvelostate->stateVector(), fullvelostate->z()) ;
	velostate.setQOverP( state.qOverP() ) ;
	m_extrapolator->propagate( velostate, m_geometricZ ) ;
      }

      for( const MyCaloPosition& cluster: calopositions) {
	//state = &(track->closestState(pos.z())) ;
	double dz = cluster.pos.z() + m_clusterZCorrection - state.z() ;
	double xtrack = state.x() + state.tx() * dz ;
	double ytrack = state.y() + state.ty() * dz ;
	double dx = cluster.pos.x() - xtrack ;
	double dy = cluster.pos.y() - ytrack ;
	if( std::abs(dy)<200 && std::abs(dx)<200 ) {
	  if( xtrack > 0 ) {
	    m_dxASideH1[cluster.cell.area()]->fill( dx ) ;
	    m_dyASideH1[cluster.cell.area()]->fill( dy ) ;
	  } else {
	    m_dxCSideH1[cluster.cell.area()]->fill( dx ) ;
	    m_dyCSideH1[cluster.cell.area()]->fill( dy ) ;
	  }
	  m_eOverPH1[cluster.cell.area()]->fill( cluster.pos.e() * state.qOverP() ) ;
	  // compute the z-coordinate for which sqrt(dx^2+dy^2) is minimal
	  double tx = state.tx() ;
	  double ty = state.ty() ;
	  double ddz = ( tx*dx + ty*dy ) / (tx*tx+ty*ty) ;
	  m_zH1[cluster.cell.area()]->fill( cluster.pos.z() + ddz ) ;
	  if( std::abs(dy)<200 && std::abs(dx)<100 ) {
	    m_dxVsXPr->fill( xtrack,dx ) ;
	    m_dxVsYPr->fill( ytrack,dx ) ;
	    m_dxVsTxPr->fill( state.tx(),dx ) ;
	    m_dxVsTyPr->fill( state.ty(),dx ) ;
	    m_dyVsXPr->fill( xtrack,dy ) ;
	    m_dyVsYPr->fill( ytrack,dy ) ;
	    m_dyVsTxPr->fill( state.tx(),dy ) ;
	    m_dyVsTyPr->fill( state.ty(),dy ) ;
	  }
	}
	if(fullvelostate && std::abs(dx)<200 ) {
	  ytrack = velostate.y() + velostate.ty() * dz ;
	  dy = cluster.pos.y() - ytrack ;
	  if( cluster.pos.x() > 0 ) {
	    m_dyVeloASideH1[cluster.cell.area()]->fill( dy ) ;
	  } else {
	    m_dyVeloCSideH1[cluster.cell.area()]->fill( dy ) ;
	  }
	}
      }
    }
  return StatusCode::SUCCESS ;
}


