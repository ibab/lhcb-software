// Include files

// STL
#include <cmath>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "SimplePVSeedTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SimplePVSeedTool
//
// 2005-11-19 : Mariusz Witek
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( SimplePVSeedTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimplePVSeedTool::SimplePVSeedTool( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPVSeeding>(this);
}
//=============================================================================
// Destructor
//=============================================================================
SimplePVSeedTool::~SimplePVSeedTool() {}

//=============================================================================
// getSeeds
//=============================================================================
void SimplePVSeedTool::getSeeds(std::vector<const LHCb::Track*>& inputTracks,
				const Gaudi::XYZPoint& beamspot,
				std::vector<Gaudi::XYZPoint>& seeds) {

  if(inputTracks.size() < 3 ) return;

  if(msgLevel(MSG::DEBUG))  {
     debug() << " Beam spot is ignored. BS: " << beamspot << endmsg;
  }

  std::vector<double> zseeds;
  Gaudi::XYZPoint ep(0., 0., 0.);
  seeds.push_back(ep);

}

//=============================================================================
