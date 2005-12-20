// $Id: GaudiOnline_load.cpp,v 1.4 2005-12-20 16:38:18 frankb Exp $
//====================================================================
//  GaudiOnline_load.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(GaudiOnline)  {
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPCnvSvc  );
  DECLARE_NAMESPACE_SERVICE(   LHCb,MBMCnvSvc  );
  DECLARE_NAMESPACE_SERVICE(   LHCb,MepManager  );
  DECLARE_ALGORITHM(           RawEventTestCreator);
  DECLARE_ALGORITHM(           MEPReader);
  DECLARE_OBJECT(              OnlineMessageSvc);
}
