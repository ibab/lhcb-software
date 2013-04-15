// $Id$
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
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Kinematics.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/Functions.h"
#include "LoKi/GenTypes.h"
#include "LoKi/iTree.h"
#include "LoKi/GenChildSelector.h"
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
 *  By usage of this code one clearly states the disagreement
 *  with the smear campaign of Dr.O.Callot et al.:
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-01-23
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
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
    class GAUDI_API BarCode : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      BarCode() {}
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  BarCode* clone() const ;
      /// MANDATORY: virtual destructor
      virtual ~BarCode() ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API Identifier : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Identifier() {}
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  Identifier* clone() const ;
      /// MANDATORY: virtual destructor
      virtual ~Identifier() ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API AbsIdentifier : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      AbsIdentifier() {}
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  AbsIdentifier* clone() const ;
      /// MANDATORY: virtual destructor
      virtual ~AbsIdentifier() ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API Status : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Status() {}
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  Status* clone() const ;
      /// MANDATORY: virtual destructor
      virtual ~Status(){} ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API ValidEndVertex : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      ValidEndVertex() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ValidEndVertex* clone() const ;
      /// MANDATORY: virtual destructor
      virtual ~ValidEndVertex() ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API MomentumDistance : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is private
      MomentumDistance() ;
      // ======================================================================
    private:
      // ======================================================================
      LoKi::LorentzVector m_vct ;
      // ======================================================================
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
    class GAUDI_API TransverseMomentumRel : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private
      TransverseMomentumRel() ;
      // ======================================================================
    private:
      // ======================================================================
      LoKi::ThreeVector m_vct ;
      // ======================================================================
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
    class GAUDI_API FromHepMCTree : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
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
      // ======================================================================
    public:
      // ======================================================================
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
      // ======================================================================
    protected:
      // ======================================================================
      template <class ITERATOR>
      void _add ( ITERATOR first , ITERATOR last  )
      { for ( ; first != last ; ++first ) { _add ( *first ) ; } }
      void _add ( const HepMC::GenParticle* p ) ;
      void _add ( const HepMC::GenVertex*   v ) ;
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is disabled
      FromHepMCTree();
      // ======================================================================
    private:
      // ======================================================================
      typedef std::vector<HepMC::GenVertex*> VERTICES ;
      VERTICES m_vertices ;
      // ======================================================================
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
    class GAUDI_API IsAnAncestor : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      // defautl constructor is disabled
      IsAnAncestor();
      // ======================================================================
    private:
      // ======================================================================
      const HepMC::GenParticle* m_particle ;
      // ======================================================================
    };
    // ========================================================================
    /** @class HasQuark
     *  simple predicate , return true if particle has quark 'quark'
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class  GAUDI_API HasQuark : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      HasQuark();
      // ======================================================================
    private :
      // ======================================================================
      LHCb::ParticleID::Quark m_quark ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsCharged
     *  simple predicate , return true of particle is charged
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class GAUDI_API IsCharged : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsCharged() {}
      /// clone method (mandatory!)
      virtual IsCharged* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsNeutral
     *  simple predicate , return true of particle is neutral
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class GAUDI_API IsNeutral : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsNeutral() {}
      /// clone method (mandatory!)
      virtual IsNeutral* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsLepton
     *  simple predicate , return true if the particle is a lepton
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class GAUDI_API IsLepton : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsLepton() {}
      /// clone method (mandatory!)
      virtual IsLepton* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsMeson
     *  simple predicate , return true if particle is a meson
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class GAUDI_API IsMeson : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsMeson() {}
      /// clone method (mandatory!)
      virtual IsMeson* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsBaryon
     *  simple predicate , return true if the particle is a baryon
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class GAUDI_API IsBaryon : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsBaryon() {}
      /// clone method (mandatory!)
      virtual IsBaryon* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsHadron
     *  simple predicate , return true of particle is hadron
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2006-01-18
     */
    class GAUDI_API IsHadron : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsHadron() {}
      /// clone method (mandatory!)
      virtual IsHadron* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class IsNucleus
     *  simple predicate , return true of particle is nucleus
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-01-25
     */
    class GAUDI_API IsNucleus : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      IsNucleus() {}
      /// clone method (mandatory!)
      virtual IsNucleus* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class ProperLifeTime
     *  simple function which evaluate the
     *  proper lifetime (c*tau) of the particle in HepMC units
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    class GAUDI_API ProperLifeTime : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      double m_bad ;
      // ======================================================================
    };
    // ========================================================================
    /** @class NominalLifeTime
     *  (c*tau) evaluator of the nominal lifetime of the particle
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    class GAUDI_API NominalLifeTime : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      NominalLifeTime() {}
      /// clone method (mandatory!)
      virtual NominalLifeTime* clone() const ;
      /// the only one essential method
      result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API AdapterToProductionVertex : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
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
      // ======================================================================
   private:
      // ======================================================================
      // default constructor is disabled
      AdapterToProductionVertex();
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Types::GVFun    m_fun ;
      double                m_bad ;
      // ======================================================================
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
    class GAUDI_API AdapterToEndVertex : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is disabled
      AdapterToEndVertex();
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Types::GVFun    m_fun ;
      double                m_bad ;
      // ======================================================================
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
    class GAUDI_API ThreeCharge : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      ThreeCharge() {}
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  ThreeCharge* clone() const { return new ThreeCharge(*this) ; }
      /// MANDATORY: virtual destructor
      virtual ~ThreeCharge(){} ;
      /// MANDATORY: the only one essential method
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
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
    class GAUDI_API NInTree : public LoKi::GenTypes::GFunc
    {
    public:
      // ======================================================================
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
      // ======================================================================
    protected:
      // ======================================================================
      /** count the particles in the tree according
       *  the predicat eand iterator range
       *  @see HepMC::GenVertex
       *  @param vertex  root of the tree
       *  @return number of particles
       */
      size_t count( HepMC::GenVertex* vertex ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      NInTree() ;                            // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      LoKi::GenTypes::GCut  m_cut   ;
      HepMC::IteratorRange  m_range ;
      // ======================================================================
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
    class GAUDI_API InTree : public LoKi::GenTypes::GCuts
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
      // ======================================================================
      /// default constructor is disabled
      InTree () ;                            // default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the criteria itself:
      LoKi::GenTypes::GCut m_cut ;                       // the criteria itself
      // ======================================================================
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
    class GAUDI_API Oscillated : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Oscillated() {}
      /// MANDATORY: virtual desctructor
      virtual ~Oscillated() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Oscillated* clone() const ;
      /// MANDATORY: the only one essential method
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Oscillated1
     *  Simple class to check the oscillation of the particle
     *  @see LoKi::GenParticles::oscillated1
     *  @see LoKi::Cuts::GOSCILLATED1
     *  @author Vanya BELYAEV Ivan.Belyaev@nkhef.nl
     *  @date 2008-07-03
     */
    class Oscillated1 : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Oscillated1() {}
      /// MANDATORY: virtual desctructor
      virtual ~Oscillated1() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Oscillated1* clone() const ;
      /// MANDATORY: the only one essential method
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Oscillated2
     *  Simple class to check the oscillation of the particle
     *  @see LoKi::GenParticles::oscillated2
     *  @see LoKi::Cuts::GOSCILLATED2
     *  @author Vanya BELYAEV Ivan.Belyaev@nkhef.nl
     *  @date 2008-07-03
     */
    class GAUDI_API Oscillated2 : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /// MANDATORY: default constructor
      Oscillated2() {}
      /// MANDATORY: virtual desctructor
      virtual ~Oscillated2() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual Oscillated2* clone() const ;
      /// MANDATORY: the only one essential method
      virtual  result_type   operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DecNode
     *  simple predicate, which acts on the particleID of the particle
     *  @see LHCb::ParticleID
     *  @see Decays::iNode
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    class GAUDI_API DecNode
      : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the actual node
      DecNode ( const Decays::iNode& node ) ;
      /// MANDATORY: virtual destructor
      virtual ~DecNode() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecNode* clone() const { return new DecNode ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DecNode () ;                       // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get the decay node
      const Decays::iNode& node() const { return m_node.node () ; }
      /// valid node ?
      bool valid() const { return m_node.valid() ; }
      // validate the node
      StatusCode validate ( const LHCb::IParticlePropertySvc* svc ) const
      { return m_node.validate ( svc ) ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the decay node itself
      Decays::Node m_node ;                            // the decay node itself
      // ======================================================================
    };
    // ========================================================================
    /** @class DecTree
     *  simple predicate, which acts on the decay structuire/tree for the particle
     *  @see Decays::iTree
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-12-17
     */
    class GAUDI_API DecTree
      : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate
    {
    public:
      // ======================================================================
      /// the actual type of tree (interface
      typedef Decays::iTree_<const HepMC::GenParticle*> iTree ; // the tree
      // ======================================================================
    private:
      // ======================================================================
      /// the actual type of tree (assignable)
      typedef Decays::Tree_<const HepMC::GenParticle*>   Tree ; // the tree
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the actual node
      DecTree ( const iTree& node , const bool autovalidate = true ) ;
      /// constructor from the decay descriptor
      DecTree ( const std::string& descriptor ) ;
      /// MANDATORY: virtual destructor
      virtual ~DecTree() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual DecTree* clone() const { return new DecTree ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      DecTree () ;                       // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get the decay node
      const iTree& tree() const { return m_tree.tree () ; }
      // ======================================================================
      /// valid tree ?
      bool valid () const { return m_tree.valid() ; }
      // validate the teh tree
      StatusCode validate ( const LHCb::IParticlePropertySvc* svc ) const
      { return m_tree.validate ( svc ) ; }
      // reset the collection
      void reset() const { m_tree.reset() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the decay tree itself
      Tree m_tree ;                                    // the decay tree itself
      //
      bool m_autovalidate ;
      // ======================================================================
    };
    // ========================================================================
    /** @class ChildFun
     *  adapter to child-function
     *  @see LoKi::Cuts::GCHILD
     *  @see LoKi::Cuts::GCHILDFUN
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-12-11
     */
    class GAUDI_API ChildFun
      : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Function
    {
    public:
      // ======================================================================
      /** constructor from the function and daughter index
       *  @param fun    the function to be used
       *  @param index  the index of daughter particle
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc& fun   ,
        const unsigned int           index ) ;
      /** constructor from the function and daughter index
       *  @param fun    the function to be used
       *  @param index  the index of daughter particle
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc& fun    ,
        const unsigned int           index1 ,
        const unsigned int           index2 ) ;
      /** constructor from the function and daughter index
       *  @param fun    the function to be used
       *  @param index  the index of daughter particle
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc& fun         ,
        const std::vector<unsigned int>& indices ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc&    fun       ,
        const LoKi::GenChild::Selector& selector  ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc& fun       ,
        const std::string&           selector  ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc&    fun      ,
        const Decays::IGenDecay::iTree& selector ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc&  fun      ,
        const Decays::iNode&          selector ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildFun
      ( const LoKi::GenTypes::GFunc& fun      ,
        const LoKi::GenTypes::GCuts& selector ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~ChildFun () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildFun*  clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the function itself
      LoKi::GenTypes::GFun     m_fun   ;                 // the function itself
      /// the child selector
      LoKi::GenChild::Selector m_child ;                 //  the child selector
      // ======================================================================
    };
    // ========================================================================
    /** @class ChildCut
     *  adapter to child-predicate
     *  @see LoKi::Cuts::GCHILDCUT
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-12-11
     */
    class GAUDI_API ChildCut
      : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate
    {
    public:
      // ======================================================================
      /** constructor from the function and daughter index
       *  @param fun    the function to be used
       *  @param index  the index of daughter particle
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts& fun   ,
        const unsigned int           index ) ;
      /** constructor from the function and daughter index
       *  @param fun    the function to be used
       *  @param index  the index of daughter particle
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts& fun    ,
        const unsigned int           index1 ,
        const unsigned int           index2 ) ;
      /** constructor from the function and daughter index
       *  @param fun    the function to be used
       *  @param index  the index of daughter particle
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts& fun         ,
        const std::vector<unsigned int>& indices ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts&    fun       ,
        const LoKi::GenChild::Selector& selector  ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts& fun       ,
        const std::string&           selector  ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts&    fun      ,
        const Decays::IGenDecay::iTree& selector ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts&  fun      ,
        const Decays::iNode&          selector ) ;
      /** constructor from the function and child selector
       *  @param fun      the function to be used
       *  @param selector the child selector
       */
      ChildCut
      ( const LoKi::GenTypes::GCuts&  fun      ,
        const LoKi::GenTypes::GCuts&  selector ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~ChildCut () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildCut*  clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the function itself
      LoKi::GenTypes::GCut     m_fun   ;                 // the function itself
      /// the child selector
      LoKi::GenChild::Selector m_child ;                 //  the child selector
      // ======================================================================
    };
    // ========================================================================
  } //                                      end of namespace LoKi::GenParticles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_GENPARTICLES_H
// ============================================================================
