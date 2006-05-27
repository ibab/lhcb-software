// $Id: GenParticles.h,v 1.8 2006-05-27 11:38:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.8 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_GENPARTICLES_H 
#define LOKI_GENPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// HepMC
// ============================================================================
#include "HepMC/GenParticle.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Functions.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenExtract.h"
// ============================================================================

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
  /** @namespace  GenParticles GenParticles.h LoKi/GenParticles.h
   * 
   *  Collection of LoKi functions and predicates to manipulate with 
   *  generator (HepMC) information 
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-26
   */
  namespace GenParticles 
  {    
    /** @struct BarCode 
     *  the most primitive function - it return the "barcode" 
     *  of HepMC::GenParticle object
     *
     *  @see LoKi::Cuts::GBAR
     *  @see HepMC::GenParticle
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    struct BarCode : 
      public LoKi::Function<const HepMC::GenParticle*> 
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
    
    /** @struct Identifier 
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
    struct Identifier : 
      public LoKi::Function<const HepMC::GenParticle*> 
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
    
    /** @struct AbsIdentifier 
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
    struct AbsIdentifier : 
      public LoKi::Function<const HepMC::GenParticle*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  AbsIdentifier* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~AbsIdentifier() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct PseudoRapidity
     *  evaluator of the seudorapidity of the particle 
     *
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GETA
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct PseudoRapidity : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  PseudoRapidity* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~PseudoRapidity();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct Phi
     *  evaluator of the 'Phi' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPHI
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct Phi : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  Phi* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Phi();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct Theta
     *  evaluator of the 'Theta' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GTHETA
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    struct Theta : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  Theta* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Theta();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct Momentum
     *  evaluator of the 'Momentum' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    struct Momentum : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  Momentum* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Momentum();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct TransverseMomentum
     *  evaluator of the 'TransverseMomentum' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPP
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    struct TransverseMomentum : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  TransverseMomentum* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~TransverseMomentum();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct Energy
     *  evaluator of the 'Energy' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GE
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    struct Energy : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  Energy* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Energy();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct Mass
     *  evaluator of the 'Mass' of the particle 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GM
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-17
     */
    struct Mass : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  Mass* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~Mass();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct MomentumX
     *  evaluator of the X-component of particle momentum
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPX
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-03
     */
    struct MomentumX : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  MomentumX* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumX();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct MomentumY
     *  evaluator of the Y-component of particle momentum
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPY
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-03
     */
    struct MomentumY : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  MomentumY* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumY();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct MomentumZ
     *  evaluator of the Z-component of particle momentum
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPZ
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-03-03
     */
    struct MomentumZ : 
      public LoKi::Function<const HepMC::GenParticle*>
    {      
      /// clone method (mandatory!)
      virtual  MomentumZ* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumZ();
      /// the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };


    /** @struct ValidEndVertex 
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
    struct ValidEndVertex :
      public LoKi::Predicate<const HepMC::GenParticle*> 
    {
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ValidEndVertex* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~ValidEndVertex() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

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
    class MomentumDistance : 
      public LoKi::Function<const HepMC::GenParticle*> 
    {
    public:
      /** constructor
       *  @param vct the reference 4-momentum 
       */
      MomentumDistance( const LoKi::LorentzVector& vct ) ;
      /** copy constructor
       *  @param right object to be copied 
       */
      MomentumDistance( const MomentumDistance& right ) ;
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
    
    /** @class NinTree 
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
    class NInTree : 
      public LoKi::Function<const HepMC::GenParticle*>
    {
    public:
      typedef LoKi::Predicate<const HepMC::GenParticle*> GCut ;
    protected:
      typedef LoKi::PredicateFromPredicate<const HepMC::GenParticle*> _Cut ;
    public:
      /** constructor 
       *  @param cut    predicate to be used for counting
       *  @param range  "iterator range", see HepMC::IteratorRange
       *  @see HepMC::IteratorRange 
       */
      NInTree ( const NInTree::GCut& cut                     , 
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
      _Cut                  m_cut   ;
      HepMC::IteratorRange  m_range ;
    };

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
    class TransverseMomentumRel :
      public LoKi::Function<const HepMC::GenParticle*> 
    {
    public:
      /** constructor
       *  @param vct direction vertor 
       *  @see LoKi::ThreeVector 
       */
      TransverseMomentumRel 
      ( const LoKi::ThreeVector& vct ) ;
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
    class FromHepMCTree 
      : public LoKi::Predicate<const HepMC::GenParticle*>
    {
    public:
      /** constructor from particle ("head")
       *  @param p pointer to HepMC::GenParticle 
       */
      FromHepMCTree 
      ( const HepMC::GenParticle* p ) ;
      /** constructor from vertex  ("head")
       *  @param v pointer to HepMC::GenParticle 
       */
      FromHepMCTree 
      ( const HepMC::GenVertex*   v ) ;
      /// templated constructor 
      template <class ITERATOR>
      FromHepMCTree 
      ( ITERATOR first , ITERATOR last  ) 
        : LoKi::Predicate<const HepMC::GenParticle*>()
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
      FromHepMCTree& add ( const HepMC::GenParticle* p ) 
      { _add ( p ) ; return *this  ; }
      FromHepMCTree& add ( const HepMC::GenVertex*   p ) 
      { _add ( p ) ; return *this  ; }
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
    class IsAnAncestor : 
      public LoKi::Predicate<const HepMC::GenParticle*> 
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
    
    /** @class HasQuark 
     *  simple predicate , return true if particle has quark 'quark'
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class  HasQuark  : 
      public LoKi::Predicate<const HepMC::GenParticle*>
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

    /** @class IsCharged
     *  simple predicate , return true of particle is charged 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    struct IsCharged : 
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsCharged* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsNeutral
     *  simple predicate , return true of particle is neutral
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    struct IsNeutral : 
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsNeutral* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsLepton
     *  simple predicate , return true if the particle is a lepton
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */ 
    struct IsLepton : 
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsLepton* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsMeson
     *  simple predicate , return true if particle is a meson
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */ 
    struct IsMeson : 
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsMeson* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class IsBaryon
     *  simple predicate , return true if the particle is a baryon
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */ 
    struct IsBaryon : 
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsBaryon* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };


    /** @class IsHadron
     *  simple predicate , return true of particle is hadron
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    struct IsHadron :
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsHadron* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
  
    /** @class IsNucleus
     *  simple predicate , return true of particle is nucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    struct IsNucleus : 
      public LoKi::Predicate<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual IsNucleus* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    };
    
    /** @class ProperLifeTime 
     *  simple function which evaluate the 
     *  proper lifetime (c*tau) of the particle in HepMC units 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    class ProperLifeTime :
      public LoKi::Function<const HepMC::GenParticle*>
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
    
    /** @struct NominalLifeTime (c*tau) 
     *  evaluator of the nominal lifetime of the particle 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    struct NominalLifeTime 
      : LoKi::Function<const HepMC::GenParticle*>
    {
      /// clone method (mandatory!)
      virtual NominalLifeTime* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    } ;

    /** @class AdapterToProductionVertex 
     *
     *  Simple adapter, which delegates the evaluation of 
     *  "vertex" funtion to "production vertex" of the particle:
     *   
     *  @see LoKi::Cuts::GFAPVX 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    class AdapterToProductionVertex 
      : LoKi::Function<const HepMC::GenParticle*>
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
    
    /** @class AdapterToEndVertex 
     *
     *  Simple adapter, which delegates the evaluation of 
     *  "vertex" funtion to "end vertex" of the particle:
     *   
     *  @see LoKi::Cuts::GFAEVX 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-08
     */
    class AdapterToEndVertex 
      : LoKi::Function<const HepMC::GenParticle*>
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
    

    /** @struct ThreeCharge
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
    struct ThreeCharge : 
      public LoKi::Function<const HepMC::GenParticle*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  ThreeCharge* clone() const { return new ThreeCharge(*this) ; }
      /// MANDATORY: virtual destructor 
      virtual ~ThreeCharge(){} ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, see @LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    };

  }; // end of namespace GenParticles
  
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENPARTICLES_H
// ============================================================================
