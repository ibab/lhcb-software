#ifndef TF_IOTRAWBANKDECODER_H
#define TF_IOTRAWBANKDECODER_H

#include "GaudiKernel/IAlgTool.h"
#include "Kernel/OTChannelID.h"
#include "TfKernel/OTLiteTime.h"

namespace Tf
{
  /// Interface ID
  static const InterfaceID IID_IOTRawBankDecoder( "Tf::IOTRawBankDecoder", 1, 0 );

  /** @class IOTRawBankDecoder TfKernel/IOTRawBankDecoder.h
   *
   *  Interface to the OT Raw Bank decoding tool
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-05-30
   */
  class IOTRawBankDecoder: virtual public IAlgTool
  {
  public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IOTRawBankDecoder ; }

    /** Decode one OT module
     *  @param[in] moduleId The OT module IN to decode
     *  @param[out] ottimes The decoded OTLiteTimes
     */
    virtual void decodeModule( const LHCb::OTChannelID& moduleId,
                               LHCb::OTLiteTimeRange& ottimes ) const = 0 ;

  };
}

#endif
