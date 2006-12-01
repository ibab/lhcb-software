
//-----------------------------------------------------------------------------
/** @file RichStatusCreator.cpp
 *
 *  Implementation file for tool : RichStatusCreator
 *
 *  CVS Log :-
 *  $Id: RichStatusCreator.cpp,v 1.13 2006-12-01 17:05:09 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichStatusCreator.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichStatusCreator );

// Standard constructor
RichStatusCreator::RichStatusCreator( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichRecToolBase      ( type, name, parent ),
    m_status             ( 0 ),
    m_richStatusLocation ( RichRecStatusLocation::Default )
{

  declareInterface<IRichStatusCreator>(this);

  if ( context() == "Offline" )
  {
    m_richStatusLocation = RichRecStatusLocation::Offline;
  }
  else if ( context() == "HLT" )
  {
    m_richStatusLocation = RichRecStatusLocation::HLT;
  }
  declareProperty( "RichRecStatusLocation", m_richStatusLocation );

}

StatusCode RichStatusCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecStatus location : " << m_richStatusLocation << endreq;
  }

  return sc;
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
  if ( !m_status )
  {
    if ( !exist<RichRecStatus>(m_richStatusLocation) )
    {
      m_status = new RichRecStatus();
      put( m_status, m_richStatusLocation );
    }
    else
    {
      m_status = get<RichRecStatus>(m_richStatusLocation);
    }
  }
  return m_status;
}
