// $Id: OTTimeCreator.h,v 1.13 2008-12-17 08:35:18 cattanem Exp $
#ifndef OTDAQ_OTTIMECREATOR_H 
#define OTDAQ_OTTIMECREATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Kernel
#include "Kernel/OTChannelID.h"

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

  virtual ~OTTimeCreator( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();
  virtual StatusCode execute   ();    ///< Algorithm execution
 
protected:

private:
  
  // job options
  std::string m_timeLocation;
  bool m_tofCorrection;                     ///< Time of Flight Correction
  DeOTDetector* m_tracker;                  ///< Pointer to XML geometry 
  ToolHandle<IOTRawBankDecoder> m_decoder;
};

#endif // OTDAQ_OTTIMECREATOR_H
