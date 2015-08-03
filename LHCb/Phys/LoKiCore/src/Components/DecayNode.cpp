// $Id$
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision$
// ============================================================================
// Include files 
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Nodes.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayBase.h"
#include "LoKi/IDecayNode.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class DecayNode 
   *  the simple implememnattion of Decays::IDEcayNode interface 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-05-27
   */
  class DecayNode 
    : public             LoKi::DecayBase 
    , public virtual  Decays::IDecayNode
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class ToolFactory<LoKi::DecayNode> ;                  // the factory
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
     *  @param descriptor (INPUT)  node deccriptor 
     *  @param constructed node 
     */
    virtual Decays::Node node ( const std::string& descriptor ) const 
    { return LoKi::DecayBase::node ( descriptor ) ; }    
    // ========================================================================
  protected:
    // ========================================================================
    /** constructor form the type, name and parent 
     *  @param type the tool type (?)
     *  @param name the instance name 
     *  @param parent the parent 
     */
    DecayNode ( const std::string& type   ,                //     the tool type  
                const std::string& name   ,                // the instance name 
                const IInterface*  parent )                //        the parent
      : LoKi::DecayBase ( type , name , parent ) 
    {
      declareInterface<Decays::IDecayNode> ( this ) ;
    }
    /// virtual & protected destructor 
    virtual ~DecayNode() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    DecayNode ( ) ;                      // the default constructor is disabled 
    /// the copy constructor is disabled 
    DecayNode ( const DecayNode& ) ;        // the copy constructor is disabled 
    /// the assignement operator is disabled 
    DecayNode& operator=( const DecayNode& ) ;       // no assignement operator
    // ========================================================================
  } ; //                                                 end of class DecayNode
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                the factory for instantiation 
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,DecayNode)
// ============================================================================


// ============================================================================
// The END 
// ============================================================================


