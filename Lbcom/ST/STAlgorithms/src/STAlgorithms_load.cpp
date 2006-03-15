// $Id: STAlgorithms_load.cpp,v 1.3 2006-03-15 16:16:03 cattanem Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"

// Declare  OBJECT / CONVERTER / ALGORITHM / TOOL using the macros DECLARE_xxx
// The statements are like that:
//
// DECLARE_ALGORITHM( MyAlgorithm );
// DECLARE_TOOL( MyTool );
// DECLARE_OBJECT( DataObject );
//
// They should be inside the 'DECLARE_FACTORY_ENTRIES' body.

DECLARE_FACTORY_ENTRIES(STAlgorithms) {

  DECLARE_ALGORITHM( MCSTDepositCreator );
  DECLARE_ALGORITHM( MCSTDigitCreator );
  DECLARE_ALGORITHM( STDigitCreator );
  DECLARE_ALGORITHM( STClusterCreator );

  DECLARE_TOOL( STAmplifierResponse );
  DECLARE_TOOL( STChargeSharingTool );
  DECLARE_TOOL( STEffCalculator );
  DECLARE_TOOL( STDepositedCharge );
  DECLARE_TOOL( STGeantDepositedCharge );
}






