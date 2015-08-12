
/** @file DumpRichTracksToTextFileAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::DumpRichTracksToTextFileAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2009-02-16
 */

// local
#include "DumpRichTracksToTextFileAlg.h"

//-----------------------------------------------------------------------------

// namespace
using namespace Rich::Rec::MC;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpRichTracksToTextFileAlg::DumpRichTracksToTextFileAlg( const std::string& name,
                                                          ISvcLocator* pSvcLocator)
  : Rich::Rec::AlgBase ( name , pSvcLocator ),
    m_truth            ( NULL ),
    m_nEvt             ( 0    ),
    m_detectors        ( Rich::NRiches, true ),
    m_textFile         ( NULL )
{
  declareProperty( "Detectors", m_detectors );
}

//=============================================================================
// Destructor
//=============================================================================
DumpRichTracksToTextFileAlg::~DumpRichTracksToTextFileAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpRichTracksToTextFileAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichRecMCTruthTool", m_truth );

  // text file
  const std::string fileName = "RichTracks.txt";
  info() << "Creating data text file : " << fileName << endmsg;
  m_textFile = new std::ofstream( fileName.c_str(), std::ios_base::out | std::ios_base::trunc );

  return sc;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode DumpRichTracksToTextFileAlg::finalize()
{
  // close the text file and clean up
  m_textFile->close();
  delete m_textFile;
  // return
  return Rich::Rec::AlgBase::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpRichTracksToTextFileAlg::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Increment event number
  ++m_nEvt;

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg )
  {
    LHCb::RichRecSegment * segment = *iSeg;

    // Track type
    const Rich::Rec::Track::Type trType = segment->richRecTrack()->trackID().trackType();

    // track segment
    const LHCb::RichTrackSegment & trackSeg = segment->trackSegment();

    const Rich::DetectorType rich = trackSeg.rich();     // which rich detector
    const Rich::RadiatorType rad  = trackSeg.radiator(); // which radiator

    // Associated MCParticle
    const LHCb::MCParticle * mcP = m_truth->mcParticle(segment);

    // Get the top level MCParticle parent
    const LHCb::MCParticle * mcPP = ( mcP ? mcP->mother() : NULL );
    if ( mcPP )
    {
      bool goOn = true;
      unsigned int n(0); // safety against infinite loops
      while ( goOn && ++n < 999999 ) 
      {
        const LHCb::MCParticle * tmpMCP = mcPP->mother();
        if ( tmpMCP ) { mcPP = tmpMCP; }
        else          { goOn = false;  }
      }
    }

    // Track production vertex
    const LHCb::MCVertex * mcOriV = ( mcP ? mcP->originVertex() : NULL );
    
    // Are you including this RICH
    if ( !m_detectors[rich] ) continue;
    
    // Write to the text file

    *m_textFile << m_nEvt << " ";         // event number

    *m_textFile << segment->key() << " "; // segment key
    *m_textFile << (int)trType << " ";    // track type

    *m_textFile << (int)rich << " ";      // RICH detector
    *m_textFile << (int)rad << " ";       // Radiator

    // Entry point to radiator (x,y,z)
    *m_textFile << trackSeg.entryPoint().x() << " "
                << trackSeg.entryPoint().y() << " "
                << trackSeg.entryPoint().z() << " ";

    // Entry Momentum vector (px,py,pz)
    *m_textFile << trackSeg.entryMomentum().x() << " "
                << trackSeg.entryMomentum().y() << " "
                << trackSeg.entryMomentum().z() << " ";

    // Middle point to radiator (x,y,z)
    *m_textFile << trackSeg.middlePoint().x() << " "
                << trackSeg.middlePoint().y() << " "
                << trackSeg.middlePoint().z() << " ";

    // Middle Momentum vector (px,py,pz)
    *m_textFile << trackSeg.middleMomentum().x() << " "
                << trackSeg.middleMomentum().y() << " "
                << trackSeg.middleMomentum().z() << " ";

    // Exit point to radiator (x,y,z)
    *m_textFile << trackSeg.exitPoint().x() << " "
                << trackSeg.exitPoint().y() << " "
                << trackSeg.exitPoint().z() << " ";

    // Exit Momentum vector (px,py,pz)
    *m_textFile << trackSeg.exitMomentum().x() << " "
                << trackSeg.exitMomentum().y() << " "
                << trackSeg.exitMomentum().z() << " ";

    // MC Info

    // Track PDG code
    *m_textFile << ( mcP ? mcP->particleID().pid() : 0  ) << " ";

    // Top level parent PDG code
    *m_textFile << ( mcPP ? mcPP->particleID().pid() : 0  ) << " ";

    // Track production vertex
    *m_textFile << ( mcOriV ? mcOriV->position().x() : 0 ) << " "
                << ( mcOriV ? mcOriV->position().y() : 0 ) << " "
                << ( mcOriV ? mcOriV->position().z() : 0 ) << " ";

    // Finally end this line in the file
    *m_textFile << std::endl;

  }

  // return
  return sc;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DumpRichTracksToTextFileAlg )

//=============================================================================
