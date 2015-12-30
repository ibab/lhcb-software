// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES26_H 
#define LOKI_PARTICLES26_H 1
// ============================================================================
// Include files
// ============================================================================
#include <memory>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/Particles3.h"
#include "LoKi/Particles4.h"
// ============================================================================
/** @file LoKi/Particles26.h
 *  File wth few functions on request  by Rob Lambert & Mat Charles   
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
    class GAUDI_API DOCA
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from two indices and the tool
      DOCA ( const size_t                                i1 ,
             const size_t                                i2 ,
             const IDistanceCalculator*                  dc , 
             const bool                                  allow = true ) ;
      /// constructor from two indices and the tool
      DOCA ( const size_t                                i1 ,
             const size_t                                i2 ,
             const LoKi::Interface<IDistanceCalculator>& dc ,
             const bool                                  allow = true ) ;
      /// constructor from two indices and the tool nick-name
      DOCA ( const size_t       i1           ,
             const size_t       i2           ,
             const std::string& nick = ""    ,
             const bool         allow = true ) ;
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
      { return m_eval.distance_  ( p1 , p2 ) ; }
      // ======================================================================
      /// evaluate the chi2 
      inline double chi2
      ( const LHCb::Particle* p1 ,
        const LHCb::Particle* p2 ) const
      { return m_eval.chi2_      ( p1 , p2 ) ; }
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
      ( const LHCb::Particle::Range&          v ) const 
      { return docamax ( v.begin() , v.end() ) ; }
      /// evaluate the max doca
      inline double docamax 
      ( const SmartRefVector<LHCb::Particle>& v ) const 
      { return docamax ( v.begin() , v.end() ) ; }
      // ======================================================================
      /// evaluate the max chi2 doca
      inline double docachi2max
      ( const LHCb::Particle::ConstVector&    v ) const 
      { return docachi2max ( v.begin() , v.end() ) ; }
      /// evaluate the max chi2 doca
      inline double docachi2max
      ( const LHCb::Particle::Range&           v ) const 
      { return docachi2max ( v.begin() , v.end() ) ; }
      /// evaluate the max chi2 doca
      inline double docachi2max 
      ( const SmartRefVector<LHCb::Particle>& v ) const 
      { return docachi2max ( v.begin() , v.end() ) ; }
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
      /// allow transtions?
      bool allow() const { return m_eval.allow () ;  }
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
      std::string  m_nick     ; // the tool nick
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
    class GAUDI_API DOCAChi2 : public LoKi::Particles::DOCA 
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
    class GAUDI_API DOCAMax : public LoKi::Particles::DOCA 
    {
    public:
      // ======================================================================
      /// constructor from the tool
      DOCAMax ( const IDistanceCalculator*                  dc , 
                const bool                                  allow = true ) ;
      /// constructor from two the tool
      DOCAMax ( const LoKi::Interface<IDistanceCalculator>& dc , 
                const bool                                  allow = true ) ;
      /// constructor from the tool nick-name
      DOCAMax ( const std::string& nick  = ""   ,
                const bool         allow = true ) ;
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
    class GAUDI_API DOCAChi2Max : public LoKi::Particles::DOCAMax
    {
    public:
      // ======================================================================
      /// constructor from the tool
      DOCAChi2Max ( const IDistanceCalculator*                  dc ,
                    const bool                                  allow = true ) ;
      /// constructor from two the tool
      DOCAChi2Max ( const LoKi::Interface<IDistanceCalculator>& dc ,
                    const bool                                  allow = true ) ;
      /// constructor from the tool nick-name
      DOCAChi2Max ( const std::string& nick = ""                   ,
                    const bool                                  allow = true ) ;
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
    // ========================================================================
    /** @class ChildIP
     *  Compute the IP of a daughter particle to the mother vertex.
     * 
     *  @see LoKi::Cuts::CHILDIP 
     *
     *  @author m.charles1@physics.ox.ac.uk
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-13
     */
    class GAUDI_API ChildIP
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// Constructor from the daughter index & tool 
      ChildIP ( const size_t               index        , 
                const IDistanceCalculator* tool  = 0    ) ;
      /// Constructor from the daughter index & tool 
      ChildIP ( const size_t                                index , 
                const LoKi::Interface<IDistanceCalculator>& tool  ) ;
      /// Constructor from the daughter index & nick 
      ChildIP ( const size_t               index     , 
                const std::string&         nick      ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChildIP ();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildIP*  clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
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
      /// load the tool
      StatusCode loadTool() const ;
      // ======================================================================
      /// get the tool name 
      std::string toolName() const ;
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
      /// default constructor is disabled 
      ChildIP ();                            // default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// The actual IP evaluator 
      LoKi::Particles::ImpParChi2 m_eval ;           // The actual IP evaluator 
      /// index of daughter particle
      size_t m_index ;                            // index of daughter particle
      /// the tool nick
      std::string  m_nick   ;                                 // the tool nick
      // ======================================================================
    }; 
    // ========================================================================    
    /** @class ChildIPChi2
     *  Compute the chi2-IP of a daughter particle to the mother vertex.
     * 
     *  @see LoKi::Cuts::CHILDIPCHI2
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-13
     */
    class GAUDI_API ChildIPChi2 : public ChildIP
    {
    public:
      // ======================================================================
      /// Constructor from the daughter index & tool 
      ChildIPChi2 ( const size_t               index     , 
                    const IDistanceCalculator* tool  = 0 ) ;
      /// Constructor from the daughter index & tool 
      ChildIPChi2 ( const size_t                                index     , 
                    const LoKi::Interface<IDistanceCalculator>& tool      ) ;
      /// Constructor from the daughter index & nick 
      ChildIPChi2 ( const size_t                                index     , 
                    const std::string&                          nick      ) ;
      /// MANDATORY: virtual destructor
      virtual ~ChildIPChi2 ();
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ChildIPChi2*  clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled 
      ChildIPChi2 () ;                       // default constructor is disabled 
      // ======================================================================
    }; 
    // ========================================================================



    // ========================================================================
    /** @class MTDOCA
     *  simple evaluator of Mother-Trajectory Distance of Closest Approach
     *  @see LoKi::Cuts::MTDOCA
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2014-06-19
     */
    class GAUDI_API MTDOCA
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// constructor from one index and the tool
      MTDOCA ( const size_t                              idaughter ,
             const IDistanceCalculator*                  dc = 0    ) ;
      /// constructor from one index and the tool
      MTDOCA ( const size_t                               idaughter ,
             const LoKi::Interface<IDistanceCalculator>&  dc        ) ;
      /// constructor from one index and the tool nick-name
      MTDOCA ( const size_t       idaughter    ,
             const std::string&   nick         ) ;
      /// MANDATORY: virtual destructor
      virtual ~MTDOCA() {} ;
      // MANDATORY: clone method ('virtual constructor')
      virtual  MTDOCA* clone() const { return new MTDOCA(*this) ; }
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
      { return m_eval.distance_  ( p1 , p2 ) ; }
      // ======================================================================
      /// evaluate the chi2
      inline double chi2
      ( const LHCb::Particle* p1 ,
        const LHCb::Particle* p2 ) const
      { return m_eval.chi2_      ( p1 , p2 ) ; }
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
      void setIndex  ( const size_t index ) const { m_index  = index ; }
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
      size_t getIndex  () const { return m_index  ; }
      // ======================================================================
      StatusCode loadTool () const ;
      // ======================================================================
      /// get the tool name
      std::string toolName() const ;
      // ======================================================================
      // Move the mother particle to the best PV of the daughter
      std::unique_ptr<LHCb::Particle> moveMother
          ( const LHCb::Particle*, const LHCb::Particle*) const ;
    private:
      // ======================================================================
      /// the default constructor is disabled
      MTDOCA() ; //                                        no default constructor
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Particles::ClosestApproach m_eval ;
      /// the daughter index
      mutable size_t m_index  ; // the daughter index
      /// the tool nick
      std::string  m_nick     ; // the tool nick
      // ======================================================================
    } ;
    class GAUDI_API MTDOCAChi2 : public MTDOCA
    {
    public:
      // ======================================================================
      /// Constructor from the daughter index & tool
      MTDOCAChi2 ( const size_t               index     ,
                   const IDistanceCalculator* tool  = 0 ) ;
      /// Constructor from the daughter index & tool
      MTDOCAChi2 ( const size_t                                index     ,
                   const LoKi::Interface<IDistanceCalculator>& tool      ) ;
      /// Constructor from the daughter index & nick
      MTDOCAChi2 ( const size_t                                index     ,
                   const std::string&                          nick      ) ;
      /// MANDATORY: virtual destructor
      virtual ~MTDOCAChi2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MTDOCAChi2*  clone() const ;
      /// MANDATORY: the only one essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL:  specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      MTDOCAChi2 () ;                       // default constructor is disabled
      // ======================================================================
    };
    // ========================================================================


    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
} //                                                      end of namespace LoKi
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
    /** @typedef CHILDIP
     *  Given a parent particle, compute the distance of closest approach of a
     *  particular daughter particle to the parent's decay vertex.
     *
     *  @author Mat Charles m.charles1@physics.ox.ac.uk
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-06
     */
    typedef LoKi::Particles::ChildIP                                  CHILDIP ;
    // ========================================================================
    /** @typedef CHILDIPCHI2
     *  Given a parent particle, compute the chi2 of distance of 
     *  closest approach of a
     *  particular daughter particle to the parent's decay vertex.
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-11-06
     */
    typedef LoKi::Particles::ChildIPChi2                          CHILDIPCHI2 ;
    // ========================================================================
    /** @typedef MTDOCA
     *  simple evaluator of mother trajectory distance of closest approach
     *
     *  @code
     *
     *  const MTDOCA doca = MTDOCA ( 1 , tool ) ;
     *
     *  const LHCb::Particle* B = ... ;
     *
     *  cound double dist = doca ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Particles::MTDOCA
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2014-06-19
     */
    typedef LoKi::Particles::MTDOCA                                    MTDOCA ;
    // ========================================================================
    /** @typedef MTDOCACHI2
     *  simple evaluator of the chi2 of mother trajectory distance of closest
     *  approach
     *
     *  @code
     *
     *  const MTDOCACHI2 docachi2 = MTDOCACHI2 ( 1 , tool ) ;
     *
     *  const LHCb::Particle* B = ... ;
     *
     *  cound double distchi2 = docachi2 ( B ) ;
     *
     *  @endcode
     *
     *  @see LoKi::Particles::MTDOCAChi2
     *  @author Albert Puig (albert.puig@cern.ch)
     *  @date   2014-06-19
     */
    typedef LoKi::Particles::MTDOCAChi2                            MTDOCACHI2 ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES26_H
// ============================================================================
