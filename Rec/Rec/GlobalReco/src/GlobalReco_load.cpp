// $Id: GlobalReco_load.cpp,v 1.4 2002-11-20 20:00:23 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/07/15 19:27:08  ibelyaev
//  add new algorithm for creation on Neutral ProtoParticles
// 
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"



DECLARE_FACTORY_ENTRIES(GlobalReco) {
  
  DECLARE_ALGORITHM( ChargedProtoPAlg     ) ;
  DECLARE_ALGORITHM( NeutralPPCreator     ) ;
  DECLARE_ALGORITHM( NeutralPPsFromCPsAlg ) ;

  DECLARE_ALGORITHM(PrimaryVertexFinderAlg) ;
  DECLARE_TOOL(PrimaryVertexFitterTool);

};


// ============================================================================
// The END 
// ============================================================================
