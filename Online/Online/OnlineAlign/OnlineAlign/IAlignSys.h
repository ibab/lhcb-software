#ifndef ALIGNSYS_H
#define ALIGNSYS_H

//#include "Gaucho/IGauchoMonitorSvc.h"
#include "GaudiKernel/IRunable.h"
#include "GaudiKernel/IInterface.h"

class IGauchoMonitorSvc;
namespace LHCb
{
  class IAlignWork: virtual public IInterface
  {
    public:
      DeclareInterfaceID(IAlignWork,1,0);
      virtual IGauchoMonitorSvc *getMonSvc()=0;
      virtual void readReference()=0;
  };
  class GAUDI_API IAlignDrv: virtual public IInterface
  {
    public:
      DeclareInterfaceID(IAlignDrv,1,0);
      virtual void setReferenceBase(long)=0;
      virtual void writeReference()=0;
      virtual void waitRunOnce()=0;
      virtual void doContinue()=0;
      virtual void doStop()=0;
  };
}
#endif // ALIGNSYS_H
