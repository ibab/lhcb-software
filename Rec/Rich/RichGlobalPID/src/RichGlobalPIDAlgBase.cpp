// $Id: RichGlobalPIDAlgBase.cpp,v 1.4 2004-03-16 13:43:34 jonesc Exp $
// Include files

// local
#include "RichGlobalPIDAlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDAlgBase
//
// 2003-05-10 : Chris Jones
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



