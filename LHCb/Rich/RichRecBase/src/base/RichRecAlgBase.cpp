// $Id: RichRecAlgBase.cpp,v 1.10 2003-11-02 21:46:02 jonrob Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecAlgBase
//
// 05/04/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ),
    m_richTracks   ( 0 ),
    m_richPixels   ( 0 ),
    m_richSegments ( 0 ),
    m_richPhotons  ( 0 ),
    m_richStatus   ( 0 )
{

}

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize() {

  // Execute the base class initialize
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  // initialise data pointers
  acquireTool("RichSegmentCreator", m_segTool);
  m_richSegments = &(m_segTool->richSegments());
  acquireTool("RichPhotonCreator", m_photTool);
  m_richPhotons = &(m_photTool->richPhotons());
  acquireTool("RichPixelCreator", m_pixTool);
  m_richPixels = &(m_pixTool->richPixels());
  acquireTool("RichTrackCreator", m_tkTool);
  m_richTracks = &(m_tkTool->richTracks());
  acquireTool("RichStatusCreator", m_statTool);
  m_richStatus = &(m_statTool->richStatus());

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichRecAlgBase::finalize() {

  // Release all tools
  releaseTool( m_pixTool  );
  releaseTool( m_tkTool   );
  releaseTool( m_statTool );
  releaseTool( m_segTool  );
  releaseTool( m_photTool );

  return RichAlgBase::finalize();
}
