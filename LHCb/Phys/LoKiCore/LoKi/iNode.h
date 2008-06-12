// $Id: iNode.h,v 1.1 2008-06-12 08:14:31 ibelyaev Exp $
// ============================================================================
#ifndef DECAYS_NODE_H 
#define DECAYS_NODE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb { class ParticleID ; }  // Kernel/LHCbKernel 
class IParticlePropertySvc ;           // GaudiKernel
// ============================================================================
namespace LoKi 
{
  namespace Decays 
  {
    // ========================================================================
    /** @class iNode Decays/iNode.h
     *  The abstract class which represents the single "node" of decay tree
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-04-12
     */
    class iNode : public std::unary_function<const LHCb::ParticleID,bool>
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
      // ======================================================================
      /// check the validity of the node 
      virtual bool valid() const = 0  ;
      // ======================================================================
      /** validate the decay node 
       *  @param svc pointer to Particle Property Service
       *  @return StatusCode 
       */
      virtual StatusCode validate ( IParticlePropertySvc* svc ) const = 0 ;
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
    class Node : public LoKi::Decays::iNode 
    {
    public:
      // ======================================================================
      /// constructor from the node 
      Node ( const LoKi::Decays::iNode& node  ) ;
      /// copy constructor 
      Node ( const LoKi::Decays::Node&  right ) ;        
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
      virtual StatusCode validate ( IParticlePropertySvc* svc ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // get the underlying node
      inline const LoKi::Decays::iNode& node() const 
      { return *m_node ; }
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
      LoKi::Decays::iNode* m_node  ; // the node itself:
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Decays 
  // ==========================================================================
} // end of namespace LoKi   
// ============================================================================
/** equality
 *  The node is "equal" to the PID , if the given pid satisfies the node criteria
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator== 
( const LoKi::Decays::iNode& node ,
  const LHCb::ParticleID&    pid  ) { return node ( pid ) ; }
// ============================================================================
/** "right" equality
 *  The node is "equal" to the PID , if the given pid satisfies the node criteria
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator==
( const LHCb::ParticleID&    pid  , 
  const LoKi::Decays::iNode& node ) { return node == pid ; }
// ============================================================================
/** non-equality
 *  The node is "equal" to the PID , if the given PID satisfies the node criteria 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator!= 
( const LoKi::Decays::iNode& node ,
  const LHCb::ParticleID&    pid  ) { return !( node == pid ) ; }
// ============================================================================
/** non-equality
 *  The node is "equal" to the PID , if the given PID satisfies the node criteria 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline bool operator!= 
( const LHCb::ParticleID&    pid  , 
  const LoKi::Decays::iNode& node ) { return node != pid ; }
// ============================================================================
/** printout to the output stream 
 *  @param s the output stream 
 *  @paramn n the node 
 *  @return the output stream (Reference) 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-21
 */
inline std::ostream& operator<<
  ( std::ostream&              s , 
    const LoKi::Decays::iNode& n ) 
{ return n.fillStream ( s ) ; }
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
#endif // DECAYS_NODE_H
// ============================================================================
