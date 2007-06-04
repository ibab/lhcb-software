
//-----------------------------------------------------------------------------
/** @file RichToolRegistry.cpp
 *
 * Implementation file for class : RichToolRegistry
 *
 * CVS Log :-
 * $Id: RichToolRegistry.cpp,v 1.15 2007-06-04 06:57:55 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
// Suppress "debug information truncated" warnings on Windows
#include "GaudiKernel/Kernel.h"

// local
#include "RichToolRegistry.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, ToolRegistry );

// Standard constructor
Rich::ToolRegistry::ToolRegistry( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool( type, name, parent )
{
  // declare interface
  declareInterface<IToolRegistry>(this);

  // job option for mapping between nickname and class name
  declareProperty( "Tools", m_names );
}

StatusCode Rich::ToolRegistry::initialize()
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // define some default tool mappings
  ToolList defaultTools;
  // MC
  defaultTools.push_back ( "Rich::MC::MCTruthTool/RichMCTruthTool"         );
  defaultTools.push_back ( "Rich::Rec::MC::MCTruthTool/RichRecMCTruthTool" );
  // Common RICH tools
  defaultTools.push_back ( "Rich::MirrorSegFinder/RichMirrorSegFinder"     );
  defaultTools.push_back ( "Rich::DAQ::RawBufferToSmartIDsTool/RichSmartIDDecoder" );
  defaultTools.push_back ( "Rich::RadiatorTool/RichRadiatorTool" );
  defaultTools.push_back ( "Rich::DAQ::RawDataFormatTool/RichRawDataFormatTool" );
  defaultTools.push_back ( "Rich::ParticleProperties/RichParticleProperties" );
  defaultTools.push_back ( "Rich::RayTracing/RichRayTracing" );
  defaultTools.push_back ( "Rich::SmartIDTool/RichSmartIDTool" );
  defaultTools.push_back ( "Rich::DetParameters/RichDetParameters" );
  defaultTools.push_back ( "Rich::TabulatedRefractiveIndex/RichRefractiveIndex" );
  // Reco Tools
  defaultTools.push_back ( "Rich::Rec::MassHypothesisRingCreator/RichMassHypoRings" );
  defaultTools.push_back ( "Rich::Rec::RayTraceCherenkovCone/RichRayTraceCKCone" );
  defaultTools.push_back ( "Rich::Rec::SegmentCreator/RichSegmentCreator" );
  defaultTools.push_back ( "Rich::Rec::StatusCreator/RichStatusCreator" );
  defaultTools.push_back ( "Rich::Rec::ExpectedTrackSignal/RichExpectedTrackSignal" );
  defaultTools.push_back ( "Rich::Rec::GeomEffPhotonTracing/RichGeomEff" );
  defaultTools.push_back ( "Rich::Rec::PhotonSignal/RichPhotonSignal" );
  defaultTools.push_back ( "Rich::Rec::CherenkovAngle/RichCherenkovAngle" );
  defaultTools.push_back ( "Rich::Rec::GeomTool/RichRecGeometry" );
  defaultTools.push_back ( "Rich::Rec::TabulatedRayleighScatter/RichRayleighScatter" );
  defaultTools.push_back ( "Rich::Rec::SellmeirFunc/RichSellmeirFunc" );
  defaultTools.push_back ( "Rich::Rec::TabulatedSignalDetectionEff/RichSignalDetectionEff" );
  defaultTools.push_back ( "Rich::Rec::TabulatedGasQuartzWindowAbs/RichGasQuartzWindow" );
  // other tools
  defaultTools.push_back ( "OdinTimeDecoder/OdinTimeDecoder" );

  // configure the default tools first
  setUpTools( defaultTools );
  // then the ones from the job options
  // any settings here for default nicknames will override 
  setUpTools( m_names );

  return sc;
}

void Rich::ToolRegistry::setUpTools( const ToolList & toolList )
{
  // setup tool registry
  for ( ToolList::const_iterator it = toolList.begin();
        it != toolList.end(); ++it )
  {
    const int slash = (*it).find_first_of( "/" );
    addEntry( ( slash>0 ? (*it).substr(slash+1) : *it ),
              ( slash>0 ? (*it).substr(0,slash) : *it ) );
  }
}

const std::string &
Rich::ToolRegistry::toolType( const std::string & nickname ) const
{
  // Test nickname is valid
  if ( nickname.empty() )
  {
    // Empty nick name !
    Exception( "Received empty tool nickname" );
  }

  // test instance name is defined
  if ( m_myTools[nickname].empty() )
  {
    // Don't allow any missing tool entries
    // Exception( "Unknown RICH tool nickname '" + nickname + "'" );
    // or... just assume same as nickname and issue a Warning
    Warning( "Unknown nickname '" + nickname + "' -> Assuming same class name",
             StatusCode::SUCCESS );
    addEntry( nickname, nickname );
  }

  // All OK, so return instance name for this nickname
  return m_myTools[nickname];
}

const std::string
Rich::ToolRegistry::toolName( const std::string & nickname ) const
{
  return ( context().empty() ? nickname : context()+"."+nickname );
}

void Rich::ToolRegistry::addEntry( const std::string & nickname,
                                   const std::string & type ) const
{
  if ( !m_myTools[nickname].empty() && type != m_myTools[nickname] )
  {
    Warning( "Nickname '" + nickname + "' mapping changed : '"
             + m_myTools[nickname] + "' to '" + type + "'", 0, StatusCode::SUCCESS );
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " Tool nickname '" << nickname
            << "' maps to type '" << type << "'" << endreq;
  }
  m_myTools[nickname] = type;
}

const std::string Rich::ToolRegistry::getContext() const
{
  return context();
}
