
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : RichGlobalPIDAlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.cpp,v 1.8 2006-06-14 18:53:46 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichGlobalPIDAlgBase.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichGlobalPIDAlgBase::RichGlobalPIDAlgBase( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name , pSvcLocator ),
    m_GPIDtracks   ( NULL ),
    m_GPIDSummary  ( NULL ),
    m_GPIDs        ( NULL )
{

  declareProperty( "RichGlobalPIDTrackLocation",
                   m_richGPIDTrackLocation =
                   RichGlobalPIDTrackLocation::Default );

  declareProperty( "RichGlobalPIDLocation",
                   m_richGPIDLocation = RichGlobalPIDLocation::Default );

  declareProperty( "RichGlobalPIDSummaryLocation",
                   m_richGPIDSummaryLocation =
                   RichGlobalPIDSummaryLocation::Default );

  declareProperty( "GlobalName", m_richGPIDName = "RichGloPID" );

}

// Destructor
RichGlobalPIDAlgBase::~RichGlobalPIDAlgBase() { }



