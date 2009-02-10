// $Id: STSelectClustersByBoard.cpp,v 1.1 2009-02-10 09:18:38 mneedham Exp $
 
// Kernel
#include "GaudiKernel/ToolFactory.h"
 
// Event
#include "Event/STCluster.h"

#include "STSelectClustersByBoard.h"
#include "STDet/DeSTSector.h"

#include <vector>
#include <algorithm>
#include <boost/foreach.hpp>

#include "Kernel/STChannelID.h"


DECLARE_TOOL_FACTORY( STSelectClustersByBoard);
 
STSelectClustersByBoard::STSelectClustersByBoard( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{

  declareProperty("boards", m_boards); 
  declareInterface<ISTClusterSelector>(this);
}

STSelectClustersByBoard::~STSelectClustersByBoard()
{
  //destructer
}

StatusCode STSelectClustersByBoard::initialize() {
  
  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // sort the board to save time by allowing binary search
  std::sort(m_boards.begin(), m_boards.end());

  return StatusCode::SUCCESS;
}

bool STSelectClustersByBoard::select( const LHCb::STCluster* cluster ) const{
  return (*this) (cluster);  
}
  
bool STSelectClustersByBoard::operator()( const LHCb::STCluster* cluster ) const{
  const unsigned int testBoard = cluster->sourceID();
  return std::binary_search(m_boards.begin(), m_boards.end(),testBoard);
}
