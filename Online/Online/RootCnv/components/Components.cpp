#include "RootCnv/RootCnvSvc.h"
#include "RootCnv/RootEvtSelector.h"
#include "RootCnv/RootPerfMonSvc.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(6,0,0)
#include "GaudiKernel/SvcFactory.h"
#define DECLARE_COMPONENT(x) DECLARE_SERVICE_FACTORY(x)
#endif

using namespace Gaudi;
DECLARE_COMPONENT(RootCnvSvc)
DECLARE_COMPONENT(RootEvtSelector)
DECLARE_COMPONENT(RootPerfMonSvc)
