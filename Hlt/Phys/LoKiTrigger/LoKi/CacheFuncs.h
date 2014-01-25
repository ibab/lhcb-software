// $Id$
// ============================================================================
#ifndef LOKI_CACHEFUNCS_H 
#define LOKI_CACHEFUNCS_H 1
// ============================================================================
// include  files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file  
 *
 *  Cach-relatd HLT1-functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2010-12-06
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Candidates 
  {
    // ========================================================================
    /** @class Log_
     *  Simple functor for logging the results in Cache 
     *  @see Hlt::Cache 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-06
     */
    template <class TYPE>
    class Log_ : public LoKi::Functor<const Hlt::Candidate*,TYPE>
    {
    public:
      // ======================================================================
      /// constructor from the function and the key 
      Log_ 
      ( const LoKi::Functor<const Hlt::Candidate*,TYPE>&    fun      , 
        const std::string&                                  key      )
        : LoKi::Functor<const Hlt::Candidate*,TYPE>()
        , m_fun  ( fun ) 
        , m_key  ( key ) 
        , m_name ()
      {
        m_algorithm = LoKi::AlgUtils::getAlg ( *this , false ) ;
        if ( !m_algorithm ) { m_algorithm = this->lokiSvc() ; }
      }   
      /// constructor from the function and the fictive argument  
      Log_ 
      ( const LoKi::Functor<const Hlt::Candidate*,TYPE>&    f     , 
        const bool                                        /* b */ )
        : LoKi::Functor<const Hlt::Candidate*,TYPE>()
        , m_fun  ( f            ) 
        , m_key  ( f.printOut() ) 
        , m_name () 
      {
        m_algorithm = LoKi::AlgUtils::getAlg ( *this , false ) ;
        if ( !m_algorithm ) { m_algorithm = this->lokiSvc() ; }
      } 
      /// MANDATORY: virtual destructor 
      virtual ~Log_ () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Log_* clone () const { return new Log_( *this ) ; }  
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<const Hlt::Candidate*,TYPE>::result_type 
      operator() 
        ( typename LoKi::Functor<const Hlt::Candidate*,TYPE>::argument a ) const 
      {
        const TYPE result = m_fun.fun ( a ) ;
        if ( 0 == a ) 
        {
          this -> Error ( "Hlt::Candidate* is NULL! Logging is impossible" ) ;
          return result ;
        }
        //
        const Hlt::Stage* s = a -> template get<Hlt::Stage>();
        //
        this -> updateInfo ( s , result ) ;
        //
        return result ;
      }  
      /// OPTIONAL: the nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { 
        s << "logging(" << m_fun ;
        if  ( !m_trivial ) { s << ",'" << m_key << "'" ;}
        //
        return s << ")" ;
      }    
      // ======================================================================
    protected:
      // ======================================================================
      /// update info
      bool updateInfo
      ( const Hlt::Stage*       stage , 
        const TYPE&             value ) const 
      {
        if ( 0 == stage ) { return false ; }
        //
        Hlt::Stage* _stage = const_cast<Hlt::Stage*>( stage ) ;
        //
        Hlt::Stage::Lock lock ( _stage , m_algorithm ) ;
        //
        if ( m_name.empty() ) { m_name = this->m_fun.printOut() ; }
        //
        lock.addToHistory ( m_name ) ;
        //
        _stage->updateInfo ( m_key , value ) ;
        //
        return true ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Log_ () ;                      // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// the function 
      LoKi::FunctorFromFunctor<const Hlt::Candidate*,TYPE> m_fun ; // the function 
      /// the key 
      Gaudi::StringKey m_key ;
      // ======================================================================
    private:
      // ======================================================================
      /// trivial?
      bool m_trivial ;
      /// own name 
      mutable std::string                              m_name      ;
      /// the  algorithm 
      LoKi::Interface<INamedInterface>                 m_algorithm ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Cache_
     *  Simple functor for getting results from Cache 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-06
     */
    template <class TYPE>
    class Cache_ : public LoKi::Candidates::Log_<TYPE>
    {
    public:
      // ======================================================================
      /// constructor from the function and the fictive argument  
      Cache_ 
      ( const LoKi::Functor<const Hlt::Candidate*,TYPE>&    f      , 
        const std::string&                                  key    , 
        const bool                                          update )
        : LoKi::Candidates::Log_<TYPE>( f , key ) 
        , m_update ( update ) 
      {}
      /// MANDATORY: virtual destructor 
      virtual ~Cache_ () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Cache_* clone () const { return new Cache_( *this ) ; }  
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<const Hlt::Candidate*,TYPE>::result_type 
      operator() 
        ( typename LoKi::Functor<const Hlt::Candidate*,TYPE>::argument a ) const 
      {
        if ( 0 == a ) 
        {
          this->Error ( "Hlt::Candidate* is NULL! Logging is impossible" ) ;
          return this->m_fun.fun ( a ) ;
        }
        //
        const Hlt::Stage* a_ = a -> template get<Hlt::Stage> () ;
        if ( 0 == a_ ) 
        {
          this->Error ( "Hlt::Stage* is NULL! Logging is impossible" ) ;
          return this->m_fun.fun ( a ) ;
        }
        //
        if ( a_ ->hasInfo_<TYPE>( this->m_key ) )
        {
          static const TYPE s_type = 0 ;
          return a_ -> info ( this->m_key , s_type ); // RETURN 
        }
        // 
        TYPE result = this->m_fun.fun ( a ) ;
        //
        if ( m_update ) { this->updateInfo ( a_ , result ) ; }
        //
        return result ;
      }  
      /// OPTIONAL: the nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { 
        s << " info ( " << this->m_fun ;
        s << " ' "      << this->m_key << "'" ;
        s << " , " <<  ( m_update ? "True" : "False" ) ;
        return s << ") ";
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Cache_ () ;                        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// update flag 
      bool             m_update ; // update flag 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                         end of namespace Hlt::Candidates
  // ==========================================================================
  /** helper function to create logging function 
   *  @see LoKi::Candidates::Log_ 
   *  @param fun    (INPUT) the input functor 
   *  @param log    (INPUT) fictive parameter 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-12-07
   */
  template <class TYPE>
  LoKi::FunctorFromFunctor<const Hlt::Candidate*,TYPE>
  log_  
  ( const LoKi::Functor<const Hlt::Candidate*,TYPE>& fun , 
    const bool                                       log ) 
  {
    if ( !log ) { return fun ; }
    return LoKi::Candidates::Log_<TYPE> ( fun , log ) ;
  }
  // ========================================================================
  /** helper function to create logging function 
   *  @see LoKi::Candidates::Log_ 
   *  @param fun    (INPUT) the input functor 
   *  @param key    (INPUT) the logging key 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-12-07
   */
  template <class TYPE>
  LoKi::FunctorFromFunctor<const Hlt::Candidate*,TYPE>
  log_  
  ( const LoKi::Functor<const Hlt::Candidate*,TYPE>& fun , 
    const std::string&                               key )  
  {
    return LoKi::Candidates::Log_<TYPE> ( fun , key ) ;
  }
  // ========================================================================
  /** helper function to create caching-function 
   *  @see LoKi::Candidates::Cache_ 
   *  @param fun    (INPUT) the input functor 
   *  @param key    (INPUT) the key for logging 
   *  @param update (INPUT) update cache? 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-12-07
   */
  template <class TYPE>
  LoKi::FunctorFromFunctor<const Hlt::Candidate*,TYPE>
  cache_  
  ( const LoKi::Functor<const Hlt::Candidate*,TYPE>& fun    , 
    const std::string&                               key    , 
    const bool                                       update )
  {
    return LoKi::Candidates::Cache_<TYPE> ( fun , key , update ) ;
  }
  // ==========================================================================
  namespace Stages 
  {
    // ========================================================================
    /** @class Log_
     *  Simple functor for logging the results in Cache 
     *  @see Hlt::Cache 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-06
     */
    template <class TYPE>
    class Log_ : public LoKi::Functor<const Hlt::Stage*,TYPE>
    {
    public:
      // ======================================================================
      /// constructor from the function and the fictive argument  
      Log_ 
      ( const LoKi::Functor<const Hlt::Stage*,TYPE>&    f     , 
        const bool                                   /* a */  )
        : LoKi::Functor<const Hlt::Stage*,TYPE>()
        , m_fun     ( f            ) 
        , m_key     ( f.printOut() ) 
        , m_trivial ( true )  
        , m_name    () 
      {
        m_algorithm = LoKi::AlgUtils::getAlg ( *this , false ) ;
        if ( !m_algorithm ) { m_algorithm = this->lokiSvc() ; }
      } 
      /// constructor from the function and the key 
      Log_ 
      ( const LoKi::Functor<const Hlt::Stage*,TYPE>&    f     , 
        const std::string&                              key   ) 
        : LoKi::Functor<const Hlt::Stage*,TYPE>()
        , m_fun     ( f   ) 
        , m_key     ( key ) 
        , m_trivial ( false  )  
        , m_name    () 
      {
        m_algorithm = LoKi::AlgUtils::getAlg ( *this , false ) ;
        if ( !m_algorithm ) { m_algorithm = this->lokiSvc() ; }
      } 
      /// MANDATORY: virtual destructor 
      virtual ~Log_ () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Log_* clone () const { return new Log_( *this ) ; }  
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<const Hlt::Stage*,TYPE>::result_type 
      operator() 
        ( typename LoKi::Functor<const Hlt::Stage*,TYPE>::argument a ) const 
      {
        const TYPE result = m_fun.fun ( a ) ;
        if ( 0 == a ) 
        {
          this -> Error ( "Hlt::Stage* is NULL! Logging is impossible" ) ;
          return result ;
        }
        //
        this -> updateInfo ( a , result ) ;
        //
        return result ;
      }  
      /// OPTIONAL: the nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { 
        s << "logging(" << m_fun ;
        if  ( !m_trivial ) { s << ",'" << m_key << "'" ;}
        return s << ")" ;
      }    
      // ======================================================================
    protected:
      // ======================================================================
      /// update info
      bool updateInfo 
      ( const Hlt::Stage*       stage , 
        const TYPE&             value ) const 
      {
        if ( 0 == stage ) { return false ; }
        //
        Hlt::Stage* _stage = const_cast<Hlt::Stage*>( stage ) ;
        //
        Hlt::Stage::Lock lock ( _stage , m_algorithm ) ;
        //
        if ( m_name.empty() ) { m_name = this ->m_fun.printOut() ; }
        //
        lock.addToHistory ( m_name ) ;
        //
        _stage->updateInfo ( m_key , value ) ;
        //
        return true ;
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Log_ () ;                      // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// the function 
      LoKi::FunctorFromFunctor<const Hlt::Stage*,TYPE> m_fun ; // the function 
      /// the key
      Gaudi::StringKey                                 m_key       ; // the key 
      // ======================================================================
    private:
      // ======================================================================
      /// trivial?
      bool m_trivial ;
      /// own name 
      mutable std::string                              m_name      ;
      /// the  algorithm 
      LoKi::Interface<INamedInterface>                 m_algorithm ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Cache_
     *  Simple functor for getting results from Cache 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-06
     */
    template <class TYPE>
    class Cache_ : public LoKi::Stages::Log_<TYPE>
    {
    public:
      // ======================================================================
      /// constructor
      Cache_ 
      ( const LoKi::Functor<const Hlt::Stage*,TYPE>&    f      , 
        const std::string&                              key    , 
        const bool                                      update )
        : LoKi::Stages::Log_<TYPE>  ( f , key ) 
        , m_update ( update ) 
      {} 
      /// MANDATORY: virtual destructor 
      virtual ~Cache_ () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Cache_* clone () const { return new Cache_( *this ) ; }  
      /// MANDATORY: the only one essential method 
      virtual typename LoKi::Functor<const Hlt::Stage*,TYPE>::result_type 
      operator() ( typename LoKi::Functor<const Hlt::Stage*,TYPE>:: argument a ) const 
      {
        if ( 0 == a ) 
        {
          this -> Error ( "Hlt::Stage* is NULL! Logging is impossible" ) ;
          return this->m_fun.fun ( a ) ;
        }
        //
        const Hlt::Stage* a_ = a ;
        if ( a_ ->hasInfo_<TYPE>( this->m_key ) )
        {
          static const TYPE s_type = 0 ;
          return a_ ->info ( this->m_key , s_type ); // RETURN 
        }
        // 
        TYPE result = this->m_fun.fun ( a ) ;
        //
        if ( m_update ) { this->updateInfo ( a_ , result ) ; }
        //
        return result ;
      }  
      /// OPTIONAL: the nice printout 
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { 
        s << " info ( " << this-> m_fun ;
        s << " ' "      << this-> m_key << "'" ;
        s << " , " <<  ( m_update ? "True" : "False" ) ;
        return s << ") ";
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Cache_ () ;                        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// update flag 
      bool             m_update ; // update flag 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Stages    
  // ==========================================================================  
  /** helper function to create logging function 
   *  @see LoKi::Stages::Log_ 
   *  @param fun    (INPUT) the input functor 
   *  @param log    (INPUT) fictive parameter 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-12-07
   */
  template <class TYPE>
  LoKi::FunctorFromFunctor<const Hlt::Stage*,TYPE>
  log_  
  ( const LoKi::Functor<const Hlt::Stage*,TYPE>& fun , 
    const bool                                   log ) 
  {
    if ( !log ) { return fun ; }
    return LoKi::Stages::Log_<TYPE> ( fun , log ) ;
  }
  // ==========================================================================
  /** helper function to create logging function 
   *  @see LoKi::Stages::Log_ 
   *  @param fun    (INPUT) the input functor 
   *  @param key    (INPUT) the key for logging 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-12-07
   */
  template <class TYPE>
  LoKi::FunctorFromFunctor<const Hlt::Stage*,TYPE>
  log_  
  ( const LoKi::Functor<const Hlt::Stage*,TYPE>& fun , 
    const std::string&                           key ) 
  {
    return LoKi::Stages::Log_<TYPE> ( fun , key ) ;
  }
  // ==========================================================================
  /** helper function to create caching-function 
   *  @see LoKi::Stages::Cache_ 
   *  @param fun    (INPUT) the input functor 
   *  @param key    (INPUT) the key for logging 
   *  @param update (INPUT) update cache? 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2010-12-07
   */
  template <class TYPE>
  LoKi::FunctorFromFunctor<const Hlt::Stage*,TYPE>
  cache_  
  ( const LoKi::Functor<const Hlt::Stage*,TYPE>& fun    , 
    const std::string&                           key    , 
    const bool                                   update )
  {
    return LoKi::Stages::Cache_<TYPE> ( fun , key , update ) ;
  }
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_CACHEFUNCS_H
// ============================================================================
