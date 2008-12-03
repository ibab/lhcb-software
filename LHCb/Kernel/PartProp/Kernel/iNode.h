// $Id: iNode.h,v 1.1 2008-12-03 17:35:54 ibelyaev Exp $
// ============================================================================
#ifndef DECAYS_NODE_H 
#define DECAYS_NODE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
#include <iosfwd>
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb 
{ 
  class IParticlePropertySvc ;
  class  ParticleID ;
}
// ============================================================================
namespace Decays 
{
  // ========================================================================
  /** @class iNode Kernel/iNode.h
   *  The abstract class which represents the single "node" of decay tree
   *
   *  The class is imported from LoKi project
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-04-12
   */
  class iNode : public std::unary_function<LHCb::ParticleID,bool>
  {
  public:
    // ======================================================================
    /** the basic operation: comparison of the particle PID with the node  
     *  @param   pid the particle to be compared 
     *  @return  true if the actual PID matched the node dectriptor 
     */
    virtual bool operator()  ( const LHCb::ParticleID& pid ) const = 0 ;
    // ======================================================================
    /// clone method ("virtual constructor")
    virtual  iNode* clone() const  = 0 ;
    // ======================================================================
    /** printout of the stream
     *  @param s the reference to the output stream 
     *  @return the reference to the output stream 
     */
    virtual std::ostream& fillStream ( std::ostream& s ) const = 0 ;
    /// check the validity of the node 
    virtual bool valid() const = 0  ;
    // ======================================================================
    /** validate the decay node 
     *  @param svc pointer to Particle Property Service
     *  @return StatusCode 
     */
    virtual StatusCode validate
    ( const LHCb::IParticlePropertySvc* svc ) const = 0 ;
    // ======================================================================
    /// the string representation of the node 
    virtual std::string toString () const ;
    // ======================================================================
    /// virtual desctructor 
    virtual ~iNode() ;
    // ======================================================================
  };
  // ========================================================================
  /** @class Node 
   *  The generic class to hold the pointer to other node
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-04-12
   */
  class Node : public iNode 
  {
  public:
    // ======================================================================
    /// constructor from the node 
    Node ( const Decays::iNode& node  ) ;
    /// copy constructor 
    Node ( const Decays::Node&  right ) ;        
    /// MANDATORY: virtual destructor
    virtual ~Node () ;
    /// MANDATORY: clone method ("virtual constructor")
    virtual  Node* clone () const ;
    /// MANDATORY: the only one essential method
    virtual bool operator() ( const LHCb::ParticleID& p ) const ;
    /// MANDATORY: the specific printout
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    /// MANDATORY: check the validity of the node 
    virtual bool valid() const ;
    /// MANDATORY: the proper validation of the node
    virtual StatusCode validate 
    ( const LHCb::IParticlePropertySvc* svc ) const ;
    // ======================================================================
  public:
    // ======================================================================
    // get the underlying node
    inline const iNode& node() const { return *m_node ; }
    // ======================================================================
    // evaluate the underlying node 
    inline bool node ( const LHCb::ParticleID& pid ) const 
    { return node() ( pid ) ; }
    // ======================================================================
  public:
    // ======================================================================
    /// assignement operator:
    Node& operator=( const  Node& right ) ;
    /// assignement from arbitrary node 
    Node& operator=( const iNode& right ) ;
    // ======================================================================
  private:
    // ======================================================================
    /// the default constructor is disabled 
    Node () ;                                      // no default constructor 
    // ======================================================================
  private:
    // ======================================================================
    /// the node itself:
    iNode* m_node  ; // the node itself:
    // ======================================================================
  } ;
  // ========================================================================
} // end of namespace Decays 
// ============================================================================
/** equality
 *  The node is "equal" to the PID , if the given pid satisfies the node criteria
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator== 
( const Decays::iNode& node , const LHCb::ParticleID&    pid  ) 
{ return node ( pid ) ; }
// ============================================================================
/** "right" equality
 *  The node is "equal" to the PID , if the given pid satisfies the node criteria
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator==
( const LHCb::ParticleID&    pid  , const Decays::iNode& node ) 
{ return node == pid ; }
// ============================================================================
/** non-equality
 *  The node is "equal" to the PID , if the given PID satisfies the node criteria 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator!= 
( const Decays::iNode& node , const LHCb::ParticleID&    pid  ) 
{ return !( node == pid ) ; }
// ============================================================================
/** non-equality
 *  The node is "equal" to the PID , if the given PID satisfies the node criteria 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator!= 
( const LHCb::ParticleID&    pid  , const Decays::iNode& node ) 
{ return node != pid ; }
// ============================================================================
/** printout to the output stream 
 *  @param s the output stream 
 *  @param n the node 
 *  @return the output stream (Reference) 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline std::ostream& operator<<
  ( std::ostream&        s ,
    const Decays::iNode& n ) { return n.fillStream ( s ) ; }
// ============================================================================
/** "on-flight" validation of the node 
 *  @param n the node 
 *  @param svc particle property service 
 *  @return the output stream (Reference) 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */    
inline StatusCode operator+ 
( const Decays::iNode&              n   , 
  const LHCb::IParticlePropertySvc* svc ) { return n.validate ( svc ) ; }
// ============================================================================
/** "on-flight" validation of the node 
 *  @param n the node 
 *  @param svc particle property service 
 *  @return the output stream (Reference) 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */    
inline StatusCode operator*
( const Decays::iNode&              n   , 
  const LHCb::IParticlePropertySvc* svc ) { return n.validate ( svc ) ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // DECAYS_NODE_H
// ============================================================================
