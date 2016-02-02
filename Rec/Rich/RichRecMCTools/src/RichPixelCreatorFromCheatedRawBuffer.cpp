
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromCheatedRawBuffer
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromCheatedRawBuffer.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromCheatedRawBuffer )

// Standard constructor
PixelCreatorFromCheatedRawBuffer::
PixelCreatorFromCheatedRawBuffer( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Rich::Rec::PixelCreatorBase ( type, name, parent ) { }

StatusCode PixelCreatorFromCheatedRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::PixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_mcTool, NULL, true );

  return sc;
}

LHCb::RichRecPixel *
PixelCreatorFromCheatedRawBuffer::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  // First run base class method to produce reconstructed pixel
  auto * pixel = Rich::Rec::PixelCreatorBase::buildPixel(cluster);

  // Now, update coords using MC information
  addMCInfo( pixel );

  // return
  return pixel;
}

LHCb::RichRecPixel *
PixelCreatorFromCheatedRawBuffer::buildPixel( const LHCb::RichSmartID& id ) const
{
  // First run base class method to produce reconstructed pixel
  auto * pixel = Rich::Rec::PixelCreatorBase::buildPixel(id);

  // Now, update coords using MC information
  addMCInfo( pixel );

  // return
  return pixel;
}

void PixelCreatorFromCheatedRawBuffer::addMCInfo( LHCb::RichRecPixel * pixel ) const
{
  if ( pixel )
  {
    // Find associated MCRichOpticalPhoton
    SmartRefVector<LHCb::MCRichHit> mcRichHits;
    m_mcTool->mcRichHits( pixel->hpdPixelCluster(), mcRichHits );

    // Loop over photons
    for ( const auto & hit : mcRichHits )
    {
      // Is this a true CK photon ?
      const auto * mcPhot = m_mcTool->mcOpticalPhoton( hit );
      if ( mcPhot )
      {
        // Update coordinates with cheated info
        pixel->setGlobalPosition( mcPhot->hpdQWIncidencePoint() );
        pixel->setLocalPosition( smartIDTool()->globalToPDPanel(pixel->globalPosition()) );
        // set the corrected local positions
        geomTool()->setCorrLocalPos(pixel,pixel->hpdPixelCluster().rich());
        _ri_verbo << "Pixel " << pixel->hpdPixelCluster()
                  << " MC cheated global pos " << pixel->globalPosition() << endmsg;
        // break out of loop (i.e. ignore any other associated hits)
        break;
      } // true photon

    } // loop over mc hits

  } // pixel OK
}
