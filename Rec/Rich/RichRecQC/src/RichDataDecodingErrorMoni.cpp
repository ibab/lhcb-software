
//-----------------------------------------------------------------------------
/** @file RichDataDecodingErrorMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::DAQ::DataDecodingErrorMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichDataDecodingErrorMoni.h"

// namespace
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( DataDecodingErrorMoni )

// Standard constructor, initializes variables
DataDecodingErrorMoni::DataDecodingErrorMoni( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : HistoAlgBase ( name, pSvcLocator ),
    m_decoder    ( NULL ) { }

// Destructor
DataDecodingErrorMoni::~DataDecodingErrorMoni() { }

//  Initialize
StatusCode DataDecodingErrorMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  return sc;
}

StatusCode DataDecodingErrorMoni::prebookHistograms()
{
  using namespace boost::assign;
  const BinLabels labels = list_of
    ("L1 Ingress Truncated")("ODIN/Ingress BXID MisMatch")
    ("HPD Inhibit")("HPD DB Lookup")("Ingress/HPD EventID MisMatch")
    ("Extended HPD Header");
  richProfile1D( HID("decodingErrors"), "DAQ Decoding Error Rates (%)", 0.5, 6.5, 6,
                 "DAQ Decoding Error Types", "Error Rate (%)", labels );
  return StatusCode::SUCCESS;
}

// Main execution
StatusCode DataDecodingErrorMoni::execute()
{

  // Obtain RichSmartIDs from raw decoding
  const DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  // Get the ODIN
  const LHCb::ODIN * odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );

  // Loop over L1 boards
  for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
        iL1 != data.end(); ++iL1 )
  {
    // loop over ingresses for this L1 board
    for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
          iIn != (*iL1).second.end(); ++iIn )
    {
      const Rich::DAQ::IngressInfo & ingressInfo       = iIn->second;
      const Rich::DAQ::L1IngressHeader & ingressHeader = ingressInfo.ingressHeader();

      // Check if all HPDs are suppressed
      richProfile1D( HID("decodingErrors") )
        -> fill( 1, ingressHeader.hpdsSuppressed() ? 100 : 0 );
      
      // Check BX ID between Rich and ODIN
      richProfile1D( HID("decodingErrors") )
        -> fill( 2, BXID(odin->bunchId()) != ingressHeader.bxID() ? 100 : 0 ); 

      // Loop over HPDs in this ingress
      for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
            iHPD != (*iIn).second.hpdData().end(); ++iHPD )
      {
        const bool inhibit = (*iHPD).second.header().inhibit();
        // inhibited HPDs
        richProfile1D( HID("decodingErrors") ) -> fill( 3, inhibit ? 100 : 0 ); 
        if ( !inhibit )
        {
          // Invalid HPD (BD lookup error)
          richProfile1D( HID("decodingErrors") )
            -> fill( 4, !(*iHPD).second.hpdID().isValid() ? 100 : 0 ); 
          // Event IDs
          richProfile1D( HID("decodingErrors") )
            -> fill( 5, 
                     (*iIn).second.ingressHeader().eventID() != 
                     (*iHPD).second.header().eventID() ? 100 : 0 ); 
          // HPD header in extended mode
          richProfile1D( HID("decodingErrors") )
            -> fill( 6, (*iHPD).second.header().extendedFormat() ? 100.0 : 0.0 );       
        }
      } // loop over HPDs

    } // ingresses

  } // L1 boards

  return StatusCode::SUCCESS;
}
