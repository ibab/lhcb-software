// $Id: RichStatusCreator.cpp,v 1.5 2004-04-19 23:06:15 jonesc Exp $

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
  : RichRecToolBase( type, name, parent ),
    m_status ( 0 )
{

  declareInterface<IRichStatusCreator>(this);

  declareProperty( "RichRecStatusLocation",
                   m_richStatusLocation = RichRecStatusLocation::Default );

}

StatusCode RichStatusCreator::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichStatusCreator::finalize() {

  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichStatusCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

RichRecStatus * RichStatusCreator::richStatus() const
{
  if ( !m_status ) {
    SmartDataPtr<RichRecStatus> status( evtSvc(), m_richStatusLocation );
    if ( !status ) {
      m_status = new RichRecStatus();
      put( m_status, m_richStatusLocation );
    } else {
      m_status = status;
    }
  }
  return m_status;
}
