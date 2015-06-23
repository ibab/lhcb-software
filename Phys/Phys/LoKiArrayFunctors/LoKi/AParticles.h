// $Id$
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
#include "LoKi/Particles23.h"
#include "LoKi/Particles26.h"
// ============================================================================
namespace LHCb 
{
  class  ParticleProperty    ;
  class IParticlePropertySvc ;
}
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
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
    class GAUDI_API Size 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      // ======================================================================
      /// Default constructor
      Size() { }
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
      // ======================================================================
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
    class GAUDI_API Count 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Count() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      LoKi::PhysTypes::Cut m_cut ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Unique
     *  Simple predicate to check the overlaps, using ICheckOverlap tool
     *  @see LoKi::Cuts::AUNIQUE
     *  @see ICheckOverlap
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API Unique
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate 
    {
    public:
      // ======================================================================
      /// Contructor from the tool 
      Unique ( const ICheckOverlap*                   tool ) ;
      /// Contructor from the tool 
      Unique ( const LoKi::Interface<ICheckOverlap>&  tool ) ;
      /// copy contructor 
      Unique ( const Unique& right  ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Unique() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Unique* clone() const { return new Unique(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "AUNIQUE" ; }
      // ======================================================================
    public:
      // ======================================================================
      /// retrieve the tool the tool 
      const LoKi::Interface<ICheckOverlap>& get      () const { return m_tool ;}      
      /// conversion to the tool 
      operator const LoKi::Interface<ICheckOverlap>& () const { return get() ;}
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Unique() ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// The underlying tool;
      LoKi::Interface<ICheckOverlap> m_tool ;           // The underlying tool;
      // ======================================================================
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
    class GAUDI_API FourMomentum 
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
    class GAUDI_API InvariantMass : public FourMomentum 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      InvariantMass ( const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      InvariantMass ( const std::size_t index1 , 
                      const std::size_t index2 ) ;
      /// constructor from three indices 
      InvariantMass ( const std::size_t index1 , 
                      const std::size_t index2 , 
                      const std::size_t index3 ) ;
      /// constructor from four indices 
      InvariantMass ( const std::size_t index1 , 
                      const std::size_t index2 , 
                      const std::size_t index3 ,
                      const std::size_t index4 ) ;
      /// constructor from four-momentum :
      InvariantMass ( const FourMomentum& right ) ;
      /// constructor form the list of indices 
      InvariantMass ( const Indices&    indices ) ;
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
    class GAUDI_API Momentum : public FourMomentum 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      Momentum ( const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      Momentum ( const std::size_t index1 , 
                 const std::size_t index2 ) ;
      /// constructor from three indices 
      Momentum ( const std::size_t index1 , 
                 const std::size_t index2 , 
                 const std::size_t index3 ) ;
      /// constructor from four indices 
      Momentum ( const std::size_t index1 , 
                 const std::size_t index2 , 
                 const std::size_t index3 ,
                 const std::size_t index4 ) ;
      /// constructor form the list of indices 
      Momentum ( const Indices& indices ) ;
      /// constructor form the list of indices 
      Momentum ( const FourMomentum& indices ) ;
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
      // ======================================================================
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
    class GAUDI_API TransverseMomentum : public FourMomentum 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      TransverseMomentum ( const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      TransverseMomentum ( const std::size_t index1 , 
                           const std::size_t index2 ) ;
      /// constructor from three indices 
      TransverseMomentum ( const std::size_t index1 , 
                           const std::size_t index2 , 
                           const std::size_t index3 ) ;
      /// constructor from four indices 
      TransverseMomentum ( const std::size_t index1 , 
                           const std::size_t index2 , 
                           const std::size_t index3 ,
                           const std::size_t index4 ) ;
      /// constructor form the list of indices 
      TransverseMomentum ( const Indices&    indices ) ;
      /// constructor form the list of indices 
      TransverseMomentum ( const FourMomentum& right ) ;
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
      // ======================================================================
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
    class GAUDI_API Eta : public FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one index 
      Eta ( const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      Eta ( const std::size_t index1 , 
            const std::size_t index2 ) ;
      /// constructor from three indices 
      Eta ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ) ;
      /// constructor from four indices 
      Eta ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ,
            const std::size_t index4 ) ;
      /// constructor form the list of indices 
      Eta ( const Indices& indices ) ;
      /// constructor form the list of indices 
      Eta ( const FourMomentum& indices ) ;
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
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the transverse momentum 
      inline double eta ( argument a ) const { return momentum ( a ) . Eta() ; }
      // ======================================================================      
    };
    // ========================================================================
    /** @class Phi
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @see LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class GAUDI_API Phi : public FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one index 
      Phi ( const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      Phi ( const std::size_t index1 , 
            const std::size_t index2 ) ;
      /// constructor from three indices 
      Phi ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ) ;
      /// constructor from four indices 
      Phi ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ,
            const std::size_t index4 ) ;
      /// constructor form the list of indices 
      Phi ( const Indices& indices ) ;
      /// constructor form the list of indices 
      Phi ( const FourMomentum& indices ) ;
      /// MANDATORY: virtual destrcutor 
      virtual ~Phi() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Phi* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return phi ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "APHI(" ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the transverse momentum 
      inline double phi ( argument a ) const { return momentum ( a ) . Phi() ; }
      // ======================================================================      
    };
    // ========================================================================
    /** @class Y
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @see LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class GAUDI_API Y : public FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one index 
      Y ( const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      Y ( const std::size_t index1 , 
          const std::size_t index2 ) ;
      /// constructor from three indices 
      Y ( const std::size_t index1 , 
            const std::size_t index2 , 
            const std::size_t index3 ) ;
      /// constructor from four indices 
      Y ( const std::size_t index1 , 
          const std::size_t index2 , 
          const std::size_t index3 ,
          const std::size_t index4 ) ;
      /// constructor form the list of indices 
      Y ( const Indices& indices ) ;
      /// constructor form the list of indices 
      Y ( const FourMomentum& indices ) ;
      /// MANDATORY: virtual destrcutor 
      virtual ~Y() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Y* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return y ( a ) ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AY(" ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the transverse momentum 
      inline double y ( argument a ) const { return momentum ( a ) . Rapidity() ; }
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
    class GAUDI_API MinChild 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinChild () ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the function to be evaluated 
      LoKi::PhysTypes::Fun m_fun    ; // the function to be evaluated 
      /// the predicate to be applied
      LoKi::PhysTypes::Cut m_cut    ; // the predicate to be applied
      double               m_minval ;
      // ======================================================================
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
    class GAUDI_API MaxChild 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function 
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MaxChild () ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the function to be evaluated 
      LoKi::PhysTypes::Fun m_fun    ;           // the function to be evaluated 
      /// the predicate to be applied
      LoKi::PhysTypes::Cut m_cut    ;            // the predicate to be applied
      double               m_maxval ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class HasChild
     *  Simple predicate to check the presence of child 
     *  which satisfy some criteria
     *  @see LoKi::Cuts::AHAS
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API HasChild 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      HasChild() ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the predicate to be evaluated 
      LoKi::PhysTypes::Cut m_cut ;            // the predicate to be evaluated 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class ChildFun
     *  Simple function which returns a value of 
     *  a function, applied to child 
     *  @see LoKi::Cuts::ACHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API ChildFun
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ChildFun() ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the function to be applied 
      LoKi::PhysTypes::Fun m_fun   ;              // the function to be applied 
      /// the index of daughter particle
      int                  m_index ;          // the index of daughter particle
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class ChildCut
     *  Simple predicate which applies another predicat to the child particle 
     *  @see LoKi::Cuts::ACHILD
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API ChildCut 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      ChildCut() ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the predicate to be evaluated 
      LoKi::PhysTypes::Cut m_cut ;             // the predicate to be evaluated 
      /// the index of daughter particle
      int                  m_index ;          // the index of daughter particle
      // ======================================================================
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
    class GAUDI_API VertexChi2
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /// constructor from the tool:
      VertexChi2 (                       IVertexFit*  fitter  ) ;
      /// constructor from the tool:
      VertexChi2 ( const LoKi::Interface<IVertexFit>& fitter  ) ;
      /// copy constructor 
      VertexChi2 ( const VertexChi2& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~VertexChi2() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexChi2* clone() const { return new VertexChi2(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      VertexChi2() ;                     // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool 
      const LoKi::Interface<IVertexFit>& tool     () const { return m_fit   ; }
      /// cast to the tool:
      operator const LoKi::Interface<IVertexFit>& () const { return tool () ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the tool used for evaluation 
      LoKi::Interface<IVertexFit> m_fit ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DOCA 
     *  Simple function which evalautes the distance of closets approach 
     *  between two daughter particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-28
     */
    class GAUDI_API DOCA 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /// constructor from two indices and the tool 
      DOCA ( const size_t                                i1           , 
             const size_t                                i2           ,  
             const IDistanceCalculator*                  dc           , 
             const bool                                  allow = true ) ;
      /// constructor from two indices and the tool 
      DOCA ( const size_t                                i1 , 
             const size_t                                i2 , 
             const LoKi::Interface<IDistanceCalculator>& dc , 
             const bool                                  allow = true ) ;
      /// constructor from two indices and the tool nick-name 
      DOCA ( const size_t       i1           , 
             const size_t       i2           , 
             const std::string& nick  = ""   ,
             const bool         allow = true ) ;
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
      { return m_eval.doca  ( p1 , p2 ) ; }
      // ======================================================================
      /// evaluate the chi2 
      inline double chi2
      ( const LHCb::Particle* p1 ,
        const LHCb::Particle* p2 ) const
      { return m_eval.chi2 ( p1 , p2 ) ; }
      // ======================================================================
    public:
      // ======================================================================
      template <class PARTICLE>
      double docamax ( PARTICLE first , 
                       PARTICLE last  ) const
      { return m_eval.docamax ( first , last ) ; }
      // ======================================================================
      template <class PARTICLE>
      double docachi2max ( PARTICLE first , 
                       PARTICLE last  ) const
      { return m_eval.docachi2max ( first , last ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the max doca
      inline double docamax
      ( const LHCb::Particle::ConstVector&    v ) const 
      { return m_eval.docamax( v ) ; }
      /// evaluate the max doca
      inline double docamax 
      ( const SmartRefVector<LHCb::Particle>& v ) const 
      { return m_eval.docamax( v ) ; }
      // ======================================================================
      /// evaluate the max chi2 doca
      inline double docachi2max
      ( const LHCb::Particle::ConstVector&    v ) const 
      { return m_eval.docachi2max( v ) ; }
      /// evaluate the max chi2 doca
      inline double docachi2max 
      ( const SmartRefVector<LHCb::Particle>& v ) const 
      { return m_eval.docachi2max( v ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool
      const LoKi::Interface<IDistanceCalculator>& tool () const
      { return m_eval.tool() ; }
      // ======================================================================
      /// load the tool 
      StatusCode loadTool  () const { return m_eval.loadTool() ; }
      /// get the tool name 
      std::string toolName () const { return m_eval.toolName() ; }
      // ======================================================================
      size_t firstIndex  () const { return m_eval.firstIndex  () ; }
      size_t secondIndex () const { return m_eval.secondIndex () ; }
      // ======================================================================      
      bool allow() const { return m_eval.allow() ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DOCA() ; //                                        no default constructor
      // ======================================================================
    protected:
      // ======================================================================
      LoKi::Particles::DOCA m_eval ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DOCAChi2 
     *  Simple function which evaluates the distance of closets approach 
     *  between two daughter particles (in chi2-units) 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-04-28
     */
    class GAUDI_API DOCAChi2 : public LoKi::AParticles::DOCA 
    {
    public:
      // ======================================================================
      /// constructor from two indices and the tool 
      DOCAChi2 ( const size_t                                i1 , 
                 const size_t                                i2 , 
                 const IDistanceCalculator*                  dc ,
                 const bool                                  allow = true ) ;
      /// constructor from two indices and the tool 
      DOCAChi2 ( const size_t                                i1 , 
                 const size_t                                i2 , 
                 const LoKi::Interface<IDistanceCalculator>& dc ,
                 const bool                                  allow = true ) ;
      /// constructor from two indices and the tool nick-name 
      DOCAChi2 ( const size_t       i1           , 
                 const size_t       i2           , 
                 const std::string& nick  = ""   ,
                 const bool         allow = true ) ;
      /// MANDATORY: virtual dectsructor
      virtual ~DOCAChi2() {}
      // MANDATORY: clone method ('virtual constructor')
      virtual  DOCAChi2* clone() const { return new DOCAChi2(*this) ; }  
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DOCAChi2() ; //                                    no default constructor
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
    class GAUDI_API MaxDOCA : public LoKi::AParticles::DOCA 
    {
    public:
      // ======================================================================
      /// constructor from the tool:
      MaxDOCA ( const                 IDistanceCalculator*  doca ,
                const bool                                  allow = true ) ;
      /// constructor from the tool:
      MaxDOCA ( const LoKi::Interface<IDistanceCalculator>& doca ,
                const bool                                  allow = true ) ;
      /// constructor from the tool nickname 
      MaxDOCA ( const std::string& doca ,
                const bool                                  allow = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCA () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDOCA* clone() const { return new MaxDOCA(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
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
     *  subcombinations using IDistanceCalculator tool
     *  @see LoKi::Cuts::ADOCACHI2
     *  @see IDistanceCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API MaxDOCAChi2 : public LoKi::AParticles::DOCAChi2 
    {
    public:
      // ======================================================================
      /// constructor from the tool:
      MaxDOCAChi2 ( const                 IDistanceCalculator*  doca ,
                    const bool                                  allow = true ) ;
      /// constructor from the tool:
      MaxDOCAChi2 ( const LoKi::Interface<IDistanceCalculator>& doca ,
                    const bool                                  allow = true ) ;
      /// constructor from the tool:
      MaxDOCAChi2 ( const std::string& doca ,
                    const bool                                  allow = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCAChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDOCAChi2* clone() const { return new MaxDOCAChi2(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
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
     *  @see IDistanceCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API MaxDOCACut
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the tool and threshold 
      MaxDOCACut ( const double                threshold ,
                   const IDistanceCalculator*  doca      ,
                   const bool                                  allow = true ) ;
      /// constructor from the tool and threshold 
      MaxDOCACut ( const double          threshold ,
                   const LoKi::Interface<IDistanceCalculator>& doca ,
                   const bool                                  allow = true ) ;
      /// constructor from the tool and threshold 
      MaxDOCACut ( const double          threshold ,
                   const std::string&    doca , 
                   const bool                                  allow = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCACut () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MaxDOCACut* clone() const { return new MaxDOCACut(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      MaxDOCACut() ; // the default constructor is disabled
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool 
      const LoKi::Interface<IDistanceCalculator>& tool     () const 
      { return m_doca.tool() ; }
      /// cast to the tool:
      operator const LoKi::Interface<IDistanceCalculator>& () const 
      { return m_doca ; }
      /// load tool 
      StatusCode loadTool() const { return m_doca.loadTool() ; }
      // ======================================================================
      /// tool name 
      std::string toolName () const { return m_doca.toolName() ; }
      // ======================================================================
      bool allow() const { return m_doca.allow() ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// the actual tool used for evaluation 
      LoKi::Particles::DOCA  m_doca      ; // the tool used for evaluation 
      /// the threshold 
      double                 m_threshold ; // the threshold 
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
     *  @see IDistanceCalculator
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 20007-07-09
     */
    class GAUDI_API MaxDOCAChi2Cut : public LoKi::AParticles::MaxDOCACut
    {
    public:
      // ======================================================================
      /// constructor from the tool and threshold 
      MaxDOCAChi2Cut ( const double          threshold ,
                       const IDistanceCalculator*  doca      ,
                       const bool                                  allow = true ) ;
      /// constructor from the tool and threshold 
      MaxDOCAChi2Cut ( const double          threshold ,
                       const LoKi::Interface<IDistanceCalculator>& doca ,
                       const bool                                  allow = true ) ;
      /// constructor from the tool and threshold 
      MaxDOCAChi2Cut ( const double          threshold ,
                       const std::string&    doca ,
                       const bool                                  allow = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCAChi2Cut () {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  MaxDOCAChi2Cut* clone() const { return new MaxDOCAChi2Cut(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled
      MaxDOCAChi2Cut() ; /// the default constructor is disabled
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
    class GAUDI_API DecayAngle
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DecayAngle();
      // ======================================================================
    private:
      // ======================================================================
      // index of the daughter particle
      std::size_t m_child ;
      // ======================================================================
    };
    // ========================================================================
    /** @class DeltaAngle 
     *  evaluate the cosine of the 
     *  angle between two daughter particles (in lab-frame)
     * 
     *  The functor is requested by 
     *     Jessica Prisciandaro and Diego Martinez Santos
     *
     *  @code
     *  
     *  xxx.CombinationCut = "         ALV ( 1 , 2 )   < 0.999999   "
     *
     *  xxx.MotherCut      = " PFUNA ( ALV ( 1 , 2 ) ) < 0.999999   "
     *
     *  xxx.MotherCut      = " PCUTA ( ALV ( 1 , 2 )   < 0.999999 ) "
     *
     *  @endcode 
     *
     *  @see @ALV 
     *  @see @LoKi::AParticles::DecayAngle
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2015-06-23
     */ 
    class GAUDI_API DeltaAngle
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public: 
      // ======================================================================
      /** constructor with daughter index (starts from 1).
       *  E.g. for 2-body decays it could be 1 or 2 
       *  @param child1 index of first daughter particle
       *  @param child2 index of first daughter particle
       */
      DeltaAngle ( const unsigned short child1 ,
                   const unsigned short child2 ) ;
      /// copy constructor 
      DeltaAngle ( const DeltaAngle& rigth) ;
      /// MANDATORY: virual destructor
      virtual ~DeltaAngle();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaAngle* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DeltaAngle();
      // ======================================================================
    private:
      // ======================================================================
      // index of the daughter particle
      unsigned short m_child1 ;
      // index of the daughter particle
      unsigned short m_child2 ;
      // ======================================================================
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
    class GAUDI_API WrongMass
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
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
      // ======================================================================
    public:
      // ======================================================================
      const std::vector<double>&           masses () const { return m_wm.masses() ; }
      const std::vector<std::string>&      names  () const { return m_wm.names () ; }      
      const std::vector<LHCb::ParticleID>& pids   () const { return m_wm.pids  () ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functor 
      LoKi::Particles::WrongMass m_wm ; // the actual functor 
      // ======================================================================
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
    class GAUDI_API DeltaMass : public InvariantMass 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      DeltaMass ( const double      m0         , 
                  const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      DeltaMass ( const double      m0     ,
                  const std::size_t index1 , 
                  const std::size_t index2 ) ;
      /// constructor from three indices
      DeltaMass ( const double      m0     , 
                  const std::size_t index1 , 
                  const std::size_t index2 , 
                  const std::size_t index3 ) ;
      /// constructor from four indices 
      DeltaMass ( const double      m0     , 
                  const std::size_t index1 , 
                  const std::size_t index2 , 
                  const std::size_t index3 ,
                  const std::size_t index4 ) ;
      /// constructor form the list of indices 
      DeltaMass ( const double   m0      , 
                  const Indices& indices ) ;
      /// constructor from the four-momentum 
      DeltaMass ( const double        m0   , 
                  const FourMomentum& four ) ;
      // ======================================================================
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp         , 
                  const std::size_t       index  = 0 ) ;
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 ) ;
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ) ;
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ,
                  const std::size_t       index4 ) ;
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp      , 
                  const Indices&          indices ) ;
      /// constructor from Particle name 
      DeltaMass ( const std::string& pp   , 
                  const FourMomentum&     four ) ;
      // ======================================================================
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp         , 
                  const std::size_t       index  = 0 ) ;
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 ) ;
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ) ;
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp     , 
                  const std::size_t       index1 , 
                  const std::size_t       index2 , 
                  const std::size_t       index3 ,
                  const std::size_t       index4 ) ;
      /// constructor from Particle ID 
      DeltaMass ( const LHCb::ParticleID& pp      , 
                  const Indices&          indices ) ;
      /// constructor from Particle ID
      DeltaMass ( const LHCb::ParticleID& pp   , 
                  const FourMomentum&     four ) ;
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
      double getMass ( const LHCb::ParticleProperty& pp      ) const ;
      /// get the mass from particle name 
      double getMass ( const std::string&      pp      , 
                       LHCb::IParticlePropertySvc*   svc = 0 ) const ;
      /// get the mass from particle ID  
      double getMass ( const LHCb::ParticleID& pp      , 
                       LHCb::IParticlePropertySvc*   svc = 0 ) const ;
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
      DeltaMass () ;
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
    class GAUDI_API AbsDeltaMass : public DeltaMass 
    {
    public:
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const double      m0         , 
                     const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      AbsDeltaMass ( const double      m0     ,
                     const std::size_t index1 , 
                     const std::size_t index2 ) ;
      /// constructor from three indices
      AbsDeltaMass ( const double      m0     , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ) ;
      /// constructor from four indices 
      AbsDeltaMass ( const double      m0     , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) ;
      /// constructor form the list of indices 
      AbsDeltaMass ( const double   m0      , 
                     const Indices& indices ) ;
      /// constructor from the four-momentum 
      AbsDeltaMass ( const double        m0   , 
                     const FourMomentum& four ) ;
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 ) ;
      /// constructor from three indices
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ) ;
      /// constructor from four indices 
      AbsDeltaMass ( const std::string&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) ;
      /// constructor form the list of indices 
      AbsDeltaMass ( const std::string&  m0 , 
                     const Indices& indices ) ;
      /// constructor from the four-momentum 
      AbsDeltaMass ( const std::string&  m0 , 
                     const FourMomentum& four ) ;
      // ======================================================================
      /// constructor from one index 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index  = 0 ) ;
      /// constructor from two indices 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 ) ;
      /// constructor from three indices
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ) ;
      /// constructor from four indices 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const std::size_t index1 , 
                     const std::size_t index2 , 
                     const std::size_t index3 ,
                     const std::size_t index4 ) ;
      /// constructor form the list of indices 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const Indices& indices )  ;
      /// constructor from the four-momentum 
      AbsDeltaMass ( const LHCb::ParticleID&  m0 , 
                     const FourMomentum& four ) ;
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
    private:
      // ======================================================================      
      AbsDeltaMass () ;
      // ======================================================================      
    public:
      // ======================================================================
      /// evaluate the delta mass mass 
      inline double admass ( argument a ) 
        const { return std::fabs ( dmass ( a ) ) ; }
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class MinDOCA
     *  Simple function that evaluates the minimal 
     *  distance of the closest approach for all two-particle 
     *  subcombinations using IDistanceCalculator tool.
     *  A straightforward adaptation of MaxDOCA.
     *  @see LoKi::Cuts::AMINDOCA
     *  @see IDistanceCalculator
     *  @author  Patrick SPRADLIN
     *  @date   2009-03-10
     */
    class GAUDI_API MinDOCA : public DOCA 
    {
    public:
      // ======================================================================
      /// constructor from the tool:
      MinDOCA ( const                 IDistanceCalculator*  doca ,
                const bool                                  allow = true ) ;
      /// constructor from the tool:
      MinDOCA ( const LoKi::Interface<IDistanceCalculator>& doca ,
                const bool                                  allow = true ) ;
      /// constructor from the tool nickname 
      MinDOCA ( const std::string& doca ,
                const bool                                  allow = true ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinDOCA () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinDOCA* clone() const { return new MinDOCA(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type  operator() ( argument v ) const ;
      /// OPTIONALLY: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled
      MinDOCA() ; // the default constructor is disabled
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AllSameBestPV
     *
     *  Predicate to determine if all daughter particles have the same
     *  related PV.  The related PV for each daughter is obtained from the
     *  IPhysDesktop tool.
     *
     *  Four algorithms for the vertex comparison are available:
     *  - comparison of C++ pointers 
     *  - comparison by the vertex distance 
     *  - comparison by the vertex chi2-distance 
     *  - comparison by the fraction of 
     * 
     *  @attention
     *  In case property 'ReFitPVs' is not activated, the first algorithm is 
     *  perfectly fine and optimal, otherwise at least one of "extra" 
     *  algorithms MUST be activated to get correct answer
     *  @see https://indico.cern.ch/event/355682/
     *  @see https://indico.cern.ch/event/355682/contribution/1/material/slides/
     *  @thanks to Jianchun Wang  
     *
     *  @see LoKi::Cuts::AALLSAMEBPV
     *  @see LoKi::Cuts::AALLSAMEBPV_
     *  @author  Patrick SPRADLIN
     *  @date   2009-03-10
     */
    class GAUDI_API AllSameBestPV
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /** constructor 
       */
      AllSameBestPV ( ) ;
      /** constructor 
       *  @param maxdist maximal distance      for two vertices to be considered as identical 
       *  @param maxchi2 maximal distance-chi2 for two vertices to be considered as identical
       *  @param maxfrac maximal fraction of common tracks 
       *                         for two vertices to be considered as identical
       *  - criteria are appied in "OR" mode
       *  - negative value means criterion is not applied 
       */
      AllSameBestPV ( const double maxdist, 
                      const double maxchi2, 
                      const double maxfrac ) ;
      /// copy constructor 
      AllSameBestPV ( const AllSameBestPV& right) ;
      /// MANDATORY: virual destructor
      virtual ~AllSameBestPV () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AllSameBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
      /// OPTIONAL: specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// check refitting property
      void checkReFit() const ; // check refitting property
      // ======================================================================
    private:
      // ======================================================================
      double m_maxdist ;
      double m_maxchi2 ;
      double m_maxfrac ;
      // ======================================================================
    };
    // ========================================================================
    /** @class ACutV 
     *  Simple adapter which allows to use "vector-predicate" as 
     *  "array-predicate"
     *  @see LoKi::Cuts::AFUNV 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    class GAUDI_API ACutV 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Predicate
    {
    public:
      // ======================================================================
      /// constructor 
      ACutV  ( const LoKi::Types::CutVals& cut ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ACutV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual ACutV* clone() const ;
      /// MANDATORY: the only one essentiual method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is disabled 
      ACutV () ;                             // default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual predicate 
      LoKi::Types::CutVal m_cut ;                       // the actual predicate 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AFunV 
     *  Simple adapter which allows to use "vector-function" as 
     *  "array-function"
     *  @see LoKi::Cuts::AFUNV 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-16
     */
    class GAUDI_API AFunV 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /// constructor 
      AFunV  ( const LoKi::Types::FunVals& fun ) ;
      /// MANDATORY: virtual destructor 
      virtual ~AFunV() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual AFunV* clone() const ;
      /// MANDATORY: the only one essentiual method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is disabled 
      AFunV () ;                             // default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      LoKi::Types::FunVal m_fun ;                       // the actual predicate 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MaxVal 
     *  Find an element whcih maximaze one functor and return 
     *  the value of another functor 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-30
     */
    class GAUDI_API MaxVal
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /** constructor with the function and "max-value"
       *  @param eval the function to be evaluated
       *  @param cmpv the function to be maximazed 
       *  @param cut  the predicate ot be satisfied 
       *  @param retv the value to be returned for "empty" selection 
       */
      MaxVal ( const LoKi::Types::Func& eval , 
               const LoKi::Types::Func& cmpv ,
               const LoKi::Types::Cuts& cuts , 
               const double             retv ) ;
      /** constructor with the function and "max-value"
       *  @param eval the function to be evaluated
       *  @param cmpv the function to be maximazed 
       *  @param cut  the predicate ot be satisfied 
       */
      MaxVal ( const LoKi::Types::Func& eval , 
               const LoKi::Types::Func& cmpv ,
               const LoKi::Types::Cuts& cuts ) ;
      /** constructor with the function and "max-value"
       *  @param eval the function to be evaluated
       *  @param cmpv the function to be maximazed 
       */
      MaxVal ( const LoKi::Types::Func& eval , 
               const LoKi::Types::Func& cmpv ) ;
      /// MANDATORY: virtual destructor 
        virtual ~MaxVal() ;
      /// MANDATORY: clone method ("virtual constructor" ) 
      virtual  MaxVal* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MaxVal() ;                         // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// The functor to be evaluated 
      LoKi::Types::Fun  m_eval ;                 // The functor to be evaluated 
      /// The functor to be maximazed 
      LoKi::Types::Fun  m_cmpv ;                 // The functor to be maximized 
      /// The predicate to be satisfied 
      LoKi::Types::Cut  m_cut  ;               // The predicate to be satisfied 
      /// return valeu for 'empty' selections 
      double            m_retv ;         // return value for 'empty' selections 
      // =======================================================================
    } ;
    // ========================================================================    
    /** @class MinVal 
     *  Find an element whoch minimizes the value of 
     *  one functor and return the value of another functor 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-30
     */
    class GAUDI_API MinVal : public MaxVal 
    {
    public:
      // ======================================================================
      /** constructor with the function and "max-value"
       *  @param eval the function to be evaluated
       *  @param cmpv the function to be maximazed 
       *  @param cut  the predicate ot be satisfied 
       *  @param retv the value to be returned for "empty" selection 
       */
      MinVal ( const LoKi::Types::Func& eval , 
               const LoKi::Types::Func& cmpv ,
               const LoKi::Types::Cuts& cuts , 
               const double             retv ) ;
      /** constructor with the function and "max-value"
       *  @param eval the function to be evaluated
       *  @param cmpv the function to be maximazed 
       *  @param cut  the predicate ot be satisfied 
       */
      MinVal ( const LoKi::Types::Func& eval , 
               const LoKi::Types::Func& cmpv ,
               const LoKi::Types::Cuts& cuts ) ;
      /** constructor with the function and "max-value"
       *  @param eval the function to be evaluated
       *  @param cmpv the function to be maximazed 
       */
      MinVal ( const LoKi::Types::Func& eval , 
               const LoKi::Types::Func& cmpv ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVal() ;
      /// MANDATORY: clone method ("virtual constructor" ) 
      virtual  MinVal* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVal() ;                         // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Sum 
     *  Simple meta-function to calculate the sum
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-08-05
     */
    class GAUDI_API Sum 
      : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
    public:
      // ======================================================================
      /// constructor from the functor 
      Sum ( const LoKi::Types::Func& fun     , 
            const double             ini = 0 ) ;
      /// constructor from the functor & predicate 
      Sum ( const LoKi::Types::Func& fun     , 
            const LoKi::Types::Cuts& cut     , 
            const double             ini = 0 ) ;
      /// constructor from the functor & predicate 
      Sum ( const LoKi::Types::Cuts& cut     ,
            const LoKi::Types::Func& fun     , 
            const double             ini = 0 ) ;
      /// MANDATORY: virtual descructor 
      virtual ~Sum() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Sum* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Sum () ;                           // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      /// the function to be accumulated 
      LoKi::Types::Fun m_fun     ;            // the function to be accumulated 
      /// the predicate to be used  
      LoKi::Types::Cut m_cut     ;            //       the predicate to be used  
      /// initial value 
      double           m_ini     ;            //                  initial value 
      /// trivial predicate ?
      bool             m_trivial ;            //            trivial predicate ?
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class Overlap 
     *  check the overlap between two daughters 
     *  @see LHCb::HashIDs::overlap 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date  2015-02-14
     */
    class GAUDI_API Overlap : public LoKi::BasicFunctors<LoKi::ATypes::Combination>::Function
    {
      // ====================================================================
      typedef bool (LHCb::LHCbID::*PMF)() const   ;
      // ====================================================================
    public :
      // ====================================================================
      /// calculate the overlap between two daughters 
      Overlap ( const unsigned short        i1   , 
                const unsigned short        i2   ) ;
      /// calculate the overlap between two daughters 
      Overlap ( const unsigned short        i1   , 
                const unsigned short        i2   , 
                LHCb::LHCbID::channelIDtype type ) ;
      /// calculate the overlap between two daughters 
      Overlap ( const unsigned short        i1   , 
                const unsigned short        i2   , 
                PMF                         good ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Overlap() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Overlap* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Overlap () ;                       // the default constructor is disabled 
      // ======================================================================      
    private:
      // ======================================================================      
      unsigned short               m_i1   ;
      unsigned short               m_i2   ;      
      PMF                          m_good ;
      LHCb::LHCbID::channelIDtype  m_type ;
      bool                         m_def  ;
      // ======================================================================      
    };
    // ========================================================================
  } //                                        end of namespace LoKi::AParticles
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_APARTICLES_H
// ============================================================================
