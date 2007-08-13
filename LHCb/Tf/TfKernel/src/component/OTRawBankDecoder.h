// $Id: OTRawBankDecoder.h,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
#ifndef DECODEOTTIMEONDEMAND_H 
#define DECODEOTTIMEONDEMAND_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//#include "OTDAQ/IDecodeOTTimeOnDemand.h"            // Interface
#include "GaudiKernel/ChronoEntity.h"
#include "GaudiKernel/IIncidentListener.h"

// Kernel

#include "TfKernel/IOTRawBankDecoder.h"

// forward declarations
class DeOTDetector;
class OTModuleMask;


namespace LHCb 
{
  class OTTime;
}



/** @class OTRawBankDecoder OTRawBankDecoder.h
 *  
 * 
 *  @author Johannes Albrecht
 *  @date   2007-04-12
 */

namespace Tf
{

  namespace OTRawBankDecoderHelpers
  {
    struct Detector ;
  }

  class OTRawBankDecoder : 
    public GaudiTool, 
    virtual public IOTRawBankDecoder,
    virtual public IIncidentListener  
  {
    
  public: 
    /// Standard constructor
    OTRawBankDecoder( const std::string& type, 
		      const std::string& name,
		      const IInterface* parent);
    
    virtual ~OTRawBankDecoder( ) ; ///< Destructor  
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual void decodeModule( const LHCb::OTChannelID& moduleId, LHCb::OTLiteTimeRange& ottimes ) const ;
    
  private:
    virtual void handle ( const Incident& incident );
    virtual StatusCode decodeGolHeaders() const ; 
    
    // job options
    std::vector<double> m_startReadOutGate;   ///< Start of readout gate
    int  m_countsPerBX;                       ///< Counts per BX
    int  m_numberOfBX;                        ///< Number of BX
    double m_timePerBX;                       ///< Time Per BX
    DeOTDetector* m_tracker;                  ///< Pointer to XML geometry
    double m_tdcConversion;
    
    mutable OTRawBankDecoderHelpers::Detector* m_detectordata ;
  };
  

}
#endif // DECODEOTTIMEONDEMAND_H
