// $Id: GaudiOnline_load.cpp,v 1.3 2005-06-08 11:38:13 frankb Exp $
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
  DECLARE_OBJECT( DecisionSendAlg  );
  DECLARE_OBJECT( DecisionSetterAlg  );
  DECLARE_OBJECT( OnlineCnvSvc  );
  DECLARE_OBJECT( OnlineCnv  );
  DECLARE_OBJECT( OnlineMessageSvc  );

} 
