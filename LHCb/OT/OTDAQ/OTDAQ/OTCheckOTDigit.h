// $Id: OTCheckOTDigit.h,v 1.1.1.1 2004-02-03 09:49:17 jnardull Exp $
#ifndef OTDAQ_OTCHECKOTDIGIT_H 
#define OTDAQ_OTCHECKOTDIGIT_H 1

// Include files
// from STL
#include <string>

// from Detector
#include "OTDet/DeOTDetector.h"
//from OTAlgorithms
#include "OTAlgorithms/OTDataFunctor.h"

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// Event
#include "Kernel/OTChannelID.h"
#include "Event/OTDigit.h"
#include "Event/RawBuffer.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/DAQTypes.h"

/** @class OTCheckOTDigit OTCheckOTDigit.h OTDAQ/OTCheckOTDigit.h
 *  
 *
 *  @author Jacopo Nardulli & Bart Hommels
 *  @date   2004-01-30
 */
class OTCheckOTDigit : public Algorithm {
public: 
  /// Standard constructor
  OTCheckOTDigit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTCheckOTDigit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  // data members
  typedef std::vector<int> vInt;// tdc times
  OTDigits* m_otdigit;
  OTDigits* m_newotdigit;
  std::string m_Container2;      ///< Name of 2nd input container
  std::string m_OTDigitLoc;
  
};
#endif // OTDAQ_OTCHECKOTDIGIT_H
