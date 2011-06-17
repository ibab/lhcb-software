#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "RootCnv/RootCnvSvc.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,RootCnvSvc)

#include "RootCnv/RootEvtSelector.h"
DECLARE_NAMESPACE_SERVICE_FACTORY(Gaudi,RootEvtSelector)

#include "RootCnv/RootOutputStream.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Gaudi,RootOutputStream)
