
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : Rich::DAQ::RawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.22 2008-09-11 14:44:30 jonrob Exp $
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
  acquireTool( "RichRawDataFormatTool", "RawDecoder", m_rawFormatT, this );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  return sc;
}

// Method that handles various Gaudi "software events"
void RawBufferToSmartIDsTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

const Rich::DAQ::L1Map & RawBufferToSmartIDsTool::allRichSmartIDs() const
{
  if ( m_newEvent )
  {
    fillRichSmartIDs(); // Fill for this event
    m_newEvent = false; // Set this event processed
  }
  return m_richData;
}

const LHCb::RichSmartID::Vector& 
RawBufferToSmartIDsTool::richSmartIDs( const LHCb::RichSmartID hpdID ) const
{
  // get the full data structure
  const Rich::DAQ::L1Map & data = allRichSmartIDs();

  // find the data for the requested HPD ...
 
  /** @attention This implementation is not particulary efficient. 
   *             Could maybe be improved if needed for speed reasons.
   *  @todo      Look into speeding this up if needed.
   */

  const LHCb::RichSmartID::Vector * found_data = NULL;

  // Loop over L1 boards
  for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
        iL1 != data.end(); ++iL1 )
  {
    // loop over ingresses for this L1 board
    for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
          iIn != (*iL1).second.end(); ++iIn )
    {
      // Find HPDInfo for given hpd ?
      // Loop over HPDs in this ingress
      for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
            iHPD != (*iIn).second.hpdData().end(); ++iHPD )
      {
        if ( hpdID == (*iHPD).second.hpdID() )
        { 
          found_data = &((*iHPD).second.smartIDs());
          break;
        }
      } // loop over HPDs
      if ( found_data ) break;
    } // loop over ingresses
    if ( found_data ) break;
  }

  // dummy vector for cases where nothing is found
  static const LHCb::RichSmartID::Vector dummy_vector;

  // return either the found data or the dummy
  return ( found_data ? *found_data : dummy_vector );
}

void RawBufferToSmartIDsTool::fillRichSmartIDs() const
{
  // clear current data
  m_richData.clear();

  // Use raw format tool to decode event
  m_rawFormatT->decodeToSmartIDs( m_richData );
}
