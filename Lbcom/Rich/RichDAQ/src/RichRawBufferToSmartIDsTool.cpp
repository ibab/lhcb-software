
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : RichRawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.11 2005-09-25 09:50:25 jonrob Exp $
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
    m_sortIDs          ( true  ),
    m_newEvent         ( true  )
{

  // Defined interface
  declareInterface<IRichRawBufferToSmartIDsTool>(this);

  // job options
  declareProperty( "SortRichSmartIDs", m_sortIDs );

}

StatusCode RichRawBufferToSmartIDsTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichRawDataFormatTool", m_rawFormatT );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  if (m_sortIDs) 
    info() << "Data will be sorted according to RICH and HPD panel" << endreq;

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

const RichSmartID::Collection & RichRawBufferToSmartIDsTool::allRichSmartIDs() const
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

  // Sort into order of Rich/Panel/HPD/Pixel if required
  if ( m_sortIDs )
  {
    m_sorter.sortByRegion( m_smartIDs );
  }

  // Printout the RichSmartIDs...
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "RichSmartIDs :-" << endreq;
    for ( RichSmartID::Collection::const_iterator iID = m_smartIDs.begin(); iID != m_smartIDs.end(); ++iID )
    {
      verbose() << "   " << *iID << " " << (*iID).key() << endreq;
    }
  }

}
