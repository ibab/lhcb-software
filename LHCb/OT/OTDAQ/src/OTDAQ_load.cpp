// $Id: OTDAQ_load.cpp,v 1.1.1.1 2004-02-03 09:49:17 jnardull Exp $
// Include files 
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"
#include "GaudiKernel/IToolFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(OTDAQ) {
  DECLARE_ALGORITHM( OTFillRawBuffer );
  DECLARE_ALGORITHM( OTRetrieveBuffer );
  DECLARE_ALGORITHM( OTCheckOTDigit );
}
