// $Id: RichRecAlgBase.cpp,v 1.11 2004-02-02 14:23:05 jonesc Exp $

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
    m_pixTool      ( 0 ),
    m_tkTool       ( 0 ),
    m_statTool     ( 0 ),
    m_segTool      ( 0 ),
    m_photTool     ( 0 ) { }

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize() {

  // Execute the base class initialize
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  // initialise data pointers and tools
  acquireTool("RichSegmentCreator", m_segTool  );
  acquireTool("RichPhotonCreator",  m_photTool );
  acquireTool("RichPixelCreator",   m_pixTool  );
  acquireTool("RichTrackCreator",   m_tkTool   );
  acquireTool("RichStatusCreator",  m_statTool );

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
