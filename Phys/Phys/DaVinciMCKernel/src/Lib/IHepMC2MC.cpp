// $Id: IHepMC2MC.cpp,v 1.1.1.1 2006-10-11 13:35:09 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// Kernel?
// ============================================================================
#include "Kernel/IHepMC2MC.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for class IHepMC2MC
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
 *  @date 2006-01-23 
 */
// ============================================================================

// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var IID_IHepMC2MC
   *  unique static identifier of IHepMC2MC interface 
   *  @see IHepMC2MC
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-12
   */
  // ==========================================================================
  const InterfaceID IID_IHepMC2MC ( "IHepMC2MC" , 1 , 0 ) ;
  // ==========================================================================
};
// ============================================================================

// ============================================================================
/// Return the unique interface identifier
// ============================================================================
const InterfaceID& IHepMC2MC::interfaceID() { return IID_IHepMC2MC ; } ;
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
IHepMC2MC::~IHepMC2MC() {}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
