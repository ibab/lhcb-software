// $Id: LoopObj.h,v 1.1.1.1 2006-03-18 10:39:21 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_LOOPOBJ_H 
#define LOKI_LOOPOBJ_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/IInterface.h"
#include "LoKi/Base.h"
#include "LoKi/Combiner.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysRangeTypes.h"
// ============================================================================

namespace LoKi 
{
  /** @class LoopObj LoopObj.h LoKi/LoopObj.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-15
   */
  class LoopObj 
    : public LoKi::Base 
  {
  public: 
    
  public:
  private:
    // the actual type of "Combiner" object
    typedef LoKi::Combiner_<LoKi::Types::Range::Container> Combiner ;
    // the "Combiner"
    Combiner                    m_combiner ; ///< the "Combiner"
    // the algorithm ("owner")
    LoKi::Interface<LoKi::Algo> m_algo     ; ///< the algorithm ("owner")
};
  
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOOPOBJ_H
// ============================================================================
