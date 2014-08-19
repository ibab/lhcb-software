#ifndef ONLINE_GAUCHO_IDUMALIGWORK_H
#define ONLINE_GAUCHO_IDUMALIGWORK_H

#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IInterface.h"


// Forward declarations
namespace LHCb
{
  class IDumAligWork: virtual public IInterface
  {
    public:
      DeclareInterfaceID(IDumAligWork,1,0);
      virtual IGauchoMonitorSvc *getMonSvc()=0;
      virtual void readReference()=0;
  };
}
#endif // ONLINE_GAUCHO_IDUMALIGWORK_H
