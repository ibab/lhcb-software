
//-----------------------------------------------------------------------------
/** @file RichStatusCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::StatusCreator
 *
 *  CVS Log :-
 *  $Id: RichStatusCreator.cpp,v 1.15 2009-07-30 11:23:55 jonrob Exp $
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

DECLARE_TOOL_FACTORY( StatusCreator );

// Standard constructor
StatusCreator::StatusCreator( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : RichRecToolBase      ( type, name, parent ),
    m_status             ( NULL )
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
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "RichRecStatus location : " << m_richStatusLocation << endmsg;
  }

  return sc;
}

StatusCode StatusCreator::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void StatusCreator::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

LHCb::RichRecStatus * StatusCreator::richStatus() const
{
  if ( !m_status )
  {
    if ( !exist<LHCb::RichRecStatus>(m_richStatusLocation) )
    {
      m_status = new LHCb::RichRecStatus();
      put( m_status, m_richStatusLocation );
    }
    else
    {
      m_status = get<LHCb::RichRecStatus>(m_richStatusLocation);
    }
  }
  return m_status;
}
