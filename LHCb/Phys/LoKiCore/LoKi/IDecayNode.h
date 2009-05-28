// $Id: IDecayNode.h,v 1.2 2009-05-28 08:55:34 ibelyaev Exp $
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
#include "Kernel/iNode.h"
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  /** @class IDecayNode LoKi/IDecayNode.h
   *  Simple factory for creation of decay nodes 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-05-22
   */
  class IDecayNode : public virtual IAlgTool
  {
    // ========================================================================
  public:
    // ========================================================================
    /** build the node form the node descriptor
     *  
     *  @code
     *
     *  const Decays::IDecayNode* finder = ... ;
     * 
     *  Decays::Node node = finder -> node ( " Beauty & Charm & Baryon " ) ;
     *  
     *  if ( !node.valid() ){  .. error here ... } 
     *
     *  @endcode 
     * 
     *  For the empty string the "default" node is returned  
     *
     *  @param descriptor (INPUT)  node descriptor 
     *  @param constructed node 
     */
    virtual Node node ( const std::string& descriptor = "" ) const = 0 ;
    // ========================================================================
  public:
    // ========================================================================
    /// the unique interface ID 
    static const InterfaceID& interfaceID() ;        // the unique interface ID 
    // ========================================================================
  protected:
    // ========================================================================
    /// virtual & protected destructor 
    virtual ~IDecayNode () ;                  // virtual & protected destructor 
    // ========================================================================
  }; //                                             end class IDecayNodeFactory  
  // ==========================================================================
} //                                                    end of namespace Decays 
// ============================================================================
// The  END 
// ============================================================================
#endif // LOKI_IDECAYNODEFINDER_H
// ============================================================================
