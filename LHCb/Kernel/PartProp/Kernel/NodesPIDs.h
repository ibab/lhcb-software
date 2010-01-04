// $Id: NodesPIDs.h,v 1.4 2010-01-04 16:43:48 ibelyaev Exp $
// ============================================================================
#ifndef PARTPROP_NODESPID_H 
#define PARTPROP_NODESPID_H 1
// ============================================================================
// Include files
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
// ============================================================================
/** @file  Kernel/NodePIDs.h
 *  The actual implementation of various decay nodes 
 *  @see Decays::iNode 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl 
 *  @date 2008-04-21
 */  
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Nodes 
  {
    // ========================================================================
    /** @class Any
     *  the most simple node in the decay tree:
     *  it matches to all valid the LHCb::Particles
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Any : public Decays::iNode
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Any() ;
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
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Decays::Nodes
  // ==========================================================================
  namespace Nodes 
  {
    // ========================================================================
    /** @class Pid
     *  The simple node in the decay tree:
     *  it matches to a certain particle ID 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Pid : public Decays::iNode 
    {
    public:
      // ======================================================================
      /// constructor from the decay item
      Pid ( const Decays::Decay::Item&     item ) ; 
      /// constructor from Particle Property
      Pid ( const LHCb::ParticleProperty*  item ) ;
      /// constructor from ParticleID
      Pid ( const LHCb::ParticleID&        item ) ;
      /// constructor from Particle name
      Pid ( const std::string&             item ) ;
      /// MANDATORY: virtual destructor
      virtual ~Pid () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Pid* clone () const  ;
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
      // ======================================================================
    public:
      // ======================================================================
      inline bool check ( const LHCb::ParticleID& pid ) const 
      { return pid == m_item.pid() ; }          
      // ======================================================================
    public:
      // ======================================================================
      /// get DaVinci Decay item 
      const Decays::Decay::Item&          item() const { return m_item ; }
      /// cast to DaVinci decay item 
      operator const Decays::Decay::Item&     () const { return item() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the decay item itself 
      Decays::Decay::Item m_item ;                            // the decay item 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CC
     *  The simple node in the decay tree:
     *  it matches to a certain particle ID or its antiparticle  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class CC : public Pid
    {
    public:
      // ======================================================================
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
      virtual ~CC () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CC* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.abspid() == item().pid().abspid() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Lepton
     *  The trivial node : it match the Lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Lepton : public Any
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Lepton() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Lepton* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Nu
     *  The trivial node : it match any neutral lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Nu : public Lepton
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Nu() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Nu* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 == pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Ell
     *  The trivial node : it match any charged lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Ell : public Lepton 
    {
    public:
      /// MANDATORY: virtual destructor
      virtual ~Ell() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Ell* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 != pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    } ;
    // ========================================================================
    /** @class EllPlus
     *  The trivial node : it match any positive lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class EllPlus : public Ell
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~EllPlus() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  EllPlus* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 < pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class EllMinus
     *  The trivial node : it match any negative lepton
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class EllMinus : public  Ell
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~EllMinus() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  EllMinus* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isLepton() && ( 0 > pid.threeCharge() ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Hadron
     *  The trivial node : it match the Hadron
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Hadron : public Any 
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Hadron() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Hadron* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.isHadron() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Meson
     *  The trivial node : it match the meson
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Meson : public Hadron
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Meson() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Meson* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.isMeson() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Baryon
     *  The trivial node : it match the Baryon
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Baryon : public Hadron
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Baryon() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Baryon* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.isBaryon() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Charged
     *  The trivial node : it match the Charged
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Charged : public Any
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Charged () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Charged* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 != pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Positive
     *  The trivial node : it match the positively charged  particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Positive : public Charged 
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Positive () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Positive* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 < pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Negative
     *  The trivial node : it match the negatively charged  particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Negative : public Charged
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Negative () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Negative* clone () const  ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 > pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Neutral
     *  The trivial node : it match the Neutral
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Neutral : public Charged
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Neutral() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Neutral* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return 0 == pid.threeCharge() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Nucleus
     *  The trivial node : it match the Nucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class Nucleus : public Any
    {
    public:
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Nucleus()  ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Nucleus* clone () const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const
      { return pid.isNucleus() ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HasQuark
     *  The trivial node : it match the quark content
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class HasQuark : public Any
    {
    public:
      // ======================================================================
      /// constructor from the quark 
      HasQuark ( LHCb::ParticleID::Quark quark ) ;
      /// MANDATORY: virtual destructor
      virtual ~HasQuark() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasQuark* clone () const  ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return pid.hasQuark ( m_quark  ) ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      HasQuark() ;                       // the default constructro is disabled 
      // ======================================================================
    private:        
      // ======================================================================
      /// the quark to be tested 
      LHCb::ParticleID::Quark m_quark ; // the quark to be tested 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class JSpin
     *  The trivial node : it match the 2J+1 spin
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class JSpin : public Any
    {
    public:
      // ======================================================================
      enum { InvalidSpin = 501 } ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the 2J+1
      JSpin ( const int spin )  ;
      /// MANDATORY: virtual destructor
      virtual ~JSpin() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  JSpin* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return spin() == pid.jSpin () ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// valid only for positive spin-values  
      virtual bool valid() const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* /* svc */ ) const ;
      // ======================================================================
    public:
      // ======================================================================
      int spin() const { return m_spin ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      JSpin() ;                          // the default constructor is disabled 
      // ======================================================================
    private:        
      // ======================================================================
      /// the j-spin to be tested 
      int m_spin ;                                     // the spin to be tested
      // ======================================================================
    } ;
    // ========================================================================
    /** @class SSpin
     *  The trivial node : it match the 2S+1 spin
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class SSpin : public JSpin
    {
    public:
      // ======================================================================
      /// constructor from the 2S+1
      SSpin ( const int spin )  ;
      /// MANDATORY: virtual destructor
      virtual ~SSpin() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SSpin* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return spin() == pid.sSpin () ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SSpin() ;                          // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LSpin
     *  The trivial node : it match the 2L+1 spin
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-12
     */
    class LSpin : public SSpin
    {
    public:
      // ======================================================================
      /// constructor from the 2L+1
      LSpin ( const int spin )  ;
      /// MANDATORY: virtual destructor
      virtual ~LSpin() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LSpin* clone () const ; 
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const 
      { return spin() == pid.lSpin () ; }
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      LSpin() ;                          // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CTau 
     *  simple pid-checker for particle lifetime (in c*tau units)
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class CTau : public Decays::iNode 
    {
    public:
      // ======================================================================
      /// constructor from c-tau range 
      CTau ( const double                      low      , 
             const double                      high     , 
             const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY : virtual destructor 
      virtual ~CTau() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CTau* clone() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ======================================================================
    public:
      // ======================================================================
      const LHCb::IParticlePropertySvc* ppSvc() const { return m_ppSvc ; }
      // ======================================================================
      /// get low  edge  
      double low  () const { return m_low  ; }
      /// get high edge  
      double high () const { return m_high ; }
      // ======================================================================      
      StatusCode setService ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ======================================================================      
    private:
      // ======================================================================
      /// the default constructor is disabled 
      CTau() ;                           // the default constructor is disabled
      // ======================================================================      
    public:
      // ======================================================================
      typedef std::vector<LHCb::ParticleID>                              PIDs ;
      const PIDs& accepted () const { return m_accepted ; }
      const PIDs& rejected () const { return m_rejected ; }      
      // ======================================================================      
    protected:
      // ======================================================================
      void addToAccepted ( const LHCb::ParticleID& pid ) const ;
      void addToRejected ( const LHCb::ParticleID& pid ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      typedef SmartIF<LHCb::IParticlePropertySvc> Service ;
      /// the service :
      mutable Service m_ppSvc ;                                  // the service 
      // ======================================================================
      /// the list of "accepted" pids 
      mutable PIDs  m_accepted ;                // the list of "accepted" pids 
      /// the list of "rejected" pids 
      mutable PIDs  m_rejected  ;                // the list of "rejected" pids 
      // ======================================================================      
      /// low edge 
      double m_low  ;                                              //  low edge 
      /// high edge 
      double m_high ;                                              // high edge
      // ======================================================================      
    } ;    
    // ========================================================================
    /** @class ShortLived_ 
     *  represent simple predicate for short-lived particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class ShortLived_ : public CTau 
    {
    public:
      // ======================================================================
      /// constructor with high edge and service 
      ShortLived_ ( const double                      high     , 
                    const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// constructor service 
      ShortLived_ ( const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ShortLived_() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ShortLived_* clone() const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class LongLived_ 
     *  represent simple predicate for Long-lived particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class LongLived_ : public CTau 
    {
    public:
      // ======================================================================
      /// constructor with high edge and service 
      LongLived_ ( const double                      high     , 
                   const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// constructor service 
      LongLived_ ( const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~LongLived_() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LongLived_* clone() const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Stable 
     *  represent simple predicate for Stable particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class Stable : public LongLived_
    {
    public:
      // ======================================================================
      /// constructor service 
      Stable ( const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Stable() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Stable* clone() const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class StableCharged
     *  represent simple predicate for Stable+Charged particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class StableCharged : public Stable
    {
    public:
      // ======================================================================
      /// constructor service 
      StableCharged ( const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~StableCharged () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  StableCharged* clone() const ;
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Mass
     *  simple pid-checker for particle mass 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class Mass : public CTau
    {
    public:
      // ======================================================================
      /// constructor from mass-range 
      Mass ( const double                      low      , 
             const double                      high     , 
             const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Mass() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Mass* clone() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      /// the default constructor is disabled 
      Mass() ;                           // the default constructor is disabled
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Light
     *  simple pid-checker for particle mass 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class Light : public Mass
    {
    public:
      // ======================================================================
      /// constructor from mass-range 
      Light ( const double                      high     , 
              const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Light() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Light* clone() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      /// the default constructor is disabled 
      Light() ;                          // the default constructor is disabled
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Heavy
     *  simple pid-checker for particle mass 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-11
     */
    class Heavy : public Mass
    {
    public:
      // ======================================================================
      /// constructor from mass-range 
      Heavy ( const double                      low      , 
              const LHCb::IParticlePropertySvc* svc  = 0 ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Heavy() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Heavy* clone() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      /// the default constructor is disabled 
      Heavy () ;                         // the default constructor is disabled
      // ======================================================================      
    } ;
    // ========================================================================
    class Symbol : public Decays::iNode
    {      
    public :
      // =======================================================================
      /// the constructor from the symbol 
      Symbol ( const std::string& sym ) ;
      // =======================================================================
      /// MANDATORY : virtual destructor 
      virtual ~Symbol() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Symbol* clone() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// MANDATORY: the only one essential method
      virtual bool operator() ( const LHCb::ParticleID& pid ) const ;
      /// MANDATORY: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      /// MANDATORY: check the validity
      virtual bool valid () const ;
      /// MANDATORY: the proper validation of the node
      virtual StatusCode validate 
      ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Symbol () ;                        // the default constructor is disabled
      // ======================================================================      
    private :
      // ======================================================================
      /// the actual symbol 
      Decays::Node m_symbol ;
      // ======================================================================
    };    
    // ========================================================================
  } // end of namespace 
  // ==========================================================================
} // end of namespace DaVinci
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_NODESPID_H
// ============================================================================
