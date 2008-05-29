// $Id: IPrintDecay.h,v 1.2 2008-05-29 14:55:49 cattanem Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IPRINTDECAY_H 
#define DAVINCIKERNEL_IPRINTDECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward decalarations 
// ============================================================================
namespace LHCb { class Particle ; }
// ============================================================================
/** @class IPrintDecay Kernel/IPrintDecay.h
 *  Simple & MC-independent abstact interface for 
 *  the printout of the decay tree of the particle
 *  Essentially it is just an subset of abstract interface IPrintDecayTreeTool 
 *  @see IPrintDecayTreeTool 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-03-30
 */
class IPrintDecay : virtual public IAlgTool 
{
public: 
  // ==========================================================================
  /** Print the decay tree for a given particle
   *
   *  @code
   * 
   *  IPrintDecay* tool = ... ;
   *  const LHCb::Particle* B = ... ;
   * 
   *  tool -> printDecay ( B ) ; 
   *
   *  @endcode 
   *
   *  @param mother the pointer to the particle 
   *  @param maxDepth the maximal depth level
   */
  virtual void printTree
  ( const LHCb::Particle* mother        , 
    int                   maxDepth = -1 ) = 0;
  // ==========================================================================
  /** Print the decay tree for the particles form some range or sequence
   *
   *  @code
   * 
   *  const LHCb::Particle::ConstVector& Bs = ... ;
   *  IPrintDecay* tool = ... ;
   *
   *  tool -> printDecay ( Bs.begin() , Bs.end() ) ; 
   *
   *  @endcode 
   *
   *  @param begin    begin iterator of the particle vector
   *  @param end      end iterator of the particle vector
   *  @param maxDepth the maximal depth level
   */
  template <class PARTICLE>
  void printTree 
  ( PARTICLE begin         , 
    PARTICLE end           , 
    int      maxDepth = -1 ) 
  {
    for ( ; begin != end ; ++begin ) { this->printTree ( *begin , maxDepth ) ; } 
  }
  // ==========================================================================
public:
  // ==========================================================================
  /// the unique interface identifier 
  static const InterfaceID& interfaceID() ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual and protected destructor 
  virtual ~IPrintDecay() ; // virtual and protected destructor 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_IPRINTDECAY_H
// ============================================================================
