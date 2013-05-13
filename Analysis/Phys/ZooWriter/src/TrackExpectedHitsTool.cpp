// $Id: $
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"


#include "Event/Track.h"
#include "Kernel/LHCbID.h"
#include "Kernel/HitPattern.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

#include "TrackExpectedHitsTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TrackExpectedHitsTool
//
// 2010-01-14 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackExpectedHitsTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackExpectedHitsTool::TrackExpectedHitsTool(const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : GaudiTool(type, name, parent)
{
  declareInterface<TrackExpectedHitsTool>(this);
  declareProperty("ForceVeloExpectation", m_forceVeloExpectation = false);
}

//=============================================================================
// Destructor
//=============================================================================
TrackExpectedHitsTool::~TrackExpectedHitsTool() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackExpectedHitsTool::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if (sc.isFailure()) return sc;  // error printed already by GaudiTool

  debug() << "==> Initialize" << endmsg;

  m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  m_otExpectation = tool<IHitExpectation>("OTHitExpectation");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackExpectedHitsTool::finalize()
{
  debug() << "==> Finalize" << endmsg;


  return GaudiTool::finalize();  // must be called after all other actions
}

StatusCode TrackExpectedHitsTool::execute(
	const LHCb::Track& tr, LHCb::HitPattern& pat, std::vector<LHCb::LHCbID>& ids) const
{
    ids.reserve(2 * tr.nLHCbIDs());

#if DV_VER >= 334
    IVeloExpectation::VeloPattern velo[4];
#else
    std::bitset<23> velo[4];
#endif
    if (!m_forceVeloExpectation) {
	m_veloExpectation->expectedInfo(tr, velo);
    } else {
	m_veloExpectation->expectedInfo(tr, -1e3, 1e3, velo);
    }
    m_ttExpectation->collect(tr, ids);
    m_itExpectation->collect(tr, ids);
    m_otExpectation->collect(tr, ids);

    pat = LHCb::HitPattern(ids);
    pat.setVeloRA(velo[0]);
    pat.setVeloRC(velo[1]);
    pat.setVeloPhiA(velo[2]);
    pat.setVeloPhiC(velo[3]);

    return StatusCode::SUCCESS;
}
