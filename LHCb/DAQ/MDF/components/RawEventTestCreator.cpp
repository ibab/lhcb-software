// $Id: RawEventTestCreator.cpp,v 1.1 2009-02-06 09:37:57 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "MDF/RawEventHelpers.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

#include <cstring> // for memset with gcc 4.3

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

 /** @class RawEventTestCreator RawEventTestCreator.cpp
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class RawEventTestCreator : public Algorithm {
    /// Flag to test bank removal
    bool m_removeBank;
  public: 
    /// Standard constructor
    RawEventTestCreator( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator), m_removeBank(false) {
      declareProperty("RemoveBank",m_removeBank);
    }

    /// Destructor
    virtual ~RawEventTestCreator()  {} 

    /// Main execution
    virtual StatusCode execute()  {
      int i, cnt, *p;
      static int trNumber = -1;
      unsigned int trMask[] = {~0u,~0u,~0u,~0u};
      ++trNumber;
      unsigned int run_no = 1 + trNumber/10000;
      RawEvent* raw = new RawEvent();
      RawBank*  bank = 0;
      for(i=0; i<16; ++i)  {
        bank = raw->createBank(i, RawBank::Rich, 1, (i+1)*64, 0);
        for(p=bank->begin<int>(), cnt=0; p != bank->end<int>(); ++p)  {
          *p = cnt++;
        }
        raw->adoptBank(bank, true);
      }
      if ( m_removeBank ) raw->removeBank(bank);
      for(i=0; i<9; ++i)  {
        bank = raw->createBank(i, RawBank::PrsE, 1, (i+1)*32, 0);
        for(p=bank->begin<int>(), cnt=0; p != bank->end<int>(); ++p)  {
          *p = cnt++;
        }
        raw->adoptBank(bank, true);
      }

      bank = raw->createBank(0, RawBank::ODIN, 2, sizeof(OnlineRunInfo), 0);
      OnlineRunInfo* run = bank->begin<OnlineRunInfo>();
      ::memset(run,0,sizeof(OnlineRunInfo));
      run->Run     = 123;
      run->Orbit   = trNumber/10;
      run->L0ID    = trNumber;
      run->bunchID = trNumber%100;
      run->triggerType = 0;
      raw->adoptBank(bank, true);

      // raw->removeBank(bank);
      size_t len = rawEventLength(raw);
      RawBank* hdrBank = raw->createBank(0, RawBank::DAQ, DAQ_STATUS_BANK, sizeof(MDFHeader)+sizeof(MDFHeader::Header1), 0);
      MDFHeader* hdr = (MDFHeader*)hdrBank->data();
      hdr->setChecksum(0);
      hdr->setCompression(0);
      hdr->setHeaderVersion(3);
      hdr->setSpare(0);
      hdr->setDataType(MDFHeader::BODY_TYPE_BANKS);
      hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
      hdr->setSize(len);
      MDFHeader::SubHeader h = hdr->subHeader();
      h.H1->setTriggerMask(trMask);
      h.H1->setRunNumber(run_no);
      h.H1->setOrbitNumber(trNumber/10);
      h.H1->setBunchID(trNumber%100);
      raw->adoptBank(hdrBank, true);
      return eventSvc()->registerObject("/Event/DAQ/RawEvent",raw);
    }
  };
}

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventTestCreator)
