// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/DecisionSetterAlg.cpp,v 1.3 2005-05-04 17:14:09 frankb Exp $
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
            data[0] = 1;
            break;
          case GaudiOnline::StreamDescriptor::DAQ_BUFFER:
            data[0] = 1;
            break;
          }
          return dsc->setDecision(&data[0], sizeof(data));
        }
      }
    }
    return StatusCode::FAILURE;
  }
};

static const AlgFactory<DecisionSetterAlg>  Factory;
const IFactory& DecisionSetterAlgFactory = Factory;
