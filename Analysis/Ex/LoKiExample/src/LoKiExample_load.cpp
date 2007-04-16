// $Id: LoKiExample_load.cpp,v 1.12 2007-04-16 16:16:53 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.12 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.11  2007/04/04 12:16:39  ibelyaev
//  v5r2: update
//
//
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================
/** @file 
 *
 *  Mandatory file for dynamic loading of component library
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2003-02-24
 */
// ============================================================================
DECLARE_FACTORY_ENTRIES( LoKiExample ) 
{
  
  DECLARE_ALGORITHM( LoKi_MCMuon          ) ;
  DECLARE_ALGORITHM( LoKi_Phi             ) ;
  DECLARE_ALGORITHM( LoKi_PhiMC           ) ;
  DECLARE_ALGORITHM( LoKi_Hybrid          ) ;
  DECLARE_ALGORITHM( LoKi_Particles       ) ;
  DECLARE_ALGORITHM( LoKi_HelloWorld      ) ;
  DECLARE_ALGORITHM( LoKi_Bs2PsiPhi       ) ;
  
};

// ============================================================================
// The END 
// ============================================================================

