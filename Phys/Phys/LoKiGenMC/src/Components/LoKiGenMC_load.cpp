// $Id: LoKiGenMC_load.cpp,v 1.2 2006-02-18 18:15:44 ibelyaev Exp $
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================ 
//  $Log: not supported by cvs2svn $
//  Revision 1.1.1.1  2006/01/26 17:35:17  ibelyaev
//  New import: Generator/MC related part of LoKi project 
//
// ============================================================================ 
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================ 

// ============================================================================
/** @file
 *
 *  mandatory file to build shared component library
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

DECLARE_FACTORY_ENTRIES(LoKiGenMC) 
{
  DECLARE_TOOL      ( LoKi_HepMC2MC    ) ; 
  DECLARE_ALGORITHM ( LoKi_HepMC2MCAlg ) ; 
};


// ============================================================================ 
// The END 
// ============================================================================ 
