// $Id: OTRawBankDecoder.h,v 1.1 2007-09-07 13:19:20 wouter Exp $
#ifndef OTRAWBANKDECODER_H
#define OTRAWBANKDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "OTDAQ/IDecodeOTTimeOnDemand.h"            // Interface
#include "GaudiKernel/ChronoEntity.h"
#include "GaudiKernel/IIncidentListener.h"

// Kernel

#include "Event/OTLiteTime.h"

// forward declarations
class DeOTDetector;

/// Interface ID

static const InterfaceID IID_OTRawBankDecoder( "OTRawBankDecoder", 1, 0 );


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
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @author Jacopo Nardulli
 *
 *  @date   2007-05-30
 */

class OTRawBankDecoder : public GaudiTool,
			 virtual public IIncidentListener
{
  
public: 
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTRawBankDecoder ; }
  
  /// Standard constructor
  OTRawBankDecoder( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);
  
  /// Destructor
  virtual ~OTRawBankDecoder( ) ; ///< Destructor
  
  /// Tool initialization
  virtual StatusCode initialize(); 
  
  /// Decode data for a single module
  virtual LHCb::OTLiteTimeRange decodeModule( const LHCb::OTChannelID& moduleId ) const ;
  
  /// Decode all gol headers
  StatusCode decodeGolHeaders() const ;
  
  /// Decode all modules
  StatusCode decode( LHCb::OTLiteTimeContainer& ottimes ) const ;
  
  /// Release a tool of this type. Necessary because of a bug in
  /// GaudiTools, which will be fixed soon.
  virtual long unsigned int release() { return AlgTool::release() ; }
  virtual StatusCode release( const IInterface* interface) const { return GaudiCommon<AlgTool>::release(interface) ; }
  
private:
  virtual void handle ( const Incident& incident );
  size_t decodeModule( OTRawBankDecoderHelpers::Module& ) const ;
  
private:
  // data
  std::vector<double> m_startReadOutGate;   ///< Start of readout gate
  int  m_countsPerBX;                       ///< Counts per BX
  int  m_numberOfBX;                        ///< Number of BX
  double m_timePerBX;                       ///< Time Per BX
  DeOTDetector* m_tracker;                  ///< Pointer to XML geometry
  double m_nsPerTdcCount ;                  ///< Conversion from tdc to ns
  
  mutable OTRawBankDecoderHelpers::Detector* m_detectordata ; ///< Contains decoded data
};

#endif // OTRAWBANKDECODER_H
