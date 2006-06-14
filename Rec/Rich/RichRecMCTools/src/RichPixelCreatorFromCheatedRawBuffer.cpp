
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRawBuffer.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPixelCreatorFromCheatedRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRawBuffer.cpp,v 1.1 2006-06-14 22:08:32 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//-----------------------------------------------------------------------------

// local
#include "RichPixelCreatorFromCheatedRawBuffer.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPixelCreatorFromCheatedRawBuffer>          s_factory ;
const        IToolFactory& RichPixelCreatorFromCheatedRawBufferFactory = s_factory ;

// Standard constructor
RichPixelCreatorFromCheatedRawBuffer::
RichPixelCreatorFromCheatedRawBuffer( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichPixelCreatorBase( type, name, parent ),
    m_mcTool      ( 0 ) { }

StatusCode RichPixelCreatorFromCheatedRawBuffer::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPixelCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichMCTruthTool", m_mcTool,      0, true );

  return sc;
}

StatusCode RichPixelCreatorFromCheatedRawBuffer::finalize()
{
  // Execute base class method
  return RichPixelCreatorBase::finalize();
}

RichRecPixel *
RichPixelCreatorFromCheatedRawBuffer::buildPixel( const RichSmartID id ) const
{

  // First run base class method to produce reconstructed pixel
  RichRecPixel * pixel = RichPixelCreatorBase::buildPixel(id);
  if ( !pixel ) return NULL;

  // Now, update coords using MC information

  // Find associated MCRichOpticalPhoton
  const SmartRefVector<MCRichHit> & mcRichHits = m_mcTool->mcRichHits(id);
  if ( !mcRichHits.empty() )
  {
    // Just use first hit for the moment (should do better)
    const MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton( mcRichHits.front() );
    if ( mcPhot )
    {
      // Update coordinates with cheated info
      pixel->setGlobalPosition( mcPhot->pdIncidencePoint() );
      pixel->setLocalPosition( smartIDTool()->globalToPDPanel(pixel->globalPosition()) );
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "Pixel " << id
                  << " MC cheated global pos " << pixel->globalPosition() << endreq;
      }
    }
  }

  // re-compute corrected local coordinates
  computeRadCorrLocalPositions( pixel );

  return pixel;
}

