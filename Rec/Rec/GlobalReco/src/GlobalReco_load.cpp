// $Id: GlobalReco_load.cpp,v 1.3 2002-07-15 19:27:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"



DECLARE_FACTORY_ENTRIES(GlobalReco) {
  
  DECLARE_ALGORITHM( ChargedProtoPAlg );
  DECLARE_ALGORITHM( NeutralPPCreator );

  DECLARE_ALGORITHM(PrimaryVertexFinderAlg);
  DECLARE_TOOL(PrimaryVertexFitterTool);

};


// ============================================================================
// The END 
// ============================================================================
