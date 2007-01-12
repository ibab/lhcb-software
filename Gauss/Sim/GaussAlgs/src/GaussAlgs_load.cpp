// $Id: GaussAlgs_load.cpp,v 1.8 2007-01-12 15:23:40 ranjard Exp $
// Include files 

// from Gaudi 
#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES(GaussAlgs) {
  
  DECLARE_ALGORITHM ( GenInit ) ;
  DECLARE_ALGORITHM ( SimInit ) ;
  DECLARE_ALGORITHM ( GiGaFlushAlgorithm  ) ;
  DECLARE_ALGORITHM ( GiGaGetHitsAlg      ) ;
  DECLARE_ALGORITHM ( GiGaGetEventAlg     ) ;
  DECLARE_ALGORITHM ( PrintEventAlg       ) ;
  DECLARE_ALGORITHM ( GiGaCheckEventStatus ) ;
  
};

