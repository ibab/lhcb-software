// $Id: GaussAlgs_load.cpp,v 1.1.1.1 2004-02-20 19:43:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// Gaudi 
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================


DECLARE_FACTORY_ENTRIES(GaussAlgs) {
  
  DECLARE_ALGORITHM ( GiGaFlushAlgorithm  ) ;
  DECLARE_ALGORITHM ( GiGaGetHitsAlg      ) ;
  DECLARE_ALGORITHM ( GiGaGetEventAlg     ) ;
  DECLARE_ALGORITHM ( PrintEventAlg       ) ;
  DECLARE_ALGORITHM ( GaussInitialisation ) ;
  
};

// ============================================================================
// The END 
// ============================================================================
