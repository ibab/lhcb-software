// $Id: Particles26.h,v 1.2 2009-04-28 16:05:12 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES26_H 
#define LOKI_PARTICLES26_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Particles3.h"
// ============================================================================
/** @file LoKi/Particles26.h
 *  File wth few functions on request  by Rob Lambert   
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-04-24
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class DOCA
     *  simple evaluator of Distance of Closest Approach between 
     *  to daughter particles 
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Cuts::DOCA
     *  @see LoKi::Particles::ClosestApproach
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    class DOCA
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
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
      /// MANDATORY: virtual destructor
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
      {
        double result = LoKi::Constants::NegativeInfinity ;
        for ( PARTICLE i1 = first ; last != i1 ; ++i1 ) 
        {
          for ( PARTICLE i2 = i1 + 1 ; last != i2 ; ++i2 )  
          { result = std::max ( result , doca ( *i1 , *i2 ) ) ; }
        }
        return result ;
      }
      // ======================================================================
      template <class PARTICLE>
      double docachi2max ( PARTICLE first , 
                           PARTICLE last  ) const
      {
        double result = LoKi::Constants::NegativeInfinity ;
        for ( PARTICLE i1 = first ; last != i1 ; ++i1 ) 
        {
          for ( PARTICLE i2 = i1 + 1 ; last != i2 ; ++i2 )  
          { result = std::max ( result , chi2 ( *i1 , *i2 ) ) ; }
        }
        return result ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the max doca
      inline double docamax
      ( const LHCb::Particle::ConstVector&    v ) const 
      { return docamax ( v.begin() , v.end() ) ; }
      /// evaluate the max doca
      inline double docamax 
      ( const SmartRefVector<LHCb::Particle>& v ) const 
      { return docamax ( v.begin() , v.end() ) ; }
      // ======================================================================
      /// evaluate the max chi2 doca
      inline double docachi2max
      ( const LHCb::Particle::ConstVector&    v ) const 
      { return docamax ( v.begin() , v.end() ) ; }
      /// evaluate the max chi2 doca
      inline double docachi2max 
      ( const SmartRefVector<LHCb::Particle>& v ) const 
      { return docamax ( v.begin() , v.end() ) ; }
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
      size_t firstIndex  () const { return m_first  ; }
      size_t secondIndex () const { return m_second ; }        
      // ======================================================================
      StatusCode loadTool () const ;
      // ======================================================================
      /// get the tool name 
      std::string toolName() const ;
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
     *  simple evaluator od Distance of Closest Approach between 
     *  to daughter particles 
     *  @see LoKi::Cuts::CLAPPCHI@
     *  @see LoKi::Cuts::DOCACHI2 
     *  @see LoKi::Particles::ClosestApproach
     *  @see LoKi::Particles::ClosestApproachChi2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */    
    class DOCAChi2 : public LoKi::Particles::DOCA 
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
      /// MANDATORY: virtual destructor
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
    /** @class DOCAMax
     *  simple evaluator of Maximal Distance of Closest Approach between 
     *  to daughter particles 
     *  @see LoKi::Cuts::DOCAMAX
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    class DOCAMax : public LoKi::Particles::DOCA 
    {
    public:
      // ======================================================================
      /// constructor from the tool
      DOCAMax ( const IDistanceCalculator*                  dc ) ;
      /// constructor from two the tool
      DOCAMax ( const LoKi::Interface<IDistanceCalculator>& dc ) ;
      /// constructor from the tool nick-name
      DOCAMax ( const std::string& nick = ""                   ) ;
      /// MANDATORY: virtual destructor
      virtual ~DOCAMax() {}
      // MANDATORY: clone method ('virtual constructor')
      virtual  DOCAMax* clone() const { return new DOCAMax(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DOCAChi2Max
     *  simple evaluator of Maximal Distance of Closest Approach between 
     *  to daughter particles 
     *  @see LoKi::Cuts::DOCAMAX
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    class DOCAChi2Max : public LoKi::Particles::DOCAMax
    {
    public:
      // ======================================================================
      /// constructor from the tool
      DOCAChi2Max ( const IDistanceCalculator*                  dc ) ;
      /// constructor from two the tool
      DOCAChi2Max ( const LoKi::Interface<IDistanceCalculator>& dc ) ;
      /// constructor from the tool nick-name
      DOCAChi2Max ( const std::string& nick = ""                   ) ;
      /// MANDATORY: virtual destructor
      virtual ~DOCAChi2Max() {}
      // MANDATORY: clone method ('virtual constructor')
      virtual  DOCAChi2Max* clone() const { return new DOCAChi2Max(*this) ; }
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
  } // end of namespace LoKi::Particles 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef DOCA
     *  simple evaluator of distance of closest approach between two 
     *  daughter particles 
     * 
     *  @code
     *
     *  const DOCA doca = DOCA ( 1 , 2 ) ;
     *  
     *  const LHCb::Particle* B = ... ;
     * 
     *  cound double dist = doca ( B ) ;
     *
     *  @endcode 
     *  
     *  @see LoKi::Particles::DOCA
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Particles::ClosestApproach
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    typedef LoKi::Particles::DOCA                                        DOCA ;
    // ========================================================================
    /** @typedef DOCACHI2
     *  simple evaluator of chi2-distance of closest approach between two 
     *  daughter particles 
     * 
     *  @code
     *
     *  const DOCACHI2 fun = DOCACHI2 ( 1 , 2 ) ;
     *  
     *  const LHCb::Particle* B = ... ;
     * 
     *  cound double chi2 = fun ( B ) ;
     *
     *  @endcode 
     *  
     *  @see LoKi::Particles::DOCA
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Cuts::CLAPPCHI2
     *  @see LoKi::Particles::ClosestApproach
     *  @see LoKi::Particles::ClosestApproachChi2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    typedef LoKi::Particles::DOCAChi2                                DOCACHI2 ;
    // ========================================================================
    /** @typedef DOCAMAX_
     *  simple evaluator of distance of closest approach between two 
     *  daughter particles 
     * 
     *  @code
     *
     *
     *  const DOCAMAX_ doca = DOCAMAX_ ( tool ) ;
     *  
     *  const LHCb::Particle* B = ... ;
     * 
     *  cound double dist = doca ( B ) ;
     *
     *  @endcode 
     *  
     *  @see LoKi::Particles::DOCA
     *  @see LoKi::Particles::DOCAMAX
     *  @see LoKi::Particles::DOCAMAX_
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Particles::ClosestApproach
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    typedef LoKi::Particles::DOCAMax                                 DOCAMAX_ ;
    // ========================================================================
    /** @typedef DOCAMAXCHI2_
     *  simple evaluator of chi2-distance of closest approach between two 
     *  daughter particles 
     * 
     *  @code
     *
     *  const DOCAMAXCHI2_ fun = DOCAMAXCHI2_ ( tool ) ;
     *  
     *  const LHCb::Particle* B = ... ;
     * 
     *  cound double chi2 = fun ( B ) ;
     *
     *  @endcode 
     *  
     *  @see LoKi::Particles::DOCAMAXCHI2
     *  @see LoKi::Particles::DOCAMAX
     *  @see LoKi::Particles::DOCAMAX_
     *  @see LoKi::Particles::DOCA
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Cuts::CLAPPCHI2
     *  @see LoKi::Particles::ClosestApproach
     *  @see LoKi::Particles::ClosestApproachChi2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    typedef LoKi::Particles::DOCAChi2Max                         DOCACHI2MAX_ ;
    // ========================================================================
    /** @var DOCAMAX
     *  simple evaluator of distance of closest approach between two 
     *  daughter particles 
     * 
     *  @code
     *
     *  const LHCb::Particle* B = ... ;
     * 
     *  cound double dist = DOCAMAX ( B ) ;
     *
     *  @endcode 
     *  
     *  @see LoKi::Particles::DOCA
     *  @see LoKi::Particles::DOCAMAX
     *  @see LoKi::Particles::DOCAMAX_
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Particles::ClosestApproach
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    const LoKi::Particles::DOCAMax                                    DOCAMAX ;
    // ========================================================================
    /** @var DOCAMAXCHI2
     *  simple evaluator of chi2-distance of closest approach between two 
     *  daughter particles 
     * 
     *  @code
     *
     *  const LHCb::Particle* B = ... ;
     * 
     *  cound double chi2 = DOCAMAXCHI2 ( B ) ;
     *
     *  @endcode 
     *  
     *  @see LoKi::Particles::DOCAMAXCHI2_
     *  @see LoKi::Particles::DOCAMAX
     *  @see LoKi::Particles::DOCAMAX_
     *  @see LoKi::Particles::DOCA
     *  @see LoKi::Cuts::CLAPP
     *  @see LoKi::Cuts::CLAPPCHI2
     *  @see LoKi::Particles::ClosestApproach
     *  @see LoKi::Particles::ClosestApproachChi2
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-04-24
     */
    const LoKi::Particles::DOCAChi2Max                            DOCACHI2MAX ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES26_H
// ============================================================================
