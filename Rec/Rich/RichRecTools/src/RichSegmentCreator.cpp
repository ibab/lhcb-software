// $Id: RichSegmentCreator.cpp,v 1.1 2003-06-30 15:47:05 jonrob Exp $

// local
#include "RichSegmentCreator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichSegmentCreator
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSegmentCreator>          s_factory ;
const        IToolFactory& RichSegmentCreatorFactory = s_factory ;

// Standard constructor
RichSegmentCreator::RichSegmentCreator ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichSegmentCreator>(this);

  // Define job option parameters
  declareProperty( "RichRecSegmentLocation",
                   m_richRecSegmentLocation = RichRecSegmentLocation::Default ); 

}

StatusCode RichSegmentCreator::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Get pointer to Event Data service
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    return StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" );   // Informed of a new event
    incSvc->release();
  }

  // Informational Printout
  //msg << MSG::DEBUG
 
  return StatusCode::SUCCESS;
}

StatusCode RichSegmentCreator::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release services and tools
  if ( m_evtDataSvc ) { m_evtDataSvc->release(); m_evtDataSvc = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichSegmentCreator::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    SmartDataPtr<RichRecSegments> tdsSegments( m_evtDataSvc,
                                               m_richRecSegmentLocation );
    if ( !tdsSegments ) {

      // Reinitialise the Photon Container
      m_segments = new RichRecSegments();

      // Register new RichRecPhoton container to Gaudi data store
      if (!m_evtDataSvc->registerObject(m_richRecSegmentLocation, m_segments)) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecSegments at "
            << m_richRecSegmentLocation << endreq;
      }

    } else {

      // Set smartref to TES segment container
      m_segments = tdsSegments;

    }

  } // begin event if

}

// Forms a new RichRecSegment object
void RichSegmentCreator::saveSegment ( RichRecSegment * segment ) {
  m_segments->insert( segment );
}

RichRecSegments *& RichSegmentCreator::richSegments() {
  return m_segments;
}
