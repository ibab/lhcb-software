// $Id: RichStatusCreator.cpp,v 1.3 2004-02-02 14:27:05 jonesc Exp $

// local
#include "RichStatusCreator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichStatusCreator
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichStatusCreator>          s_factory ;
const        IToolFactory& RichStatusCreatorFactory = s_factory ;

// Standard constructor
RichStatusCreator::RichStatusCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichStatusCreator>(this);

  declareProperty( "RichRecStatusLocation",
                   m_richStatusLocation = RichRecStatusLocation::Default );

}

StatusCode RichStatusCreator::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    return StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->release();
  }

  return StatusCode::SUCCESS;
}

StatusCode RichStatusCreator::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichStatusCreator::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    SmartDataPtr<RichRecStatus> status( eventSvc(), m_richStatusLocation );
    if ( !status ) {

      m_status = new RichRecStatus();
      if ( !eventSvc()->registerObject(m_richStatusLocation, m_status) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecStatus at "
            << m_richStatusLocation << endreq;
      }

    } else {
      m_status = status;
    }

  } // end begin event if

}

RichRecStatus * RichStatusCreator::richStatus() const
{
  return m_status;
}
