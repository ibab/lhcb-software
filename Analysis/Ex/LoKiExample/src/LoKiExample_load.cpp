// $Id: LoKiExample_load.cpp,v 1.8 2005-02-16 11:16:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

// ============================================================================
/** @file 
 *  Mandatory file for dynamic loading of component lbrary
 *  @date 2003-02-24
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================


DECLARE_FACTORY_ENTRIES( LoKiExample ) 
{
  // trivial eamples 
  DECLARE_ALGORITHM( LoKi_Histos          ) ;
  DECLARE_ALGORITHM( LoKi_Tuple           ) ;
  DECLARE_ALGORITHM( LoKi_EventTagTuple   ) ;
  
  // mor erealistic exmaple
  DECLARE_ALGORITHM( LoKi_Bs2PhiGamma     ) ;
  DECLARE_ALGORITHM( LoKi_Pi0fromBdTo3pi  ) ;

  // "real" example 
  DECLARE_ALGORITHM( LoKi_Bd2KStarGamma   ) ;

  DECLARE_ALGORITHM( LoKi_Bs2PhiPhi       ) ;
  
  // LoKi DEMO
  DECLARE_ALGORITHM( LoKi_Demo1           ) ;
 
  // charge conjugated loop 
  DECLARE_ALGORITHM( LoKiLoopCC           ) ;

  DECLARE_ALGORITHM( LoKi_MCEvent         ) ;
  DECLARE_ALGORITHM( LoKi_MCMuon          ) ;
  DECLARE_ALGORITHM( LoKi_MCDecays        ) ;

  DECLARE_ALGORITHM( LoKi_VeloClusterMC   ) ;

  DECLARE_ALGORITHM( LoKi_Pi0             ) ;

  DECLARE_ALGORITHM( LoKi_Bs2PhiMM        ) ;
  DECLARE_ALGORITHM( LoKi_Higgs1          ) ;
  
};

// ============================================================================
// The END 
// ============================================================================

