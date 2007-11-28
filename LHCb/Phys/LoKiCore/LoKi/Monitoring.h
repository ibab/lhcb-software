// $Id: Monitoring.h,v 1.3 2007-11-28 13:56:33 ibelyaev Exp $
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
#include "GaudiKernel/HistoDef.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/HistoBook.h"
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
     *     cut = Counter<const LHCb::Particle*,bool> ( cut , &counter("efficiency") ) ;
     *   }
     *  
     *  // and use it as a regular predicate
     *  
     *  @endcode 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-06-14
     */
    template <class TYPE, class TYPE2> 
    class Counter : public LoKi::Functor<TYPE,TYPE2>
    {
    public:
      /// constructor from the predicate and the generic counter 
      Counter ( const LoKi::Functor<TYPE,TYPE2>& cut  , 
                StatEntity*                      stat )
        : LoKi::Functor<TYPE,TYPE2>() 
        , m_cut  ( cut  ) 
        , m_stat ( stat ) 
      {} 
      /// copy constructor 
      Counter ( const Counter& right )
        : LoKi::AuxFunBase          ( right        ) 
        , LoKi::Functor<TYPE,TYPE2> ( right        ) 
        , m_cut                     ( right.m_cut  ) 
        , m_stat                    ( right.m_stat ) 
      {} 
      /// MANDATORY: virtual constructor
      virtual ~Counter() { m_stat = 0 ; }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Counter* clone() const { return new Counter(*this); }
      /// MANDATORY: the only one essential method:
      virtual typename LoKi::Functor<TYPE,TYPE2>::result_type operator() 
        ( typename LoKi::Functor<TYPE,TYPE2>::argument a ) const 
      {
        const typename LoKi::Functor<TYPE,TYPE2>::result_type result = 
          m_cut.fun ( a ) ;
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
      LoKi::FunctorFromFunctor<TYPE,TYPE2> m_cut ; ///< the "main" predicate:
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
    template <class TYPE, class TYPE2=double> 
    class Plot: public LoKi::Functor<TYPE,TYPE2>
    {
    public:
      /// constructor from the function and the histogram  
      Plot ( const LoKi::Functor<TYPE,TYPE2>& fun   , 
             AIDA::IHistogram1D*              histo )
        : LoKi::Functor<TYPE,TYPE2>() 
        , m_fun     ( fun   ) 
        , m_histo   ( histo ) 
      {} 
      /// constructor from the function and the histogram  
      Plot ( AIDA::IHistogram1D*              histo ,
             const LoKi::Functor<TYPE,TYPE2>& fun   ) 
        : LoKi::Functor<TYPE,TYPE2>() 
        , m_fun     ( fun   ) 
        , m_histo   ( histo ) 
      {} 
      /// copy constructor 
      Plot ( const Plot& right )
        : LoKi::AuxFunBase          ( right ) 
        , LoKi::Functor<TYPE,TYPE2> ( right ) 
        , m_fun                 ( right.m_fun   ) 
        , m_histo               ( right.m_histo ) 
      {} 
      /// MANDATORY: virtual constructor
      virtual ~Plot () { m_histo = 0 ; }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Plot* clone() const { return new Plot(*this); }
      /// MANDATORY: the only one essential method:
      virtual typename LoKi::Functor<TYPE,TYPE2>::result_type operator() 
        ( typename LoKi::Functor<TYPE,TYPE2>::argument a ) const 
      {
        const typename LoKi::Functor<TYPE,TYPE2>::result_type result = 
          m_fun.fun ( a ) ;
        // perform monitoring
        if ( 0 != m_histo ) { m_histo -> fill ( result ) ;  } ///< perform monitoring
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
      LoKi::FunctorFromFunctor<TYPE,TYPE2> m_fun ; ///< the "main" function
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
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Counter<TYPE,TYPE2>
  monitor ( const LoKi::Functor<TYPE,TYPE2>& cut , StatEntity* entity )
  { return LoKi::Monitoring::Counter<TYPE,TYPE2>( cut , entity ) ; }
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
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Counter<TYPE,TYPE2>
  monitor ( const LoKi::Functor<TYPE,TYPE2>& cut , StatEntity& entity )
  { return LoKi::Monitoring::Counter<TYPE,TYPE2>( cut , &entity ) ; }
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
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Plot<TYPE,TYPE2>
  monitor ( const LoKi::Functor<TYPE,TYPE2>& cut , 
            AIDA::IHistogram1D* histo  )
  { return LoKi::Monitoring::Plot<TYPE,TYPE2>( cut , histo ) ; }
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
   *     fun = plot ( fun , histo ) ; ;
   *   }
   *
   * 
   *  @endcode 
   * 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Plot<TYPE,TYPE2>
  plot ( const LoKi::Functor<TYPE,TYPE2>& cut , 
         AIDA::IHistogram1D* histo  )
  { return LoKi::Monitoring::Plot<TYPE,TYPE2>( cut , histo ) ; }
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
   *     const std::string&       path = ... ;
   *     const Gaudi::Histo1DDef& desc = ... ;
   *     fun = plot ( fun , path , desc ) ; ;
   *   }
   *
   * 
   *  @endcode 
   * 
   *  @param cut the function to be monitored 
   *  @param path the historgam path in HDS 
   *  @param histo the histogram desctriptor 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Plot<TYPE,TYPE2>
  plot ( const LoKi::Functor<TYPE,TYPE2>& cut   , 
         const std::string&               path  , 
         const Gaudi::Histo1DDef&         histo )
  { 
    return plot ( cut , LoKi::HistoBook::book ( path , histo ) ) ;
  }
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
   *     const std::string&       dir = ... ;
   *     const std::string&       id  = ... ;
   *     const Gaudi::Histo1DDef& desc = ... ;
   *     fun = plot ( fun , path , desc ) ; ;
   *   }
   *
   * 
   *  @endcode 
   * 
   *  @param cut the function to be monitored 
   *  @param dir the historgam directory in HDS 
   *  @param id  the histogram  ID 
   *  @param histo the histogram descriptor 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Plot<TYPE,TYPE2>
  plot ( const LoKi::Functor<TYPE,TYPE2>& cut   , 
         const std::string&               dir   , 
         const std::string&               id    , 
         const Gaudi::Histo1DDef&         histo )
  { 
    return plot ( cut , LoKi::HistoBook::book ( dir, id , histo ) ) ;
  }
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
   *     const std::string&       dir = ... ;
   *     const int                id  = ... ;
   *     const Gaudi::Histo1DDef& desc = ... ;
   *     fun = plot ( fun , path , desc ) ; ;
   *   }
   *
   * 
   *  @endcode 
   * 
   *  @param cut the function to be monitored 
   *  @param dir the historgam directory in HDS 
   *  @param id  the histogram  ID 
   *  @param histo the histogram desctritor 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-06-14
   */
  template <class TYPE, class TYPE2> 
  inline 
  LoKi::Monitoring::Plot<TYPE,TYPE2>
  plot ( const LoKi::Functor<TYPE,TYPE2>& cut   , 
         const std::string&               dir   , 
         const int                        id    , 
         const Gaudi::Histo1DDef&         histo )
  { 
    return plot ( cut , LoKi::HistoBook::book ( dir, id , histo ) ) ;
  }
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MONITORING_H
// ============================================================================
