// $Id$
// ============================================================================
#ifndef LOKI_RANDOM_H 
#define LOKI_RANDOM_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Random
  {
    // ========================================================================
    /** @class Numbers 
     *  The generic "random" functor
     *
     *
     *  @code
     *  
     *  Rndm::Numbers breit ( randSvc() , 
     *                      Rndm::BreitWigner (   0.0 ,  1.0 ) ) ;
     * 
     *  // create the functor:
     *  LoKi::Random::Numbers fun ( breit ) ;
     *  
     *  const double result = fun ( 0 ) ;
     *
     *  @endcode 
     * 
     *  @see LoKi::Random::Flat
     *  @see LoKi::Random::Gauss
     *  @see LoKi::Cuts::GAUSS
     *  @see LoKi::Cuts::RNDM 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 200803-19     
     */  
    class Numbers : public LoKi::BasicFunctors<double>::Function
    {
    public:
      // ======================================================================
      /// constructor from the numbers 
      Numbers ( const  Rndm::Numbers&  numbers     ) ;
      /// constructor from the generator and the service 
      Numbers ( const IRndmGen::Param& param       , 
                IRndmGenSvc*           service = 0 ) ;
      /// constructor from the generator and the service 
      Numbers ( IRndmGenSvc*           service     ,
                const IRndmGen::Param& param       ) ;
      /// copy constructor 
      Numbers ( const Numbers& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Numbers() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Numbers* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument /* a */ ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the params (if are known) 
      virtual const IRndmGen::Param* params() const { return m_param ; }
      // ======================================================================
    private:
      // ======================================================================
      /// no assignement operator 
      Numbers& operator=( const Numbers& right ) ;   // no assignement operator 
      // ======================================================================
    public:
      // ======================================================================
      /// shoot! 
      result_type shoot () const ;                                    // shoot! 
      // ======================================================================
    private:
      // ======================================================================
      /// the random number service 
      mutable LoKi::Interface<IRndmGenSvc>  m_service  ; // the random number service 
      /// the parameters 
      const IRndmGen::Param*                m_param    ; // the parameters 
      /// the source of random numbers 
      mutable Rndm::Numbers                 m_numbers  ; // the source of random numbers 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Uniform 
     *
     *  Simple functor which produces random numbes according to uniform 
     *  distribution
     *  
     *  @code 
     *  
     *   LoKi::Random::Uniform fun1 = LoKi::Cuts::XRNDM ;
     *
     *   // use it:
     *   const double rndm1 = fun1( 0 ) ;
     *
     *   LoKi::Random::Uniform fun2 = LoKi::Cuts::XRND( -100 , 100 )  ;
     *
     *   // use it:
     *   const double rndm2 = fun2( 0 ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::Cuts::XRND
     *  @see LoKi::Cuts::XRNDM
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 200803-19
     */
    class Uniform : public LoKi::Random::Numbers 
    {
    public:
      // ======================================================================
      /// constructor  from the descriptor and the service 
      Uniform 
      ( const Rndm::Flat& flat    , 
        IRndmGenSvc*      svc = 0 ) ;
      /// constructor from service and descriptor 
      Uniform
      ( IRndmGenSvc*      svc     , 
        const Rndm::Flat& flat    ) ;
      /// constructor from low, high and service 
      Uniform
      ( const double      low   = 0 , 
        const double      high  = 1 , 
        IRndmGenSvc*      svc   = 0 ) ;
      /// constructor from service , low and high 
      Uniform
      ( IRndmGenSvc*      svc       ,
        const double      low   = 0 , 
        const double      high  = 1 ) ;
      /// MANDATORY: virtual desctructor
      virtual ~Uniform () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Uniform* clone() const ;
      /// OPTIONAL: nice a nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s  ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the params (if are known) 
      virtual const Rndm::Flat* params() const { return &m_flat ; }
      /// cast:
      operator const Rndm::Flat& () const { return m_flat ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the descriptor 
      Rndm::Flat m_flat ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Gauss
     *
     *  Simple functor which produces random numbes according to uniform 
     *  distribution
     *  
     *  @code 
     *  
     *   LoKi::Random::Flat fun1 = LoKi::Cuts::RNDM ;
     *
     *   // use it:
     *   const double rndm1 = fun1( 0 ) ;
     *
     *   LoKi::Random::Flat fun2 = LoKi::Cuts::RND( -100 , 100 )  ;
     *
     *   // use it:
     *   const double rndm2 = fun2( 0 ) ;
     * 
     *  @endcode 
     * 
     *  @see LoKi::Cuts::GAUS
     *  @see LoKi::Cuts::GAUSS
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 200803-19
     */
    class Gaussian : public LoKi::Random::Numbers 
    {
    public:
      // ======================================================================
      /// constructor  from the descriptor and the service 
      Gaussian 
      ( const Rndm::Gauss& gauss    , 
        IRndmGenSvc*       svc  = 0 ) ;
      /// constructor from service and descriptor 
      Gaussian
      ( IRndmGenSvc*       svc      , 
        const Rndm::Gauss& gauss    ) ;
      /// constructor from mean, sigma and service 
      Gaussian
      ( const double      mean  = 0 , 
        const double      sigma = 1 , 
        IRndmGenSvc*      svc   = 0 ) ;
      /// constructor from service , mean and sigma 
      Gaussian
      ( IRndmGenSvc*      svc       ,
        const double      mean  = 0 , 
        const double      sigma = 1 ) ;
      /// MANDATORY: virtual desctructor
      virtual ~Gaussian() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Gaussian* clone() const ;
      /// OPTIONAL: nice a nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s  ) const ;    
      // ======================================================================
    public:
      // ======================================================================
      /// get the params (if are known) 
      virtual const Rndm::Gauss* params() const { return &m_gauss ; }
      /// cast:
      operator const Rndm::Gauss& () const { return m_gauss ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the descriptor 
      Rndm::Gauss m_gauss ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Rand    LoKi/Random.h
     *
     *  Simple functor, which return the random value according to
     *  <c>rand</c> generator.
     *
     *  @attention This functor should be used *only* for tests!
     *
     *  @see LoKi::Cuts::RAND 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-03-19
     */
    class Rand : public LoKi::BasicFunctors<double>::Function
    {
    public:
      // ======================================================================
      /** constructor. If seed is not equal to 0, 
       *    the call for <c>srand</c> will be performed
       *  @param seed thje seed (if not zero) 
       */
      Rand ( const unsigned int seed = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Rand() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Rand* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument /* a */ ) const ;
      /// OPTIONAL: just a nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s  ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Blind 
     *  Helper class to 'blind' the data 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-10
     */
    class Blind : public LoKi::Functor<void,double>
    {
    public:
      // ======================================================================
      /// constructor from the seed , min & max values 
      Blind ( const std::string& seed , 
              const double       minv , 
              const double       maxv ) ;
      /// constructor from the seed , min & max values 
      Blind ( const std::string& seed ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Blind () ;
      /// MANDATORY: clone method ( "virtual construtor")
      virtual  Blind* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( /* argument a */ ) const ;
      /// OPTIONAL: just a nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s  ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default contructor is disabled 
      Blind () ;                              // default contructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      const std::string& seed () const { return m_seed ; }
      double             minv () const { return m_min  ; }
      double             maxv () const { return m_max  ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the hidden return value 
      double      m_result ;                         // the hidden return value 
      // ======================================================================
    private:
      // ======================================================================
      /// the seed string 
      std::string m_seed   ;                         // the seed string 
      /// the minimal value 
      double      m_min    ;                         // the minimal value 
      /// the maximal value 
      double      m_max    ;                         // the maximal value 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class XBlind 
     *  Helper class to 'blind' the data 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date 2010-10-10
     */
    class XBlind : public LoKi::BasicFunctors<double>::Function
    {
    public:
      // ======================================================================
      /// constructor from the seed , min & max values 
      XBlind ( const std::string& seed , 
              const double       minv , 
              const double       maxv ) ;
      /// constructor from the seed , min & max values 
      XBlind ( const std::string& seed ) ;
      /// MANDATORY: virtual destructor 
      virtual ~XBlind () ;
      /// MANDATORY: clone method ( "virtual construtor")
      virtual  XBlind* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type   operator() ( argument      a ) const ;
      /// OPTIONAL: just a nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default contructor is disabled 
      XBlind () ;                             // default contructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      const std::string& seed () const { return m_blind.seed () ; }
      double             minv () const { return m_blind.minv () ; }
      double             maxv () const { return m_blind.maxv () ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functor 
      LoKi::Random::Blind m_blind ;                       // the actual functor 
      // ======================================================================
    } ;  
    // ========================================================================
  } // end of namespace LoKi::Random
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef XRANDOM
     *
     *  Simple functor to generate the random numbers according to some 
     *  distributions
     *  
     *  @code
     * 
     *   const double mean  = 1 ;
     *   const double sigma = 2 ;
     *   XRANDOM gauss ( randSvc() , Rndm::Gauss ( mean , sigma ) ) ;
     *  
     *   // use it:
     *  const double result = gauss ( 0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Random::Numbers 
     *  @see IRndmGenSvc 
     *  @see Rndm::Numbers 
     *  @see IRndmGen
     *  @see IRndmGen::Param
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-19
     */
    typedef LoKi::Random::Numbers                               XRANDOM ;
    // ========================================================================
    /** @typedef XRND
     *  Simple functor which produces the random numbers acording to 
     *  the uniform distribution between minimal and maximal values:
     * 
     *  @code
     *
     *   const double minv = -100 ;
     *   const double maxv =  100 ;
     *
     *   XRND uniform ( randSvc() , minv , maxv ) ;
     *  
     *   // use it:
     *  const double result = uniform ( 0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::XRNDM
     *  @see LoKi::Random::Flat
     *  @see LoKi::Random::Numbers 
     *  @see Rndm::Uniform
     *  @see Rndm::Numbers
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-19
     */
    typedef LoKi::Random::Uniform                                 XRND     ;
    // ========================================================================
    /** @var XRNDM
     *  Simple functor which produces the random numbers acording to 
     *  the uniform distribution between 0 and 1 
     * 
     *  @code
     * 
     *   // use it:
     *  const double result = XRNDM ( 0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::XRND
     *  @see LoKi::Random::Flat
     *  @see LoKi::Random::Numbers 
     *  @see Rndm::Uniform
     *  @see Rndm::Numbers
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-19
     */
    const LoKi::Random::Uniform                                   XRNDM () ;
    // ========================================================================
    /** @typedef XGAUS 
     *  Simple functor which produces the random numbers acording to 
     *  the gaussian distribution with mean and sigma 
     * 
     *  @code
     * 
     *   const double mean  = 100 ;
     *   const double sigma = 200 ;
     *   XGAUS gauss ( randSvc() , mean , sigma  ) ;
     *  
     *   // use it:
     *  const double result = gauss ( 0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::XGAUSS
     *  @see LoKi::Random::Gaussian
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-19
     */
    typedef LoKi::Random::Gaussian                               XGAUS     ;
    // ========================================================================
    /** @var XGAUSS 
     *  Simple functor whcih produces the random numbers acording to 
     *  the gaussian distribution with mean=0 and sigma=1 
     * 
     *  @code
     * 
     *   // use it:
     *  const double result = XGAUSS ( 0 ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::XGAUS
     *  @see LoKi::Random::Gaussian
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-19
     */        
    const LoKi::Random::Gaussian                                XGAUSS () ;
    // ========================================================================
    /** @var XRAND 
     *  The trivial functor, whcih produced the random numbers  using the 
     *  system <c>rand()/RAND_MAX</c> function.
     *
     *  @code
     * 
     *   // use it:
     *  const double result = XRAND ( 0 ) ;
     *
     *  @endcode 
     *
     *  @attention It is known to be a bad generator!
     *  @attention The behaviour is not reproducible!
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-19
     */        
    const LoKi::Random::Rand                                    XRAND () ;
    // ========================================================================
    /** @typedef BLIND 
     *  simple 'void'-function useful for varuosu 'blind'-operations
     *
     * 
     *  @code
     * 
     *    BLIND blind ( "BlindMyValue" ,  0 , 1  ) ;
     * 
     *    double myvalue = ... ;
     * 
     *    myvalue += blind() ;
     *
     *  @endcode 
     *
     *  @see LoKi::Random::Blind
     *  @see LoKi::Cuts::XBLIND 
     *  @see Gaudi::Math::blind
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-10
     */
    typedef LoKi::Random::Blind                                  BLIND   ;
    // ==========================================================================
    /** @typedef XBLIND 
     *  simple 'void'-function useful for various 'blind'-operations
     *
     * 
     *  @code
     * 
     *    XBLIND blind ( "BlindMyValue" ,  0 , 1  ) ;
     * 
     *    double myvalue = ... ;
     * 
     *    double blindedvalue =  blind( myvalue ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Random::XBlind
     *  @see LoKi::Cuts::BLIND 
     *  @see Gaudi::Math::blind
     *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
     *  @date   2010-10-10
     */
    typedef LoKi::Random::XBlind                                XBLIND   ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_RANDOM_H
// ============================================================================
