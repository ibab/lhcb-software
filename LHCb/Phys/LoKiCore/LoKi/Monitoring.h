// $Id: Monitoring.h,v 1.2 2007-07-25 15:14:13 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MONITORING_H 
#define LOKI_MONITORING_H 1
// ============================================================================
// Include files
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/Functions.h"
// ============================================================================
namespace LoKi 
{
  /** @namespace LoKi::Monitoring Monitoring.h LoKi/Monitoring.h
   *  Collection of  useful smart "self-monitoring" meta-functors
   *
   *  Many thanks to Hugo RUIZ PEREZ for his nice and brilliant ideas!
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2007-06-14
   */
  namespace Monitoring 
  {
    // ========================================================================
    /** @class Counter 
     *  Simple predicate which decorates another predicate 
     *  with some "monitoring" features : it uses the generic 
     *  counter to monitor the performace.
     *
     *
     *  @code
     * 
     *  // some predicate:
     *  Cut cut = ... 
     * 
     *  // in the monitoring context, substitute it the monitored predicate:
     *  if ( monitoring )
     *   {
     *     cut = Counter<const LHCb::Particle> ( cut , &counter("efficiency") ) ;
     *   }
     *  
     *  // and use it as a regular predicate
     *  
     *  @endcode 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-14
     */
    template <class TYPE> 
    class Counter : public LoKi::Predicate<TYPE>
    {
    public:
      /// constructor from the predicate and the generic counter 
      Counter ( const LoKi::Predicate<TYPE>& cut  , 
                StatEntity*                  stat )
        : LoKi::Predicate<TYPE>() 
        , m_cut  ( cut  ) 
        , m_stat ( stat ) 
      {} ;
      /// copy constructor 
      Counter ( const Counter& right )
        : LoKi::AuxFunBase      ( right        ) 
        , LoKi::Predicate<TYPE> ( right        ) 
        , m_cut                 ( right.m_cut  ) 
        , m_stat                ( right.m_stat ) 
      {} ;
      /// MANDATORY: virtual constructor
      virtual ~Counter() { m_stat = 0 ; }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Counter* clone() const { return new Counter(*this); }
      /// MANDATORY: the only one essential method:
      virtual typename LoKi::Predicate<TYPE>::result_type operator() 
      ( typename LoKi::Predicate<TYPE>::argument a ) const 
      {
        const typename LoKi::Predicate<TYPE>::result_type result = m_cut ( a ) ;
        // perform monitoring
        if ( 0 != m_stat ){ (*m_stat) += result ; } ///< perform monitoring
        return result ;                                           // RETURN 
      }
      /// OPTIONAL: just a nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return m_cut.fillStream ( s ) ; }
      /// delegate ID:
      virtual std::size_t id() const { return m_cut.id() ; }
    private: 
      // no default constructor
      Counter () ; ///<  no default constructor
    private:
      // the "main" predicate:
      LoKi::PredicateFromPredicate<TYPE> m_cut ; ///< the "main" predicate:
      // generic counter used for monitoring:
      StatEntity* m_stat ;///< generic counter used for monitoring
    };
    // ========================================================================
    /** @class Stat
     *  Simple function which decorates another function 
     *  with some "monitoring" features : it uses the generic 
     *  counter to monitor the performace.
     *
     *  @code
     * 
     *  // some function
     *  Fun fun = PT ;
     * 
     *  // in the monitoring context, substitute it with the monitored predicate:
     *  if ( monitoring )
     *   {
     *     fun = Stat<const LHCb::Particle> ( fun , &counter("PT") ) ;
     *   }
     *  
     *  // and use it as a regular function:
     *  
     *  @endcode 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-14
     */
    template <class TYPE> 
    class Stat : public LoKi::Function<TYPE>
    {
    public:
      /// constructor from the function and the generic counter 
      Stat ( const LoKi::Function<TYPE>&  fun  , 
             StatEntity*                  stat )
        : LoKi::Function<TYPE>() 
        , m_fun  ( fun  ) 
        , m_stat ( stat ) 
      {} ;
      /// copy constructor 
      Stat ( const Stat& right )
        : LoKi::AuxFunBase      ( right        ) 
        , LoKi::Function<TYPE>  ( right        ) 
        , m_fun                 ( right.m_fun  ) 
      {} ;
      /// MANDATORY: virtual constructor
      virtual ~Stat() { m_stat = 0 ; }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Stat* clone() const { return new Stat(*this); }
      /// MANDATORY: the only one essential method:
      virtual typename LoKi::Function<TYPE>::result_type operator() 
      ( typename LoKi::Function<TYPE>::argument a ) const 
      {
        const typename LoKi::Function<TYPE>::result_type result = m_fun ( a ) ;
        // perform monitoring
        if ( 0 != m_stat ){ (*m_stat) += result ; } ///< perform monitoring
        return result ;
      }
      /// OPTIONAL: just a nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return m_fun.fillStream ( s ) ; }
      /// delegate ID:
      virtual std::size_t id() const { return m_fun.id() ; }
    private: 
      // no default constructor
      Stat () ; ///<  no default constructor
    private:
      // the "main" function:
      LoKi::FunctionFromFunction<TYPE> m_fun ; ///< the "main" function
      // generic counter used for monitoring:
      StatEntity* m_stat ;///< generic counter used for monitoring
    };
    // ========================================================================
    /** @class Plot
     *  Simple function which decorates another function 
     *  with some "monitoring" features : it fills the monitoring histogram
     *
     *  @code
     * 
     *  // some function
     *  Fun fun = PT ;
     * 
     *  // in the monitoring context, substitute it with the monitored predicate:
     *  if ( monitoring )
     *   {
     *     IHistogram1D* histo = book (... ) ;
     *     fun = Plot<const LHCb::Particle> ( fun , histo ) ;
     *   }
     *  
     *  // and use it as a regular function:
     *  
     *  @endcode 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-14
     */
    template <class TYPE> 
    class Plot: public LoKi::Function<TYPE>
    {
    public:
      /// constructor from the function and the histogram  
      Plot ( const LoKi::Function<TYPE>&  fun   , 
             AIDA::IHistogram1D*          histo )
        : LoKi::Function<TYPE>() 
        , m_fun     ( fun   ) 
        , m_histo   ( histo ) 
      {} ;
      /// copy constructor 
      Plot ( const Plot& right )
        : LoKi::AuxFunBase      ( right         ) 
        , LoKi::Function<TYPE>  ( right         ) 
        , m_fun                 ( right.m_fun   ) 
        , m_histo               ( right.m_histo ) 
      {} ;
      /// MANDATORY: virtual constructor
      virtual ~Plot () { m_histo = 0 ; }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Plot* clone() const { return new Plot(*this); }
      /// MANDATORY: the only one essential method:
      virtual typename LoKi::Function<TYPE>::result_type operator() 
      ( typename LoKi::Function<TYPE>::argument a ) const 
      {
        const typename LoKi::Function<TYPE>::result_type result = m_fun ( a ) ;
        // perform monitoring
        if ( 0 != m_histo ) { m_histo->fill ( result ) ;  } ///< perform monitoring
        return result ;
      }
      /// OPTIONAL: just a nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return m_fun.fillStream ( s ) ; }
      /// delegate ID:
      virtual std::size_t id() const { return m_fun.id() ; }
    private:
      // no default constructor
      Plot() ;
    private:
      // the "main" function:
      LoKi::FunctionFromFunction<TYPE> m_fun   ; ///< the "main" function
      // the histogram for monitoring 
      AIDA::IHistogram1D*              m_histo ; ///< the histogram for monitoring 
    };
    // ========================================================================
  } // end of namespace LoKi::Monitoring
  // ==========================================================================
  /** helper function for creation of monitored predicate
   *
   *  @code 
   * 
   *  // some predicate:
   *  Cut cut = ... ;
   * 
   *  // for monitoring mode, decorate it with self-monitoring abilities:
   *  if ( monitoring ) { cut = monitor ( cut , &counter("some counter") ) ; }
   *
   * 
   *  @endcode 
   * 
   *  @see LoKi::Predicate
   *  @see LoKi::Monitoring::Counter
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE> 
  inline 
  LoKi::Monitoring::Counter<TYPE>
  monitor ( const LoKi::Predicate<TYPE>& cut , StatEntity* entity )
  { return LoKi::Monitoring::Counter<TYPE>( cut , entity ) ; }
  // ==========================================================================
  /** helper function for creation of monitored predicate
   *
   *  @code 
   * 
   *  // some predicate:
   *  Cut cut = ... ;
   * 
   *  // for monitoring mode, decorate it with self-monitoring abilities:
   *  if ( monitoring ) { cut = monitor ( cut , counter("some counter") ) ; }
   *
   * 
   *  @endcode 
   * 
   *  @see LoKi::Predicate
   *  @see LoKi::Monitoring::Counter
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE> 
  inline 
  LoKi::Monitoring::Counter<TYPE>
  monitor ( const LoKi::Predicate<TYPE>& cut , StatEntity& entity )
  { return LoKi::Monitoring::Counter<TYPE>( cut , &entity ) ; }
  // ==========================================================================
  /** helper function for creation of monitored function  
   *
   *  @code 
   * 
   *  // some function
   *  Fun fun = ... ;
   * 
   *  // for monitoring mode, decorate it with self-monitoring abilities:
   *  if ( monitoring ) { fun = monitor ( fun , &counter("some counter") ) ; }
   *
   * 
   *  @endcode 
   * 
   *  @see LoKi::Predicate
   *  @see LoKi::Monitoring::Counter
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE> 
  inline 
  LoKi::Monitoring::Stat<TYPE>
  monitor ( const LoKi::Function<TYPE>& cut , StatEntity* entity )
  { return LoKi::Monitoring::Stat<TYPE>( cut , entity ) ; }
  // ==========================================================================
  /** helper function for creation of monitored function  
   *
   *  @code 
   * 
   *  // some function
   *  Fun fun = ... ;
   * 
   *  // for monitoring mode, decorate it with self-monitoring abilities:
   *  if ( monitoring ) { fun = monitor ( fun , counter("some counter") ) ; }
   *
   * 
   *  @endcode 
   * 
   *  @see LoKi::Predicate
   *  @see LoKi::Monitoring::Counter
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE> 
  inline 
  LoKi::Monitoring::Stat<TYPE>
  monitor ( const LoKi::Function<TYPE>& cut , StatEntity& entity )
  { return LoKi::Monitoring::Stat<TYPE>( cut , &entity ) ; }
  // ==========================================================================
  /** helper function for creation of monitored function  
   *
   *  @code 
   * 
   *  // some function
   *  Fun fun = ... ;
   * 
   *  // for monitoring mode, decorate it with self-monitoring abilities:
   *  if ( monitoring ) 
   *   { 
   *     AIDA::IHistogram1D* histo = ... ;
   *     fun = monitor ( fun , histo ) ; ;
   *   }
   *
   * 
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE> 
  inline 
  LoKi::Monitoring::Plot<TYPE>
  monitor ( const LoKi::Function<TYPE>& cut , AIDA::IHistogram1D* histo  )
  { return LoKi::Monitoring::Plot<TYPE>( cut , histo ) ; }
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MONITORING_H
// ============================================================================
