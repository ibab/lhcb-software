// $Id: MCTrees.h,v 1.4 2007-04-16 16:16:08 pkoppenb Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/11/27 11:58:37  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/02/18 18:10:57  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/01/26 16:13:39  ibelyaev
// New Packaage: MC-dependent part of LoKi project 
//
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
} 
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================


namespace LoKi 
{
  /** @namespace LoKi::MCTrees 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-11-25 
   */
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
    
  } // end of namespace LoKi::MCTrees
} // end of namespace LoKi 


// ===========================================================================
// The END 
// ===========================================================================
#endif // LOKI_MCTREES_H
// ===========================================================================
