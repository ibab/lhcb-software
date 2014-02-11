#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/AlgFactory.h" 

//
#include "GaudiOnline/AMSService.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AMSService)
#include "GaudiOnline/Class0Task.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class0Task)
#include "GaudiOnline/Class1Task.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class1Task)
#include "GaudiOnline/Class2Task.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class2Task)
#include "GaudiOnline/MEPOverflowWriterSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPOverflowWriterSvc)
#include "GaudiOnline/MBMCnvSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MBMCnvSvc)
#include "GaudiOnline/NetworkEvtSelector.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,NetworkEvtSelector)
#include "GaudiOnline/SLICEReaderSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,SLICEReaderSvc)
#include "GaudiOnline/MEPManager.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPManager)
#include "GaudiOnline/DimErrorLogger.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimErrorLogger)
#include "GaudiOnline/DimMessageSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimMessageSvc)
#include "GaudiOnline/DimRPCFileReader.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,DimRPCFileReader)
#include "GaudiOnline/DimTaskFSM.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,DimTaskFSM)
#include "GaudiOnline/EventRequestServerRunable.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,EventRequestServerRunable)
#include "GaudiOnline/EventRunable.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,EventRunable)
#include "GaudiOnline/EventServerRunable.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,EventServerRunable)
#include "GaudiOnline/FIDManipAlg.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,FIDManipAlg)
#include "GaudiOnline/FmcMessageSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FmcMessageSvc)
#include "GaudiOnline/HLTMonWriterSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(MonWriter,HLTMonWriterSvc)
#include "GaudiOnline/MDFReceiver.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFReceiver)
#include "GaudiOnline/MDFSender.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFSender)
#include "GaudiOnline/MemMonitorSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MemMonitorSvc)
#include "GaudiOnline/GaudiTask.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiTask)
#include "GaudiOnline/MEPRxSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, MEPRxSvc)
#include "GaudiOnline/OnlineRunable.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineRunable)
#include "GaudiOnline/PanoramixSim.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,PanoramixSim)
#include "GaudiOnline/GaudiDeamon.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiDeamon)
#include "GaudiOnline/MEPErrorAdder.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPErrorAdder)
