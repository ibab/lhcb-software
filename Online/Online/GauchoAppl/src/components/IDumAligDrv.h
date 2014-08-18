#ifndef ONLINE_GAUCHO_IDUMALIGDRV_H
#define ONLINE_GAUCHO_IDUMALIGDRV_H

#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IIncidentSvc.h"

//#include "Gaucho/BRTL_Lock.h"

// Forward declarations
//class DimService;
//class ISimpleTrendWriter;
namespace LHCb
{
  class IDumAligDrv: virtual public IRunable, virtual public IService
  {
    public:
      DeclareInterfaceID(IDumAligDrv,1,0);
      virtual IGauchoMonitorSvc *getMonSvc()=0;
      virtual void writeReference()=0;
      virtual void waitRunOnce()=0;
      virtual void setRunOnce()=0;
      virtual IIncidentSvc *incidentSvc()=0;
      IFitter *Fitter();
  };
}
#endif // ONLINE_GAUCHO_IDUMALIGDRV_H
