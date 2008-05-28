// $Id: AParticles.h,v 1.8 2008-05-28 11:39:20 cattanem Exp $
// ============================================================================
#ifndef LOKI_APARTICLES_H 
#define LOKI_APARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <cmath>
#include <algorithm>
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ATypes.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/AKinematics.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/Particles3.h"
// ============================================================================
class  ParticleProperty    ;
class IParticlePropertySvc ;
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::AParticles
   *  helper namespace with functions which accept the array of
   *  particles as argument
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 20007-07-09
   */
  namespace AParticles 
  {
    // ========================================================================
    /** @class Size 
     *  the most trivial function - it evaluates the size of input container
     *  @see LoKi::Cuts::ASIZE 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class Size 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~Size() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Size* clone() const { return new Size(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const
      { return v.size() ; }      
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ASIZE" ; }
    } ;
    // ========================================================================
    /** @class Count 
     *  Simple function to count number of particles, which satisfy 
     *  the predicate 
     *  @see LoKi::Cuts::ANUM
     *  @see LoKi::Algs::count_if 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class Count 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      /// Contructor from the critearia:
      Count ( const LoKi::PhysTypes::Cuts& cut ) ;
      /// copy contructor 
      Count ( const Count& right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Count() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Count* clone() const { return new Count(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "ANUM(" << m_cut << ")" ; }
    private:
      // the default constructor is disabled 
      Count() ; ///< the default constructor is disabled 
    private:
      LoKi::PhysTypes::Cut m_cut ;
    } ;
    // ========================================================================
    /** @class Unique
     *  Simple predicate to check the overlaps, using ICheckOverlap tool
     *  @see LoKi::Cuts::AUNIQUE
     *  @see ICheckOverlap
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class Unique
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate 
    {
    public:
      /// Contructor from the tool 
      Unique ( const ICheckOverlap*                   tool ) ;
      /// Contructor from the tool 
      Unique ( const LoKi::Interface<ICheckOverlap>&  tool ) ;
      /// copy contructor 
      Unique ( const Unique& right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Unique() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Unique* clone() const { return new Unique(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "AUNIQUE" ; }
    public:
      /// retrieve the tool the tool 
      const LoKi::Interface<ICheckOverlap>& get      () const { return m_tool ;}      
      /// conversion to the tool 
      operator const LoKi::Interface<ICheckOverlap>& () const { return get() ;}
    private:
      // the default constructor is disabled 
      Unique() ; ///< the default constructor is disabled 
    private:
      // The underlying tool;
      LoKi::Interface<ICheckOverlap> m_tool ; ///< The underlying tool;
    } ;
    // ========================================================================
    /** @class FourMomentum
     *  base class for implementation of many function dealing
     *  with the various subcombinations of particles 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class FourMomentum 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    protected :
      // ======================================================================
      /// the actual type of the list of indices
      typedef std::vector<std::size_t> Indices ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from one index 
      FourMomentum ( const std::size_t index = 0 ) ;
      /// constructor from two indices 
      FourMomentum ( const std::size_t index1 , 
                     const std::size_t index2 ) ;
      /// constructor from three indices 
      FourMomentum ( const std::size_t index1 , 
                     const std::size_t index2 ,
                     const std::size_t index3 ) ;
      /// constructor from four indices 
      FourMomentum ( const std::size_t index1 , 
                     const std::size_t index2 ,
                     const std::size_t index3 ,
                     const std::size_t index4 ) ;
      /// contructor from list of indices 
      FourMomentum ( const Indices& indices ) ;      
      /// copy constructor
      FourMomentum ( const FourMomentum& right ) ;
      /// vitual destructor 
      virtual ~FourMomentum() {}
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the full four momentum of sub-combination
      inline LoKi::LorentzVector momentum( argument a ) const 
      { return LoKi::Kinematics::momentum ( a.begin() , a.end() , indices() ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get access to all indices 
      const Indices& indices() const { return m_indices ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// base class printput 
      std::ostream& print_ ( std::ostream& s , const std::string& nam ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual list of indices 
      Indices m_indices ; // the actual list of indices 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class InvariantMass 
     *  Simple evaluator of the invarinat mass of group of subparticles.
     *  @see LoKi::Cuts::AM
     *  @see LoKi::Cuts::AMASS
     *  @see LoKi::Cuts::AM12
     *  @see LoKi::Cuts::AM13
     *  @see LoKi::Cuts::AM14
     *  @see LoKi::Cuts::AM23
     *  @see LoKi::Cuts::AM24
     *  @see LoKi::Cuts::AM34
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @see LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class InvariantMass : public FourMomentum 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      InvariantMass ( const std::size_t index  = 0 ) 
        : FourMomentum( index ) {}
      /// constructor from two indices 
      InvariantMass ( const std::size_t index1 , 
                      const std::size_t index2 ) 
        : FourMomentum( index1 , index2  ) {}
      /// constructor from three indices 
      InvariantMass ( const std::size_t index1 , 
                      const std::size_t index2 , 
                      const std::size_t index3 ) 
        : FourMomentum( index1 , index2 , index3 ) {}
      /// constructor from four indices 
      InvariantMass ( const std::size_t index1 , 
                      const std::size_t index2 , 
                      const std::size_t index3 ,
                      const std::size_t index4 ) 
        : FourMomentum( index1 , index2 , index3 , index4 ) {}
      /// constructor from four-momentum :
      InvariantMass ( const FourMomentum& right ) : FourMomentum ( right ) {}
      // ======================================================================
      /// constructor form the list of indices 
      InvariantMass ( const Indices& indices ) : FourMomentum ( indices ) {}
      /// MANDATORY: virtual destrcutor 
      virtual ~InvariantMass() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  InvariantMass* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return mass ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AMASS(" ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the mass 
      inline double mass ( argument a ) const { return momentum ( a ) . M() ; }
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Momentum
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @see LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class Momentum : public FourMomentum 
    {
    public:
      /// constructor from one index 
      Momentum ( const std::size_t index  = 0 ) 
        : FourMomentum( index ) {}
      /// constructor from two indices 
      Momentum ( const std::size_t index1 , 
                 const std::size_t index2 ) 
        : FourMomentum( index1 , index2  ) {}
      /// constructor from three indices 
      Momentum ( const std::size_t index1 , 
                 const std::size_t index2 , 
                 const std::size_t index3 ) 
        : FourMomentum( index1 , index2 , index3 ) {}
      /// constructor from four indices 
      Momentum ( const std::size_t index1 , 
                 const std::size_t index2 , 
                 const std::size_t index3 ,
                 const std::size_t index4 ) 
        : FourMomentum( index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      Momentum ( const Indices& indices ) 
        : FourMomentum ( indices ) {}
      /// MANDATORY: virtual destrcutor 
      virtual ~Momentum() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Momentum* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return p ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AMOM(" ) ; }
    public:
      // ======================================================================
      /// evaluate the momentum 
      inline double p ( argument a ) const { return momentum ( a ) . P() ; }
      // ======================================================================      
    };
    // ========================================================================
    /** @class TransverseMomentum
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @see LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class TransverseMomentum : public FourMomentum 
    {
    public:
      /// constructor from one index 
      TransverseMomentum ( const std::size_t index  = 0 ) 
        : FourMomentum( index ) {}
      /// constructor from two indices 
      TransverseMomentum ( const std::size_t index1 , 
                           const std::size_t index2 ) 
        : FourMomentum( index1 , index2  ) {}
      /// constructor from three indices 
      TransverseMomentum ( const std::size_t index1 , 
                           const std::size_t index2 , 
                           const std::size_t index3 ) 
        : FourMomentum( index1 , index2 , index3 ) {}
      /// constructor from four indices 
      TransverseMomentum ( const std::size_t index1 , 
                           const std::size_t index2 , 
                           const std::size_t index3 ,
                           const std::size_t index4 ) 
        : FourMomentum( index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      TransverseMomentum ( const Indices& indices ) : FourMomentum ( indices ) {}
      /// MANDATORY: virtual destrcutor 
      virtual ~TransverseMomentum() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TransverseMomentum* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return pt( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AMOMT(" ) ; }
    public:
      // ======================================================================
      /// evaluate the transverse momentum 
      inline double pt ( argument a ) const { return momentum ( a ) . Pt() ; }
      // ======================================================================      
    };
    // ========================================================================
    /** @class Eta
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @see LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class Eta : public FourMomentum
    {
    public:
      /// constructor from one index 
      Eta ( const std::size_t index  = 0 ) 
        : FourMomentum ( index ) {}
      /// constructor from two indices 
      Eta ( const std::size_t index1 , 
            const std::size_t index2 ) 
        : FourMomentum ( index1 , index2  ) {}
      /// constructor from three indices 
      Eta ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ) 
        : FourMomentum ( index1 , index2 , index3 ) {}
      /// constructor from four indices 
      Eta ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ,
            const std::size_t index4 ) 
        : FourMomentum ( index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      Eta ( const Indices& indices ) 
        : FourMomentum ( indices ) {}
      /// MANDATORY: virtual destrcutor 
      virtual ~Eta() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Eta* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return eta ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AETAP(" ) ; }
    public:
      // ======================================================================
      /// evaluate the transverse momentum 
      inline double eta ( argument a ) const { return momentum ( a ) . Eta() ; }
      // ======================================================================      
    };
    // ========================================================================
    /** @class MinChild
     *  Simple function to return a min-value 
     *  for another function over the children 
     *  @see LoKi::Cuts::AMINCHILD
     *  @see LoKi::Algs::min_value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class MinChild 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      /// Constructor from the function 
      MinChild ( const LoKi::PhysTypes::Func& fun  ) ;
      /// Contructor from the function and minval 
      MinChild ( const LoKi::PhysTypes::Func& fun  ,
            const double                 minv ) ;
      /// Contructor from the function and criteria 
      MinChild ( const LoKi::PhysTypes::Func& fun  ,
            const LoKi::PhysTypes::Cuts& cut  ) ;
      /// Contructor from the function and criteria 
      MinChild ( const LoKi::PhysTypes::Cuts& cut  ,
            const LoKi::PhysTypes::Func& fun  ) ;
      /// Contructor from the function, criteria and minval
      MinChild ( const LoKi::PhysTypes::Func& fun  ,
            const LoKi::PhysTypes::Cuts& cut  ,
            const double                 minv ) ;
      /// Contructor from the function, criteria and minval
      MinChild ( const LoKi::PhysTypes::Cuts& cut  ,
            const LoKi::PhysTypes::Func& fun  ,
            const double                 minv ) ;
      /// copy contructor 
      MinChild ( const MinChild& right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinChild() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinChild* clone() const { return new MinChild (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      /// the default constructor is disabled 
      MinChild () ; // the default constructor is disabled 
    private:
      /// the function to be evaluated 
      LoKi::PhysTypes::Fun m_fun    ; // the function to be evaluated 
      /// the predicate to be applied
      LoKi::PhysTypes::Cut m_cut    ; // the predicate to be applied
      double               m_minval ;
    } ;
    // ========================================================================
    /** @class MaxChild
     *  Simple function to return a max-value 
     *  for another function over the children
     *  @see LoKi::Cuts::AMAXCHILD
     *  @see LoKi::Algs::max_value 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class MaxChild 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      /// Constructor from the function 
      MaxChild ( const LoKi::PhysTypes::Func& fun  ) ;
      /// Contructor from the function and minval 
      MaxChild ( const LoKi::PhysTypes::Func& fun  ,
                 const double                 minv ) ;
      /// Contructor from the function and criteria 
      MaxChild ( const LoKi::PhysTypes::Func& fun  ,
                 const LoKi::PhysTypes::Cuts& cut  ) ;
      /// Contructor from the function and criteria 
      MaxChild ( const LoKi::PhysTypes::Cuts& cut  ,
                 const LoKi::PhysTypes::Func& fun  ) ;
      /// Contructor from the function, criteria and minval
      MaxChild ( const LoKi::PhysTypes::Func& fun  ,
                 const LoKi::PhysTypes::Cuts& cut  ,
                 const double                 minv ) ;
      /// Contructor from the function, criteria and minval
      MaxChild ( const LoKi::PhysTypes::Cuts& cut  ,
                 const LoKi::PhysTypes::Func& fun  ,
                 const double                 minv ) ;
      /// copy contructor 
      MaxChild ( const MaxChild& right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxChild() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxChild* clone() const { return new MaxChild (*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      MaxChild () ; ///< the default constructor is disabled 
    private:
      // the function to be evaluated 
      LoKi::PhysTypes::Fun m_fun    ; ///< the function to be evaluated 
      // the predicate to be applied
      LoKi::PhysTypes::Cut m_cut    ; ///< the predicate to be applied
      double               m_maxval ;
    } ;
    // ========================================================================
    /** @class HasChild
     *  Simple predicate to check the presence of child 
     *  which satisfy some criteria
     *  @see LoKi::Cuts::AHAS
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class HasChild 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      /// Constructor from the critearia:
      HasChild ( const LoKi::PhysTypes::Cuts& cut ) ;
      /// copy constructor 
      HasChild ( const HasChild & right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~HasChild() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  HasChild* clone() const { return new HasChild(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      HasChild() ; ///< the default constructor is disabled 
    private:
      // the predicate to be evaluated 
      LoKi::PhysTypes::Cut m_cut ; ///< the predicate to be evaluated 
    } ;
    // ========================================================================
    /** @class ChildFun
     *  Simple function which returns a value of 
     *  a function, applied to child 
     *  @see LoKi::Cuts::ACHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class ChildFun
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      /// Constructor from the fuction and index 
      ChildFun ( const LoKi::PhysTypes::Func& fun   , 
              const int                    index ) ;
      /// Constructor from the fuction and index 
      ChildFun ( const int                    index ,
              const LoKi::PhysTypes::Func& fun   ) ;
      /// copy constructor 
      ChildFun ( const ChildFun & right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ChildFun() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildFun* clone() const { return new ChildFun(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      ChildFun() ; ///< the default constructor is disabled 
    private:
      // the function to be applied 
      LoKi::PhysTypes::Fun m_fun   ; ///< the function to be applied 
      // the index of daughter particle
      int                  m_index ; ///< the index of daughter particle
    } ;    
    // ========================================================================
    /** @class ChildCut
     *  Simple predicate which applies another predicat to the child particle 
     *  @see LoKi::Cuts::ACHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class ChildCut 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      /// Constructor from the fuction and index 
      ChildCut ( const LoKi::PhysTypes::Cuts& cut   , 
                 const int                    index ) ;
      /// Constructor from the fuction and index 
      ChildCut ( const int                    index ,
                 const LoKi::PhysTypes::Cuts& cut   ) ;
      /// copy constructor 
      ChildCut ( const ChildCut& right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ChildCut() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildCut* clone() const { return new ChildCut(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      ChildCut() ; ///< the default constructor is disabled 
    private:
      // the predicate to be evaluated 
      LoKi::PhysTypes::Cut m_cut ; ///< the predicate to be evaluated 
      // the index of daughter particle
      int                  m_index ; ///< the index of daughter particle
    } ;
    // ========================================================================
    /** @class VertexChi2 
     *  Simple function which evaluates the chi2 of th vertex fit
     *  for the combination using IVertexFit tool
     *  @see LoKi::Cuts::AVCHI2
     *  @see LoKi::Cuts::ACHI2V
     *  @see LoKi::Cuts::ACHI2VX
     *  @see IVertexFit
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class VertexChi2
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      /// constructor from the tool:
      VertexChi2 (                       IVertexFit*  fitter  ) ;
      /// constructor from the tool:
      VertexChi2 ( const LoKi::Interface<IVertexFit>& fitter  ) ;
      /// copy constructor 
      VertexChi2 ( const VertexChi2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexChi2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexChi2* clone() const { return new VertexChi2(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled
      VertexChi2() ; /// the default constructor is disabled
    public:
      /// get the tool 
      const LoKi::Interface<IVertexFit>& tool     () const { return m_fit   ; }
      /// cast to the tool:
      operator const LoKi::Interface<IVertexFit>& () const { return tool () ; }
    private:
      // the tool used for evaluation 
      LoKi::Interface<IVertexFit> m_fit ;
    } ;
    // ========================================================================
    /** @class DOCA 
     *  Simple function which evalautes the distance of closets approach 
     *  between two daughter particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-28
     */
    class DOCA 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /// constructor from two indices and the tool 
      DOCA ( const size_t                                i1 , 
             const size_t                                i2 , 
             const IDistanceCalculator*                  dc ) ;
      /// constructor from two indices and the tool 
      DOCA ( const size_t                                i1 , 
             const size_t                                i2 , 
             const LoKi::Interface<IDistanceCalculator>& dc ) ;
      /// constructor from two indices and the tool nick-name 
      DOCA ( const size_t       i1        , 
             const size_t       i2        , 
             const std::string& nick = "" ) ;
      /// MANDATORY: virtual dectsructor
      virtual ~DOCA() {}
      // MANDATORY: clone method ('virtual constructor')
      virtual  DOCA* clone() const { return new DOCA(*this) ; }  
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the doca 
      inline double doca 
      ( const LHCb::Particle* p1 , 
        const LHCb::Particle* p2 ) const 
      { return m_eval.distance ( p1 , p2 ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool 
      const LoKi::Interface<IDistanceCalculator>& tool () const 
      { return m_eval ; }      
      /// cast to the tool 
      operator const LoKi::Interface<IDistanceCalculator>& () const 
      { return tool() ; }      
      // ======================================================================
      const std::string& nickname() const { return m_nick ; }
      // ======================================================================
      void setFirst  ( const size_t i1 ) const { m_first  = i1 ; }
      // ======================================================================
      void setSecond ( const size_t i2 ) const { m_second = i2 ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// set the tool 
      void setTool ( const IDistanceCalculator* t ) const 
      { m_eval.setTool ( t ) ; }
      /// set the tool 
      void setTool ( const LoKi::Interface<IDistanceCalculator>& t ) const 
      { m_eval.setTool ( t ) ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DOCA() ; //                                        no default constructor
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Particles::ClosestApproach m_eval ;
      /// the first  index 
      mutable size_t m_first  ; // the first  index 
      /// the second index 
      mutable size_t m_second ; // the second index
      /// the tool nick
      std::string  m_nick   ; // the tool nick
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DOCAChi2 
     *  Simple function which evaluates the distance of closets approach 
     *  between two daughter particles (in chi2-units) 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-28
     */
    class DOCAChi2 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /// constructor from two indices and the tool 
      DOCAChi2 ( const size_t                                i1 , 
                 const size_t                                i2 , 
                 const IDistanceCalculator*                  dc ) ;
      /// constructor from two indices and the tool 
      DOCAChi2 ( const size_t                                i1 , 
                 const size_t                                i2 , 
                 const LoKi::Interface<IDistanceCalculator>& dc ) ;
      /// constructor from two indices and the tool nick-name 
      DOCAChi2 ( const size_t       i1        , 
                 const size_t       i2        , 
                 const std::string& nick = "" ) ;
      /// MANDATORY: virtual dectsructor
      virtual ~DOCAChi2() {}
      // MANDATORY: clone method ('virtual constructor')
      virtual  DOCAChi2* clone() const { return new DOCAChi2(*this) ; }  
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the doca 
      inline double chi2 
      ( const LHCb::Particle* p1 , 
        const LHCb::Particle* p2 ) const { return m_eval.chi2 ( p1 , p2 ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool 
      const LoKi::Interface<IDistanceCalculator>& tool () const 
      { return m_eval ; }      
      /// cast to the tool 
      operator const LoKi::Interface<IDistanceCalculator>& () const 
      { return tool() ; }      
      // ======================================================================
      const std::string& nickname() const { return m_nick ; }
      // ======================================================================
      void setFirst  ( const size_t i1 ) const { m_first  = i1 ; }
      // ======================================================================
      void setSecond ( const size_t i2 ) const { m_second = i2 ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// set the tool 
      void setTool ( const IDistanceCalculator* t ) const 
      { m_eval.setTool ( t ) ; }
      /// set the tool 
      void setTool ( const LoKi::Interface<IDistanceCalculator>& t ) const 
      { m_eval.setTool ( t ) ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DOCAChi2() ; //                                    no default constructor
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Particles::ClosestApproachChi2 m_eval ;
      /// the first  index 
      mutable size_t m_first  ; // the first  index 
      /// the second index 
      mutable size_t m_second ; // the second index
      /// the tool nick
      std::string  m_nick   ; // the tool nick
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class MaxDOCA
     *  Simple function which evaluates the maximnal 
     *  distance of the closest approach for all two-particle 
     *  subcombinations using IDistanceCalculator tool
     *  @see LoKi::Cuts::AMAXDOCA
     *  @see IDistanceCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-09
     */
    class MaxDOCA : public DOCA 
    {
    public:
      /// constructor from the tool:
      MaxDOCA ( const                 IDistanceCalculator*  doca ) ;
      /// constructor from the tool:
      MaxDOCA ( const LoKi::Interface<IDistanceCalculator>& doca ) ;
      /// constructor from the tool nickname 
      MaxDOCA ( const std::string& doca ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCA () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDOCA* clone() const { return new MaxDOCA(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // ======================================================================
      /// the default constructor is disabled
      MaxDOCA() ; // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class MaxDOCAChi2
     *  Simple function which evaluates the maximal 
     *  value of the chi2-distance of the closest approach for all two-particle 
     *  subcombinations using IGeomDispCalculator tool
     *  @see LoKi::Cuts::ADOCACHI2
     *  @see IGeomDispCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class MaxDOCAChi2 : public DOCAChi2 
    {
    public:
      /// constructor from the tool:
      MaxDOCAChi2 ( const                 IDistanceCalculator*  doca ) ;
      /// constructor from the tool:
      MaxDOCAChi2 ( const LoKi::Interface<IDistanceCalculator>& doca ) ;
      /// constructor from the tool:
      MaxDOCAChi2 ( const std::string& doca ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCAChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDOCAChi2* clone() const { return new MaxDOCAChi2(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // ======================================================================
      /// the default constructor is disabled
      MaxDOCAChi2() ; // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class MaxDOCACut
     *  Simple predicate which check if *ALL* two-particle 
     *  subcombinations have the distance of the closest 
     *  approach below some threshold.
     *  It should be more efficient than the 
     *   combination of MaxDOCA and Less
     *  @see LoKi::Cuts::ACUTDOCA
     *  @see IGeomDispCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class MaxDOCACut
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      /// constructor from the tool and threshold 
      MaxDOCACut ( const double                threshold ,
                   const IDistanceCalculator*  doca      ) ;
      /// constructor from the tool and threshold 
      MaxDOCACut ( const double          threshold ,
                   const LoKi::Interface<IDistanceCalculator>& doca ) ;
      /// constructor from the tool and threshold 
      MaxDOCACut ( const double          threshold ,
                   const std::string&    doca ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCACut () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MaxDOCACut* clone() const { return new MaxDOCACut(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // ======================================================================
      /// the default constructor is disabled
      MaxDOCACut() ; // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool 
      const LoKi::Interface<IDistanceCalculator>& tool     () const 
      { return m_doca.getTool() ; }
      /// cast to the tool:
      operator const LoKi::Interface<IDistanceCalculator>& () const 
      { return m_doca ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the actual tool used for evaluation 
      LoKi::Particles::ClosestApproach m_doca ; // the tool used for evaluation 
      /// the threshold 
      double                 m_threshold ; // the threshold 
      /// the nickname of Distance Calculator 
      std::string            m_nick      ; //  Distance Calculator 
      // ======================================================================
    } ;
    // ========================================================================    
    /** @class MaxDOCAChi2Cut
     *  Simple predicate which check if *ALL* two-particle 
     *  subcombinations have the chi2-distance of the closest 
     *  approach below some threshold.
     *  It should be more efficient than the 
     *  combination of MaxDOCAChi2 and Less
     *  @see LoKi::Cuts::ACUTDOCACHI2
     *  @see IGeomDispCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class MaxDOCAChi2Cut
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      /// constructor from the tool and threshold 
      MaxDOCAChi2Cut ( const double          threshold ,
                       const IDistanceCalculator*  doca      ) ;
      /// constructor from the tool and threshold 
      MaxDOCAChi2Cut ( const double          threshold ,
                       const LoKi::Interface<IDistanceCalculator>& doca ) ;
      /// constructor from the tool and threshold 
      MaxDOCAChi2Cut ( const double          threshold ,
                       const std::string&    doca ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCAChi2Cut () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MaxDOCAChi2Cut* clone() const { return new MaxDOCAChi2Cut(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the default constructor is disabled
      MaxDOCAChi2Cut() ; /// the default constructor is disabled
    public:
      // get the tool 
      const LoKi::Interface<IDistanceCalculator>& tool     () const 
      { return m_doca  ; }
      // cast to the tool:
      operator const LoKi::Interface<IDistanceCalculator>& () const 
      { return tool()  ; }
    private:
      // ======================================================================
      /// the actual tool used for evaluation 
      LoKi::Particles::ClosestApproachChi2 m_doca ; // the tool used for evaluation 
      /// the threshold 
      double                 m_threshold ; // the threshold 
      /// the nickname of Distance Calculator 
      std::string            m_nick      ; //  Distance Calculator 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DecayAngle
     *
     *  For partice, which has a some daughter particles, 
     *  it evaluates the cosine of the angle between daughter's 
     *  momentum and mother flight direction in the rest 
     *  system of mother particle. For 2-body decays it is just 
     *  a polarization angle of mother particle.
     *  
     *  It corresponds to a predefined variable <tt>LV0..</tt> from 
     *  H.Albrecht's KAL language used in ARGUS collaboaration
     *
     *  @see LoKi::Cuts::ALV0 
     *  @see LoKi::Cuts::ALV01
     *  @see LoKi::Cuts::ALV02
     *  @see LoKi::Cuts::ALV03
     *  @see LoKi::Cuts::ALV04
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-11-02
     */
    class DecayAngle
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      /** constructor with daughter index (starts from 1).
       *  E.g. for 2-body decays it could be 1 or 2 
       *  @param child of daughter particles
       */
      DecayAngle ( std::size_t child ) ;
      /// copy constructor 
      DecayAngle ( const DecayAngle& rigth) ;
      /// MANDATORY: virual destructor
      virtual ~DecayAngle(){};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DecayAngle* clone() const { return new DecayAngle(*this) ; }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      DecayAngle();
    private:
      // index of the daughter particle
      std::size_t m_child ;
    };
    // ========================================================================
    /** @class WrongMass
     *  Simple function which evaluates the invarinat mass of the 
     *  combinations using wrong mass-assignements
     *  @see LoKi::Cuts::WM
     *  @see LoKi::Cuts::WRONGMASS
     *  @see LoKi::Kinematics::wrongMass 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-07-26
     */
    class WrongMass
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      /// constructor from two masses 
      WrongMass  ( const double m1 , const double m2 ) ;
      /// constructor from three masses 
      WrongMass  ( const double m1 , const double m2 , const double m3 ) ;
      /// constructor from four masses 
      WrongMass  ( const double m1 , const double m2 , 
                   const double m3 , const double m4 ) ;
      /// constructor from the vector of masses 
      WrongMass ( const std::vector<double>& masses ) ;
      /// constructor from two pids 
      WrongMass  ( const LHCb::ParticleID& pid1 , 
                   const LHCb::ParticleID& pid2 ) ;
      /// constructor from three pids 
      WrongMass  ( const LHCb::ParticleID& pid1 , 
                   const LHCb::ParticleID& pid2 ,
                   const LHCb::ParticleID& pid3 ) ;
      /// constructor from four pids 
      WrongMass  ( const LHCb::ParticleID& pid1 , 
                   const LHCb::ParticleID& pid2 ,
                   const LHCb::ParticleID& pid3 ,
                   const LHCb::ParticleID& pid4 ) ;
      /// constructor from the vector of pids  
      WrongMass ( const std::vector<LHCb::ParticleID>& pids ) ;
      /// constructor from two names  
      WrongMass  ( const std::string& name1 , 
                   const std::string& name2 ) ;
      /// constructor from three names  
      WrongMass  ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ) ;
      /// constructor from four  names  
      WrongMass  ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ,
                   const std::string& name4 ) ;
      /// constructor from the vector of names 
      WrongMass ( const std::vector<std::string>& names ) ;
      /** templated constructor from the sequence and functor 
       * 
       *  e.g. to "clone" the mass -assigements form other sequnce 
       *  of particles:
       *  @code
       *
       *  const LHCb::Particle::ConstVector& sample = ... ; 
       *  
       *  AFun wm = WrongMass ( sample.begin ()                  , 
       *                        sample.end   ()                  ,
       *                        std::mem_fun(&LHCb::Particle::m) ) ;
       *
       *  @endcode 
       *
       *  Or to close the mass-assigements from MC-truth:
       *
       *  @code
       *
       *  const LHCb::MCParticle::Vector& sample = ... ; 
       *  
       *  AFun wm = WrongMass ( sample.begin ()                    , 
       *                        sample.end   ()                    ,
       *                        std::mem_fun(&LHCb::MCParticle::m) ) ;
       *
       *  @endcode 
       *
       *  Or get the masses from ParticleProperty:
       *
       *  @code
       *
       *  const std::vector<ParticleProperty*>& sample = ... ; 
       *  
       *  AFun wm = WrongMass ( sample.begin ()                       , 
       *                        sample.end   ()                       ,
       *                        std::mem_fun(&ParticleProperty::mass) ) ;
       *
       *  @endcode 
       */
      template <class OBJECT, class FUNCTOR>
      WrongMass ( OBJECT  begin , 
                  OBJECT  end   , 
                  FUNCTOR func  ) 
        : LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function () 
        , m_masses ( end - begin ) 
      {
        std::transform ( begin , end , m_masses.begin() , func ) ;
      }
      /// copy constructor 
      WrongMass ( const WrongMass& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~WrongMass() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  WrongMass* clone() const { return new WrongMass(*this) ; }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ; 
      /// OPTIONAL: specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;      
   public:
      /// get all masses:
      const std::vector<double>& masses() const { return m_masses ; }
    private:
      // the list of masses to be used 
      std::vector<double> m_masses ; ///< the list of masses to be used 
    };
    // ========================================================================    
    /** @class DeltaMass 
     *  Simple evaluator of the difference in invarinat mass 
     *   with respect to 
     *   some reference mass 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class DeltaMass : public InvariantMass 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      DeltaMass ( const double      m0         , 
                  const std::size_t index  = 0 ) 
        : InvariantMass ( index ) , m_m0 ( m0 ) {}
      /// constructor from two indices 
      DeltaMass ( const double      m0     ,
                  const std::size_t index1 , 
                  const std::size_t index2 ) 
        : InvariantMass ( index1 , index2 ) , m_m0 ( m0 ) {}
      /// constructor from three indices
      DeltaMass ( const double      m0     , 
                  const std::size_t index1 , 
                  const std::size_t index2 , 
                  const std::size_t index3 ) 
        : InvariantMass ( index1 , index2 , index3 ) , m_m0 ( m0 ) {}
      /// constructor from four indices 
      DeltaMass ( const double      m0     , 
                  const std::size_t index1 , 
                  const std::size_t index2 , 
                  const std::size_t index3 ,
                  const std::size_t index4 ) 
        : InvariantMass ( index1 , index2 , index3 , index4 ) , m_m0 ( m0 ) {}
      /// constructor form the list of indices 
      DeltaMass ( const double   m0      , 
                  const Indices& indices ) 
        : InvariantMass ( indices ) , m_m0( m0 ) {}
      /// constructor from the four-momentum 
      DeltaMass ( const double        m0   , 
                  const FourMomentum& four ) 
        : InvariantMass ( four ) , m_m0( m0 ) {}
      // ======================================================================
      /// constructor from Particle Property 
      DeltaMass ( const ParticleProperty& pp         , 
                  const std::size_t       index  = 0 ) 
        : InvariantMass ( index ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from ParticleProperty 
      DeltaMass ( const ParticleProperty& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 ) 
        : InvariantMass ( index1 , index2 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from ParticleProperty 
      DeltaMass ( const ParticleProperty& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ) 
        : InvariantMass ( index1 , index2 , index3 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from ParticleProperty 
      DeltaMass ( const ParticleProperty& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ,
                  const std::size_t       index4 ) 
        : InvariantMass ( index1 , index2 , index3 , index4 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from ParticleProperty 
      DeltaMass ( const ParticleProperty& pp      , 
                  const Indices&          indices ) 
        : InvariantMass ( indices ) , m_m0() { m_m0 = getMass ( pp ) ; }
      /// constructor from ParticleProperty 
      DeltaMass ( const ParticleProperty& pp   , 
                  const FourMomentum&     four ) 
        : InvariantMass ( four    ) , m_m0() { m_m0 = getMass ( pp ) ; }
      // ======================================================================
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp         , 
                  const std::size_t       index  = 0 ) 
        : InvariantMass ( index ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 ) 
        : InvariantMass ( index1 , index2 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ) 
        : InvariantMass ( index1 , index2 , index3 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ,
                  const std::size_t       index4 ) 
        : InvariantMass ( index1 , index2 , index3 , index4 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp      , 
                  const Indices&          indices ) 
        : InvariantMass ( indices ) , m_m0() { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp   , 
                  const FourMomentum&     four ) 
        : InvariantMass ( four    ) , m_m0() { m_m0 = getMass ( pp ) ; }
      // ======================================================================
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp         , 
                  const std::size_t       index  = 0 ) 
        : InvariantMass ( index ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 ) 
        : InvariantMass ( index1 , index2 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ) 
        : InvariantMass ( index1 , index2 , index3 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ,
                  const std::size_t       index4 ) 
        : InvariantMass ( index1 , index2 , index3 , index4 ) , m_m0 () { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp      , 
                  const Indices&          indices ) 
        : InvariantMass ( indices ) , m_m0() { m_m0 = getMass ( pp ) ; }
      /// constructor from Particle ID
      DeltaMass ( const LHCb::ParticleID& pp   , 
                  const FourMomentum&     four ) 
        : InvariantMass ( four    ) , m_m0() { m_m0 = getMass ( pp ) ; }
      // ======================================================================
      /// MANDATORY: virtual destrcutor 
      virtual ~DeltaMass() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaMass* clone() const { return new DeltaMass(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return dmass ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the mass from particle property 
      double getMass ( const ParticleProperty& pp      ) const ;
      /// get the mass from particle name 
      double getMass ( const std::string&      pp      , 
                       IParticlePropertySvc*   svc = 0 ) const ;
      /// get the mass from particle ID  
      double getMass ( const LHCb::ParticleID& pp      , 
                       IParticlePropertySvc*   svc = 0 ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the delta mass mass 
      inline double dmass ( argument a ) const { return mass ( a ) - m_m0 ; }
      // ======================================================================      
      /// get the reference mass 
      inline double m0    () const { return m_m0 ; }
      // ======================================================================      
    private:
      // ======================================================================      
      /// the reference mass
      double m_m0 ; // the reference mass
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class AbsDeltaMass 
     *  Simple evaluator of the absolute valeu for the 
     *   difference in invarinat mass 
     *   with respect to 
     *   some reference mass 
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class AbsDeltaMass : public DeltaMass 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const double      m0         , 
                     const std::size_t index  = 0 ) 
        : DeltaMass ( m0 , index ) {}
      /// constructor from two indices 
      AbsDeltaMass ( const double      m0     ,
                     const std::size_t index1 , 
                     const std::size_t index2 ) 
        : DeltaMass ( m0 , index1 , index2 ) {}
      /// constructor from three indices
      AbsDeltaMass ( const double      m0     , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 )
        : DeltaMass ( m0 , index1 , index2 , index3 ) {}
      /// constructor from four indices 
      AbsDeltaMass ( const double      m0     , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) 
        : DeltaMass ( m0 , index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      AbsDeltaMass ( const double   m0      , 
                     const Indices& indices ) 
        : DeltaMass ( m0 , indices ) {}
      /// constructor from the four-momentum 
      AbsDeltaMass ( const double        m0   , 
                     const FourMomentum& four ) 
        : DeltaMass ( m0 , four    ) {}
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const ParticleProperty&  m0 , 
                     const std::size_t index  = 0 ) 
        : DeltaMass ( m0 , index ) {}
      /// constructor from two indices 
      AbsDeltaMass ( const ParticleProperty&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 ) 
        : DeltaMass ( m0 , index1 , index2 ) {}
      /// constructor from three indices
      AbsDeltaMass ( const ParticleProperty&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 )
        : DeltaMass ( m0 , index1 , index2 , index3 ) {}
      /// constructor from four indices 
      AbsDeltaMass ( const ParticleProperty&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) 
        : DeltaMass ( m0 , index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      AbsDeltaMass ( const ParticleProperty&  m0 , 
                     const Indices& indices ) 
        : DeltaMass ( m0 , indices ) {}
      /// constructor from the four-momentum 
      AbsDeltaMass ( const ParticleProperty&  m0 , 
                     const FourMomentum& four ) 
        : DeltaMass ( m0 , four    ) {}
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index  = 0 ) 
        : DeltaMass ( m0 , index ) {}
      /// constructor from two indices 
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 ) 
        : DeltaMass ( m0 , index1 , index2 ) {}
      /// constructor from three indices
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 )
        : DeltaMass ( m0 , index1 , index2 , index3 ) {}
      /// constructor from four indices 
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) 
        : DeltaMass ( m0 , index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      AbsDeltaMass ( const std::string&  m0 , 
                     const Indices& indices ) 
        : DeltaMass ( m0 , indices ) {}
      /// constructor from the four-momentum 
      AbsDeltaMass ( const std::string&  m0 , 
                     const FourMomentum& four ) 
        : DeltaMass ( m0 , four    ) {}
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index  = 0 ) 
        : DeltaMass ( m0 , index ) {}
      /// constructor from two indices 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 ) 
        : DeltaMass ( m0 , index1 , index2 ) {}
      /// constructor from three indices
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 )
        : DeltaMass ( m0 , index1 , index2 , index3 ) {}
      /// constructor from four indices 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) 
        : DeltaMass ( m0 , index1 , index2 , index3 , index4 ) {}
      /// constructor form the list of indices 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const Indices& indices ) 
        : DeltaMass ( m0 , indices ) {}
      /// constructor from the four-momentum 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const FourMomentum& four ) 
        : DeltaMass ( m0 , four    ) {}
      // ======================================================================
      /// MANDATORY: virtual destrcutor 
      virtual ~AbsDeltaMass() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AbsDeltaMass* clone() const 
      { return new AbsDeltaMass ( *this ) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return admass ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the delta mass mass 
      inline double admass ( argument a ) 
        const { return std::fabs ( dmass ( a ) ) ; }
      // ======================================================================      
    } ;
    // ========================================================================
  } // end of namespace LoKi::AParticles
} // end of namespace LoKi 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_APARTICLES_H
// ============================================================================
