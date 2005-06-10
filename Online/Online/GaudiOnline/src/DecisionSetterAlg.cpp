// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/DecisionSetterAlg.cpp,v 1.4 2005-06-10 10:08:44 frankb Exp $
//	====================================================================
//  DecisionSetterAlg.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiOnline/OnlineAddress.h"
#include "GaudiOnline/StreamDescriptor.h"
#include "Event/L1Score.h"
#include "Event/L1Event.h"

using GaudiOnline::StreamDescriptor;

/**@class DecisionSetterAlg 
  *
  *
  * @author:  M.Frank
  * @version: 1.0
  */
class DecisionSetterAlg : public Algorithm   {
public:
  /// Standard algorithm constructor
  DecisionSetterAlg(const std::string& name, ISvcLocator* pSvcLocator)
  :	Algorithm(name, pSvcLocator) {                                }
  /// Standard Destructor
  virtual ~DecisionSetterAlg()      {                                 }
  /// Initialize
  virtual StatusCode initialize()   {    return StatusCode::SUCCESS;  }
  /// Finalize
  virtual StatusCode finalize()     {    return StatusCode::SUCCESS;  }
  /// Execute procedure
  virtual StatusCode execute()    {
    SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
    if ( evt )  {
      IRegistry* reg = evt->registry();
      if ( reg )   {
        GaudiOnline::Address* addr = dynamic_cast<GaudiOnline::Address*>(reg->address());
        const GaudiOnline::StreamDescriptor* dsc = addr->descriptor();
        int data[8];     // 8 words of L1 decision data see: LHCb 2003-80

        memset(data,0,sizeof(data));
        if ( dsc )  {
          switch( dsc->type() ) {
          case GaudiOnline::StreamDescriptor::L1_BUFFER:
	    {
              SmartDataPtr<L1Score> l1_score(eventSvc(),L1ScoreLocation::Default);
              L1Score* score = l1_score;
              const double ushort_max = 327670.;
              if ( score ) {
                int l1_decision = score->decisionPattern();
                // Values according to T.Schietinger (10/06/2005)
                l1_int pt1 = static_cast<l1_int>(std::max(ushort_max,score->pt1()));
                l1_int pt2 = static_cast<l1_int>(std::max(ushort_max,score->pt2()));
                l1_int ptm = static_cast<l1_int>(std::max(ushort_max,score->ptMu()));
                l1_int dmm = static_cast<l1_int>(std::max(ushort_max,score->diMuMass()));
                l1_int dmi = static_cast<l1_int>(std::max(ushort_max,score->diMuIP()));
                l1_int ete = static_cast<l1_int>(std::max(ushort_max,score->etElec()));
                l1_int etp = static_cast<l1_int>(std::max(ushort_max,score->etPhot()));
                data[0] = score->eventNumber();
                data[1] = ((l1_decision&0xFFFFFF)<<8) + (l1_decision&0x1);
                data[2] = l1_decision;
                data[3] = static_cast<int>((pt1 << 16)  + pt2);
                data[4] = static_cast<int>(ptm << 16);
                data[5] = static_cast<int>((dmm << 16) + dmi);
                data[6] = static_cast<int>((ete << 16) + etp);
                data[7] = 0;
                return dsc->setDecision(&data[0], sizeof(data));
	      }
	      MsgStream err(msgSvc(),name());
              err << MSG::ERROR << "No L1Score result available." << endmsg;
              return StatusCode::FAILURE;              
	    }
            break;
          case GaudiOnline::StreamDescriptor::DAQ_BUFFER:
	    {
              SmartDataPtr<L1Score> l1_score(eventSvc(),L1ScoreLocation::Default);
              L1Score* score = l1_score;
              if ( score ) {
                // Values according to T.Schietinger (10/06/2005)
                int hlt_decision = 1;
                data[0] = score->eventNumber();
                data[1] = ((~0x0)<<8) + (hlt_decision&0xFF);
                return dsc->setDecision(&data[0], sizeof(data));
	      }
	    }
            break;
          }
        }
      }
    }
    return StatusCode::FAILURE;
  }
};

static const AlgFactory<DecisionSetterAlg>  Factory;
const IFactory& DecisionSetterAlgFactory = Factory;
