#ifndef OTDAQ_IOTRAWBANKENCODER_H
#define OTDAQ_IOTRAWBANKENCODER_H 1

// Include files
// from std
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event
#include "Kernel/OTChannelID.h"

/// Interface ID
static const InterfaceID IID_OTRawBankEncoder( "IOTRawBankEncoder", 1, 0 );

/** @class IOTRawBankEncoder IOTRawBankEncoder.h OTDAQ/IOTRawBankEncoder.h
 *
 *  Interface for tools that encode the channel ids and
 *  put them in the raw bank. 
 *
 *  @author Jan Mennis Amoraal
 *
 *  @date   2008-05-22
 */

class IOTRawBankEncoder : virtual public IAlgTool {

public: 
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTRawBankEncoder ; }

  virtual StatusCode encodeChannels( const std::vector<LHCb::OTChannelID>& channels ) const = 0;
  
};

#endif // OTDAQ_IOTRAWBANKENCODER_H
