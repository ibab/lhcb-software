#include "STSelectChannelIDByTruth.h"

#include <boost/assign/list_of.hpp>

#include "Kernel/STChannelID.h"
#include "Event/MCParticle.h"
#include "Event/STCluster.h"

#include "MCInterfaces/IMCParticleSelector.h"

DECLARE_TOOL_FACTORY( STSelectChannelIDByTruth )
 
STSelectChannelIDByTruth::STSelectChannelIDByTruth( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent),
  m_links(0,0,""),
  m_configured(false)
{
  std::vector<int> tmp = boost::assign::list_of(Spillover)(Real);
  declareProperty("Types",m_types = tmp); 
  declareSTConfigProperty("InputData" , m_clusterLocation  , LHCb::STClusterLocation::TTClusters);
  declareInterface<ISTChannelIDSelector>(this);
}

STSelectChannelIDByTruth::~STSelectChannelIDByTruth()
{
  //destructer
}

StatusCode STSelectChannelIDByTruth::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_selector = tool<IMCParticleSelector>( "MCParticleSelector", 
                                          "Selector", this );

  incSvc()->addListener(this, IncidentType::BeginEvent);

  return StatusCode::SUCCESS;
}

void STSelectChannelIDByTruth::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) {
    m_configured = false; 
  }
}

bool STSelectChannelIDByTruth::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STSelectChannelIDByTruth::operator()( const LHCb::STChannelID& id) const{

  // get the linker table
  if (!m_configured){
    m_configured = true;
    m_links = STLinks(evtSvc(), msgSvc(),
                      m_clusterLocation);
    if (m_links.notFound()) {
      throw GaudiException("no Linker", name() ,StatusCode::FAILURE);
    }
  }  

  // spillover or not
  int clusType = Real;
  const LHCb::MCParticle* aParticle = m_links.first(id);
  if (0 != aParticle) {
    clusType = Spillover; 
  }

  // check we want this type
  if (std::find(m_types.begin(), m_types.end(), clusType) == m_types.end()){
    return false;
  }

  // select using the select [if there is a link]
  return aParticle != 0 ? m_selector->accept(aParticle) : true;
}



