#include "MuonDet/DeMuonDetector.h"
#include "Kernel/MuonTileID.h"

#include "MuonDetPosTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDetPosTool
//
// 2009-03-17 : Giacomo GRAZIANI
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonDetPosTool )

MuonDetPosTool::MuonDetPosTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_muonDetector(NULL)
{
  declareInterface<IMuonFastPosTool>(this);

}

MuonDetPosTool::~MuonDetPosTool() {} 


StatusCode MuonDetPosTool::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  
  if (!sc) return sc;
  m_muonDetector = getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDetector){
    err()<<"error retrieving the Muon detector element "<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

 
StatusCode MuonDetPosTool::finalize ()
{
  return   GaudiTool::finalize() ;
}


StatusCode MuonDetPosTool::calcTilePos(const LHCb::MuonTileID& tile, 
                                       double& x, double& deltax,
                                       double& y, double& deltay,
                                       double& z, double& deltaz) {
  return m_muonDetector->Tile2XYZ(tile, 
                                  x,deltax,
                                  y,deltay,
                                  z,deltaz);
}

