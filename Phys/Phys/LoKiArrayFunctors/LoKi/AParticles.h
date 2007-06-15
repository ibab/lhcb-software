// $Id: AParticles.h,v 1.1.1.1 2007-06-15 08:04:18 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_APARTICLES_H 
#define LOKI_APARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/ICheckOverlap.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ATypes.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/AKinematics.h"
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::AParticles
   *  helper namespace with functions with accepts array of
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
    class Size : public LoKi::Function<LoKi::ATypes::Combination> 
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
      /// OPTIONALLY: identification (if generic one is not OK)
      virtual std::size_t id() const 
      {
        Warning("id(): return arbitrary number 144 just for tests");
        return 144 ;
      }
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
    class Count : public LoKi::Function<LoKi::ATypes::Combination> 
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
      { return s << "ANUM" ; }
      /// OPTIONALLY: identification (if generic one is not OK)
      virtual std::size_t id() const 
      {
        Warning("id(): return arbitrary number 145 just for tests");
        return 145 ;
      }
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
    class Unique : public LoKi::Predicate<LoKi::ATypes::Combination> 
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
      /// OPTIONALLY: identification (if generic one is not OK)
      virtual std::size_t id() const 
      {
        Warning("id(): return arbitrary number 145 just for tests");
        return 145 ;
      }
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
      : public LoKi::Function<LoKi::ATypes::Combination>
    {
    protected :
      /// the actual type of the list of indices
      typedef std::vector<std::size_t> Indices ;
    public:
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
    protected:
      /// evaluate the full four momentum of sub-combination
      inline LoKi::LorentzVector momentum( argument a ) const 
      { return LoKi::Kinematics::momentum ( a , indices() ) ; }
    public:
      // get access to all indices 
      const Indices& indices() const { return m_indices ; }
    protected:
      /// base class printput 
      std::ostream& print_ ( std::ostream& s , const std::string& nam ) const ;
    private:
      // the actual list of indices 
      Indices m_indices ; ///< the actual list of indices 
    } ;  
    // ========================================================================
    /** @class InvariantMass 
     *  Simple evaluator in iunvarinat mass of group of subparticles.
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
     *  @se  LoKi::AParticles::FourMomentum
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-06-15
     */
    class InvariantMass : public FourMomentum 
    {
    public:
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
      /// constructor form the list of indices 
      InvariantMass ( const Indices& indices ) 
        : FourMomentum ( indices ) {}
      /// MANDATORY: virtual destrcutor 
      virtual ~InvariantMass() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  InvariantMass* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return momentum( a ).M() ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AMASS" ) ; }
    } ;
    // ========================================================================
    /** @class Momentum
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @se  LoKi::AParticles::FourMomentum
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
      { return momentum( a ).P() ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AMOM" ) ; }
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
      TransverseMomentum ( const Indices& indices ) 
        : FourMomentum ( indices ) {}
      /// MANDATORY: virtual destrcutor 
      virtual ~TransverseMomentum() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TransverseMomentum* clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const 
      { return momentum( a ).Pt() ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AMOMT" ) ; }
    };
    // ========================================================================
    /** @class Eta
     *  evaluator of the momentum of the various subcombinations
     *  @attention indices starts from one! Zero corresponds to the combination!
     *  @see LoKi::AKinematics::momentum
     *  @se  LoKi::AParticles::FourMomentum
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
      { return momentum( a ).Eta() ; }
      /// OPTIONAL: the nice printout  
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return print_ ( s , "AETAP" ) ; }
    };
  } // end of namespace LoKi::AParticles
} // end of namespace LoKi 
// ============================================================================
// The END
// ============================================================================
#endif // LOKI_APARTICLES_H
// ============================================================================
