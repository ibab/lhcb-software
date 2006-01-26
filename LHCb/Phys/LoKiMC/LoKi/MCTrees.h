// $Id: MCTrees.h,v 1.1.1.1 2006-01-26 16:13:39 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1.1.1 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
#ifndef LOKI_MCTREES_H 
#define LOKI_MCTREES_H 1
// ===========================================================================
// forward declarations 
// ===========================================================================
namespace LHCb
{
  class MCParticle ;
  class MCVertex   ;
} ;
// ===========================================================================

// ============================================================================
/** @file
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
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================


namespace LoKi 
{
  namespace MCTrees 
  {
    
    /** simple function with evaluates to true 
     *  if MC particle  originates in the decay 
     *  tree of parent particle 
     *  @param particle pointer to MC particle 
     *  @param parent pointer to the mother particle 
     *  @return true if particle belongd to decay tree of parent 
     */
    bool fromMCTree 
    ( const LHCb::MCParticle* particle  , 
      const LHCb::MCParticle* parent    ) ;
    
    /** simple function with evaluates to true 
     *  if MC particle  originates in the decay 
     *  tree of parent veretx  
     *  @param particle pointer to MC particle 
     *  @param parent pointer to the mother veretx  
     *  @return true if particle belongd to decay tree of parent 
     */
    bool fromMCTree 
    ( const LHCb::MCParticle* particle  , 
      const LHCb::MCVertex*   parent    ) ;
    
  }; // end of namespace MCTrees 
  
}; // end of namespace LoKi 


// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_MCTREES_H
// ===========================================================================
