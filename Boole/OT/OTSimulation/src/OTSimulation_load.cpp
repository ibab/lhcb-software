// $Id: OTSimulation_load.cpp,v 1.2 2004-09-10 13:14:23 cattanem Exp $
// include file
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTSimulation) {

  DECLARE_ALGORITHM( MCOTDepositCreator );
  DECLARE_ALGORITHM( MCOTTimeCreator );
  DECLARE_ALGORITHM( OTFillRawBuffer );
  
  DECLARE_TOOL( OTSmearer );
  DECLARE_TOOL( OTrtRelation );
  DECLARE_TOOL( OTEffCalculator );
  DECLARE_TOOL( OTRandomDepositCreator );
}
