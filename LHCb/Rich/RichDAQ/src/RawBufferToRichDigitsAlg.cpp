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
  acquireTool( "RichSmartIDDecoder", m_decoder, nullptr, true );

  if ( m_decodeOnly )
  { info() << "Will only decode Raw Buffer -> No RichDigits produced" << endmsg; }

  return sc;
}

// Main execution
StatusCode RawBufferToRichDigitsAlg::execute()
{

  // Get RichSmartIDs decoded from RawEvent
  const auto & data = m_decoder->allRichSmartIDs();

  if ( !m_decodeOnly )
  {
    // Make new container for RichDigits and give to Gaudi
    LHCb::RichDigits * digits = new LHCb::RichDigits();
    put( digits, m_richDigitsLoc );

    // Loop over L1 boards
    for ( const auto& L1 : data )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "L1 board " << L1.first << endmsg;
      // loop over ingresses for this L1 board
      for ( const auto& In : L1.second )
      {
        // Loop over HPDs in this ingress
        for ( const auto& HPD : In.second.hpdData() )
        {
          // Valid HPD ID
          if ( !HPD.second.hpdID().isValid() ) { continue; }
          // inhibited HPD ?
          if ( HPD.second.header().inhibit() ) { continue; }
          // Loop over RichSmartIDs in this HPD
          for ( const auto& ID : HPD.second.smartIDs() )
          {
            digits->insert( new LHCb::RichDigit(), ID );
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
