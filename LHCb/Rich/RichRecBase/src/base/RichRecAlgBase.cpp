// $Id: RichRecAlgBase.cpp,v 1.15 2004-06-18 09:44:11 jonesc Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecAlgBase
//
// 05/04/2002 : Chris Jones    Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor
RichRecAlgBase::RichRecAlgBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
  : RichAlgBase ( name, pSvcLocator ),
    m_pixTool      ( 0 ),
    m_tkTool       ( 0 ),
    m_segTool      ( 0 ),
    m_photTool     ( 0 ),
    m_statTool     ( 0 ) { }

// Destructor
RichRecAlgBase::~RichRecAlgBase() {};

// Initialise
StatusCode RichRecAlgBase::initialize()
{
  // Initialise base class
  StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Cache creator tools
  acquireTool( "RichPixelCreator",   m_pixTool  );
  acquireTool( "RichTrackCreator",   m_tkTool   );
  acquireTool( "RichSegmentCreator", m_segTool  );
  acquireTool( "RichPhotonCreator",  m_photTool );
  acquireTool( "RichStatusCreator",  m_statTool );

  return StatusCode::SUCCESS;
};

// Finalize
StatusCode RichRecAlgBase::finalize()
{
  //
  // Leave space to do something here later on if needed
  //

  // Finalize base class
  return RichAlgBase::finalize();
}
