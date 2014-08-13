#ifndef ONLINE_GAUCHO_IDUMALIGWORK_H
#define ONLINE_GAUCHO_IDUMALIGWORK_H

#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"

//#include "Gaucho/BRTL_Lock.h"

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class IDumAligWork: virtual public IRunable
  {
    public:
//      virtual ~IDumAligWork()=0;
      //IInterface pure virtual member functions
      virtual IGauchoMonitorSvc *getMonSvc()=0;
      virtual std::string getPartitionName()=0;
      virtual void readReference()=0;
      virtual void *getThis()=0;
  };
}
#endif // ONLINE_GAUCHO_IDUMALIGWORK_H
