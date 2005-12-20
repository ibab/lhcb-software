// $Id: RawEventTestCreator.cpp,v 1.1 2005-12-20 16:38:18 frankb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

#include "Event/RawBank.h"
#include "Event/RawEvent.h"

using LHCb::RawBank;
using LHCb::RawEvent;

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
    RawEvent* raw = new RawEvent();
    for(i=0; i<16; ++i)  {
      RawBank* bank = raw->createBank(i, RawBank::DAQ, 1, (i+1)*64, 0);
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
    return eventSvc()->registerObject(LHCb::RawEventLocation::Default, raw);
  }
};

// Declaration of the Algorithm Factory
static const  AlgFactory<RawEventTestCreator>          s_factory ;
const        IAlgFactory& RawEventTestCreatorFactory = s_factory ; 
