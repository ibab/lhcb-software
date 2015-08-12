
/** @file DumpRichHitsToTextFileAlg.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::DumpRichHitsToTextFileAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2009-02-16
 */

// local
#include "DumpRichHitsToTextFileAlg.h"

//-----------------------------------------------------------------------------

// namespace
using namespace Rich::Rec::MC;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DumpRichHitsToTextFileAlg::DumpRichHitsToTextFileAlg( const std::string& name,
                                                      ISvcLocator* pSvcLocator)
  : Rich::Rec::AlgBase ( name , pSvcLocator ),
    m_truth            ( NULL ),
    m_nEvt             ( 0    ),
    m_detectors        ( Rich::NRiches, true )
{
  declareProperty( "UseRedCorrPositions",  m_useCorrPos = true );
  declareProperty( "OnlyOneMCAssoc",   m_onlyOneMCassoc = true );
  declareProperty( "Detectors",        m_detectors );
}

//=============================================================================
// Destructor
//=============================================================================
DumpRichHitsToTextFileAlg::~DumpRichHitsToTextFileAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DumpRichHitsToTextFileAlg::initialize()
{
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichMCTruthTool", m_truth, NULL, true );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DumpRichHitsToTextFileAlg::execute()
{
  ++m_nEvt;
  StatusCode sc = StatusCode::SUCCESS;
  if ( m_detectors[Rich::Rich1] )
  {
    sc = sc && dumpToTextfile(Rich::Rich1,Rich::top,Rich::Rich1Gas);
    sc = sc && dumpToTextfile(Rich::Rich1,Rich::bottom,Rich::Rich1Gas);
  }
  if ( m_detectors[Rich::Rich2] )
  {
    sc = sc && dumpToTextfile(Rich::Rich2,Rich::left,Rich::Rich2Gas);
    sc = sc && dumpToTextfile(Rich::Rich2,Rich::right,Rich::Rich2Gas);
  }
  return sc;
}

StatusCode
DumpRichHitsToTextFileAlg::dumpToTextfile( const Rich::DetectorType rich,
                                           const Rich::Side         panel,
                                           const Rich::RadiatorType rad ) const
{
  // file name
  std::ostringstream filename;
  filename << Rich::text(rich) << "-" << Rich::text(rich,panel) << "-" << Rich::text(rad)
           << "_" << m_nEvt << ".txt";

  // open file
  info() << "Creating data text file : " << filename.str() << endmsg;
  std::ofstream file( filename.str().c_str(), std::ios_base::out | std::ios_base::trunc );

  // Iterate over pixels
  const IPixelCreator::PixelRange range = pixelCreator()->range( rich, panel );
  debug() << " -> Selected " << range.size() << " hits for " << rich
          << " " << Rich::text(rich,panel) << endmsg;
  for ( LHCb::RichRecPixels::const_iterator iPix = range.begin(); iPix != range.end(); ++iPix )
  {
    // get X and Y
    const double X ( m_useCorrPos ?
                     (*iPix)->radCorrLocalPositions().position(rad).x() :
                     (*iPix)->localPosition().x()
                     );
    const double Y ( m_useCorrPos ?
                     (*iPix)->radCorrLocalPositions().position(rad).y() :
                     (*iPix)->localPosition().y()
                     );
    file << X << " " << Y << " ";
    verbose() << "  -> Hit " << X << " " << Y << endmsg;

    // MC history
    typedef std::vector< const LHCb::MCRichDigitSummary * > Histories;
    Histories histories;
    m_truth->getMcHistories( (*iPix)->hpdPixelCluster(), histories );
    unsigned int i(0);
    for ( Histories::const_iterator iH = histories.begin();
          iH != histories.end(); ++iH )
    {
      if ( NULL != (*iH)->mcParticle() )
      {
        if ( i>0 ) file << ",";
        file << (*iH)->mcParticle()->key();
        ++i;
        if ( m_onlyOneMCassoc ) break;
      }
    }

    // end this line
    file << std::endl;
  }

  // close the file
  file.close();

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DumpRichHitsToTextFileAlg )

//=============================================================================
