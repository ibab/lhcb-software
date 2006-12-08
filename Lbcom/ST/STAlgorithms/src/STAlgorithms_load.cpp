// $Id: STAlgorithms_load.cpp,v 1.4 2006-12-08 09:54:23 mneedham Exp $

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES(STAlgorithms) {

  DECLARE_ALGORITHM( MCSTDepositCreator );
  DECLARE_ALGORITHM( MCSTDigitCreator );
  DECLARE_ALGORITHM( STDigitCreator );
  DECLARE_ALGORITHM( STClusterCreator );

  DECLARE_TOOL( STChargeSharingTool );
  DECLARE_TOOL( STEffCalculator );

}






