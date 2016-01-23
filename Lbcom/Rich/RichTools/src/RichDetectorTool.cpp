
//-----------------------------------------------------------------------------
/** @file RichDetectorTool.cpp
 *
 * Implementation file for class : Rich::DetectorTool
 *
 * @author Antonis Papanestis
 * @date 2012-10-26
 */
//-----------------------------------------------------------------------------

// local
#include "RichDetectorTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, DetectorTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  Rich::DetectorTool::DetectorTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : Rich::ToolBase   ( type, name, parent )
{
  // Interface
  declareInterface<IDetectorTool>(this);
}

//=============================================================================
// Destructor
Rich::DetectorTool::~DetectorTool() {}
//=============================================================================

//=============================================================================
StatusCode Rich::DetectorTool::initialize()
{
  // Initialise base class
  const StatusCode sc = Rich::ToolBase::initialize();
  return sc;
}

//=========================================================================
//  deRichDetectors
//=========================================================================
const std::vector<DeRich*>& Rich::DetectorTool::deRichDetectors ( ) const
{

  if ( m_deRichDets.empty() )
  {
    // find the DeRich objects
    std::vector<std::string> locations;

    DetectorElement* afterMagnet = getDet<DetectorElement>("/dd/Structure/LHCb/AfterMagnetRegion");
    if ( afterMagnet->exists("RichDetectorLocations") )
    {
      locations = afterMagnet->paramVect<std::string>("RichDetectorLocations");
    }
    else
    {
      locations.push_back(DeRichLocations::Rich1);
      locations.push_back(DeRichLocations::Rich2);
    }

    for ( const auto & loc : locations )
    {
      m_deRichDets.push_back( getDet<DeRich>(loc) );
    }
  }

  return m_deRichDets;

}
