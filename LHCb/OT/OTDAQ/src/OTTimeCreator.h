// $Id: OTTimeCreator.h,v 1.11 2007-09-07 13:19:21 wouter Exp $
#ifndef OTDAQ_OTTIMECREATOR_H 
#define OTDAQ_OTTIMECREATOR_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// Kernel
#include "Kernel/OTChannelID.h"
#include "OTDAQ/OTRawBankDecoder.h"


// forward declarations
class DeOTDetector;
namespace LHCb 
{
  class OTTime;
}

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
  ToolHandle<OTRawBankDecoder> m_rawbankdecoder ;
};

#endif // OTDAQ_OTTIMECREATOR_H

