// Kernel
#include "GaudiKernel/IIncidentSvc.h" 

// Event
#include "Event/STCluster.h"

#include "STSelectSpilloverCluster.h"

DECLARE_TOOL_FACTORY( STSelectSpilloverCluster)
 
STSelectSpilloverCluster::STSelectSpilloverCluster( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_configured(false)
{

  declareProperty("spilloverFraction", m_spilloverFraction = 0.5 );
  declareProperty("nBits",m_nBits = 7u);
  declareSTConfigProperty("spilloverData" , m_spilloverLocation  , LHCb::STClusterLocation::TTClusters);

  declareInterface<ISTClusterSelector>(this);
}

StatusCode STSelectSpilloverCluster::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_maxCharge = double (2 << m_nBits) -1;

  incSvc()->addListener(this, IncidentType::BeginEvent);

  return StatusCode::SUCCESS;
}

STSelectSpilloverCluster::~STSelectSpilloverCluster()
{
  //destructer
}


void STSelectSpilloverCluster::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) {
    m_configured = false; 
  }
}

bool STSelectSpilloverCluster::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STSelectSpilloverCluster::operator()( const LHCb::STCluster* cluster ) const{

  if (!m_configured){
    // get the spillover clusters
    m_spilloverData = get<LHCb::STClusters>(m_spilloverLocation);
    m_configured = true;
  }

  const LHCb::STCluster* spillClus = m_spilloverData->object(cluster->key());
 
  return (spillClus == 0 ? false : chargeSelection(spillClus, cluster));
}

bool STSelectSpilloverCluster::chargeSelection(const LHCb::STCluster* spillClus, 
                                               const LHCb::STCluster* cluster) const {

  const double spillCharge = std::min(spillClus->totalCharge() , m_maxCharge );
  const double totCharge = std::min(cluster->totalCharge(), m_maxCharge);
  return totCharge < m_spilloverFraction * spillCharge;  
}

