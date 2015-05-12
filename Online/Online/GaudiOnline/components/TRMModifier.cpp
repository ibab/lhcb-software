// $Header:  $
//  ====================================================================
//  TRMModifier.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: $
// Framework include files
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "MDF/OnlineRunInfo.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "RTL/rtl.h"
#include "MBM/Requirement.h"
#include "MBM/bmdef.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MDFHeader.h"

// C/C++ include files
#include <cstdlib>
#include <vector>
#include <ctime>

/*
 *  LHCb namespace declaration
 */
namespace LHCb
{
  using namespace std;
  /**@class TRMModifier TRMModifier.cpp
  *
  * Modifies the 4th word of the trigger mask in the MDF header by the option.
  *
  *
  * @author:  B. Jost
  * @version: 1.0
  */
  class GAUDI_API TRMModifier: public Algorithm
  {

    public:
      /// Standard Algorithm Constructor(s)
      unsigned int m_4thWord;
      TRMModifier(const std::string& nam, ISvcLocator* svc) :
          Algorithm(nam, svc)
      {
        declareProperty("New4thWord", m_4thWord,
            "Event's 4th word replacement");
      }

      /// Default Destructor
      virtual ~TRMModifier()
      {
      }

      /// Algorithm overload: Initialize the algorithm
      virtual StatusCode initialize()
      {
        Algorithm::initialize();
        return StatusCode::SUCCESS;
      }

      /// Algorithm overload: Finalize the algorithm
      virtual StatusCode finalize()
      {
        return StatusCode::SUCCESS;
      }

      /// Algorithm overload: Event execution routine
      virtual StatusCode execute()
      {
//        MsgStream log(msgSvc(), name());
        SmartDataPtr<RawEvent> rawEvt(eventSvc(),
            LHCb::RawEventLocation::Default);
        typedef std::vector<RawBank*> _V;
        const _V& bnks = rawEvt->banks(RawBank::DAQ);
        RawBank* b = 0;
        for (auto i = bnks.begin(); i != bnks.end(); ++i)
        {
          if ((*i)->version() == DAQ_STATUS_BANK)
          {
            b = *i;
            rawEvt->removeBank(b);
            RawBank *newb = rawEvt->createBank(b->sourceID(), b->type(),b->version(), b->size(), b->data());
            rawEvt->adoptBank(newb, true);
            MDFHeader *MDFHd = (MDFHeader*) newb->data();
            unsigned int *trm = (unsigned int*) (MDFHd->subHeader().H1->triggerMask());
            trm[3] = m_4thWord;
            return StatusCode::SUCCESS;
          }
        }
        return StatusCode::FAILURE;
      }
  };
}

#include "GaudiKernel/AlgFactory.h"

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb, TRMModifier)
