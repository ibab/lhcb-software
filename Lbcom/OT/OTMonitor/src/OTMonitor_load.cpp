// $Id: OTMonitor_load.cpp,v 1.1.1.1 2004-09-03 13:30:48 jnardull Exp $
// include file
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTMonitor) {

  DECLARE_ALGORITHM( MCOTDepositMonitor );
  DECLARE_ALGORITHM( OTTimeChecker );
  DECLARE_ALGORITHM( OTTimeMonitor );
  
}
