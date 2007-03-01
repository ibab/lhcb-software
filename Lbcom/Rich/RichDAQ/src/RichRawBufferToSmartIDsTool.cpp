
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : Rich::DAQ::RawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.17 2007-03-01 19:39:07 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichRawBufferToSmartIDsTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( RawBufferToSmartIDsTool );

// Standard constructor
RawBufferToSmartIDsTool::RawBufferToSmartIDsTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : ToolBase       ( type, name, parent ),
    m_rawFormatT   ( NULL  ),
    m_newEvent     ( true  )
{

  // Defined interface
  declareInterface<IRawBufferToSmartIDsTool>(this);

}

StatusCode RawBufferToSmartIDsTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichRawDataFormatTool", m_rawFormatT, 0, true );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;
}

StatusCode RawBufferToSmartIDsTool::finalize()
{
  // Execute base class method
  return ToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RawBufferToSmartIDsTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

const Rich::DAQ::PDMap & RawBufferToSmartIDsTool::allRichSmartIDs() const
{
  if ( m_newEvent )
  {
    fillRichSmartIDs(); // Fill for this event
    m_newEvent = false; // Set this event processed
  }
  return m_smartIDs;
}

void RawBufferToSmartIDsTool::fillRichSmartIDs() const
{
  // clear current data
  m_smartIDs.clear();

  // Use raw format tool to decode event
  m_rawFormatT->decodeToSmartIDs( m_smartIDs );

  // Printout the RichSmartIDs...
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "RichSmartIDs :-" << endreq;
    for ( Rich::DAQ::PDMap::const_iterator iPD = m_smartIDs.begin();
          iPD != m_smartIDs.end(); ++iPD )
    {
      verbose() << "  HPD " << (*iPD).first << " :-" << endreq;
      for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iPD).second.begin();
            iID != (*iPD).second.end(); ++iID )
      {
        verbose() << "   " << *iID << " " << (*iID).key() << endreq;
      }
    }
  }

}
