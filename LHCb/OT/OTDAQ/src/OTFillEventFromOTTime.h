// $Id: OTFillEventFromOTTime.h,v 1.3 2007-11-26 11:08:30 wouter Exp $
#ifndef OTDAQ_OTFILLEVENTFROMOTTIME_H 
#define OTDAQ_OTFILLEVENTFROMOTTIME_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

//local
#include "GolHeaderDC06.h"
#include "DataWord.h"

namespace LHCb 
{
  class RawEvent;
  class OTChannelID;
  class OTTime;
  typedef OTDAQ::GolHeaderDC06 GolHeader ;
}

  
/** @class OTFillEventFromOTTime OTFillEventFromOTTime.h 
 * OTDAQ/OTFillEventFromOTTime.h
 *  
 *
 *  @author Jacopo Nardulli
 *  @date   2004-08-31
 */

class OTFillEventFromOTTime : public GaudiAlgorithm 
{
  

public: 

  /// Standard constructor

  OTFillEventFromOTTime( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~OTFillEventFromOTTime( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  
  typedef std::vector<LHCb::OTTime*> vOTime;
  typedef std::map<int,vOTime*> mBank;// contains the bank vectors*
  typedef std::map<int,vOTime*> mGol;// contains the Gol vectors*
  typedef std::vector<unsigned int> dataBank;
  typedef std::vector<dataBank*> dataBuffer;

  // global pointer to vectors container
  LHCb::OTTimes* m_time;
  mBank* dataContainer;
  mGol* goldatacontainer;
  dataBuffer* finalBuf;
  dataBank* aBank;

  int numberOfBanks;
  int numberOfGols;
  
  // converts channel ID number into bank number for sorting
  int chID2int(LHCb::OTChannelID otChannel); // int is bank ID
  int chID2Otis(LHCb::OTChannelID otChannel);
 
  // sort OTTimes into banks
  StatusCode sortTimesIntoBanks();
  // sort OTTimes into GOL
  StatusCode sortTimesIntoGol(vOTime* BankOTime, dataBank* aBank);
  //Converting in raw_int format
  StatusCode convertToRAWDataBank(vOTime* vToConvert, dataBank* aBank);
  StatusCode convertToRAWEmptyBank(dataBank* aBank);

};
#endif // OTDAQ_OTFILLEVENTFROMOTTIME_H
