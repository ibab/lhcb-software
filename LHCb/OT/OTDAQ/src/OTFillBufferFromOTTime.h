// $Id: OTFillBufferFromOTTime.h,v 1.2 2004-11-10 13:02:08 jnardull Exp $
#ifndef OTDAQ_OTFILLBUFFERFROMOTTIME_H 
#define OTDAQ_OTFILLBUFFERFROMOTTIME_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from Detector
#include "OTDet/DeOTDetector.h"

//local
#include "Event/GolHeader.h"
#include "Event/DataWord.h"

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Kernel/OTChannelID.h"
#include "Event/OTTime.h"
#include "Event/RawBuffer.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/DAQTypes.h"

/** @class OTFillBufferFromOTTime OTFillBufferFromOTTime.h 
 * OTDAQ/OTFillBufferFromOTTime.h
 *  
 *
 *  @author Jacopo Nardulli
 *  @date   2004-08-31
 */

class OTFillBufferFromOTTime : public GaudiAlgorithm 
{
  

public: 

  /// Standard constructor

  OTFillBufferFromOTTime( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTFillBufferFromOTTime( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:  
  
  std::string m_RawBuffLoc;
  std::string m_OTTimeLoc;
  std::string m_otTrackerPath;

  typedef std::vector<OTTime*> vOTime;
  typedef std::map<int,vOTime*> mBank;// contains the bank vectors*
  typedef std::map<int,vOTime*> mGol;// contains the Gol vectors*
  typedef std::vector<raw_int> dataBank;
  typedef std::vector<dataBank*> dataBuffer;

  // detector geometry
  DeOTDetector* m_otTracker;
  RawBuffer* m_rawBuffer;
  
  // global pointer to vectors container
  OTTimes* m_Time;
  mBank* dataContainer;
  mGol* goldatacontainer;
  dataBuffer* finalBuf;
  dataBank* aBank;

  int numberOfBanks;
  int numberOfGols;
  
  // converts channel ID number into bank number for sorting
  int chID2int(OTChannelID otChannel); // int is bank ID
  int chID2Otis(OTChannelID otChannel);
 
  // sort OTTimes into banks
  StatusCode sortTimesIntoBanks();
  // sort OTTimes into GOL
  StatusCode sortTimesIntoGol(vOTime* BankOTime, dataBank* aBank);
  //Converting in raw_int format
  StatusCode convertToRAWDataBank(vOTime* vToConvert, dataBank* aBank);
  StatusCode convertToRAWEmptyBank(dataBank* aBank);

};
#endif // OTDAQ_OTFILLBUFFERFROMOTTIME_H
