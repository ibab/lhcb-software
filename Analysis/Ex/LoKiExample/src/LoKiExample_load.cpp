// $Id: LoKiExample_load.cpp,v 1.2 2004-04-09 07:23:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2003/07/24 16:43:49  ibelyaev
//  new package with LoKi examples 
//
// ============================================================================
// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"

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
  
};

// ============================================================================
// The END 
// ============================================================================

