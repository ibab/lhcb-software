// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/DecisionSetterAlg.cpp,v 1.1 2005-04-19 15:27:26 frankb Exp $
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
  :	Algorithm(name, pSvcLocator)    {                                 }
  /// Standard Destructor
  virtual ~DecisionSetterAlg()        {                                 }
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
        if ( dsc )  {
          return dsc->setDecision(1);
        }
      }
    }
    return StatusCode::FAILURE;
  }
};

static const AlgFactory<DecisionSetterAlg>  Factory;
const IFactory& DecisionSetterAlgFactory = Factory;
