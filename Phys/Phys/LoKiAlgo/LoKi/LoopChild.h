// $Id: LoopChild.h,v 1.1 2007-06-03 20:43:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef LOKI_LOOPCHILD_H 
#define LOKI_LOOPCHILD_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace LoKi 
{
  class Loop ;
  namespace Child 
  {
    /** get the number of children 
     *  @param mother poiunter to mother particle 
     *  @return number of daugthter particles 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */   
    std::size_t nChildren ( const LoKi::Loop& particle ) ;
    /** Trivial accessor to the dauthter particles for the given "particle"
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index    index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */
    LHCb::Particle* child
    ( const LoKi::Loop& particle , 
      const size_t      index    ) ;
    /** Trivial accessor to the dauthter particles for the given "particle"
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */    
    LHCb::Particle* child
    ( const LoKi::Loop& particle , 
      const size_t      index1   , 
      const size_t      index2   ) ;
    /** Trivial accessor to the dauthter particles for the given "particle"
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */    
    LHCb::Particle* child
    ( const LoKi::Loop& particle , 
      const size_t      index1   , 
      const size_t      index2   ,
      const size_t      index3   ) ;
    /** Trivial accessor to the dauthter particles for the given "particle"
     *
     *  @attention index starts with 1 , null index corresponds 
     *             to the original particle 
     *
     *  @param  particle (const) pointer to mother particle 
     *  @param  index1   index   index of the child particle 
     *  @param  index2   index   index of the child particle 
     *  @param  index3   index   index of the child particle 
     *  @param  index4   index   index of the child particle 
     *  @return daughter particle with given index 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-02
     */    
    LHCb::Particle* child
    ( const LoKi::Loop&  particle , 
      const size_t       index1   , 
      const size_t       index2   ,
      const size_t       index3   ,
      const size_t       index4   ) ;
  } // end of namespace Child 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_LOOPCHILD_H
// ============================================================================
