// $Id: IDecayNodeFactory.h,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IDECAYNODEFINDER_H 
#define LOKI_IDECAYNODEFINDER_H 1
// ============================================================================
// Include files
// ============================================================================
// GauidKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  /// forward decalration 
  class Node ;                                          // forward declaration 
  // ==========================================================================
  /** @class IDecayNodeFactory LoKi/IDecayNodeFactory.h
   *  Simple factory for creation of decay nodes 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-05-22
   */
  class IDecayNodeFactory : public virtual IAlgTool
  {
    // ========================================================================
  public:
    // ========================================================================
    /** build the node form the node descriptor
     *  
     *  @code
     *
     *  const Decays::IDecayNodeFactory* finder = ... ;
     * 
     *  Decays::Node node = ... ; 
     *  StatusCode sc = finder -> node ( " Beauty & Charm & Baryon " , node ) ;
     *  
     *  if ( sc.isFailure() || !node.valid() ){  .. error here ... } 
     *
     *  @endcode 
     *  @param descriptor (INPUT)  node deccriptor 
     *  @param result       (OUTPUT) the constructed node 
     *  @return status code 
     */
    virtual StatusCode node 
    ( const std::string& decriptor , 
      Decays::Node&      result    ) const = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /// the unique interface ID 
    static const InterfaceID& interfaceID() ;        // the unique interface ID 
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~IDecayNodeFactory () ;           // virtual & protected destructor 
    // ========================================================================
  }; //                                             end class IDecayNodeFactory  
  // ==========================================================================
} //                                                    end of namespace Decays 
// ============================================================================
// The  END 
// ============================================================================
#endif // LOKI_IDECAYNODEFINDER_H
// ============================================================================
