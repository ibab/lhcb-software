// $Id: CaloDAQ_load.cpp,v 1.8 2006-06-27 16:55:38 odescham Exp $
// Include files 


#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(CaloDAQ) {
  DECLARE_ALGORITHM( CaloFillRawBuffer );
  DECLARE_ALGORITHM( CaloFillPrsSpdRawBuffer );
  DECLARE_ALGORITHM( CaloZSupAlg );
  DECLARE_ALGORITHM( CaloTriggerAdcsFromRawAlg );
  DECLARE_ALGORITHM( CaloTriggerBitsFromRawAlg );

  DECLARE_TOOL( CaloReadoutTool );

  DECLARE_TOOL( CaloTriggerAdcsFromRaw );
  DECLARE_TOOL( CaloTriggerBitsFromRaw );
  DECLARE_TOOL( CaloEnergyFromRaw );

  DECLARE_ALGORITHM( CaloDigitsFromRaw );
  DECLARE_ALGORITHM( CompareCaloDigits );
  DECLARE_ALGORITHM( CaloReCreateMCLinks );
}
