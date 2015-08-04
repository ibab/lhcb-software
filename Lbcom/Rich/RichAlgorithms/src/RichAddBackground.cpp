
//---------------------------------------------------------------------------------
/** @file RichAddBackground.cpp
 *
 * Implementation file for class : Rich::AddBackground
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2008-10-10
 */
//---------------------------------------------------------------------------------

// local
#include "RichAddBackground.h"

//-----------------------------------------------------------------------------

using namespace Rich;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AddBackground )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AddBackground::AddBackground( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : Rich::AlgBase    ( name , pSvcLocator ),
    m_richSys        ( NULL ),
    m_background     ( NULL ),
    m_SmartIDDecoder ( NULL ) 
{ }

//=============================================================================
// Destructor
//=============================================================================
AddBackground::~AddBackground() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AddBackground::initialize()
{
  StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // tools
  acquireTool( "RichAddBackground", m_background, this );
  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, NULL, true );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AddBackground::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  // ALICE or LHCb mode (to do)
  const bool aliceMode = false;

  // Create the additional background hits
  Rich::IAddBackground::HPDBackgrounds backgrounds;
  sc = m_background->createBackgrounds(backgrounds,aliceMode);

  // loop over the new backgrounds and merge them with the main decoded data
  for ( Rich::IAddBackground::HPDBackgrounds::const_iterator iB = backgrounds.begin();
        iB != backgrounds.end(); ++iB )
  {
    debug() << "Found " << iB->second.size() << " background hits for " << iB->first << endmsg;
    counter("Added background hits / HPD") += iB->second.size();

    LHCb::RichSmartID::Vector & smartIDs 
      = *(const_cast<LHCb::RichSmartID::Vector*>(&(m_SmartIDDecoder->richSmartIDs(iB->first,true))));

    debug() << "  -> Found " << smartIDs.size() << " pre-existing hits" << endmsg;

    // add the hits
    for ( LHCb::RichSmartID::Vector::const_iterator iBHit = iB->second.begin();
          iBHit != iB->second.end(); ++iBHit )
    {
      // Is this hit already in the list ? (binary so should only be there once)
      if ( smartIDs.end() == std::find(smartIDs.begin(),smartIDs.end(),*iBHit) )
      {
        smartIDs.push_back(*iBHit);
      }
    }

    debug() << "  ->       " << smartIDs.size() << " hits after background addition" << endmsg;
  }

  return sc;
}

//=============================================================================
