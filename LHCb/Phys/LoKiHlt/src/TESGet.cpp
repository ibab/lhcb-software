// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "Event/HltDecReports.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/TESGet.h"
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class TYPE>
  class _C : public LoKi::Functor<void,bool>
  {
  public:
    // ========================================================================
    _C ( const LoKi::TES::Get&                  obj , 
         const LoKi::Functor<const TYPE*,bool>& cut ) 
      : LoKi::Functor<void,bool> () 
      , m_get ( obj ) 
      , m_cut ( cut ) 
      , m_t   ( 0   ) 
    {}
    // ========================================================================     
    virtual ~_C () {}
    virtual  _C* clone() const { return new _C(*this) ; }
    // ========================================================================
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return  s << " (" << m_get << " >> " << m_cut  << ") " ; }
    //
    virtual std::string toCpp() const 
    {
      return
        "LoKi::TES::TESGet::get( "    +
        Gaudi::Utils::toCpp ( m_get ) + ", " + 
        Gaudi::Utils::toCpp ( m_cut ) + ") " ;
    }
    // ========================================================================
    virtual result_type operator() ( /* argument */ ) const 
    {
      //
      if ( 0 == m_t || !sameEvent() ) 
      { 
        m_t = LoKi::TES::get_<TYPE> ( m_get ) ; 
        setEvent () ;
      }
      //
      if ( 0 == m_t ) 
      {
        Error ( "No valid object is found at '" + m_get.location() + "', return False") ;
        return false ;
      }
      //
      return m_cut ( m_t ) ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    _C() ;                                // default constructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    LoKi::TES::Get                             m_get ;
    LoKi::FunctorFromFunctor<const TYPE*,bool> m_cut ;
    mutable const TYPE*                        m_t   ;
    // ========================================================================    
  };
  // ==========================================================================
  template <class TYPE>
  class _F : public LoKi::Functor<void,double>
  {
  public:
    // ========================================================================
    _F ( const LoKi::TES::Get&                    obj , 
         const LoKi::Functor<const TYPE*,double>& fun ) 
      : LoKi::Functor<void,double> () 
      , m_get ( obj ) 
      , m_fun ( fun ) 
      , m_t   ( 0   ) 
    {}
    // ========================================================================     
    virtual ~_F () {}
    virtual  _F* clone() const { return new _F(*this) ; }
    // ========================================================================
    virtual std::ostream& fillStream ( std::ostream& s ) const 
    { return  s << " (" << m_get << " >> " << m_fun  << ") " ; }
    // ========================================================================
    virtual std::string toCpp() const 
    {
      return
        "LoKi::TES::TESGet::get( "    +
        Gaudi::Utils::toCpp ( m_get ) + ", " + 
        Gaudi::Utils::toCpp ( m_fun ) + ") " ;
    }
    // ========================================================================
    virtual result_type operator() ( /* argument */ ) const 
    {
      //
      if ( 0 == m_t || !sameEvent() ) 
      { 
        m_t = LoKi::TES::get_<TYPE> ( m_get ) ; 
        setEvent () ;
      }
      //
      if ( 0 == m_t ) 
      {
        Error ( "No valid object is found at '" + m_get.location() + "', return -inf") ;
        return false ;
      }
      //
      return m_fun ( m_t ) ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    _F() ;                                 // default constructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    LoKi::TES::Get                               m_get ;
    LoKi::FunctorFromFunctor<const TYPE*,double> m_fun ;
    mutable const TYPE*                          m_t  ;
    // ========================================================================    
  };
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
LoKi::FunctorFromFunctor<void,bool>  
LoKi::TES::TESGet::get 
( const LoKi::TES::Get&                          obj , 
  const LoKi::Functor<const LHCb::ODIN*,bool>&   cut )
{ return _C<LHCb::ODIN>          ( obj , cut ) ; }
// ============================================================================
LoKi::FunctorFromFunctor<void,double>  
LoKi::TES::TESGet::get 
( const LoKi::TES::Get&                            obj , 
  const LoKi::Functor<const LHCb::ODIN*,double>&   fun )
{ return _F<LHCb::ODIN>          ( obj , fun ) ; }
// ============================================================================
LoKi::FunctorFromFunctor<void,bool>  
LoKi::TES::TESGet::get 
( const LoKi::TES::Get&                                obj , 
  const LoKi::Functor<const LHCb::L0DUReport*,bool>&   cut )
{ return _C<LHCb::L0DUReport>    ( obj , cut ) ; }
// ============================================================================
LoKi::FunctorFromFunctor<void,double>  
LoKi::TES::TESGet::get 
( const LoKi::TES::Get&                                  obj , 
  const LoKi::Functor<const LHCb::L0DUReport*,double>&   fun )
{ return _F<LHCb::L0DUReport>    ( obj , fun ) ; }
// ============================================================================
LoKi::FunctorFromFunctor<void,bool>  
LoKi::TES::TESGet::get 
( const LoKi::TES::Get&                                   obj , 
  const LoKi::Functor<const LHCb::HltDecReports*,bool>&   cut )
{ return _C<LHCb::HltDecReports> ( obj , cut ) ; }
// ============================================================================
LoKi::FunctorFromFunctor<void,double>  
LoKi::TES::TESGet::get 
( const LoKi::TES::Get&                                     obj , 
  const LoKi::Functor<const LHCb::HltDecReports*,double>&   fun )
{ return _F<LHCb::HltDecReports> ( obj , fun ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
