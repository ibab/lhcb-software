
/** @file RichStatusCreator.cpp
 *
 *  Implementation file for tool : RichStatusCreator
 *
 *  CVS Log :-
 *  $Id: RichStatusCreator.cpp,v 1.7 2004-07-27 20:15:32 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichStatusCreator.h"

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

StatusCode RichStatusCreator::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichStatusCreator::finalize() 
{
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
