// $Id: GaudiOnline_load.cpp,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
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

  DECLARE_OBJECT( OnlineEvtSelector  );
  DECLARE_OBJECT( RawBufferWriter  );
  DECLARE_OBJECT( OnlineCnvSvc  );
  DECLARE_OBJECT( OnlineCnv  );

} 
