#ifndef ONLINE_GAUCHO_IDUMALIGDRV_H
#define ONLINE_GAUCHO_IDUMALIGDRV_H

#include "GaudiKernel/IInterface.h"


namespace LHCb
{
  class GAUDI_API IDumAligDrv: virtual public IInterface
  {
    public:
      DeclareInterfaceID(IDumAligDrv,1,0);
      virtual void writeReference()=0;
      virtual void waitRunOnce()=0;
      virtual void setRunOnce()=0;
      virtual void doContinue()=0;
      virtual void doStop()=0;
  };
}
#endif // ONLINE_GAUCHO_IDUMALIGDRV_H
