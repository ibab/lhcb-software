
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : RichRawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.7 2005-01-13 13:11:33 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/01/07 12:35:59  jonrob
 * Complete rewrite
 *
 * Revision 1.5  2004/11/05 20:05:00  jonrob
 * update functor
 *
 * Revision 1.4  2004/11/03 09:30:16  jonrob
 * Update RichSmartID + add functionality to sort the data
 *
 * Revision 1.3  2004/11/02 13:13:38  jonrob
 * minro update - add constness
 *
 * Revision 1.2  2004/10/30 21:45:57  jonrob
 * update decoding tool
 *
 * Revision 1.1  2004/10/30 19:13:05  jonrob
 * Reworking RawBuffer decoding as a tool, to allow reconstruction to skip RichDigit creation
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 *
 * @todo Review RichSmartID bit packing to facilitate more efficient and finer grained sorting
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
    m_sortIDs          ( false ),
    m_newEvent         ( true  )
{

  declareInterface<IRichRawBufferToSmartIDsTool>(this);

  declareProperty( "SortRichSmartIDs", m_sortIDs = false );

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

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
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
  if ( m_newEvent ) {
    fillRichSmartIDs(); // Fill for this event
    m_newEvent = false; // Set this event processed
  }
  return m_smartIDs;
}

void RichRawBufferToSmartIDsTool::fillRichSmartIDs() const
{

  // Use raw format tool to decode event
  m_rawFormatT->decodeToSmartIDs( m_smartIDs );

  // Sort into order of Rich/Panel/HPD/Pixel
  if ( m_sortIDs )
  {
    RichSmartIDSorter sorter;
    sorter.sortByRegion( m_smartIDs );
  }

}
