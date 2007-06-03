// $Id: MCDecayVertex.h,v 1.1 2007-06-03 20:41:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revison:$
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
    /** @struct MCDecayVertex
     *  trivial structure needed for locationof valid decay vertoces 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    struct MCDecayVertex : public std::unary_function<const LHCb::MCVertex*,bool>
    {
      inline bool operator() ( const LHCb::MCVertex* vertex ) const 
      { return 0 != vertex && vertex->isDecay () ; }
    } ;
    /** @var IsDecay
     *  useful variable to indicate the decay vertex 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    const MCDecayVertex               IsDecay = MCDecayVertex()  ;
    
  } // end of namespace LoKi::MCVertices
} //end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCDECAYVERTEX_H
// ============================================================================

