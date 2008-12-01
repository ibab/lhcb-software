// $Id: OTRawBankDecoder.h,v 1.12 2008-12-01 21:36:20 wouter Exp $
#ifndef OTRAWBANKDECODER_H
#define OTRAWBANKDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "OTDAQ/IOTRawBankDecoder.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"

// Kernel
#include "OTDet/DeOTModule.h"
#include "Event/OTLiteTime.h"

// forward declarations
class DeOTDetector;
class IOTChannelMapTool ;
namespace LHCb {
  class RawBank ;
}

namespace OTRawBankDecoderHelpers
{
  class Detector ;
  class Module ;
}

/** @class OTRawBankDecoder OTRawBankDecoder.h
 *
 *  Decodes the OT data for single modules. The output is a list of
 *  OTLiteTimes, which are essnetially just wrappers of an
 *  OTChannelId.
 *
 *  Based on old OTTimeCreator by Jacopo Nardulli
 *
 *  @author Jacopo Nardulli, Jan Amoraal, Wouter Hulsbergen
 *
 *  @date   2007-05-30
 */

class OTRawBankDecoder : public GaudiTool,
                         virtual public IOTRawBankDecoder,
                         virtual public IIncidentListener
{
  
public: 
  
  /// Standard constructor
  OTRawBankDecoder( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  
  /// Destructor
  virtual ~OTRawBankDecoder( ) ; ///< Destructor
  
  /// Tool initialization
  virtual StatusCode initialize();
 
  /// Tool finalize
  virtual StatusCode finalize(); 
  
  /// Decode data for a single module
  virtual LHCb::OTLiteTimeRange decodeModule( const LHCb::OTChannelID& moduleId ) const ;
  
  /// Decode all gol headers
  StatusCode decodeGolHeaders() const ;

  /// Decode all gol headers in a particular RawEvent
  virtual StatusCode decodeGolHeaders(const LHCb::RawEvent& rawevent) const ;

  /// Decode all modules
  StatusCode decode( LHCb::OTLiteTimeContainer& ottimes ) const ;
  
  /// Translate the raw bank in an ot-specific raw bank.
  StatusCode decode( OTDAQ::RawEvent& otevent ) const ;

  /// Retrieve the total number of hits in the OT without actually
  /// decoding the modules. Useful in pattern reco to remove full
  /// events.
  size_t totalNumberOfHits() const ;

  /// Get the conversion factor
  double nsPerTdcCount() const { return m_nsPerTdcCount ; }
  
  /// Get an OTLiteTime from a channel
  LHCb::OTLiteTime time( LHCb::OTChannelID channel ) const ;
  
protected:
  virtual void handle ( const Incident& incident );
  size_t decodeModule( OTRawBankDecoderHelpers::Module& ) const ;
  StatusCode decodeGolHeadersV3(const LHCb::RawBank&, int bankversion) const ;
  StatusCode decodeGolHeadersDC06(const LHCb::RawBank&, int bankversion) const ;
  
private:
  // data
  std::vector<double> m_startReadOutGate;   ///< Start of readout gate
  int  m_countsPerBX;                       ///< Counts per BX
  int  m_numberOfBX;                        ///< Number of BX
  double m_timePerBX;                       ///< Time Per BX
  int m_forcebankversion;                   ///< Overwrite bank version in bank header
  DeOTDetector* m_otdet  ;                  ///< Pointer to OT geometry
  IOTChannelMapTool* m_channelmaptool ;     ///< Pointer to IOTChannelMapTool
  double m_nsPerTdcCount ;                  ///< Conversion from tdc to ns
  std::string m_rawEventLocation;           ///< Location where we get the RawEvent
  
  mutable OTRawBankDecoderHelpers::Detector* m_detectordata ; ///< Contains decoded data
};

#endif // OTRAWBANKDECODER_H
