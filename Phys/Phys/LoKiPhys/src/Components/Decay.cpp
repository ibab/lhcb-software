// $Id$
// ============================================================================
// Include
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
    typedef Decays::Trees::Types_<const LHCb::Particle*>::Invalid     Invalid ;
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
      , m_tree ( Invalid () )
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
  private:
    // ========================================================================
    /// the default tree 
    mutable Tree m_tree ;                                   // the default tree 
    // ========================================================================
  } ; //                                                     end of class Decay
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// create the decay tree from the descriptor
// ============================================================================
Decays::IDecay::Tree 
LoKi::Decay::tree ( const std::string& decay ) const 
{
  // check for the default tree
  if ( decay.empty() && m_tree.valid() ) { return m_tree ; }  // default tree? 
  
  MsgStream& err = error() ;
  
  // 1) parse the the descriptor into "generic tree" 
  Decays::Parsers::Tree ptree ;
  StatusCode sc = _parse  ( ptree , decay ) ;
  if ( sc.isFailure() ) 
  {
    Error("Error from _parse('" + decay + "')" , sc ) ;
    return  Invalid () ;                                              // RETURN 
  }
  // 2) convert parsed tree into reasonable decay tree
  Tree tree = Invalid () ;
  sc = Decays::Trees::factory ( tree , ptree , err.stream() )  ;
  if ( sc.isFailure() ) 
  {
    err << endmsg ;
    Error ( "Unable to create the tree from '" + ptree.toString() + "'", sc ) ;  
    return  Invalid () ;                                              // RETURN
  }
  // 3) validate the tree 
  if ( !tree ) 
  {
    sc = tree.validate ( ppSvc () ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Unable to validate the tree '" + tree.toString() + "'" , sc ) ;  
      return  Invalid () ;                                           // RETURN
    }
  }
  
  // 4) store the default tree 
  if ( decay.empty() ) { m_tree = tree ; }            // store the default tree 
  
  // 5) finally: 
  return tree ;
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
