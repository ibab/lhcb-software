
//-----------------------------------------------------------------------------
/** @file RichNULLMCTruthTool.cpp
 *
 * Implementation file for class : RichNULLMCTruthTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichNULLMCTruthTool.h"

// From PartProp
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// namespace
using namespace Rich::MC;

DECLARE_TOOL_FACTORY( NULLMCTruthTool )

// Standard constructor
NULLMCTruthTool::NULLMCTruthTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Rich::ToolBase ( type, name, parent )
{
  // interface
  declareInterface<IMCTruthTool>(this);
}

NULLMCTruthTool::~NULLMCTruthTool() { }

bool NULLMCTruthTool::mcParticles( const Rich::HPDPixelCluster&,
                                   std::vector<const LHCb::MCParticle*>& ) const
{
  return false;
}

bool NULLMCTruthTool::mcParticles( const LHCb::RichSmartID,
                                   std::vector<const LHCb::MCParticle*>& ) const
{
  return false;
}

const LHCb::MCRichDigit * NULLMCTruthTool::mcRichDigit( const LHCb::RichSmartID ) const
{
  return NULL;
}

Rich::ParticleIDType
NULLMCTruthTool::mcParticleType( const LHCb::MCParticle * ) const
{
  return Rich::Unknown;
}

const LHCb::MCRichTrack *
NULLMCTruthTool::mcRichTrack( const LHCb::MCParticle * ) const
{
  return NULL;
}

const LHCb::MCRichOpticalPhoton *
NULLMCTruthTool::mcOpticalPhoton( const LHCb::MCRichHit * ) const
{
  return NULL;
}

bool NULLMCTruthTool::isBackground ( const Rich::HPDPixelCluster& ) const
{
  return false;
}

bool NULLMCTruthTool::isHPDReflection ( const Rich::HPDPixelCluster& ) const
{
  return false;
}

bool NULLMCTruthTool::isSiBackScatter ( const Rich::HPDPixelCluster& ) const
{
  return false;
}

bool NULLMCTruthTool::isRadScintillation ( const Rich::HPDPixelCluster& ) const
{
  return false;
}

bool NULLMCTruthTool::isBackground ( const LHCb::RichSmartID ) const
{
  return false;
}

bool NULLMCTruthTool::isHPDReflection ( const LHCb::RichSmartID ) const
{
  return false;
}

bool NULLMCTruthTool::isSiBackScatter ( const LHCb::RichSmartID ) const
{
  return false;
}

bool NULLMCTruthTool::isRadScintillation ( const LHCb::RichSmartID ) const
{
  return false;
}

bool NULLMCTruthTool::isCherenkovRadiation( const Rich::HPDPixelCluster&,
                                            const Rich::RadiatorType ) const
{
  return false;
}

bool
NULLMCTruthTool::isCherenkovRadiation( const LHCb::RichSmartID,
                                       const Rich::RadiatorType ) const
{
  return false;
}

bool
NULLMCTruthTool::getMcHistories( const Rich::HPDPixelCluster&,
                                 std::vector<const LHCb::MCRichDigitSummary*> & ) const
{
  return false;
}

bool
NULLMCTruthTool::getMcHistories( const LHCb::RichSmartID,
                                 std::vector<const LHCb::MCRichDigitSummary*> & ) const
{
  return false;
}

const SmartRefVector<LHCb::MCRichHit> &
NULLMCTruthTool::mcRichHits( const LHCb::MCParticle * ) const
{
  static const SmartRefVector<LHCb::MCRichHit> tmp;
  return tmp;
}

void
NULLMCTruthTool::mcRichHits( const Rich::HPDPixelCluster&,
                             SmartRefVector<LHCb::MCRichHit> & ) const
{
}

const SmartRefVector<LHCb::MCRichHit> &
NULLMCTruthTool::mcRichHits( const LHCb::RichSmartID ) const
{
  static const SmartRefVector<LHCb::MCRichHit> tmp;
  return tmp;
}

bool NULLMCTruthTool::richMCHistoryAvailable() const
{
  return false;
}

bool NULLMCTruthTool::extendedMCAvailable() const
{
  return false;
}
