// $Id: RawEventTestCreator.cpp,v 1.2 2006-06-26 08:37:18 frankb Exp $
// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

/*
 *    LHCb namespace declaration
 */
namespace LHCb  {

  /** @class RawEventTestCreator RawEventTestCreator.h tests/RawEventTestCreator.h
  *  Creates and fills dummy RawEvent  
  *
  *  @author Markus Frank
  *  @date   2005-10-13
  */
  class RawEventTestCreator : public Algorithm {
  public: 
    /// Standard constructor
    RawEventTestCreator( const std::string& name, ISvcLocator* pSvcLocator )
    : Algorithm(name, pSvcLocator) {}

    /// Destructor
    virtual ~RawEventTestCreator()  {} 

    /// Main execution
    virtual StatusCode execute()  {
      int i, cnt, *p;
      static int trNumber = 0;
      unsigned int trMask[] = {~0,~0,~0,~0};
      ++trNumber;
      unsigned int run_no = 1 + trNumber/1000;
      RawEvent* raw = new RawEvent();
      for(i=0; i<16; ++i)  {
        RawBank* bank = raw->createBank(i, RawBank::Rich, 1, (i+1)*64, 0);
        for(p=bank->begin<int>(), cnt=0; p != bank->end<int>(); ++p)  {
          *p = cnt++;
        }
        raw->adoptBank(bank, true);
      }
      for(i=0; i<9; ++i)  {
        RawBank* bank = raw->createBank(i, RawBank::PrsE, 1, (i+1)*32, 0);
        for(p=bank->begin<int>(), cnt=0; p != bank->end<int>(); ++p)  {
          *p = cnt++;
        }
        raw->adoptBank(bank, true);
      }
      size_t len = rawEventLength(raw);
      RawBank* hdrBank = raw->createBank(0, RawBank::DAQ, DAQ_STATUS_BANK, sizeof(MDFHeader)+sizeof(MDFHeader::Header1), 0);
      MDFHeader* hdr = (MDFHeader*)hdrBank->data();
      hdr->setSize(len);
      hdr->setChecksum(0);
      hdr->setCompression(0);
      hdr->setHeaderVersion(1);
      hdr->setSubheaderLength(sizeof(MDFHeader::Header1));
      MDFHeader::SubHeader h = hdr->subHeader();
      h.H1->setTriggerMask(trMask);
      h.H1->setRunNumber(run_no);
      h.H1->setOrbitNumber(trNumber/10);
      h.H1->setBunchID(trNumber%100);
      raw->adoptBank(hdrBank, true);
      return eventSvc()->registerObject(LHCb::RawEventLocation::Default, raw);
    }
  };
}

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,RawEventTestCreator)
