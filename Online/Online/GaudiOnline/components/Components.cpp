#include "GaudiKernel/Algorithm.h"

//
#include "GaudiOnline/AMSService.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,AMSService)
#include "GaudiOnline/Class0Task.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class0Task)
#include "GaudiOnline/Class1Task.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class1Task)
#include "GaudiOnline/Class2Task.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,Class2Task)
namespace LHCb { typedef GaudiTask::Configuration GaudiTaskConfiguration;  }
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,GaudiTaskConfiguration)
#include "GaudiOnline/MEPOverflowWriterSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPOverflowWriterSvc)
#include "GaudiOnline/MBMCnvSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MBMCnvSvc)
#include "GaudiOnline/NetworkEvtSelector.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,NetworkEvtSelector)
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
#include "GaudiOnline/LazyRunable.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,LazyRunable)
#include "GaudiOnline/MDFReceiver.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFReceiver)
#include "GaudiOnline/MDFSender.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFSender)
#include "GaudiOnline/MemMonitorSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MemMonitorSvc)
#include "GaudiOnline/GaudiTask.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiTask)
#include "GaudiOnline/OnlineRunable.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,OnlineRunable)
#include "GaudiOnline/PanoramixSim.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,PanoramixSim)
#include "GaudiOnline/GaudiDeamon.h"
DECLARE_NAMESPACE_OBJECT_FACTORY(LHCb,GaudiDeamon)
#include "GaudiOnline/FileWriteMgr.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FileWriteMgr)
#include "GaudiOnline/FileWriterSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FileWriterSvc)

