// $Id: $
#ifndef LOKI_TIMERS_H 
#define LOKI_TIMERS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IChronoSvc.h"
#include "GaudiKernel/ChronoEntity.h"
#include "GaudiKernel/Chrono.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functor.h"
// ============================================================================
/** @file 
 *
 *  This file is a part of LoKi project - 
 *  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-02-02
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi
{
  // ==========================================================================
  /** @class Timer 
   *  Helepr class for implementation of timers 
   *  @see LoKi::Functors::Timer_
   *  @see LoKi::timer 
   *  @see Chrono
   *  @see ChronoEntity
   *  @see ChronoSvc
   */
  class GAUDI_API Timer
  {
  public:
    // ========================================================================
    /// constructor from the timer name 
    Timer ( const std::string& name ) ;
    /// destructor 
    virtual ~Timer () ; // destructor 
    // ========================================================================
  public:
    // ========================================================================
    /// get the timer name 
    const std::string& name() const { return m_name ; }   // get the timer name 
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Timer () ;                           // the default constructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// the actual name of timer 
    std::string m_name ;                            // the actual name of timer 
    // ========================================================================    
  };
  // ==========================================================================
  namespace Functors  
  {
    // ========================================================================
    /** @class Timer_
     *  Helper class for monitoring/timing of LoKi functors 
     *  @see ChronoEntity
     *  @see Chrono
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2011-02-02
     */
    template <class TYPE1,class TYPE2>
    class Timer_ : public LoKi::Functor<TYPE1,TYPE2>
    {
    public :
      // ======================================================================
      /// constructor from functor&timer 
      Timer_ ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
               ChronoEntity*                     timer  ) 
        : LoKi::Functor<TYPE1,TYPE2> () 
        , m_fun   ( fun   )
        , m_svc   ( )
        , m_tname ( ) 
        , m_timer ( timer )
        , m_first ( false )
      {}  
      /// constructor from functor&service&timer name 
      Timer_ ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
               IChronoSvc*                       svc    , 
               const std::string&                timer  )
        : LoKi::Functor<TYPE1,TYPE2> () 
        , m_fun   ( fun   )
        , m_svc   ( svc   )
        , m_tname ( timer ) 
        , m_timer ( 0     )
        , m_first ( true  )
      {
        if ( !m_svc ) 
        { 
          SmartIF<IChronoSvc> cs ( this->lokiSvc().getObject() ) ;
          m_svc = cs.get() ;
        }
      }
      /// constructor from functor&timer name 
      Timer_ ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
               const std::string&                timer  ) 
        : LoKi::Functor<TYPE1,TYPE2> () 
        , m_fun   ( fun   )
        , m_svc   ()
        , m_tname ( timer ) 
        , m_timer ( 0     )
        , m_first ( true  )
      { 
        SmartIF<IChronoSvc> cs ( this->lokiSvc().getObject() ) ;
        m_svc = cs.get() ;
      }
      /// MANDATORY: virtual destructor 
      virtual ~Timer_ () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Timer_ * clone() const { return new Timer_ ( *this ) ; }
      /// MANDATORY: the only one essenital method 
      virtual typename LoKi::Functor<TYPE1,TYPE2>::result_type operator() 
        ( typename LoKi::Functor<TYPE1,TYPE2>::argument a ) const 
      {
        if ( 0 == m_timer && m_first && !(!m_svc ) )
        { m_timer = m_svc->chronoStart ( m_tname ) ; }
        //
        Chrono _timer ( m_timer ) ;
        m_first = false ;
        //
        return m_fun.fun ( a ) ;
      }
      // ======================================================================
      /// optional: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      {
        //
        if ( 0 == m_timer && m_tname.empty() && !m_svc ) { return s << m_fun ; }
        //
        s << " timer(" << m_fun ;
        return s << ",'" << m_tname << "')" ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Timer_ () ;                        // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the timer
      const ChronoEntity* timer() const { return m_timer ; }   // get the timer
      // ======================================================================
    private:
      // ======================================================================
      /// the functor 
      LoKi::FunctorFromFunctor<TYPE1,TYPE2> m_fun   ;   // the functor
      /// the service 
      mutable LoKi::Interface<IChronoSvc>   m_svc   ;   // the service 
      /// the timer name 
      std::string                           m_tname ;   // the timer name 
      /// the actual timer 
      mutable ChronoEntity*                 m_timer ;   // the actual timer 
      /// the first time?
      mutable bool                          m_first ;   // the first time?
      // ======================================================================
    };
    // ========================================================================
    /// specialization for "sources"
    template <class TYPE2>
    class Timer_ <void,TYPE2> : public LoKi::Functor<void,TYPE2>
    {
    public :
      // ======================================================================
      /// constructor from functor&timer 
      Timer_ ( const LoKi::Functor<void,TYPE2>& fun    , 
              ChronoEntity*                    timer  ) 
        : LoKi::Functor<void,TYPE2> () 
        , m_fun   ( fun   )
        , m_svc   ( )
        , m_tname ( ) 
        , m_timer ( timer )
        , m_first ( false )
      {}  
      /// constructor from functor&service&timer name 
      Timer_ ( const LoKi::Functor<void,TYPE2>& fun    , 
               IChronoSvc*                      svc    , 
               const std::string&               timer  )
        : LoKi::Functor<void,TYPE2> () 
        , m_fun   ( fun   )
        , m_svc   ( svc   )
        , m_tname ( timer ) 
        , m_timer ( 0     )
        , m_first ( true  )
      {
        if ( !m_svc ) 
        {
          SmartIF<IChronoSvc> cs ( this->lokiSvc().getObject() ) ;
          m_svc = cs.get() ;
        } 
      }
      /// constructor from functor&timer name 
      Timer_ ( const LoKi::Functor<void,TYPE2>& fun    , 
               const std::string&               timer  ) 
        : LoKi::Functor<void,TYPE2> () 
        , m_fun   ( fun   )
        , m_svc   ()
        , m_tname ( timer ) 
        , m_timer ( 0     )
        , m_first ( true  )
      {
        SmartIF<IChronoSvc> cs ( this->lokiSvc().getObject() ) ;
        m_svc = cs.get() ;
      }
      /// MANDATORY: virtual destructor 
      virtual ~Timer_ () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Timer_ * clone() const { return new Timer_ ( *this ) ; }
      // ======================================================================
      /// MANDATORY: the only one essenital method 
      virtual typename LoKi::Functor<void,TYPE2>::result_type operator() () const 
      {
        if ( 0 == m_timer && m_first && !(!m_svc ) )
        { m_timer = m_svc->chronoStart ( m_tname ) ; }
        //
        //
        Chrono _timer ( m_timer ) ;
        m_first = false ;
        //
        return m_fun.fun () ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Timer_ () ;                        // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the timer
      const ChronoEntity* timer() const { return m_timer ; }   // get the timer
      // ======================================================================
    private:
      // ======================================================================
      /// the functor 
      LoKi::FunctorFromFunctor<void,TYPE2>  m_fun   ;   // the functor
      /// the service 
      mutable LoKi::Interface<IChronoSvc>   m_svc   ;   // the service 
      /// the timer name 
      std::string                           m_tname ;   // the timer name 
      /// the actual timer 
      mutable ChronoEntity*                 m_timer ;   // the actual timer 
      /// the first time?
      mutable bool                          m_first ;   // the first time?
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Functors
  // ========================================================================
  /** get the timer functor 
   *  @see LoKi::Timer
   *  @see ChronoEntity
   *  @see Chrono
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-02-02
   */
  template <class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Timer_<TYPE1,TYPE2> timer 
  ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
    ChronoEntity*                    timer  ) 
  { return LoKi::Functors::Timer_<TYPE1,TYPE2>( fun , timer ) ; }
  // ==========================================================================
  /** get the timer functor 
   *  @see LoKi::Timer
   *  @see ChronoEntity
   *  @see Chrono
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-02-02
   */
  template <class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Timer_<TYPE1,TYPE2> timer 
  ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
    IChronoSvc*                      svc    , 
    const std::string&               timer  )
  { return LoKi::Functors::Timer_<TYPE1,TYPE2>( fun , svc , timer ) ; }
  // ==========================================================================
  /** get the timer functor 
   *  @see LoKi::Timer
   *  @see ChronoEntity
   *  @see Chrono
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-02-02
   */
  template <class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Timer_<TYPE1,TYPE2> timer 
  ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
    const std::string&               timer  )
  { return LoKi::Functors::Timer_<TYPE1,TYPE2>( fun , timer ) ; }
  // ==========================================================================
  /** get the timer functor 
   *  @see LoKi::Timer
   *  @see ChronoEntity
   *  @see Chrono
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-02-02
   */
  template <class TYPE1,class TYPE2>
  inline 
  LoKi::Functors::Timer_<TYPE1,TYPE2> timer 
  ( const LoKi::Functor<TYPE1,TYPE2>& fun    , 
    const LoKi::Timer&                timer  )
  { return LoKi::Functors::Timer_<TYPE1,TYPE2>( fun , timer.name() ) ; }
  // ==========================================================================
  /// operator form of timers 
  template <class TYPE1, class TYPE2>
  inline 
  LoKi::FunctorFromFunctor<TYPE1,TYPE2>
  operator %( const LoKi::Timer&                timer  , 
              const LoKi::Functor<TYPE1,TYPE2>& fun    )
  {
    if ( timer.name().empty() ) { return fun ; }
    return LoKi::Functors::Timer_<TYPE1,TYPE2>( fun , timer.name() ) ; 
  }
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
/// operator form of timers 
template <class TYPE1, class TYPE2>
inline 
LoKi::FunctorFromFunctor<TYPE1,TYPE2>
operator %( ChronoEntity*                     timer  , 
            const LoKi::Functor<TYPE1,TYPE2>& fun    )
{
  if ( 0 == timer ) { return fun ; }
  return LoKi::Functors::Timer_<TYPE1,TYPE2> ( fun , timer ) ;
}
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TIMERS_H
// ============================================================================
