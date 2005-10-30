// $Id: GaussAlgs_load.cpp,v 1.5 2005-10-30 21:58:49 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/10/25 18:04:00  gcorti
// new algorithm
//
// Revision 1.3  2005/04/22 14:46:48  gcorti
// remove obsolete algorithm
//
// Revision 1.2  2005/04/20 15:02:51  gcorti
// new algorithm to check G4Event status
//
// Revision 1.1.1.1  2004/02/20 19:43:29  ibelyaev
// New package 
// 
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
  DECLARE_ALGORITHM ( GiGaCheckEventStatus ) ;
  DECLARE_ALGORITHM ( GeneratorToG4       ) ;
  DECLARE_ALGORITHM ( G4HepMCToMCTruth    );
  
};

// ============================================================================
// The END 
// ============================================================================
