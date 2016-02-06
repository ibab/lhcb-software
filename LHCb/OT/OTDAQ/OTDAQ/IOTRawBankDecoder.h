#ifndef OTDAQ_IOTRAWBANKDECODER_H
#define OTDAQ_IOTRAWBANKDECODER_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Event/OTLiteTime.h"
#include "OTDAQ/RawEvent.h"

// Forward declarations
class DeOTModule ;

namespace LHCb {
  class RawEvent ;
}


/** @class IOTRawBankDecoder IOTRawBankDecoder.h OTDAQ/IOTRawBankDecoder.h
 *
 *  Decodes the OT data for single modules. The output is a list of
 *  OTLiteTimes, which are essentially just wrappers of an OTChannelId.
 *
 *  Based on old OTTimeCreator by Jacopo Nardulli
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @author Jacopo Nardulli
 *
 *  @date   2007-05-30
 */

class IOTRawBankDecoder : public extend_interfaces<IAlgTool>
{
  
public: 
  /// Interface ID
  DeclareInterfaceID( IOTRawBankDecoder, 2, 0 );
  
  /// Decode data for a single module
  virtual LHCb::OTLiteTimeRange decodeModule( const LHCb::OTChannelID& moduleId ) const = 0 ;
  
  /// Decode all gol headers
  virtual StatusCode decodeGolHeaders() const = 0;
  
  /// Decode all gol headers.
  virtual StatusCode decodeGolHeaders(const LHCb::RawEvent& rawevent) const = 0;
  
  /// Decode all modules
  virtual StatusCode decode( LHCb::OTLiteTimeContainer& ottimes ) const = 0;

  /// Translate the raw bank in an ot-specific raw bank.
  virtual StatusCode decode( OTDAQ::RawEvent& otevent ) const = 0;

  /// Retrieve the total number of hits in the OT without actually
  /// decoding the modules. Useful in pattern reco to remove full
  /// events.
  virtual size_t totalNumberOfHits() const = 0 ;

  /// Conversion factor from tdc to ns
  virtual double nsPerTdcCount() const = 0 ;

  /// Get a lite time from an OTChannelId
  virtual LHCb::OTLiteTime time( LHCb::OTChannelID id ) const = 0 ;

  virtual bool isTotalNumberOfHitsLessThen(size_t nmax) const
  { return totalNumberOfHits() < nmax; }
};

#endif // OTDAQ_IOTRAWBANKDECODER_H
