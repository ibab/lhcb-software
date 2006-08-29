// $Id: LoKiPhysMC_load.cpp,v 1.2 2006-08-29 11:40:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/03/19 12:12:25  ibelyaev
//  add PV <--> MC and PV <--> pp-Collision links
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
 *  the mandatory file for implementation of component libraries
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
 *  @date 2006-03-19 
 */
// ============================================================================


// ============================================================================
DECLARE_FACTORY_ENTRIES(LoKiPhysMC) 
{
  //
  DECLARE_TOOL      ( LoKi_PV2MC    ) ;
  DECLARE_ALGORITHM ( LoKi_PV2MCAlg ) ;
  DECLARE_ALGORITHM ( LoKi_Track2MC ) ;
  DECLARE_ALGORITHM ( LoKi_P2MC     ) ;
  DECLARE_ALGORITHM ( LoKi_P2MCW    ) ;
  //
};
// ============================================================================


// ============================================================================
// The END
// ============================================================================
