// $Id: GenParticles.h,v 1.19 2008-07-09 16:19:16 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_GENPARTICLES_H 
#define LOKI_GENPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Functions.h"
#include "LoKi/GenTypes.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23 
 */
// ============================================================================
namespace LoKi 
{ 
  /** @namespace LoKi::GenParticles GenParticles.h LoKi/GenParticles.h
   * 
   *  Collection of LoKi functions and predicates to manipulate with 
   *  generator (HepMC) information 
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-26
   */
  namespace GenParticles 
  {    
    // ========================================================================
    /** @class BarCode 
     *  the most primitive function - it return the "barcode" 
     *  of HepMC::GenParticle object
     *
     *  @see LoKi::Cuts::GBAR
     *  @see HepMC::GenParticle
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class BarCode : public LoKi::GenTypes::GFunc 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  BarCode* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~BarCode() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Identifier 
     *  primitive function to extract the 
     *  particle identifier from HepMC::GenParticle
     *
     *   Algorithm: 
     *
     *   int -> ParticleID -> ParticleID::pid() 
     *
     *  @see ParticleID 
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GID
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class Identifier : public LoKi::GenTypes::GFunc 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  Identifier* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Identifier() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class AbsIdentifier 
     *  primitive function to extract the absolute value of 
     *  particle identifier from HepMC::GenParticle
     *  
     *   Algorithm: 
     *
     *   int -> ParticleID -> ParticleID::abspid() 
     *
     *  @see ParticleID 
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GABSID
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class AbsIdentifier : public LoKi::GenTypes::GFunc 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  AbsIdentifier* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~AbsIdentifier() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class Status
     *  the most primitive function - it return the "status" 
     *  of HepMC::GenParticle object
     *
     *  @see LoKi::Cuts::GTATUS
     *  @see HepMC::GenParticle
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class Status : public LoKi::GenTypes::GFunc 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  Status* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Status(){} ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GETA
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PseudoRapidity : public LoKi::GenTypes::GFunc
    {   
    public:
      /// clone method (mandatory!)
      virtual  PseudoRapidity* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~PseudoRapidity();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPHI
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Phi : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  Phi* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Phi();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class Theta
     *  evaluator of the 'Theta' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GTHETA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    class Theta : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  Theta* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Theta();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class Momentum
     *  evaluator of the 'Momentum' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    class Momentum : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  Momentum* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Momentum();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class TransverseMomentum
     *  evaluator of the 'TransverseMomentum' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    class TransverseMomentum : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  TransverseMomentum* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~TransverseMomentum();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Energy
     *  evaluator of the 'Energy' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GE
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    class Energy : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  Energy* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Energy();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class Mass
     *  evaluator of the 'Mass' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GM
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    class Mass : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  Mass* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Mass();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MomentumX
     *  evaluator of the X-component of particle momentum
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPX
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-03
     */
    class MomentumX : public LoKi::GenTypes::GFunc
    {
    public:
      /// clone method (mandatory!)
      virtual  MomentumX* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumX();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MomentumY
     *  evaluator of the Y-component of particle momentum
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPY
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-03
     */
    class MomentumY : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  MomentumY* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumY();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MomentumZ
     *  evaluator of the Z-component of particle momentum
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPZ
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-03
     */
    class MomentumZ : public LoKi::GenTypes::GFunc
    { 
    public:
      /// clone method (mandatory!)
      virtual  MomentumZ* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumZ();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class ValidEndVertex 
     *  primitive predicate to test the validity of 
     *  "end-vertex" for HepMC::GenParticle object 
     *
     *  @see HepMC::GenVertex
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GVEV
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class ValidEndVertex : public LoKi::GenTypes::GCuts
    {
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ValidEndVertex* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~ValidEndVertex() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class MomentumDistance 
     *  Trivial evaluator of euclidian distance 
     *  of 4-momentums useful e.g. for trivial 
     *  "kinematical matching". 
     *  One can find 
     *   the particle with "closest" 4-momentum 
     *   to the given one:
     *
     *  @code 
     *
     *  SEQUENCE            genparticles = ... ;
     *  LoKi::LorentzVector vct       = ... ; 
     *
     *  // find "best match" 
     *  SEQUENCE::const_iterator imin = 
     *    LoKi::select_min( genparticles.begin () , 
     *                      genparticles.end   () , 
     *                      GMOMDIST ( vct )    ) ;
     *
     *  @endcode 
     *
     *  @see HepMC::GenParticle 
     *  @see LoKi::LorentzVector
     *  @see LoKi::Cuts::GMOMDIST
     * 
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class MomentumDistance : public LoKi::GenTypes::GFunc 
    {
    public:
      /// constructor from 4 components
      MomentumDistance
      ( const double px , 
        const double py , 
        const double pz ,
        const double e  ) ;
      /** constructor
       *  @param vct the reference 4-momentum 
       */
      MomentumDistance ( const LoKi::LorentzVector& vct ) ;
      /** copy constructor
       *  @param right object to be copied 
       */
      MomentumDistance ( const MomentumDistance& right ) ;
      /// MANDATORY::virtual destructor 
      virtual ~MomentumDistance() ;
      /// MANDATORY: clone function ("virtual constructor")
      virtual MomentumDistance* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is private
      MomentumDistance() ;
    private:
      LoKi::LorentzVector m_vct ;
    }; 
    // ========================================================================   
    /** @class TransverseMomentumRel
     *  Trivial evaluator of the transverse momenrum, 
     *  relatve to the given direction 
     *
     *  @code 
     *
     *  const HepMC::GenParticle*  genparticle = ... ;
     *  LoKi::ThreeVector  vct  = ... ; 
     *
     *  GFun pt = GPTDIR( vct ) ;
     *  const double pt = pt ( genparticle ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::GPTDIR
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class TransverseMomentumRel : public LoKi::GenTypes::GFunc 
    {
    public:
      /** constructor from theta and phi
       *  @param theta theta angle for the direction 
       *  @param phi phi angle for the direction
       */
      TransverseMomentumRel 
      ( const double theta , 
        const double phi   ) ;
      /** constructor from x,y,z
       *  @param x x-component of the direction vector  
       *  @param y y-component of the direction vector  
       *  @param z z-component of the direction vector  
       */
      TransverseMomentumRel 
      ( const double x , 
        const double y ,
        const double z ) ;
      /** constructor from direction vector
       *  @param vct direction vertor 
       *  @see LoKi::ThreeVector 
       */
      TransverseMomentumRel 
      ( const LoKi::ThreeVector& vct ) ;
      /** constructor from direction vector
       *  @param vct direction vertor 
       *  @see LoKi::LorentzVector 
       */
      TransverseMomentumRel 
      ( const LoKi::LorentzVector& vct ) ;
      /** copy constructor 
       *  @param right object to be copied 
       */
      TransverseMomentumRel 
      ( const TransverseMomentumRel& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TransverseMomentumRel();
      /// MANDATORY: clone function ("virtual constructor")
      virtual TransverseMomentumRel* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      /// default constructor is private
      TransverseMomentumRel() ;
    private:
      LoKi::ThreeVector m_vct ;
    };
    // ========================================================================    
    /** @class FromHepMCTree
     *  simple predicate to test if a certain 
     *  HepMC::GenParticle comes from HepMC graph 
     *  of another HepMC::GenParticle or vertex 
     *  
     *  
     *  @see LoKi::Cuts::GFROMTREE 
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class FromHepMCTree : public LoKi::GenTypes::GCuts 
    {
    public:
      /** constructor from particle ("head")
       *  @param p pointer to HepMC::GenParticle 
       */
      FromHepMCTree 
      ( const HepMC::GenParticle* p ) ;
      /// constructor from vector of particles 
      FromHepMCTree 
      ( const LoKi::GenTypes::GenContainer& r  ) ;
      /// constructor from range of particles 
      FromHepMCTree 
      ( const LoKi::Types::GRange& r  ) ;
      /** constructor from vertex  ("head")
       *  @param v pointer to HepMC::GenParticle 
       */
      FromHepMCTree 
      ( const HepMC::GenVertex*   v ) ;
      /// templated constructor 
      template <class ITERATOR>
      FromHepMCTree 
      ( ITERATOR first , ITERATOR last  ) 
        : LoKi::GenTypes::GCuts ()
      { _add ( first , last ) ; }
      /** copy constructor 
       *  @param right object to be copied 
       */
      FromHepMCTree 
      ( const FromHepMCTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~FromHepMCTree(){} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  FromHepMCTree* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      template <class ITERATOR>
      FromHepMCTree& add ( ITERATOR first , ITERATOR last  ) 
      { _add ( first , last ) ; return *this ; }
      FromHepMCTree& add ( const HepMC::GenParticle*    p ) 
      { _add ( p ) ; return *this  ; }
      FromHepMCTree& add ( const HepMC::GenVertex*      p ) 
      { _add ( p ) ; return *this  ; }
      FromHepMCTree& add ( const LoKi::Types::GRange&   p ) 
      { _add ( p.begin() , p.end() ) ; return *this  ; }
      FromHepMCTree& remove ( const HepMC::GenVertex*   v ) ;
      FromHepMCTree& remove ( const HepMC::GenParticle* v ) ;      
    protected:
      template <class ITERATOR>
      void _add ( ITERATOR first , ITERATOR last  ) 
      { for ( ; first != last ; ++first ) { _add ( *first ) ; } }
      void _add ( const HepMC::GenParticle* p ) ;
      void _add ( const HepMC::GenVertex*   v ) ;
    private:
      // defautl constructor is disabled 
      FromHepMCTree();
    private:
      typedef std::vector<HepMC::GenVertex*> VERTICES ;
      VERTICES m_vertices ;
    };
    // ========================================================================    
    /** @class IsAnAncestor
     *  simple predicate to test if a certain 
     *  HepMC::GenParticle is an ancestor 
     *  of another HepMC::GenParticle
     *  
     *  
     *  @see LoKi::Cuts::GANCESTOR
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class IsAnAncestor : public LoKi::GenTypes::GCuts 
    {
    public:
      /** constructor from particle 
       *  @param p pointer to HepMC::GenParticle 
       */
      IsAnAncestor 
      ( const HepMC::GenParticle* p ) ;
      /** copy constructor 
       *  @param right object to be copied 
       */
      IsAnAncestor
      ( const  IsAnAncestor& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~IsAnAncestor() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsAnAncestor* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // defautl constructor is disabled 
      IsAnAncestor();
    private:
      const HepMC::GenParticle* m_particle ;
    };
    // ========================================================================    
    /** @class HasQuark 
     *  simple predicate , return true if particle has quark 'quark'
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class  HasQuark : public LoKi::GenTypes::GCuts
    {
    public:
      HasQuark (  const LHCb::ParticleID::Quark quark ) ;
      // copy constructor 
      HasQuark (  const HasQuark& right ) ;
      /// clone method (mandatory!)
      virtual HasQuark* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
      LHCb::ParticleID::Quark quark() const { return m_quark ; }
    private:
      HasQuark();
    private :
      LHCb::ParticleID::Quark m_quark ;
    };
    // ========================================================================
    /** @class IsCharged
     *  simple predicate , return true of particle is charged 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class IsCharged : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsCharged* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class IsNeutral
     *  simple predicate , return true of particle is neutral
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class IsNeutral : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsNeutral* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class IsLepton
     *  simple predicate , return true if the particle is a lepton
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */ 
    class IsLepton : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsLepton* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class IsMeson
     *  simple predicate , return true if particle is a meson
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */ 
    class IsMeson : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsMeson* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class IsBaryon
     *  simple predicate , return true if the particle is a baryon
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */ 
    class IsBaryon : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsBaryon* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class IsHadron
     *  simple predicate , return true of particle is hadron
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class IsHadron : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsHadron* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class IsNucleus
     *  simple predicate , return true of particle is nucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class IsNucleus : public LoKi::GenTypes::GCuts
    {
    public:
      /// clone method (mandatory!)
      virtual IsNucleus* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    // ========================================================================
    /** @class ProperLifeTime 
     *  simple function which evaluate the 
     *  proper lifetime (c*tau) of the particle in HepMC units 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    class ProperLifeTime : public LoKi::GenTypes::GFunc
    {
    public:
      /** constructor
       *  @param bad the valut toi be returned for particles
       *  where the evaluation end vertex is not possible 
       */
      ProperLifeTime 
      ( const double bad = LoKi::Constants::InfiniteTime ) ;
      /// copy constructor
      ProperLifeTime 
      ( const  ProperLifeTime& right ) ;
      /// destructor 
      virtual ~ProperLifeTime() ;
      /// clone method (mandatory!)
      virtual  ProperLifeTime* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    private:
      double m_bad ;
    };
    // ========================================================================
    /** @class NominalLifeTime
     *  (c*tau) evaluator of the nominal lifetime of the particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    class NominalLifeTime : public LoKi::GenTypes::GFunc
    {
    public:
      /// clone method (mandatory!)
      virtual NominalLifeTime* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    } ;
    // ========================================================================
    /** @class AdapterToProductionVertex 
     *
     *  Simple adapter, which delegates the evaluation of 
     *  "vertex" funtion to "production vertex" of the particle:
     *   
     *  @see LoKi::Cuts::GFAPVX 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    class AdapterToProductionVertex : public LoKi::GenTypes::GFunc
    {
    public:
      /** constructor from vertex function and "bad" value 
       *  @param fun verted function to be used 
       *  @param bad the value to be returnedd for invalid vertex 
       */
      AdapterToProductionVertex 
      ( const LoKi::Types::GVFunc& fun , 
        const double               bad ) ;
      /// copy constructor 
      AdapterToProductionVertex 
      ( const AdapterToProductionVertex& right ) ;
      /// virtual destructor
      virtual ~AdapterToProductionVertex();
      /// clone method (mandatory!)
      virtual AdapterToProductionVertex* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // default constructor is disabled 
      AdapterToProductionVertex();
      // assignement operator is disabled 
      AdapterToProductionVertex& operator= 
      ( const AdapterToProductionVertex& ) ;
    private:
      LoKi::Types::GVFun    m_fun ;
      double                m_bad ;
    };
    // ========================================================================
    /** @class AdapterToEndVertex 
     *
     *  Simple adapter, which delegates the evaluation of 
     *  "vertex" funtion to "end vertex" of the particle:
     *   
     *  @see LoKi::Cuts::GFAEVX 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    class AdapterToEndVertex : public LoKi::GenTypes::GFunc
    {
    public:
      /** constructor from vertex function and "bad" value 
       *  @param fun verted function to be used 
       *  @param bad the value to be returnedd for invalid vertex 
       */
      AdapterToEndVertex 
      ( const LoKi::Types::GVFunc& fun , 
        const double               bad ) ;
      /// copy constructor 
      AdapterToEndVertex 
      ( const AdapterToEndVertex& right ) ;
      /// virtual destructor
      virtual ~AdapterToEndVertex();
      /// clone method (mandatory!)
      virtual AdapterToEndVertex* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    private:
      // default constructor is disabled 
      AdapterToEndVertex();
      // assignement operator is disabled 
      AdapterToEndVertex& operator= 
      ( const AdapterToEndVertex& ) ;
    private:
      LoKi::Types::GVFun    m_fun ;
      double                m_bad ;
    };
    // ========================================================================
    /** @class ThreeCharge
     *  primitive function to extract the 3*charge from HepMC::GenParticle
     *  
     *   Algorithm: 
     *
     *   pdg_id -> LHCb::ParticleID -> threeCharge 
     *
     *  @see ParticleID 
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::G3Q
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-07
     */
    class ThreeCharge : public LoKi::GenTypes::GFunc 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  ThreeCharge* clone() const { return new ThreeCharge(*this) ; }
      /// MANDATORY: virtual destructor 
      virtual ~ThreeCharge(){} ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class DeltaPhi
     *  Simple evaluator of "DeltaPhi" for the particle
     *
     *  @see LoKi::Cuts::GDPHI 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaPhi : public LoKi::GenTypes::GFunc
    {
    public:
      /// constructor from the angle
      DeltaPhi ( const double phi ) ;
      /// constructor from the vector 
      DeltaPhi ( const LoKi::ThreeVector& v ) ;
      /// constructor from the vector 
      DeltaPhi ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaPhi ( const VECTOR& v ) 
        : LoKi::GenTypes::GFunc () 
        , m_eval (         )
        , m_phi  ( v.phi() )
      { 
        m_phi = adjust ( m_phi ) ;
      } ;
      /// constructor from the particle
      DeltaPhi ( const HepMC::GenParticle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaPhi ( const PARTICLE* p ) 
        : LoKi::GenTypes::GFunc () 
        , m_eval (  )
        , m_phi  (  )
      { 
        if ( 0 == p ) { Exception("Invalid PARTICLE*") ;}
        m_phi = p->momentum().phi() ;
        m_phi = adjust ( m_phi ) ;
      } ;
      /// copy constructor
      DeltaPhi ( const DeltaPhi& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaPhi() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaPhi* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      /// adjust delta phi into the raneg of [-180:180]degrees 
      const double adjust ( double phi ) const ;
    public:
      // get the phi
      double phi() const { return m_phi ; }
    private:
      // the default constructor is disabled 
      DeltaPhi ();
    private:
      // the actual evaluator of phi
      LoKi::GenParticles::Phi m_eval ; ///< the actual evaluator of phi
      // the angle itself 
      double                  m_phi  ; ///< the angle itself 
    } ;
    // ========================================================================
    /** @class DeltaEta
     *  Simple evaluator of "DeltaEta" for the particle
     *
     *  @see LoKi::Cuts::GDETA
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaEta : public LoKi::GenTypes::GFunc
    {
    public:
      /// constructor from the eta
      DeltaEta ( const double eta ) ;
      /// constructor from the vector 
      DeltaEta ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaEta ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaEta ( const VECTOR& v ) 
        : LoKi::GenTypes::GFunc () 
        , m_eval (         )
        , m_eta  ( v.Eta() )
      {} ;
      /// constructor from the particle
      DeltaEta ( const HepMC::GenParticle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaEta ( const PARTICLE* p ) 
        : LoKi::GenTypes::GFunc () 
        , m_eval (  )
        , m_eta  (  )
      { 
        if ( 0 == p ) { Exception("Invalid PARTICLE*") ;}
        m_eta = p->momentum().Eta() ;
      } ;
      /// copy constructor
      DeltaEta ( const DeltaEta& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaEta() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaEta* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      // get the eta
      double eta() const { return m_eta ; }
    private:
      // the default constructor is disabled 
      DeltaEta ();
    private:
      // the actual evaluator of eta
      LoKi::GenParticles::PseudoRapidity m_eval ; ///< the actual evaluator of eta
      // the angle itself 
      double                             m_eta  ; ///< the angle itself 
    } ;
    // ========================================================================
    /** @class DeltaR2
     *  Simple evaluator of "DeltaPhi**2+ DeltaEta**2"
     *
     *  @see LoKi::Cuts::GDR2 
     *  @see LoKi::Cuts::GDELTAR2 
     *  @see LoKi::Cuts::GDPHI
     *  @see LoKi::Cuts::GDETA
     *  @see LoKi::GenParticles::DeltaPhi
     *  @see LoKi::GenParticles::DeltaEta
     *  @see LoKi::GenParticles::Phi
     *  @see LoKi::GenParticles::PseudoRapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class DeltaR2 : public LoKi::GenTypes::GFunc
    {
    public:
      /// constructor from eta & phi
      DeltaR2 ( const double eta , const double phi ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::LorentzVector& v ) ;
      /// templated constructor from vector 
      template <class VECTOR> 
      DeltaR2 ( const VECTOR& v ) 
        : LoKi::GenTypes::GFunc () 
        , m_dphi ( v )
        , m_deta ( v )
      {} ;
      /// constructor from the particle
      DeltaR2 ( const HepMC::GenParticle* p ) ;
      /// templated constructor from particle
      template <class PARTICLE> 
      DeltaR2 ( const PARTICLE* p ) 
        : LoKi::GenTypes::GFunc () 
        , m_dphi ( p )
        , m_deta ( p )
      {} ;
      /// copy constructor
      DeltaR2 ( const DeltaR2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaR2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaR2* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      DeltaR2 ();
    private:
      // the actual evaluator of delta phi
      LoKi::GenParticles::DeltaPhi m_dphi ; ///< the actual evaluator of delta phi
      // the actual evaluator of delta eta
      LoKi::GenParticles::DeltaEta m_deta ; ///< the actual evaluator of delta eta
    } ;
    // ========================================================================
    /** @class NInTree 
     *  simple evaluator of number of particles in the tree 
     *  which satisfy a certain predicate 
     *
     *  @warning current implementation is valid only for 
     *       HepMC::parents, HepMC::children, 
     *       HepMC::ancestors and HepMC::descendants 
     *
     *  @code
     *
     *  // get Generator information
     *  const HepMCEvents* events = 
     *          get<HepMCEvents>( HepMCEventLocation::Default ) ;
     *
     *  typedef std::vector<const HepMC::GenParticle*> GenParticles ;
     *
     *  // select b(and antib) quarks from decay of higgs 
     *  GenParticles bquarks ;
     *  LoKi::Extract::genParticles
     *   ( events                         , 
     *     std::back_inserter( bquarks )  , 
     *     ( "b" == GABSID ) && 
     *    1 == GNINTREE( "H_10" == GABSID , HepMC::parents ) ) ;
     * 
     *  @endcode 
     *   
     *  @see HepMC::IteratorRange 
     *  @see HepMC::parents 
     *  @see HepMC::GenParticle 
     *  @see HepMC::GenVertex 
     *  @see LoKi::Cuts::GABSID 
     *  @see LoKi::Extract::getParticles 
     *  @see LoKi::Cuts::GNINTREE 
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date 2005-03-23 
     */
    class NInTree : public LoKi::GenTypes::GFunc
    {
    public:
      /** constructor 
       *  @param cut    predicate to be used for counting
       *  @param range  "iterator range", see HepMC::IteratorRange
       *  @see HepMC::IteratorRange 
       */
      NInTree ( const LoKi::GenTypes::GCuts& cut                     , 
                HepMC::IteratorRange range = HepMC::children ) ;
      /** copy constructor 
       *  @param right object to be copied 
       */
      NInTree ( const NInTree& right ) ;
      /// MANDATORY: virtual contructor 
      virtual ~NInTree();
      /// MANDATORY: clone function ("virtual constructor")
      virtual NInTree* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    protected:
      /** count the particles in the tree according 
       *  the predicat eand iterator range 
       *  @see HepMC::GenVertex
       *  @param vertex  root of the tree 
       *  @return number of particles 
       */
      size_t count( HepMC::GenVertex* vertex ) const ;
    private:
      // default constructor is disabled 
      NInTree();
    private:
      LoKi::GenTypes::GCut  m_cut   ;
      HepMC::IteratorRange  m_range ;
    } ;
    // ========================================================================
    /** @class InTree
     *  The trivial predicate which evaluates to true 
     *  if there is at least one particle in the decay 
     *  tree of the given particle which satisfies the 
     *  certain criteria
     *
     *  The function uses the algorithm LoKi::GenAlgs::found 
     *
     *  @see LoKi::GenAlgs::found 
     *  @see LoKi::Cuts::GINTREE 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2004-05-05
     */
    class InTree : public LoKi::GenTypes::GCuts 
    {
    public:
      /** standard constructor 
       *  @param cut cut to be checked 
       */
      InTree  ( const LoKi::GenTypes::GCuts& cut ) ;
      /// copy constructor 
      InTree  ( const InTree& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~InTree (){};
      /// MANDATORY: clone function ("virtual constructor")
      virtual  InTree*       clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // default constructor is disabled 
      InTree () ; ///< default constructor is disabled 
    private:
      // the criteria itself:
      LoKi::GenTypes::GCut m_cut       ; ///< the criteria itself:
    } ;    
    // ========================================================================
    /** @class Oscillated 
     *  Simple class to check the oscillation of the particle 
     *  It evaluates to "true" for both particles 
     *  @see LoKi::GenParticles::oscillated1 
     *  @see LoKi::GenParticles::oscillated2
     *  @see LoKi::Cuts::GOSCILLATED 
     *  @author Vanya BELYAEV Ivan.Belyaev@nkhef.nl
     *  @date 2008-07-03
     */
    class Oscillated : public LoKi::GenTypes::GCuts 
    {
    public:
      /// MANDATORY: virtual desctructor 
      virtual ~Oscillated() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Oscillated* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;      
    } ;
  } // end of namespace GenParticles  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENPARTICLES_H
// ============================================================================
