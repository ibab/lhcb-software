// $Id: OTFillRawBuffer.h,v 1.2 2004-03-25 15:53:37 jnardull Exp $
#ifndef OTDAQ_OTFILLRAWBUFFER_H 
#define OTDAQ_OTFILLRAWBUFFER_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from Detector
#include "OTDet/DeOTDetector.h"


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

#include "OTDAQ/OTRetrieveBuffer.h"

/** @class OTFillRawBuffer OTFillRawBuffer.h OTDAQ/OTFillRawBuffer.h
 *  
 *
 *  @author Jacopo Nardulli & Bart Hommels
 *  @date   2004-01-09
 */
class OTFillRawBuffer : public Algorithm 
{
  

public: 

  /// Standard constructor
  OTFillRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTFillRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  friend class OTRetrieveBuffer;// uses the typedefs and the member functions(?)
  

protected:

private:  
  
  std::string m_RawBuffLoc;
  std::string m_OTDigitLoc;
  std::string m_otTrackerPath;

  typedef std::vector<OTDigit*> vDigi;
  typedef std::vector<int> vInt;// tdc times
  typedef std::map<int,vDigi*> mBank;// contains the bank vectors*
  typedef std::map<int,vDigi*> mGol;// contains the Gol vectors*
  typedef std::vector<raw_int> dataBank;
  typedef std::vector<dataBank*> dataBuffer;

  // detector geometry
  DeOTDetector* m_otTracker;
  RawBuffer* m_rawBuffer;
  
  // global pointer to vectors container
  OTDigits* m_otdigit;
  mBank* dataContainer;
  mGol* goldatacontainer;
  dataBuffer* finalBuf;
  dataBank* aBank;

  int numberOfBanks;
  int numberOfGols;
  
 //Masks
  enum GOLIDMasks {StationIDMask = 0x60000000,
                   LayerIDMask = 0x18000000,
                   QuarterIDMask = 0x06000000,
                   ModuleIDMask = 0x01e00000,
                   golSizeMask = 0x0000efff };
  enum datawordMasks {NextTimeMask =  0x000000ff, 
                      NextChannelMask = 0x00001f00, 
                      NextOtisMask = 0x00006000,
                      FirstTimeMask =  0x00ff0000, 
                      FirstChannelMask = 0x1f000000, 
                      FirstOtisMask = 0x60000000};

  // converts channel ID number into bank number for sorting
  int chID2int(OTChannelID otChannel); // int is bank ID
  int chID2Otis(OTChannelID otChannel);
 
  // sort OTDigits into banks
  StatusCode sortDigitsIntoBanks();
  // sort OTDigits into GOL
  StatusCode sortDigitsIntoGol(vDigi* BankDigi, dataBank* aBank);
  //Converting in raw_int format
  StatusCode convertToRAWDataBank(vDigi* vToConvert, dataBank* aBank);
  //Decoding Stuff
  StatusCode rawInt2Decode(raw_int dataWord);
  //To create the Gol Header
  raw_int createGolHeader(long nStation, long nLayer, long nQuarter,
                          long nModule, int size);
  //To create the OTIS Header
  raw_int createOtisHeader(long nStation, long nLayer, long nQuarter,
                           long nModule ,int otisID);

};
#endif // OTDAQ_OTFILLRAWBUFFER_H
