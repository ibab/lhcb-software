
//-----------------------------------------------------------------------------
/** @file RawBufferToRichDigitsAlg.cpp
 *
 *  Implementation file for RICH DAQ algorithm : RawBufferToRichDigitsAlg
 *
 *  CVS Log :-
 *  $Id: RawBufferToRichDigitsAlg.cpp,v 1.19 2007-02-01 17:42:28 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RawBufferToRichDigitsAlg.h"

// RICH DAQ
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RawBufferToRichDigitsAlg );

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
RawBufferToRichDigitsAlg::~RawBufferToRichDigitsAlg() {};

// Initialisation.
StatusCode RawBufferToRichDigitsAlg::initialize()// RICH software
{
  // intialise base
  const StatusCode sc = AlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire tools
  acquireTool( "RichSmartIDDecoder", m_decoder );

  if ( m_decodeOnly )
  { info() << "Will only decode Raw Buffer -> No RichDigits produced" << endreq; }

  return sc;
}

// Main execution
StatusCode RawBufferToRichDigitsAlg::execute()
{

  // Get RichSmartIDs decoded from RawEvent
  const Rich::DAQ::PDMap & smartIDs = m_decoder->allRichSmartIDs();

  if ( !m_decodeOnly )
  {
    // Make new container for RichDigits and give to Gaudi
    LHCb::RichDigits * digits = new LHCb::RichDigits();
    put( digits, m_richDigitsLoc );

    // Create a RichDigit for each SmartID
    for ( Rich::DAQ::PDMap::const_iterator iPD = smartIDs.begin();
          iPD != smartIDs.end(); ++iPD )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iID = (*iPD).second.begin();
            iID != (*iPD).second.end(); ++iID )
      {
        digits->insert( new LHCb::RichDigit(), *iID );
      }
    }

    // Final printout
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "Successfully registered " << digits->size()
              << " RichDigits at " << m_richDigitsLoc << endreq;
    }
  }

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RawBufferToRichDigitsAlg::finalize()
{
  // finalise base
  return AlgBase::finalize();
}
