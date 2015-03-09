//-----------------------------------------------------------------------------
/** @file RawBufferToRichDigitsAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RawBufferToRichDigitsAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// local
#include "RawBufferToRichDigitsAlg.h"

// RICH DAQ
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RawBufferToRichDigitsAlg )

// Standard constructor, initializes variables
RawBufferToRichDigitsAlg::RawBufferToRichDigitsAlg( const std::string& name,
                                                    ISvcLocator* pSvcLocator )
  : AlgBase ( name , pSvcLocator )
  , m_decoder(NULL)
{
  declareProperty( "RichDigitsLocation",
                   m_richDigitsLoc = LHCb::RichDigitLocation::Default );
  declareProperty( "DecodeBufferOnly", m_decodeOnly = false );
}

// Destructor
RawBufferToRichDigitsAlg::~RawBufferToRichDigitsAlg() {}

// Initialisation.
StatusCode RawBufferToRichDigitsAlg::initialize()// RICH software
{
  // intialise base
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichSmartIDDecoder", m_decoder, NULL, true );

  if ( m_decodeOnly )
  { info() << "Will only decode Raw Buffer -> No RichDigits produced" << endmsg; }

  return sc;
}

// Main execution
StatusCode RawBufferToRichDigitsAlg::execute()
{

  // Get RichSmartIDs decoded from RawEvent
  const Rich::DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  if ( !m_decodeOnly )
  {
    // Make new container for RichDigits and give to Gaudi
    LHCb::RichDigits * digits = new LHCb::RichDigits();
    put( digits, m_richDigitsLoc );

    // Loop over L1 boards
    for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
          iL1 != data.end(); ++iL1 )
    {
      debug() << "L1 board " << (*iL1).first << endmsg;
      // loop over ingresses for this L1 board
      for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
            iIn != (*iL1).second.end(); ++iIn )
      {
        // Loop over HPDs in this ingress
        for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
              iHPD != (*iIn).second.hpdData().end(); ++iHPD )
        {
          // Valid HPD ID
          if ( !(*iHPD).second.hpdID().isValid() ) { continue; }
          // inhibited HPD ?
          if ( (*iHPD).second.header().inhibit() ) { continue; }
          // Loop over RichSmartIDs in this HPD
          for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iHPD).second.smartIDs().begin();
                iID != (*iHPD).second.smartIDs().end(); ++iID )
          {
            digits->insert( new LHCb::RichDigit(), *iID );
          }
        }
      }
    }

    // Final printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Successfully registered " << digits->size()
              << " RichDigits at " << m_richDigitsLoc << endmsg;
    }

  } // make digits

  return StatusCode::SUCCESS;
}
