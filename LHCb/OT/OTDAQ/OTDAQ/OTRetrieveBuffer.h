// $Id: OTRetrieveBuffer.h,v 1.2 2004-03-25 15:53:37 jnardull Exp $
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
  
  enum GOLIDMasks {StationMask = 0x60000000, LayerMask = 0x18000000, 
                   QuarterMask = 0x06000000, ModuleMask = 0x01e00000,
                   SizeMask = 0x00007fff};

  enum DataMasks {dataMask1 = 0x80000000, dataMask2 = 0x00008000 };

  enum GollBuffMask {nextBuffMask = 0x7f000000, nextBuffMaskBis = 0x00007f00,
                     multiBuffMask = 0x0000007f};

  enum BuffyMask {timeBuffMask = 0x7f800000, firstBuffMask = 0x007f0000 };
  
  enum datawordMasks {NextTimeMask =  0x000000ff, NextChannelMask = 0x00001f00, 
                      NextOtisMask = 0x00006000, FirstTimeMask =  0x00ff0000, 
                      FirstChannelMask = 0x1f000000, 
                      FirstOtisMask = 0x60000000};

  StatusCode raw2OTDigit(int Station, int Layer, int Quarter, int Module, 
                         raw_int dataWord, OTDigits& vOTDs);
 
  int getStrawID(int otisID, int channel);
  int getStation(raw_int golHeader);
  int getLayer(raw_int golHeader);
  int getQuarter(raw_int golHeader);
  int getModule(raw_int golHeader);
  OTChannelID getChannelID( int station, int layer, int quarter, 
                            int module, int straw);

  //Useful Definitions for Multiple Hit Implementation 
  int firstChannelIDCopy ;
  int firstOtisIDCopy;
  int firstTimeCopy;
  int nextTimeCopy;
  int nextBuff;
  int nextBuffBis;
  int multiBuffer;
  int firstBuff;
  int firstTimeBuffer;
  int buffer;

  // job options
  std::string m_RawEventLoc;
  std::string m_digitLocation;

};

#endif // OTDAQ_OTRETRIEVEBUFFER_H
