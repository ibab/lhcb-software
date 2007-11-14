// $Id: OTRawBankDecoder.h,v 1.3 2007-11-14 16:05:39 wouter Exp $
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

namespace OTRawBankDecoderHelpers
{
  struct Detector ;
  struct Module ;
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
  
  /// Decode all modules
  StatusCode decode( LHCb::OTLiteTimeContainer& ottimes ) const ;
  
  /// Get the conversion factor
  double nsPerTdcCount() const { return m_nsPerTdcCount ; }
  
  /// Create a single OTLiteTime
  LHCb::OTLiteTime time( LHCb::OTChannelID channel, const DeOTModule& module ) const ;
  
private:
  virtual void handle ( const Incident& incident );
  size_t decodeModule( OTRawBankDecoderHelpers::Module& ) const ;
  
private:
  // data
  std::vector<double> m_startReadOutGate;   ///< Start of readout gate
  int  m_countsPerBX;                       ///< Counts per BX
  int  m_numberOfBX;                        ///< Number of BX
  double m_timePerBX;                       ///< Time Per BX
  DeOTDetector* m_otdet  ;                  ///< Pointer to OT geometry
  double m_nsPerTdcCount ;                  ///< Conversion from tdc to ns
  
  mutable OTRawBankDecoderHelpers::Detector* m_detectordata ; ///< Contains decoded data
};

//////////////////////////////////////////////////////////////////////////////////////////

inline LHCb::OTLiteTime 
OTRawBankDecoder::time( LHCb::OTChannelID channel, const DeOTModule& module ) const
{
  return LHCb::OTLiteTime( channel, channel.tdcTime() * m_nsPerTdcCount - module.strawT0(channel.straw())) ;
}

#endif // OTRAWBANKDECODER_H
