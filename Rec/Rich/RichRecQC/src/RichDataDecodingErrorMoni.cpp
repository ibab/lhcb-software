
//-----------------------------------------------------------------------------
/** @file RichDataDecodingErrorMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::DAQ::DataDecodingErrorMoni
 *
 *  $Id: RichDataDecodingErrorMoni.cpp,v 1.12 2010-02-11 20:01:30 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichDataDecodingErrorMoni.h"

// namespace
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( DataDecodingErrorMoni );

// Standard constructor, initializes variables
DataDecodingErrorMoni::DataDecodingErrorMoni( const std::string& name,
                                              ISvcLocator* pSvcLocator)
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
  richHisto1D( HID("decodingErrors"), "Decoding Errors", 0.5, 6.5, 6 );
  return StatusCode::SUCCESS;
}

// Main execution
StatusCode DataDecodingErrorMoni::execute()
{

  // Obtain RichSmartIDs from raw decoding
  const DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  // Get the ODIN
  LHCb::ODIN * odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );

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
      if ( ingressHeader.hpdsSuppressed() ) { richHisto1D( HID("decodingErrors") )->fill(1); }

      // Check Event ID between Rich and ODIN
      if ( EventID(odin->eventNumber()) != ingressHeader.eventID() )
      { richHisto1D( HID("decodingErrors") )->fill(2); } 
      
      // Check BX ID between Rich and ODIN
      if ( EventID(odin->bunchId()) != ingressHeader.bxID() )
      { richHisto1D( HID("decodingErrors") )->fill(3); } 

      // Loop over HPDs in this ingress
      for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
            iHPD != (*iIn).second.hpdData().end(); ++iHPD )
      {
        // inhibited HPDs
        if      ( (*iHPD).second.header().inhibit() ) { richHisto1D( HID("decodingErrors") )->fill(4); }
        // Invalid HPD (BD lookup error)
        else if ( !(*iHPD).second.hpdID().isValid() ) { richHisto1D( HID("decodingErrors") )->fill(5); }
        // Event IDs
        else if ( (*iIn).second.ingressHeader().eventID() != (*iHPD).second.header().eventID() )
        { richHisto1D( HID("decodingErrors") )->fill(6); }
      } // loop over HPDs

    } // ingresses

  } // L1 boards

  return StatusCode::SUCCESS;
}
