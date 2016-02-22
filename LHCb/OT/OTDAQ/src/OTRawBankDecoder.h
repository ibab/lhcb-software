#ifndef OTRAWBANKDECODER_H
#define OTRAWBANKDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "OTDAQ/IOTRawBankDecoder.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"
#include "DAQKernel/DecoderToolBase.h"

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
 *  OTLiteTimes, which are essentially just wrappers of an
 *  OTChannelId.
 *
 *  Based on old OTTimeCreator by Jacopo Nardulli
 *
 *  @author Jacopo Nardulli, Jan Amoraal, Wouter Hulsbergen
 *
 *  @date   2007-05-30
 */

class OTRawBankDecoder : public extends<Decoder::ToolBase,
                                        IOTRawBankDecoder,
                                        IIncidentListener>
{

public:

  /// Standard constructor
  OTRawBankDecoder( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  /// Tool initialization
  StatusCode initialize() override;

  /// Tool finalize
  StatusCode finalize() override;

  /// Decode data for a single module
  LHCb::OTLiteTimeRange decodeModule( const LHCb::OTChannelID& moduleId ) const override;

  /// Decode all gol headers
  StatusCode decodeGolHeaders() const override;

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
  void handle ( const Incident& incident ) override;
  size_t decodeModule( const OTRawBankDecoderHelpers::Module& ) const;
  StatusCode decodeGolHeadersV3(const LHCb::RawBank&, int bankversion) const;
  StatusCode decodeGolHeadersDC06(const LHCb::RawBank&, int bankversion) const;

private:
  // data
  mutable std::unique_ptr<OTRawBankDecoderHelpers::Detector> m_detectordata; ///< Contains decoded data
  DeOTDetector* m_otdet = nullptr;                  ///< Pointer to OT geometry
  IOTChannelMapTool* m_channelmaptool = nullptr ;     ///< Pointer to IOTChannelMapTool
  std::vector<double> m_startReadOutGate;   ///< Start of readout gate

  std::pair<double,double> m_timewindow = { 999*Gaudi::Units::ns,-999*Gaudi::Units::ns};    ///< ignore hits outside of this window, only if first < second; if not, use all
  double m_timePerBX = 25*Gaudi::Units::ns;                       ///< Time Per BX
  double m_nsPerTdcCount = -1. ;                  ///< Conversion from tdc to ns (initialization uses m_countPerBx and m_timePerBx)
  int  m_countsPerBX = 64;                       ///< Counts per BX
  int  m_numberOfBX = 3;                        ///< Number of BX
  int  m_forcebankversion = OTBankVersion::UNDEFINED;                  ///< Overwrite bank version in bank header
  bool m_vetoOutOfTimeHitPairs = true;		    ///< veto out of time hit pairs?
  friend class OTRawBankDecoderHelpers::Detector;
  friend class OTRawBankDecoderHelpers::Module;
};

#endif // OTRAWBANKDECODER_H
