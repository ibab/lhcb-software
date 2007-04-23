
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::PixelCreatorFromCheatedRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRawBuffer.cpp,v 1.5 2007-04-23 13:23:54 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichPixelCreatorFromCheatedRawBuffer.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PixelCreatorFromCheatedRawBuffer );

// Standard constructor
PixelCreatorFromCheatedRawBuffer::
PixelCreatorFromCheatedRawBuffer( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Rich::Rec::PixelCreatorBase( type, name, parent ),
    m_mcTool        ( NULL ) { }

StatusCode PixelCreatorFromCheatedRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::PixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_mcTool,      0, true );

  return sc;
}

LHCb::RichRecPixel *
PixelCreatorFromCheatedRawBuffer::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  // Should here produce one pixel per true hit in the cluster ??

  // First run base class method to produce reconstructed pixel
  LHCb::RichRecPixel * pixel = Rich::Rec::PixelCreatorBase::buildPixel(cluster);
  if ( !pixel ) return NULL;

  // Now, update coords using MC information

  // Find associated MCRichOpticalPhoton
  SmartRefVector<LHCb::MCRichHit> mcRichHits;
  m_mcTool->mcRichHits(cluster,mcRichHits);
  if ( !mcRichHits.empty() )
  {
    // Just use first hit for the moment (should do better)
    const LHCb::MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton( mcRichHits.front() );
    if ( mcPhot )
    {
      // Update coordinates with cheated info
      pixel->setGlobalPosition( mcPhot->pdIncidencePoint() );
      pixel->setLocalPosition( smartIDTool()->globalToPDPanel(pixel->globalPosition()) );
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Pixel " << cluster
                  << " MC cheated global pos " << pixel->globalPosition() << endreq;
      }
    }
  }

  return pixel;
}

