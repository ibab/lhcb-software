#ifndef ONLINE_GAUCHO_IFITTER_H
#define ONLINE_GAUCHO_IFITTER_H
#include "GaudiKernel/IAlgTool.h"


namespace LHCb
{
  class IFitter : virtual public IAlgTool
  {
    public:
      DeclareInterfaceID(LHCb::IFitter,1,0);
      virtual StatusCode i_start()=0;
  };
}
#endif // ONLINE_GAUCHO_IFITTER_H
