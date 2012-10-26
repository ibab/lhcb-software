
/** @file DumpRichTracksToTextFileAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::DumpRichTracksToTextFileAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2009-02-16
 */

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
  m_textFile = new std::ofstream(fileName.c_str(),std::ios::app);

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

    // Exit point to radiator (x,y,z)
    *m_textFile << trackSeg.exitPoint().x() << " "
                << trackSeg.exitPoint().y() << " "
                << trackSeg.exitPoint().z() << " ";

    // Exit Momentum vector (px,py,pz)
    *m_textFile << trackSeg.exitMomentum().x() << " "
                << trackSeg.exitMomentum().y() << " "
                << trackSeg.exitMomentum().z() << " ";

    // MC Info
    *m_textFile << ( mcP ? mcP->particleID().pid() : 0  ) << " ";

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
