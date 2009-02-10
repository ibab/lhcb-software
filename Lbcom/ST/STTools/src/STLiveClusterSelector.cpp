// $Id: STLiveClusterSelector.cpp,v 1.1 2009-02-10 09:18:38 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
 
// Event
#include "Event/STCluster.h"

#include "STLiveClusterSelector.h"
#include "STDet/DeSTSector.h"

DECLARE_TOOL_FACTORY( STLiveClusterSelector);
 
STLiveClusterSelector::STLiveClusterSelector( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{
  setForcedInit();
  declareInterface<ISTClusterSelector>(this);
}

STLiveClusterSelector::~STLiveClusterSelector()
{
  //destructer
}

bool STLiveClusterSelector::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STLiveClusterSelector::operator()( const LHCb::STCluster* cluster ) const{
  const LHCb::STChannelID chan = cluster->channelID();
  return findSector(chan)->isOKStrip(chan);
}
