
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : Rich::DAQ::RawBufferToSmartIDsTool
 *
 * CVS Log :-
 * $Id: RichRawBufferToSmartIDsTool.cpp,v 1.24 2008-10-15 12:29:30 jonrob Exp $
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
    m_richSys      ( NULL ),
    m_rawFormatT   ( NULL ),
    m_newEvent     ( true )
{
  // Defined interface
  declareInterface<IRawBufferToSmartIDsTool>(this);
  // JOs
  declareProperty( "RawEventLocations",
                   m_rawEventLocs = IRawDataFormatTool::RawEventLocations(1,"") );
}

StatusCode RawBufferToSmartIDsTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );
  
  // acquire tools
  acquireTool( "RichRawDataFormatTool", "RawDecoder", m_rawFormatT, this );
  
  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  
  debug() << "RawEvent TAEs : " << m_rawEventLocs << endreq;

  return sc;
}

// Method that handles various Gaudi "software events"
void RawBufferToSmartIDsTool::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
}

// ---------------------------------------------------------------------------------------------

const LHCb::RichSmartID::Vector&
RawBufferToSmartIDsTool::richSmartIDs( const IRawBufferToSmartIDsTool::RawEventLocations& taeLocs,
                                       const LHCb::RichSmartID hpdID,
                                       const bool createIfMissing ) const
{
  // get the full data structure
  const Rich::DAQ::L1Map & data = allRichSmartIDs(taeLocs);
  // find the data vector
  return richSmartIDs( hpdID, data, createIfMissing );
}

const LHCb::RichSmartID::Vector&
RawBufferToSmartIDsTool::richSmartIDs( const LHCb::RichSmartID hpdID,
                                       const bool createIfMissing ) const
{
  // get the full data structure
  const Rich::DAQ::L1Map & data = allRichSmartIDs();
  // find the data vector
  return richSmartIDs( hpdID, data, createIfMissing );
}

const LHCb::RichSmartID::Vector&
RawBufferToSmartIDsTool::richSmartIDs( const LHCb::RichSmartID hpdID,
                                       const Rich::DAQ::L1Map & data,
                                       const bool createIfMissing ) const
{
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

  // If not found and requested, create appropriate entry
  if ( !found_data && createIfMissing )
  {
    const Rich::DAQ::Level1HardwareID l1HID   = m_richSys->level1HardwareID(hpdID);
    const Rich::DAQ::Level1Input 	    l1Input = m_richSys->level1InputNum(hpdID);
    // require non-const access
    Rich::DAQ::L1Map & l1Map = *(const_cast<Rich::DAQ::L1Map*>(&data));
    Rich::DAQ::IngressMap & ingressMap   = l1Map[l1HID];
    Rich::DAQ::IngressInfo & ingressInfo = ingressMap[l1Input.ingressID()];
    // what to do about header words ??
    Rich::DAQ::HPDInfo & hpdInfo = ingressInfo.hpdData()[l1Input];
    if ( hpdInfo.hpdID() != hpdID )
    {
      error() << "HPD Mis-match : Expected " << hpdID           << endreq;
      error() << "              : Received " << hpdInfo.hpdID() << endreq;
    }
  }

  // dummy vector for cases where nothing is found
  static const LHCb::RichSmartID::Vector dummy_vector;

  // return either the found data or the dummy
  return ( found_data ? *found_data : dummy_vector );
}

const Rich::DAQ::L1Map &
RawBufferToSmartIDsTool::allRichSmartIDs( const IRawBufferToSmartIDsTool::RawEventLocations& taeLocs ) const
{
  Rich::DAQ::L1Map & data = m_richDataTAE[ taeKey(taeLocs) ];
  if ( data.empty() )
  {
    // Use raw format tool to decode event
    m_rawFormatT->decodeToSmartIDs( taeLocs, data );
  }
  return data;
}

const Rich::DAQ::L1Map & RawBufferToSmartIDsTool::allRichSmartIDs() const
{
  if ( m_newEvent )
  {
    // clear current data
    m_richData.clear();
    // Use raw format tool to decode event
    m_rawFormatT->decodeToSmartIDs( m_rawEventLocs, m_richData );
    // Set this event processed
    m_newEvent = false;
  }
  return m_richData;
}

