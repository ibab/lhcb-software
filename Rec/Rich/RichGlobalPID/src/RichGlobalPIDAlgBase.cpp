
/** @file RichGlobalPIDAlgBase.cpp
 *
 *  Implementation file for RICH Global PID algorithm base class : RichGlobalPIDAlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.cpp,v 1.5 2004-07-27 10:56:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-05-10
 */

// local
#include "RichGlobalPIDAlgBase.h"

//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichGlobalPIDAlgBase::RichGlobalPIDAlgBase( const std::string& name,
                                            ISvcLocator* pSvcLocator )
  : RichRecAlgBase ( name , pSvcLocator ),
    m_GPIDtracks  ( 0 ),
    m_GPIDSummary ( 0 ),
    m_GPIDs       ( 0 )
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



