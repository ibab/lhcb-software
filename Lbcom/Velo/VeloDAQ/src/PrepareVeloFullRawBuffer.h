#ifndef PREPAREVELOFULLRAWBUFFER_H 
#define PREPAREVELOFULLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// STL
#include <vector>
#include <map>
#include <utility>

// data model
#include "Kernel/VeloChannelID.h"
#include "Tell1Kernel/VeloDecodeCore.h"
#include "Tell1Kernel/VeloDecodeConf.h"
#include "Tell1Kernel/VeloTell1Core.h"
#include "VeloEvent/VeloFullBank.h"

#include "Event/RawBank.h"

/** @class PrepareVeloFullRawBuffer PrepareVeloFullRawBuffer.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-22
 *
 *  Algorithm dedicated to decoding the new 1MHz non-zero suppresed
 *  RawBank which is sent out by TELL1 pre-processing board.
 *  The input for the algorithm is binary file and the output 
 *  is container with VeloDigits objects.
 *  The container is the input for the TELL1
 *  algorithms (pedestal subtraction and following, FIR filter nad
 *  linear CM subtraction)
 */

namespace LHCb
{
  class RawEvent;
}

using namespace VeloTELL1;
using namespace LHCb;

class PrepareVeloFullRawBuffer : public GaudiTupleAlg {
public: 

  typedef std::map<unsigned int, std::pair<unsigned int, unsigned int* > > DATA_REPO;
  
  enum parameters{
    ERROR_HEADER_1=1,
    ERROR_HEADER_2=2,
    ERROR_SECOND_SECTION=3,
    ADCHeadersShift=4,
    ERROR_FIRST_SECTION=5,
    ERROR_EVT_INFO=8,
    ODIN_DATA_WORDS=9,
    ERROR_THIRD_SECTION=10,
    ALinkShift=36
  };
  enum bitMasks{
    ADCMask=0x000003ff
  };  

  enum bitShifts{
    ADCShift=10
  };  

  enum BANKS_SIZES{
    WORD2BYTE=4,
    EB_FPGAx1=6,
    COMPLETE_EB=52,
    FPGAx1=224,
    FPGAx4=896
  };

  //
  /// Standard constructor
  PrepareVeloFullRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareVeloFullRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

  // fetch the RawEvent
  StatusCode getRawEvent();
  // extract vector of the RawBanks of the type VeloFull (non-zero supp.)
  StatusCode getRawBanks();
  void createOrderedSections();
  // write-out the Gaudi model of the RawBank (with digit objects,
  // header infos and Beetle headers)
  StatusCode writeVeloFull();
  // run after decoding each event
  void resetMemory();
  // use only as emergency checker!
  void dumpADCs(const dataVec& inADCs);
  // find object function
  bool findObject(const dataVec& inCont, const unsigned int obj) const;

protected:

  void setADCBankFlag();
  void setPedBankFlag();
  void unsetADCBankFlag();
  void unsetPedBankFlag();
  bool adcBankFlag();
  bool pedBankFlag();
  void setDecodeWithErrorBank();
  bool getDecodeWithErrorBank() const;

private:

  LHCb::RawEvent* m_rawEvent;          /// pointer to RawEvent container
  std::string m_rawEventLoc;           /// location of RawEvent container
  DATA_REPO m_fullData2Decode;         /// source id and ptr to the bank body - full
  DATA_REPO m_partialData2Decode;      /// source id and ptr to the bank body - partial
  DATA_REPO m_pedestalData2Decode;     /// src id and ptr to the bank body - pedestals
  DATA_REPO m_errorBanks2Check;        /// the same for the error banks
  VeloFullBanks* m_veloADCData;        /// container to store ordered data
  VeloFullBanks* m_veloADCPartialData;
  VeloFullBanks* m_veloPedestals;      /// container to store ordered pedestals
  std::string m_veloADCDataLoc;        /// TES locations of decoded data
  std::string m_veloPedestalsLoc;
  std::string m_veloADCPartialDataLoc;
  bool m_adcBankPresent;               /// flag to check if data is sent out
  bool m_pedBankPresent;               /// flag to check if ped is sent out
  bool m_runWithODIN;
  bool m_isDebug;
  bool m_roundRobin;  
  bool m_ignoreErrorBanks;
  
};
#endif // DECODEVELOFULLRAWBUFFER_H

struct errorBankFinder{

  errorBankFinder(unsigned int tell1): 
    currentTell1 (tell1) 
  { }

  bool operator()(LHCb::RawBank* aBank) const{ 
    return (aBank->sourceID())==static_cast<int>(currentTell1);
  }

private:

  unsigned int currentTell1;

};
