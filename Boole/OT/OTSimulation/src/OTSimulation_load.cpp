// $Id: OTSimulation_load.cpp,v 1.1.1.1 2004-09-03 13:35:47 jnardull Exp $
// include file
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTSimulation) {

  DECLARE_ALGORITHM( MCOTDepositCreator );
  DECLARE_ALGORITHM( MCOTTimeCreator );
  DECLARE_ALGORITHM( OTFillRawBuffer );
  
  DECLARE_TOOL( OTSmearer );
  DECLARE_TOOL( OTrtRelation );
  DECLARE_TOOL( OTEffCalculator );
  DECLARE_TOOL( OTReadOutWindow );
  DECLARE_TOOL( OTRandomDepositCreator );
}
