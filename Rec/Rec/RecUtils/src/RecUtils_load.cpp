// $Id: RecUtils_load.cpp,v 1.2 2006-07-18 16:27:30 pkoppenb Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx

DECLARE_FACTORY_ENTRIES(RecUtils) {

  DECLARE_ALGORITHM( ReadStripETC );
  DECLARE_ALGORITHM( CopyStripETC );

}
