// $Id: OTTimeCreator.h,v 1.12 2007-10-05 11:54:39 cattanem Exp $
#ifndef OTDAQ_OTTIMECREATOR_H 
#define OTDAQ_OTTIMECREATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Kernel
#include "Kernel/OTChannelID.h"

// forward declarations
class IOTRawBankDecoder;
class DeOTDetector;

/** @class OTTimeCreator OTTimeCreator.h OTDAQ/OTTimeCreator.h
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
  virtual StatusCode execute   ();    ///< Algorithm execution
 
protected:

private:
  
  // job options
  std::string m_timeLocation;
  bool m_tofCorrection;                     ///< Time of Flight Correction
  DeOTDetector* m_tracker;                  ///< Pointer to XML geometry 
  IOTRawBankDecoder* m_rawBankDecoder ;     ///< Pointer to OTRawBankDecoder tool
};

#endif // OTDAQ_OTTIMECREATOR_H
