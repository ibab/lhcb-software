// $Id: OTFillRawBuffer.h,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $
#ifndef OTSIMULATION_OTFILLRAWBUFFER_H 
#define OTSIMULATION_OTFILLRAWBUFFER_H 1

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
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// Event
#include "Kernel/OTChannelID.h"
#include "Event/MCOTTime.h"
#include "Event/RawBuffer.h"
#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Event/DAQTypes.h"

/** @class OTFillRawBuffer OTFillRawBuffer.h OTSimulation/OTFillRawBuffer.h
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

protected:

private:  
  
  std::string m_RawBuffLoc;
  std::string m_MCOTTimeLoc;
  std::string m_otTrackerPath;

  typedef std::vector<MCOTTime*> vmcOTime;
  typedef std::map<int,vmcOTime*> mBank;// contains the bank vectors*
  typedef std::map<int,vmcOTime*> mGol;// contains the Gol vectors*
  typedef std::vector<raw_int> dataBank;
  typedef std::vector<dataBank*> dataBuffer;

  // detector geometry
  DeOTDetector* m_otTracker;
  RawBuffer* m_rawBuffer;
  
  // global pointer to vectors container
  MCOTTimes* m_mcTime;
  mBank* dataContainer;
  mGol* goldatacontainer;
  dataBuffer* finalBuf;
  dataBank* aBank;

  int numberOfBanks;
  int numberOfGols;
  
  // converts channel ID number into bank number for sorting
  int chID2int(OTChannelID otChannel); // int is bank ID
  int chID2Otis(OTChannelID otChannel);
 
  // sort MCOTTimes into banks
  StatusCode sortMcTimesIntoBanks();
  // sort MCOTTimes into GOL
  StatusCode sortMcTimesIntoGol(vmcOTime* BankmcOTime, dataBank* aBank);
  //Converting in raw_int format
  StatusCode convertToRAWDataBank(vmcOTime* vToConvert, dataBank* aBank);
  StatusCode convertToRAWEmptyBank(dataBank* aBank);

};
#endif // OTSIMULATION_OTFILLRAWBUFFER_H
