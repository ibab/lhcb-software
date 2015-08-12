
//-----------------------------------------------------------------------------
/** @file RichRecNULLMCTruthTool.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::MC::NULLMCTruthTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------

// local
#include "RichRecNULLMCTruthTool.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( NULLMCTruthTool )

// Standard constructor
NULLMCTruthTool::NULLMCTruthTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : ToolBase ( type, name, parent )
{
  // interface
  declareInterface<Rich::Rec::MC::IMCTruthTool>(this);
}

// destructor
NULLMCTruthTool::~NULLMCTruthTool() { }

const LHCb::MCParticle *
NULLMCTruthTool::mcParticle( const LHCb::Track *,
                             const double ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::mcParticle( const LHCb::RichRecTrack *,
                             const double ) const
{
  return NULL;
}

void
NULLMCTruthTool::mcRichHits( const LHCb::RichRecPixel *,
                             SmartRefVector<LHCb::MCRichHit> & ) const
{
}

bool
NULLMCTruthTool::mcParticle( const LHCb::RichRecPixel *,
                             std::vector<const LHCb::MCParticle*> & ) const
{
  return false;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueRecPhoton( const LHCb::RichRecPhoton * ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueRecPhoton( const LHCb::RichRecSegment *,
                                const LHCb::RichRecPixel * ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueRecPhoton( const LHCb::MCParticle *,
                                const Rich::HPDPixelCluster& ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueRecPhoton( const LHCb::MCParticle *,
                                const LHCb::RichSmartID ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueCherenkovPhoton( const LHCb::RichRecPhoton * ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueCherenkovPhoton( const LHCb::RichRecSegment *,
                                      const LHCb::RichRecPixel * ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueCherenkovPhoton( const LHCb::MCParticle *,
                                      const LHCb::RichSmartID,
                                      const Rich::RadiatorType ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueCherenkovRadiation( const LHCb::RichRecPixel *,
                                         const Rich::RadiatorType ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueCherenkovRadiation( const Rich::HPDPixelCluster&,
                                         const Rich::RadiatorType ) const
{
  return NULL;
}

const LHCb::MCParticle *
NULLMCTruthTool::trueCherenkovRadiation( const LHCb::RichSmartID,
                                         const Rich::RadiatorType ) const
{
  return NULL;
}

Rich::ParticleIDType
NULLMCTruthTool::mcParticleType( const LHCb::Track *,
                                 const double ) const
{
  return Rich::Unknown;
}

Rich::ParticleIDType
NULLMCTruthTool::mcParticleType( const LHCb::RichRecTrack *,
                                 const double ) const
{
  return Rich::Unknown;
}

Rich::ParticleIDType
NULLMCTruthTool::mcParticleType( const LHCb::RichRecSegment *,
                                 const double ) const
{
  return Rich::Unknown;
}

const LHCb::MCParticle *
NULLMCTruthTool::mcParticle( const LHCb::RichRecSegment *,
                             const double ) const
{
  return NULL;
}

bool
NULLMCTruthTool::mcRichOpticalPhoton( const LHCb::RichRecPixel *,
                                      SmartRefVector<LHCb::MCRichOpticalPhoton> & ) const
{
  return false;
}

const LHCb::MCRichSegment *
NULLMCTruthTool::mcRichSegment( const LHCb::RichRecSegment * ) const
{
  return NULL;
}

const SmartRefVector<LHCb::MCRichSegment> *
NULLMCTruthTool::mcRichSegments( const LHCb::RichRecTrack * ) const
{
  return NULL;
}

const LHCb::MCRichTrack *
NULLMCTruthTool::mcRichTrack( const LHCb::RichRecSegment * ) const
{
  return NULL;
}

const LHCb::MCRichTrack *
NULLMCTruthTool::mcRichTrack( const LHCb::RichRecTrack * ) const
{
  return NULL;
}

bool
NULLMCTruthTool::isBackground( const LHCb::RichRecPixel * ) const
{
  return false;
}

bool
NULLMCTruthTool::isRadScintillation( const LHCb::RichRecPixel * ) const
{
  return false;
}

const LHCb::MCRichHit *
NULLMCTruthTool::trueCherenkovHit( const LHCb::RichRecPhoton * ) const
{
  return NULL;
}

const LHCb::MCRichOpticalPhoton *
NULLMCTruthTool::trueOpticalPhoton( const LHCb::RichRecPhoton * ) const
{
  return NULL;
}

const LHCb::MCRichOpticalPhoton *
NULLMCTruthTool::trueOpticalPhoton( const LHCb::RichRecSegment *,
                                    const LHCb::RichRecPixel * ) const
{
  return NULL;
}

const LHCb::RichRecRing *
NULLMCTruthTool::mcCKRing( const LHCb::RichRecSegment * ) const
{
  return NULL;
}

IMCTruthTool::MCPartAssocInfo
NULLMCTruthTool::mcParticle( const LHCb::RichRecRing *,
                             const double ) const
{
  return IMCTruthTool::MCPartAssocInfo();
}

bool NULLMCTruthTool::trackToMCPAvailable() const
{
  return false;
}

bool NULLMCTruthTool::pixelMCHistoryAvailable() const
{
  return false;
}

bool NULLMCTruthTool::photonMCAssocAvailable() const
{
  return false;
}

bool NULLMCTruthTool::extendedMCAvailable() const
{
  return false;
}
