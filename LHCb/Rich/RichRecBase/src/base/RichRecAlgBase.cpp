
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.cpp,v 1.17 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRecBase/RichRecAlgBase.h"

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
  // ================================================================
  // To be removed at a later date since the tools are now acquire on
  // in the accessor methods (see RichRecAlgBase.h). However, these
  // calls are kept for a while to allow the new inline functions to
  // get used everywhere first
  // ================================================================
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
