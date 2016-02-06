#ifndef OTDAQ_OTTIMECREATOR_H 
#define OTDAQ_OTTIMECREATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Kernel
#include "GaudiKernel/ToolHandle.h"
#include "OTDAQ/IOTRawBankDecoder.h"

// forward declarations
class IOTRawBankDecoder;
class DeOTDetector;

/** @class OTTimeCreator OTTimeCreator.h
 *  
 *
 *  @author Jacopo Nardulli
 *  @date   2004-06-06
 */

class OTTimeCreator : public GaudiAlgorithm {
public: 

  /// Standard constructor
  OTTimeCreator( const std::string& name, ISvcLocator* pSvcLocator );

  ~OTTimeCreator( ) override; ///< Destructor
  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode finalize() override;
  StatusCode execute() override;    ///< Algorithm execution
 
private:
  
  // job options
  std::string m_timeLocation;
  DeOTDetector* m_tracker = nullptr;                  ///< Pointer to XML geometry 
  ToolHandle<IOTRawBankDecoder> m_decoder = { "OTRawBankDecoder" };
};

#endif // OTDAQ_OTTIMECREATOR_H
