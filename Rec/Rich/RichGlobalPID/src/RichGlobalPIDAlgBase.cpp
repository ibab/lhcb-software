// $Id: RichGlobalPIDAlgBase.cpp,v 1.1.1.1 2003-06-30 16:10:53 jonesc Exp $
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
  : RichRecAlgBase ( name , pSvcLocator ) {
  
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
RichGlobalPIDAlgBase::~RichGlobalPIDAlgBase() {}

StatusCode RichGlobalPIDAlgBase::gpidTracks() 
{  
  SmartDataPtr<RichGlobalPIDTracks> GPIDtracks ( eventSvc(), m_richGPIDTrackLocation );
  if ( !GPIDtracks ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::WARNING << "Failed to locate RichGlobalPIDTracks" << endreq;
    return StatusCode::FAILURE;
  }
  m_GPIDtracks = GPIDtracks;

  return StatusCode::SUCCESS;
}

StatusCode RichGlobalPIDAlgBase::gpidPIDs() 
{  
  SmartDataPtr<RichGlobalPIDs> GPIDs ( eventSvc(), m_richGPIDLocation );
  if ( !GPIDs ) {
    MsgStream msg ( msgSvc(), name() );
    msg << MSG::WARNING << "Failed to locate RichGlobalPIDs at " 
        << m_richGPIDLocation << endreq;
    return StatusCode::FAILURE;
  }
  m_GPIDs = GPIDs;

  return StatusCode::SUCCESS;
}

StatusCode RichGlobalPIDAlgBase::gpidSummary() 
{  
  SmartDataPtr<RichGlobalPIDSummary> summary ( eventSvc(), m_richGPIDSummaryLocation );
  if ( !summary ) {
    MsgStream msg ( msgSvc(), name() );
    msg << MSG::WARNING << "Failed to locate RichGlobalSummary at " 
        << m_richGPIDSummaryLocation << endreq;
    return StatusCode::FAILURE;
  }
  m_GPIDSummary = summary;

  return StatusCode::SUCCESS;
}

