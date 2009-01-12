// $Id: Nodes.h,v 1.3 2009-01-12 06:35:38 cattanem Exp $
// ============================================================================
#ifndef DAVINCI_DECAYNODES_H 
#define DAVINCI_DECAYNODES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
#include "Kernel/Decay.h"
#include "Kernel/iNode.h"
// ============================================================================
namespace Decays
{
  // ==========================================================================
  /** check the validness of the trees or nodes
   *  @param begin begin-iterator for the sequence of trees/nodes  
   *  @param end end-iterator for the sequence of trees/nodes  
   *  @return true of no invalid trees are found 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl 
   *  @date 2008-04-21
   */
  template <class TREE> 
  inline bool valid ( TREE begin , TREE end   ) 
  {
    for ( ; begin != end ; ++begin ) 
    { if  ( !begin->valid() ) { return false ; } }
    return true ;
  }
  // ==========================================================================
  /** validate trees/nodes 
   *  @param begin begin-iterator for the sequence of trees/nodes  
   *  @param end end-iterator for the sequence of trees/nodes  
   *  @param svc the Particle Property Service for validation  
   *  @return statuis code 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl 
   *  @date 2008-04-21
   */
  template <class TREE> 
  inline StatusCode validate 
  ( TREE begin , TREE end , const LHCb::IParticlePropertySvc* svc ) 
  {
    for ( ; begin != end ; ++begin )
    { 
      StatusCode sc = begin->validate ( svc ) ;
      if ( sc.isFailure() ) { return sc ; }
    }
    return StatusCode::SUCCESS ;
  }
  // ==========================================================================
  namespace Nodes 
  {
    // ======================================================================
    /** @class Any
     *  the most simple node in the decay tree:
     *  it matches to all valid the LHCb::Particles
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Any : public Decays::iNode
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Any() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Any* clone () const  ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& /* p */ ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid() const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
    } ;
    // ======================================================================
    /** @struct _Node 
     *  Helper structure (esspectially it is a regular node-holder but with 
     *  the default constructor 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    struct _Node : public std::unary_function<LHCb::ParticleID,bool>
    {
    public:
      /// the default constructor 
      _Node () : m_node ( Any() ) {}
      /// the constructor from iNode 
      _Node ( const iNode& node ) : m_node ( node ) {}
      /// the constructor from  Node 
      _Node ( const  Node& node ) : m_node ( node ) {}
      /// Check the validity
      inline bool valid() const 
      { return m_node.node().valid() ; }
      /// The proper validation of the node
      inline StatusCode validate ( const LHCb::IParticlePropertySvc* svc ) const
      { return m_node.node().validate( svc ) ; }
      /// The major method
      inline bool operator() ( const LHCb::ParticleID& pid ) const 
      { return m_node.node( pid ) ; }
      // ====================================================================
    public:
      // ====================================================================
      inline bool operator==( const LHCb::ParticleID& pid ) const 
      { return  m_node.node ( pid ) ; }
      // ====================================================================
      inline bool operator!=( const LHCb::ParticleID& pid ) const 
      { return !m_node.node ( pid ) ; }
      // ====================================================================
    public:
      // ====================================================================
      /// pseudo-assignement operator:
      _Node& operator=( const  Node& right ) 
      {  m_node = right ; return *this ; }
      /// pseudo-assignement from arbitrary node 
      _Node& operator=( const iNode& right ) 
      {  m_node = right ; return *this ; }
      // ====================================================================
    public:
      // ====================================================================
      /// the accessor to the node 
      const Decays::iNode& node() const { return m_node.node() ; }
      /// the cast operator to the node 
      operator const Decays::iNode&() const { return node() ; }
      // ====================================================================
    private:
      // ====================================================================
      /// the node holder itself 
      Decays::Node m_node ; // the node holder itself 
      // ====================================================================
    } ;
    // ======================================================================
  } // end of namespace Decays::Nodes
    // ========================================================================
    /// the actual type of the sequence of nodes 
  typedef std::vector<Decays::Nodes::_Node>                  SubNodes ;
  // ========================================================================
  namespace Nodes 
  {
    // ======================================================================
    /** @class Pid
     *  The simple node in the decay tree:
     *  it matches to a certain particle ID 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Pid : public Decays::iNode 
    {
    public:
      // ====================================================================
      /// constructor from the decay item
      Pid ( const Decays::Decay::Item&     item ) ; 
      /// constructor from Particle Property
      Pid ( const LHCb::ParticleProperty*  item ) ;
      /// constructor from ParticleID
      Pid ( const LHCb::ParticleID&        item ) ;
      /// constructor from Particle name
      Pid ( const std::string&             item ) ;
      /// MANDATORY: virtual destructor
      virtual ~Pid() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Pid* clone () const { return new Pid(*this) ; }
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return check ( pid ) ; }
        /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid () const ;
        /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ====================================================================
    public:
      // ====================================================================
      inline bool check ( const LHCb::ParticleID& pid ) const 
      { return pid == m_item.pid() ; }          
      // ====================================================================        
    public:
      // ====================================================================
      /// get DaVinci Decay item 
      const Decays::Decay::Item&          item() const { return m_item ; }
      /// cast to DaVinci decay item 
      operator const Decays::Decay::Item&     () const { return item() ; }
      // ====================================================================
    private:
      // ====================================================================
      /// the decay item itself 
      Decays::Decay::Item m_item ; // the decay item 
      // ====================================================================
    } ;
    // ======================================================================
    /** @class CC
     *  The simple node in the decay tree:
     *  it matches to a certain particle ID or its antiparticle  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class CC : public Pid
    {
    public:
      // ====================================================================
      /// constructor from the decay item
      CC ( const Decays::Decay::Item&        item ) ; 
      /// constructor from Particle Property
      CC ( const LHCb::ParticleProperty*     item ) ;
      /// constructor from ParticleID
      CC ( const LHCb::ParticleID&           item ) ;
      /// constructor from Particle name
      CC ( const std::string&                item ) ;
      /// constructor from node 
      CC ( const Decays::Nodes::Pid& pid  ) ;
      /// MANDATORY: virtual destructor
      virtual ~CC() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CC* clone () const { return new CC(*this) ; }
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.abspid() == item().pid().abspid() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ====================================================================
    } ;
    // ======================================================================
    /** @class Lepton
     *  The trivial node : it match the Lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Lepton : public Any
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Lepton() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Lepton* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Nu
     *  The trivial node : it match any neutral lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Nu : public Lepton
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Nu() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Nu* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 == pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Ell
     *  The trivial node : it match any charged lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Ell : public Lepton 
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Ell() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Ell* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 != pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class EllPlus
     *  The trivial node : it match any positive lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class EllPlus : public Ell
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~EllPlus() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  EllPlus* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 < pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class EllMinus
     *  The trivial node : it match any negative lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class EllMinus : public  Ell
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~EllMinus() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  EllMinus* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 > pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Hadron
     *  The trivial node : it match the Hadron
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Hadron : public Any 
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Hadron() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Hadron* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.isHadron() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Meson
     *  The trivial node : it match the meson
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Meson : public Hadron
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Meson() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Meson* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.isMeson() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Baryon
     *  The trivial node : it match the Baryon
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Baryon : public Hadron
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Baryon() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Baryon* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.isBaryon() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Charged
     *  The trivial node : it match the Charged
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Charged : public Any
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Charged() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Charged* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 != pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Positive
     *  The trivial node : it match the positively charged  particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Positive : public Charged 
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Positive() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Positive* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 < pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Negative
     *  The trivial node : it match the negatively charged  particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Negative : public Charged
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Negative() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Negative* clone () const  ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 > pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class Neutral
     *  The trivial node : it match the Neutral
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Neutral : public Charged
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Neutral() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Neutral* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 == pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ======================================================================
    /** @class HasQuark
     *  The trivial node : it match the quark content
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class HasQuark : public Any
    {
    public:
      // ====================================================================
      /// constructor from the quark 
      HasQuark ( LHCb::ParticleID::Quark quark ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasQuark() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasQuark* clone () const  ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.hasQuark ( m_quark  ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ====================================================================
    private:
      // ====================================================================
      /// the default construictor is disabled 
      HasQuark() ; // the defautl construictro is disabled 
      // ====================================================================
    private:        
      // ====================================================================
      /// the quark to be tested 
      LHCb::ParticleID::Quark m_quark ; // the quark to be tested 
      // ====================================================================
    } ;
    // ======================================================================
    /** @class JSpin
     *  The trivial node : it match the 2J+1 spin
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class JSpin : public Any
    {
    public:
      // ====================================================================
      /// constructor from the 2J+1
      JSpin ( const int spin )  ;
      /// MANDATORY: virtual destructor
      virtual ~JSpin() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  JSpin* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return m_spin == pid.jSpin () ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ====================================================================
    private:
      // ====================================================================
      /// the default constructor is disabled 
      JSpin() ; // the default constructor is disabled 
      // ====================================================================
    private:        
      // ====================================================================
      /// the j-spin to be tested 
      int m_spin ; // the spin to be tested
      // ====================================================================
    } ;
    // ======================================================================
    /** @class Or
     *  the rather simple (but powerful) node in the decay tree:
     *  it matches .OR. for sub-nodes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Or : public Decays::iNode
    {
    public:
      // ====================================================================
      /// constructor from two nodes 
      Or ( const Decays::iNode& n1 , 
           const Decays::iNode& n2 ) ;
      /// constructor from three nodes 
      Or ( const Decays::iNode& n1 , 
           const Decays::iNode& n2 ,
           const Decays::iNode& n3 ) ;
      /// constructor from four nodes 
      Or ( const Decays::iNode& n1 , 
           const Decays::iNode& n2 ,
           const Decays::iNode& n3 , 
           const Decays::iNode& n4 ) ;
      /// constructor from list of nodes 
      Or ( const Decays::SubNodes& nodes ) ;
      /// MANDATORY: virtual destructor
      virtual ~Or () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Or* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ====================================================================
    public:
      // ====================================================================
      Or& operator+= ( const Decays::iNode& node );
      // ====================================================================
    private:
      // ====================================================================
      /// the default constructor is disabled
      Or () ;// the default constructor is disabled
      // ====================================================================
    private:
      // ====================================================================
      /// the sub-nodes 
      Decays::SubNodes m_nodes ; // the sub-nodes 
      // ====================================================================
    } ;
    // ======================================================================
    /** @class And
     *  the rather simple (but powerful) node in the decay tree:
     *  it matches .AND. for sub-nodes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class And : public Decays::iNode
    {
    public:
      // ====================================================================
      /// constructor from two nodes 
      And ( const Decays::iNode& n1 , 
            const Decays::iNode& n2 ) ;
      /// constructor from three nodes 
      And ( const Decays::iNode& n1 , 
            const Decays::iNode& n2 ,
            const Decays::iNode& n3 ) ;
      /// constructor from four nodes 
      And ( const Decays::iNode& n1 , 
            const Decays::iNode& n2 ,
            const Decays::iNode& n3 , 
            const Decays::iNode& n4 ) ;
      /// constructor from list of nodes 
      And ( const SubNodes& nodes ) ;
      /// MANDATORY: virtual destructor
      virtual ~And () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  And* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& p ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ====================================================================
    public:
      // ====================================================================
      And& operator+= ( const Decays::iNode& node );
      // ====================================================================
    private:
      // ====================================================================
      /// the default constructor is disabled
      And() ;// the default constructor is disabled
      // ====================================================================
    private:
      // ====================================================================
      /// the sub-nodes 
      SubNodes m_nodes ; // the sub-nodes 
      // ====================================================================
    } ;
    // ======================================================================
    /** @class Not 
     *  Simple node whch match "NOT" for the subnode
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Not : public Decays::iNode
    {
    public:
      // ====================================================================
      /// constructor from the node 
      Not ( const Decays::iNode& node )  ;
      /// MANDATORY: virtual destrctir 
      virtual ~Not() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Not* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ====================================================================
    public:
      // ====================================================================
      /// get the underlying node
      const Decays::iNode& node() const { return m_node.node() ; }
      // ====================================================================
    private:
      // ====================================================================
      /// the node itself
      Decays::Node m_node ; // the node itself
      // ====================================================================
    } ;
    // ======================================================================
  } // end of namespace 
  // ==========================================================================
} // end of namespace DaVinci
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator||
( const Decays::iNode& o1 , 
  const Decays::iNode& o2 ) 
{ return Decays::Nodes::Or ( o1 , o2 ) ; }
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator|
( const Decays::iNode& o1 , 
  const Decays::iNode& o2 ) 
{ return Decays::Nodes::Or ( o1 , o2 ) ; }
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::And operator&&
( const Decays::iNode& o1 , 
  const Decays::iNode& o2 ) 
{ return Decays::Nodes::And ( o1 , o2 ) ; }
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::And operator&  
( const Decays::iNode& o1 , 
  const Decays::iNode& o2 ) 
{ return Decays::Nodes::And ( o1 , o2 ) ; }
// ============================================================================
/** Create the "NOT" for the node
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Not operator! 
( const Decays::iNode& o ) 
{ return Decays::Nodes::Not ( o ) ; }
// ============================================================================
/** Create the "NOT" for the node
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Not operator~ 
( const Decays::iNode& o ) 
{ return Decays::Nodes::Not ( o ) ; }
// ============================================================================
/** Create the "NOT" for the node
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Not operator! 
( const Decays::Nodes::Not& o ) 
{ return Decays::Node( o.node() ) ; }
// ============================================================================
/// output operator
inline std::ostream& operator<< 
  ( std::ostream&                     s , 
    const Decays::Nodes::_Node& n ) { return s << n.node() ; }
// ============================================================================


// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const Decays::iNode& o1 , 
  const std::string&   o2 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const Decays::iNode&    o1 , 
  const LHCb::ParticleID& o2 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const Decays::iNode&     o1 , 
  const Decays::Decay::Item& o2 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const Decays::iNode&          o1 , 
  const LHCb::ParticleProperty* o2 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const std::string&   o2 ,
  const Decays::iNode& o1 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const LHCb::ParticleID& o2 ,
  const Decays::iNode&    o1 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const Decays::Decay::Item& o2 ,
  const Decays::iNode&       o1 ) ;
// ============================================================================
/** Create the "OR" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::Or operator||
( const LHCb::ParticleProperty* o2 ,
  const Decays::iNode&          o1 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const Decays::iNode& o1 , 
  const std::string&   o2 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const Decays::iNode&    o1 , 
  const LHCb::ParticleID& o2 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const Decays::iNode&       o1 , 
  const Decays::Decay::Item& o2 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const Decays::iNode&          o1 , 
  const LHCb::ParticleProperty* o2 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const std::string&   o2 ,
  const Decays::iNode& o1 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const LHCb::ParticleID& o2 ,
  const Decays::iNode&    o1 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const Decays::Decay::Item& o2 ,
  const Decays::iNode&     o1 ) ;
// ============================================================================
/** Create the "AND" of two nodes
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
Decays::Nodes::And operator&&
( const LHCb::ParticleProperty* o2 ,
  const Decays::iNode&          o1 ) ;
// ============================================================================


// ============================================================================
/** sequencer operator (the same as "OR" here)
 *
 *  The composed node could be constructed as "sequence" of two other nodes 
 *  (It it equaivalent to the logical OR)
 * 
 *  @code
 * 
 *  const Decays::iNode& node1 = ... ;
 *  const Decays::iNode& node2 = ... ;
 *
 *  Decays::Node node = node1, node2 ;
 *
 *  @endcode 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const Decays::iNode& o1 ,
    const Decays::iNode& o2 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *
 *  The composed node could be constructed as "sequence" of two other nodes 
 *  (It it equaivalent to the logical OR)
 * 
 *  @code
 * 
 *  const Decays::iNode& node1 = ... ;
 *  const Decays::Decay::Item&   item  = ... ;
 *
 *  Decays::Node node = node1, item ;
 *
 *  @endcode 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const Decays::iNode&     o1 ,
    const Decays::Decay::Item& o2 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *
 *  The composed node could be constructed as "sequence" of two other nodes 
 *  (It it equaivalent to the logical OR)
 *     
 *  @code
 * 
 *  const Decays::iNode& node1 = ... ;
 *
 *  Decays::Node node = node1, "B_s0";
 *
 *  @endcode 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const Decays::iNode& o1 ,
    const std::string&   o2 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *
 *  The composed node could be constructed as "sequence" of two other nodes 
 *  (It it equaivalent to the logical OR)
 *     
 *  @code
 * 
 *  const Decays::iNode& node1 = ... ;
 *
 *  Decays::Node node = node1, LHCb::ParticleID(511) ;
 *
 *  @endcode 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const Decays::iNode&    o1 ,
    const LHCb::ParticleID& o2 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *
 *  The composed node could be constructed as "sequence" of two other nodes 
 *  (It it equaivalent to the logical OR)
 *     
 *  @code
 * 
 *  const Decays::iNode& node1 = ... ;
 *  const ParticleProperty*    pp    = ... ;
 *
 *  Decays::Node node = node1, pp ;
 *
 *  @endcode 
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const Decays::iNode&          o1 ,
    const LHCb::ParticleProperty* o2 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const Decays::Decay::Item& o2 ,
    const Decays::iNode&     o1 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const std::string&   o2 ,
    const Decays::iNode& o1 )  { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const LHCb::ParticleID& o2 ,
    const Decays::iNode&    o1 ) { return o1 || o2 ; }
// ============================================================================
/** sequencer operator (the same as "OR" here)
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-04-12
 */
inline Decays::Nodes::Or operator,
  ( const LHCb::ParticleProperty* o2 ,
    const Decays::iNode&          o1 ) { return o1 || o2 ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_DECAYNODES_H
// ============================================================================
