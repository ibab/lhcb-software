
//-----------------------------------------------------------------------------
/** @file RichRecMoniAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction monitor
 *  algorithm base class : RichRecMoniAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecMoniAlgBase.cpp,v 1.2 2005-02-02 10:01:20 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005/01/13
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecMoniAlgBase.h"

// Standard constructor
RichRecMoniAlgBase::RichRecMoniAlgBase( const std::string& name,
                                        ISvcLocator* pSvcLocator )
  : RichMoniAlgBase ( name, pSvcLocator ),
    m_pixTool      ( 0 ),
    m_tkTool       ( 0 ),
    m_segTool      ( 0 ),
    m_photTool     ( 0 ),
    m_statTool     ( 0 ) { }

// Destructor
RichRecMoniAlgBase::~RichRecMoniAlgBase() {};

// Initialise
StatusCode RichRecMoniAlgBase::initialize()
{
  // Initialise base class
  const StatusCode sc = RichMoniAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Cache creator tools
  acquireTool( "RichPixelCreator",   m_pixTool  );
  acquireTool( "RichTrackCreator",   m_tkTool   );
  acquireTool( "RichSegmentCreator", m_segTool  );
  acquireTool( "RichPhotonCreator",  m_photTool );
  acquireTool( "RichStatusCreator",  m_statTool );

  return sc;
}

// Main execute method
StatusCode RichRecMoniAlgBase::execute()
{
  // All algorithms should re-implement this method
  return Error ( "Default RichRecMoniAlgBase::execute() called !!" );
}

// Finalize
StatusCode RichRecMoniAlgBase::finalize()
{
  //
  // Leave space to do something here later on if needed
  //

  // Finalize base class
  return RichMoniAlgBase::finalize();
}
