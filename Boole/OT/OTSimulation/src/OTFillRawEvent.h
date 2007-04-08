#ifndef OTSIMULATION_OTFILLRAWEVENT_H 
#define OTSIMULATION_OTFILLRAWEVENT_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// MCEvent
#include "Event/MCOTTime.h"

// Event
#include "Kernel/OTChannelID.h"

// OTDAQ
#include "Event/RawEvent.h"

/** @class OTFillRawEvent OTFillRawEvent.h OTSimulation/OTFillRawEvent.h
 *  
 *
 *  @author Jacopo Nardulli & Bart Hommels
 *  @date   2004-01-09
 */

class OTFillRawEvent : public GaudiAlgorithm 
{
public: 

  /// Standard constructor

  OTFillRawEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTFillRawEvent( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:  
  
  typedef std::vector<LHCb::MCOTTime*> VecMCOTTimes;
  typedef std::map<int, VecMCOTTimes*> OTBank;// contains the bank vectors*
  typedef std::map<int, VecMCOTTimes*> OTGol;// contains the Gol vectors*
  typedef std::vector<unsigned int> DataBank;

  // flat vectors
  OTBank m_dataContainer;
  OTGol  m_goldatacontainer;

  // Properties
  std::string m_EventLoc;
  std::string m_MCOTTimeLoc;
  std::string m_otTrackerPath;
  int m_numberOfBanks;
  int m_numberOfGols;
  
  // converts channel ID number into bank number for sorting
  int chID2int(const LHCb::OTChannelID& aChannel); // int is bank ID
  int chID2Otis(const LHCb::OTChannelID& aChannel);
 
  // sort MCOTTimes into banks
  void sortMcTimesIntoBanks();
  // sort MCOTTimes into GOL
  void sortMcTimesIntoGol(VecMCOTTimes* mcOTTimes);
  //Converting in raw_int format
  void convertToRAWDataBank(VecMCOTTimes* mcOTTimes, DataBank* aBank);
  void convertToRAWEmptyBank(DataBank* aBank);
};
#endif // OTSIMULATION_OTFILLRAWEVENT_H
