
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromAllMCRichHits.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromAllMCRichHits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromAllMCRichHits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromAllMCRichHits )

// Standard constructor
PixelCreatorFromAllMCRichHits::
PixelCreatorFromAllMCRichHits( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : Rich::Rec::PixelCreatorBase ( type, name, parent ),
    m_mcTool                    ( NULL               )
{
  // book keeping cannot work with this tool
  setProperty( "DoBookKeeping", false );
}

StatusCode PixelCreatorFromAllMCRichHits::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::PixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // book keeping cannot work with this tool
  if ( bookKeep() )
    return Error( "Pixel Book-keeping cannot work with this pixel creator" );

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_mcTool, NULL, true );

  return sc;
}

LHCb::RichRecPixel *
PixelCreatorFromAllMCRichHits::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  LHCb::RichRecPixel * pix(NULL);
  // build hits for each pixel in the cluster
  for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
        iS != cluster.smartIDs().end(); ++iS )
  {
    pix = buildPixel( *iS );
  }
  // return (last one ...)
  return pix;
}

LHCb::RichRecPixel *
PixelCreatorFromAllMCRichHits::buildPixel( const LHCb::RichSmartID& id ) const
{
  // Note : This method actually builds more than one pixel ...

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Making pixels for " << id << endmsg;
  }

  // First run base class method to produce reconstructed pixel
  LHCb::RichRecPixel * pixel = Rich::Rec::PixelCreatorBase::buildPixel(id);

  // set the associated cluster
  pixel->setAssociatedCluster( Rich::HPDPixelCluster(id) );

  // save original global hit position before MC cheating starts
  const Gaudi::XYZPoint gPosOriginal = pixel->globalPosition();

  // Get MC hits
  const SmartRefVector<LHCb::MCRichHit>& mcRichHits = m_mcTool->mcRichHits( id );

  // Loop over hits
  bool first(true);
  for ( SmartRefVector<LHCb::MCRichHit>::const_iterator iHit = mcRichHits.begin();
        iHit != mcRichHits.end(); ++iHit )
  {
    // if first, just reuse the original, otherwise clone
    LHCb::RichRecPixel * pix = ( first ? pixel : new LHCb::RichRecPixel(*pixel) );
    // save this pixel for clones
    if ( !first ) savePixel( pix );
    // Is this a true CK photon ?
    const LHCb::MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton( *iHit );
    // Set global coordinates
    pix->setGlobalPosition( mcPhot ? mcPhot->hpdQWIncidencePoint() : gPosOriginal );
    // local position
    pix->setLocalPosition( smartIDTool()->globalToPDPanel(pix->globalPosition()) );
    // set the corrected local positions
    geomTool()->setCorrLocalPos(pix,id.rich());
    // some printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " -> MC cheated global pos " << pix->globalPosition() << endmsg;
    }
    // no longer first ...
    first = false;
  }

  // return
  return pixel;
}
