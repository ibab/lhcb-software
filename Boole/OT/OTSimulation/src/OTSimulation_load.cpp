// $Id: OTSimulation_load.cpp,v 1.3 2006-01-20 14:16:01 cattanem Exp $
// include file
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTSimulation) {

  DECLARE_ALGORITHM( MCOTDepositCreator );
  DECLARE_ALGORITHM( MCOTTimeCreator );
  DECLARE_ALGORITHM( OTFillRawEvent );
  
  DECLARE_TOOL( OTSmearer );
  DECLARE_TOOL( OTrtRelation );
  DECLARE_TOOL( OTEffCalculator );
  DECLARE_TOOL( OTRandomDepositCreator );
}
