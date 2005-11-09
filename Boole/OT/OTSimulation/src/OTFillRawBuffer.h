// $Id: OTFillRawBuffer.h,v 1.5 2005-11-09 16:52:25 jnardull Exp $
#ifndef OTSIMULATION_OTFILLRAWBUFFER_H 
#define OTSIMULATION_OTFILLRAWBUFFER_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Kernel/OTChannelID.h"
#include "Event/MCOTTime.h"
#include "Event/RawEvent.h"

/** @class OTFillRawBuffer OTFillRawBuffer.h OTSimulation/OTFillRawBuffer.h
 *  
 *
 *  @author Jacopo Nardulli & Bart Hommels
 *  @date   2004-01-09
 */

class OTFillRawBuffer : public GaudiAlgorithm 
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
  
  typedef std::vector<MCOTTime*> vmcOTime;
  typedef std::map<int,vmcOTime*> mBank;// contains the bank vectors*
  typedef std::map<int,vmcOTime*> mGol;// contains the Gol vectors*
  typedef std::vector<unsigned int> dataBank;
  typedef std::vector<dataBank*> dataBuffer;

  // global vectors containers
  mBank m_dataContainer;
  mGol  m_goldatacontainer;

  // Properties
  std::string m_EventLoc;
  std::string m_MCOTTimeLoc;
  std::string m_otTrackerPath;
  int m_numberOfBanks;
  int m_numberOfGols;
  
  // converts channel ID number into bank number for sorting
  int chID2int(OTChannelID otChannel); // int is bank ID
  int chID2Otis(OTChannelID otChannel);
 
  // sort MCOTTimes into banks
  StatusCode sortMcTimesIntoBanks();
  // sort MCOTTimes into GOL
  StatusCode sortMcTimesIntoGol(vmcOTime* BankmcOTime);
  //Converting in raw_int format
  StatusCode convertToRAWDataBank(vmcOTime* vToConvert, dataBank* aBank);
  StatusCode convertToRAWEmptyBank(dataBank* aBank);

};
#endif // OTSIMULATION_OTFILLRAWBUFFER_H
