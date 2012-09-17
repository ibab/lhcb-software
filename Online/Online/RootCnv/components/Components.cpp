#include "GaudiKernel/SvcFactory.h"

#include "RootCnv/RootCnvSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,RootCnvSvc)

#include "RootCnv/RootEvtSelector.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,RootEvtSelector)

#include "RootCnv/RootPerfMonSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,RootPerfMonSvc)
