
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : RichRawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.14 2006-03-01 09:56:12 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawBufferToSmartIDsTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichRawBufferToSmartIDsTool>          s_factory ;
const        IToolFactory& RichRawBufferToSmartIDsToolFactory = s_factory ;

// Standard constructor
RichRawBufferToSmartIDsTool::RichRawBufferToSmartIDsTool( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : RichToolBase       ( type, name, parent ),
    m_rawFormatT       ( 0     ),
    m_newEvent         ( true  )
{

  // Defined interface
  declareInterface<IRichRawBufferToSmartIDsTool>(this);

}

StatusCode RichRawBufferToSmartIDsTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichRawDataFormatTool", m_rawFormatT, 0, true );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;
}

StatusCode RichRawBufferToSmartIDsTool::finalize()
{
  // Execute base class method
  return RichToolBase::finalize();
}

// Method that handles various Gaudi "software events"
void RichRawBufferToSmartIDsTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

const RichDAQ::PDMap & RichRawBufferToSmartIDsTool::allRichSmartIDs() const
{
  if ( m_newEvent )
  {
    fillRichSmartIDs(); // Fill for this event
    m_newEvent = false; // Set this event processed
  }
  return m_smartIDs;
}

void RichRawBufferToSmartIDsTool::fillRichSmartIDs() const
{

  // Use raw format tool to decode event
  m_rawFormatT->decodeToSmartIDs( m_smartIDs );

  // Printout the RichSmartIDs...
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "RichSmartIDs :-" << endreq;
    for ( RichDAQ::PDMap::const_iterator iPD = m_smartIDs.begin();
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
