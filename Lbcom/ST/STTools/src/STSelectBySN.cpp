// Event
#include "Event/STCluster.h"

//boost
#include "boost/numeric/conversion/bounds.hpp"


#include "STDet/DeSTSector.h"

#include "STSelectBySN.h"


DECLARE_TOOL_FACTORY( STSelectBySN)
 
STSelectBySN::STSelectBySN( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{

  declareProperty("minSN", m_minSN = 0.0 );
  declareProperty("maxSN", m_maxSN = boost::numeric::bounds<double>::highest()); 
  declareInterface<ISTClusterSelector>(this);
}

STSelectBySN::~STSelectBySN()
{
  //destructer
}

StatusCode STSelectBySN::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  info() << "Min Charge set to " << m_minSN << " / max charge set to " 
	 << m_maxSN << endmsg;

  return StatusCode::SUCCESS;
}


bool STSelectBySN::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STSelectBySN::operator()( const LHCb::STCluster* cluster ) const{
  const DeSTSector* sector = findSector(cluster->channelID());
  const double sn =  cluster->totalCharge() / sector->noise(cluster->channelID());
  return sn > m_minSN && sn < m_maxSN;
}
