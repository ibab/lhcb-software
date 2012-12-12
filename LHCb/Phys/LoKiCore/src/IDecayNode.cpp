// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "LoKi/IDecayNode.h"
// ============================================================================
/** @file 
 *  Implementation file for class  Decays::IDecayNode
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
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-22
 */
// ============================================================================
// virtual & protected destructi
// ============================================================================
Decays::IDecayNode::~IDecayNode () {}
// ============================================================================
// unique interface ID 
// ============================================================================
const InterfaceID& Decays::IDecayNode::interfaceID() 
{
  static const InterfaceID s_ID  ( "Decays::IDecayNode" , 1 , 0 );
  return s_ID ;
}
// ============================================================================

    

// ============================================================================
// The END 
// ============================================================================
