// Include files 
// from std
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// MCEvent
#include "Event/MCOTTime.h"

// Event
#include "Kernel/OTChannelID.h"

// from Boost
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "OTFillRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OTFillRawBuffer
//
// 2008-05-23 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTFillRawBuffer )

OTFillRawBuffer::OTFillRawBuffer( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_encoder(NULL)
{
 
}

OTFillRawBuffer::~OTFillRawBuffer() {}

StatusCode OTFillRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_encoder = tool<IOTRawBankEncoder>( "OTRawBankEncoder", this );

  return StatusCode::SUCCESS;
}

StatusCode OTFillRawBuffer::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  /// Get MCOTTimes
  const LHCb::MCOTTimes* times = getIfExists<LHCb::MCOTTimes>( LHCb::MCOTTimeLocation::Default );
  
  /// They should be there, but it doesn't hurt to check
  if ( 0 == times ) {
    return Error( "There are no MCOTTimes in TES", StatusCode::SUCCESS );
  }
  
  /// Tmp vector of channel ids we want encode
  std::vector<LHCb::OTChannelID> channels;
  channels.reserve( times->size() ); ///< Should be 1-to-1

  /// Now insert the channel id we get from the MCOTTime into the tmp vector of channel ids
  std::transform( times->begin(), 
                  times->end(), 
                  std::back_inserter( channels ),
                  boost::lambda::bind( &LHCb::MCOTTime::channel, boost::lambda::_1) );
  
  StatusCode sc = m_encoder->encodeChannels( channels );
  if ( sc.isFailure() ) Error( "Could not encode channels", sc ).ignore();
  
  return StatusCode::SUCCESS;
}
