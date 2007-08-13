#ifndef TF_IOTRAWBANKDECODER_H
#define TF_IOTRAWBANKDECODER_H

/** @class IOTDataSvc IOTDataSvc.h TsaKernel/IOTDataSvc.h
 *
 *  Interface to data svc
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

#include "GaudiKernel/IAlgTool.h"
#include "Kernel/OTChannelID.h"
#include "TfKernel/OTLiteTime.h"

namespace Tf
{
  static const InterfaceID IID_IOTRawBankDecoder( "Tf::IOTRawBankDecoder", 1, 0 );

  class IOTRawBankDecoder: virtual public IAlgTool 
  {
  public:
    
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IOTRawBankDecoder ; }   
    
    /// Decode one module
    virtual void decodeModule( const LHCb::OTChannelID& moduleId, 
			       LHCb::OTLiteTimeRange& ottimes ) const = 0 ;
    
  };
}

#endif
