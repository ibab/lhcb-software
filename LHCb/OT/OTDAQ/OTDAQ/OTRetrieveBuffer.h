// $Id: OTRetrieveBuffer.h,v 1.1.1.1 2004-02-03 09:49:17 jnardull Exp $
#ifndef OTDAQ_OTRETRIEVEBUFFER_H 
#define OTDAQ_OTRETRIEVEBUFFER_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

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

#include "OTDAQ/OTFillRawBuffer.h"



/** @class OTRetrieveBuffer OTRetrieveBuffer.h OTDAQ/OTRetrieveBuffer.h
 *  
 *
 *  @author Jacopo Nardulli & Bart Hommels
 *  @date   2004-01-26
 */
class OTRetrieveBuffer : public Algorithm {
public: 
  /// Standard constructor
  OTRetrieveBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTRetrieveBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
 
protected:

private:
  
  enum GOLIDMasks {golIDMask = 0x60000000, golSizeMask = 0x0000ffff };
  enum DataMasks {dataMask = 0x80000000 };  
  
  ///< Bitmasks for bitfield channelID
  enum datawordMasks {NextTimeMask =  0x000000ff, 
                      NextChannelMask = 0x00001f00, 
                      NextOtisMask = 0x00006000,
                      FirstTimeMask =  0x00ff0000, 
                      FirstChannelMask = 0x1f000000, 
                      FirstOtisMask = 0x60000000};

  StatusCode raw2OTDigit(int nbank, int GolID, 
                             raw_int dataWord, OTDigits& vOTDs);
  int raw2OTGolID(raw_int golHeader);

  OTChannelID getChannelID( int bank, int GolID, 
                            unsigned int OtisID, 
                            unsigned int bstraw );

  // job options
  std::string m_RawEventLoc;
  std::string m_digitLocation;

};

#endif // OTDAQ_OTRETRIEVEBUFFER_H
