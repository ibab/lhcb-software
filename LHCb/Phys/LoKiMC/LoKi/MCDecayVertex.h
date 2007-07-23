// $Id: MCDecayVertex.h,v 1.2 2007-07-23 17:27:30 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCDECAYVERTEX_H 
#define LOKI_MCDECAYVERTEX_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL 
// ============================================================================
#include <functional>
// ============================================================================
namespace LoKi 
{
  namespace MCVertices
  {
    // ========================================================================
    /** @struct MCDecayVertex
     *  trivial structure needed for location of valid decay vertices 
     *  @see LHCb::MCVertex::isDecay
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    struct MCDecayVertex : public std::unary_function<const LHCb::MCVertex*,bool>
    {
      inline bool operator() ( const LHCb::MCVertex* vertex ) const 
      { return 0 != vertex && vertex->isDecay () ; }
    } ;
    // ========================================================================
    /** @var IsDecay
     *  useful variable to indicate the decay vertex 
     *  @see LHCb::MCVertex::isDecay
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    const MCDecayVertex               IsDecay = MCDecayVertex()  ;
    // ========================================================================    
  } // end of namespace LoKi::MCVertices
} //end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCDECAYVERTEX_H
// ============================================================================

