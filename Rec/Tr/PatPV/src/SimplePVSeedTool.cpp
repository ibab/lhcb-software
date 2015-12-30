// Include files

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
  : base_class( type, name , parent )
{
  declareInterface<IPVSeeding>(this);
}
//=============================================================================
// Destructor
//=============================================================================
SimplePVSeedTool::~SimplePVSeedTool() = default;

//=============================================================================
// getSeeds
//=============================================================================
std::vector<Gaudi::XYZPoint> 
SimplePVSeedTool::getSeeds(const std::vector<const LHCb::Track*>& inputTracks,
                           const Gaudi::XYZPoint& beamspot) const {

  std::vector<Gaudi::XYZPoint> seeds;
  if(inputTracks.size() < 3 ) return seeds;

  if(msgLevel(MSG::DEBUG))  {
     debug() << " Beam spot is ignored. BS: " << beamspot << endmsg;
  }

  seeds.emplace_back( 0., 0., 0.);
  return seeds;
}

//=============================================================================
