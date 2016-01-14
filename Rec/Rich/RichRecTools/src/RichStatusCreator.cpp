
//-----------------------------------------------------------------------------
/** @file RichStatusCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::StatusCreator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichStatusCreator.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Standard constructor
StatusCreator::StatusCreator( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : ToolBase( type, name, parent )
{
  // interface
  declareInterface<IStatusCreator>(this);
  // JOs
  declareProperty( "RichRecStatusLocation",
                   m_richStatusLocation = contextSpecificTES(LHCb::RichRecStatusLocation::Default) );
}

StatusCode StatusCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  _ri_debug << "RichRecStatus location : " << m_richStatusLocation << endmsg;

  return sc;
}

// Method that handles various Gaudi "software events"
void StatusCreator::handle ( const Incident& /* incident */ )
{
  //if ( IncidentType::BeginEvent == incident.type() ) 
  // No need to check incident type if only one callback is registered.
  InitNewEvent();
}

LHCb::RichRecStatus * StatusCreator::richStatus() const
{
  if ( !m_status )
  {
    m_status = getIfExists<LHCb::RichRecStatus>(m_richStatusLocation);
    if ( !m_status )
    {
      m_status = new LHCb::RichRecStatus();
      put( m_status, m_richStatusLocation );
    }
  }
  return m_status;
}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( StatusCreator )

//-----------------------------------------------------------------------------
