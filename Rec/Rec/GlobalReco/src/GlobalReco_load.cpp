// $Id: GlobalReco_load.cpp,v 1.6 2003-02-07 16:54:42 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/12/20 16:29:50  gcorti
// calo tools
//
// Revision 1.4  2002/11/20 20:00:23  ibelyaev
//  new algorithm for creation of Neutral ProtoParticles
//
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

  // Temporarly for reprocessing of Brunel v13r1 data
  DECLARE_TOOL( CaloE1Correction );
  DECLARE_TOOL( CaloSummerPrsCorrection );

};


// ============================================================================
// The END 
// ============================================================================
