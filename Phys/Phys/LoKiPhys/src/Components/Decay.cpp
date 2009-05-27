// $Id: Decay.cpp,v 1.1 2009-05-27 18:45:24 ibelyaev Exp $
// ============================================================================
// Include
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/IDecay.h"
#include "LoKi/DecayBase.h"
#include "LoKi/Trees.h"
#include "LoKi/TreeHelpers.h"
#include "LoKi/TreeFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class Decay
   *  simple implementation of interface Decays::IDecay
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-05-27
   */
  class Decay
    : public            LoKi::DecayBase 
    , public virtual Decays::IDecay 
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class ToolFactory<LoKi::Decay> ;                      // the factory
    // ========================================================================
  public:
    // ========================================================================
    /** create the decay tree from the descriptor
     *  @param descriptor (INPUT)  the decay descriptor 
     *  @param tree       (OUTPUT) the decay tree 
     */
    virtual Tree tree ( const std::string& decay ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// build the node form the node descriptor
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
    Decay ( const std::string& type   ,                    //     the tool type  
            const std::string& name   ,                    // the instance name 
            const IInterface*  parent )                    //        the parent
      : LoKi::DecayBase ( type , name , parent ) 
    {
      declareInterface<Decays::IDecay>     ( this ) ;
      declareInterface<Decays::IDecayNode> ( this ) ;
    }
    /// virtual & protected destructor 
    virtual ~Decay() {}                       // virtual & protected destructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Decay ( ) ;                          // the default constructor is disabled 
    /// the copy constructor is disabled 
    Decay ( const Decay& ) ;                // the copy constructor is disabled 
    /// the assignement operator is disabled 
    Decay& operator=( const Decay& ) ;               // no assignement operator
    // ========================================================================
  } ; //                                                   end of class MCDecay
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// create the decay tree from the descriptor
// ============================================================================
Decays::IDecay::Tree 
LoKi::Decay::tree ( const std::string& decay ) const 
{
  MsgStream& err = error() ;
  
  typedef Decays::Trees::Types_<const LHCb::Particle*>  Types ;
  
  std::string _decay = _makeCC ( decay ) ;
  
  
  // 1) parse the intout into "generic tree" 
  Decays::Parsers::Tree tree ;
  
  StatusCode sc = _parse  ( tree , _decay ) ;
  if ( sc.isFailure() ) 
  {
    Error("Error from _parse('" + _decay + "')" , sc ) ;
    return  Types::Invalid() ;                                         // RETURN 
  }
  // 2) convert it into reasonable decay tree
  Tree mctree = Types::Invalid() ;
  
  sc = Decays::Trees::factory ( mctree , tree , err.stream() )  ;
  if ( sc.isFailure() ) 
  {
    err << endmsg ;
    Error ( "Unable to create the tree from '" + tree.toString() + "'", sc ) ;  
    return  Types::Invalid() ;                                        // RETURN
  }
  
  /// validate the tree 
  if ( !mctree.valid() ) 
  {
    sc = mctree.validate ( ppSvc () ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Unable to validate the tree '" + mctree.toString() + "'" , sc ) ;  
      return  Types::Invalid() ;                                        // RETURN
    }
  }
  // finally: 
  return mctree ;
}
// ============================================================================


// ============================================================================
//                                                the factory for instantiation 
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,Decay)
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
