
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.cpp
 *
 *  Implementation file for RICH reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.cpp,v 1.19 2004-11-09 10:53:49 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.18  2004/10/30 19:24:19  jonrob
 *  minor update
 *
 *  Revision 1.17  2004/07/26 18:00:58  jonrob
 *  Various improvements to the doxygen comments
 *
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
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Cache creator tools
  acquireTool( "RichPixelCreator",   m_pixTool  );
  acquireTool( "RichTrackCreator",   m_tkTool   );
  acquireTool( "RichSegmentCreator", m_segTool  );
  acquireTool( "RichPhotonCreator",  m_photTool );
  acquireTool( "RichStatusCreator",  m_statTool );

  return sc;
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
