// $Id: LoKiGenMC_load.cpp,v 1.4 2007-04-16 16:16:45 pkoppenb Exp $
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================ 
//  $Log: not supported by cvs2svn $
//  Revision 1.3  2006/03/18 18:26:43  ibelyaev
//   add (primary)MCVErtex->GenCollision links
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

DECLARE_FACTORY_ENTRIES(LoKiGenMC) 
{
  DECLARE_TOOL      ( LoKi_HepMC2MC        ) ; 
  DECLARE_ALGORITHM ( LoKi_HepMC2MCAlg     ) ; 
  DECLARE_TOOL      ( LoKi_MC2Collision    ) ; 
  DECLARE_ALGORITHM ( LoKi_MC2CollisionAlg ) ; 
};


// ============================================================================ 
// The END 
// ============================================================================ 
