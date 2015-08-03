
//-----------------------------------------------------------------------------
/** @file RichRawBufferToSmartIDsTool.cpp
 *
 * Implementation file for class : Rich::DAQ::RawBufferToSmartIDsTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawBufferToSmartIDsTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( RawBufferToSmartIDsTool )

// Standard constructor
  RawBufferToSmartIDsTool::RawBufferToSmartIDsTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
    : ToolBase       ( type, name, parent ),
      m_richSys      ( NULL ),
      m_rawFormatT   ( NULL )
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

  // cached variables
  m_taeKey = taeKey(m_rawEventLocs);

  if ( msgLevel(MSG::DEBUG) ) debug() << "RawEvent TAEs : " << m_rawEventLocs << endmsg;

  return sc;
}

// Method that handles various Gaudi "software events"
void RawBufferToSmartIDsTool::handle ( const Incident& /* incident */ )
{
  // Only subscribed to BeginEvent incidents, so no need to check type
  //if ( IncidentType::BeginEvent == incident.type() ) { InitNewEvent(); }
  InitNewEvent();
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

const LHCb::RichSmartID::Vector & RawBufferToSmartIDsTool::dummyVector() const
{
  // dummy vector for cases where nothing is found or created
  static LHCb::RichSmartID::Vector dummy_vector;
  return dummy_vector;
}

const LHCb::RichSmartID::Vector&
RawBufferToSmartIDsTool::richSmartIDs( const LHCb::RichSmartID hpdID,
                                       const Rich::DAQ::L1Map & data,
                                       const bool createIfMissing ) const
{
  // find the data for the requested HPD ...

  const LHCb::RichSmartID::Vector * found_data = NULL;

  // First seach in a read-only way

  /** @attention This implementation is not particulary efficient.
   *             Could maybe be improved if needed for speed reasons.
   *  @todo      Look into speeding this up if needed.
   */

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
    // Get some L1 information for this HPD from the DB
    const Rich::DAQ::Level1HardwareID l1HID   = m_richSys->level1HardwareID(hpdID);
    const Rich::DAQ::Level1Input      l1Input = m_richSys->level1InputNum(hpdID);

    // require non-const access to L1 Map
    Rich::DAQ::L1Map & l1Map = *(const_cast<Rich::DAQ::L1Map*>(&data));
    Rich::DAQ::IngressMap & ingressMap   = l1Map[l1HID];
    Rich::DAQ::IngressInfo & ingressInfo = ingressMap[l1Input.ingressID()];
    Rich::DAQ::HPDInfo & hpdInfo = ingressInfo.hpdData()[l1Input];

    // check HPDID is invalid (should be as not set so far ....)
    if ( hpdInfo.hpdID().isValid()   ) { Error( "HPDID already set ...." ).ignore();     }
    // check hit vector is empty
    if ( !hpdInfo.smartIDs().empty() ) { Error( "Hit list is not empty ...." ).ignore(); }

    // Set some information
    hpdInfo.setHpdID(hpdID);
    // Set what we can in the header / footer
    //hpdInfo.header().setL0ID(m_richSys->level0ID(hpdID));

    // set found data pointer
    found_data = &(hpdInfo.smartIDs());
  }

  // return either the found data or the dummy
  return ( found_data ? *found_data : dummyVector() );
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
  Rich::DAQ::L1Map & data = m_richDataTAE[ m_taeKey ];
  if ( data.empty() )
  {
    // Use raw format tool to decode event
    m_rawFormatT->decodeToSmartIDs( m_rawEventLocs, data );
  }
  return data;
}

unsigned int
RawBufferToSmartIDsTool::nTotalHits( const Rich::DetectorType rich ) const
{
  return countTotalHits( allRichSmartIDs(), rich );
}

unsigned int
RawBufferToSmartIDsTool::nTotalHits( const RawEventLocations& taeLocs,
                                     const Rich::DetectorType rich ) const
{
  return countTotalHits( allRichSmartIDs(taeLocs), rich );
}

unsigned int
RawBufferToSmartIDsTool::countTotalHits( const Rich::DAQ::L1Map & data,
                                         const Rich::DetectorType rich ) const
{
  // tally of the number of hits
  unsigned int nHits(0);


  // Loop over L1 boards
  for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
        iL1 != data.end(); ++iL1 )
  {
    // For MaPMTs this could for the moment throw an exception ... To Be Fixed
    // catch it to prevent processing termination
    try
    {

      // Is the RICH detector to be included in the count ?
      if ( Rich::InvalidDetector               == rich ||
           m_richSys->richDetector(iL1->first) == rich )
      {
        // loop over ingresses for this L1 board
        for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
              iIn != (*iL1).second.end(); ++iIn )
        {
          // Loop over HPDs in this ingress
          for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
                iHPD != (*iIn).second.hpdData().end(); ++iHPD )
          {
            // skip inhibited HPDs ?
            if ( (*iHPD).second.header().inhibit() ) { continue; }
            // Is the smart ID valid ?
            if ( !(*iHPD).second.hpdID().isValid() ) { continue; }
            // all OK so count hits
            nHits += (*iHPD).second.smartIDs().size();
          }
        }
      }

    }
    catch ( const GaudiException & excpt )
    {
      Warning( excpt.message() ).ignore();
    }

  }

  // return the final count
  return nHits;
}
