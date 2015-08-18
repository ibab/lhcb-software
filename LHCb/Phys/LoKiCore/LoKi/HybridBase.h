// ============================================================================
#ifndef LOKIHYBRID_HYBRIDBASE_H 
#define LOKIHYBRID_HYBRIDBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD &SLT
// ============================================================================
#include <vector>
#include <map>
// ============================================================================
// GaudiAlg/GaudiTools 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/FunctorCache.h"
#include "LoKi/CacheFactory.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // =========================================================================
    /** @class Base LoKi/HybridBase.h
     *
     *  The base class for "hybrid" (C++/Python) tools  
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-29
     */
    class Base : public                 GaudiTool 
    {
      // ======================================================================
    public:
      // ======================================================================
      typedef std::vector<std::string> Strings ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize   () ;
      // ======================================================================
    protected:
      // ======================================================================
      /** execute the python code 
       *  @attention the method must be invoked *ONLY* with the appropriate scope!!!
       *  @param code python code 
       *  @return status code 
       */
      StatusCode executeCode ( const std::string& code ) const ;
      // ======================================================================
    protected:    
      // ======================================================================
      /// Standard constructor
      Base 
      ( const std::string& type   , 
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destructor: virtual and protected 
      virtual ~Base( );
      // ======================================================================
    protected:
      // ======================================================================
      /// helper method to decrease amount of the typing
      template <class TYPE>
      void       _set 
      ( TYPE*& local , const TYPE& right ) ;
      // ======================================================================
      template <class TYPE1,class TYPE2>
      StatusCode _get_
      ( const std::string&                                            code    , 
        LoKi::Functor<TYPE1,TYPE2>*&                                  local   , 
        typename LoKi::Assignable<LoKi::Functor<TYPE1,TYPE2> >::Type& output  ) ;
      // ======================================================================
    public:
      // ======================================================================
      // flag to show the prepared code 
      bool showCode () const { return m_showCode ; }
      // ======================================================================
    protected:
      // ======================================================================
      /** prepare the actual python code from the functor:
       *  @param modules list of modules 
       *  @param actor the actual actor
       *  @param code the code for the functor 
       *  @param lines more python lines to be used 
       *  @param context  even more python code to be used 
       *  @return the valid python code 
       */
      std::string makeCode 
      ( const Strings&     modules             , 
        const std::string& actor               ,
        const std::string& code                , 
        const Strings&     lines   = Strings() , 
        const std::string& context = ""        ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// write C++ code 
      void writeCpp () const ;
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      Base () ; ///< the default constructor is disabled 
      // copy constructor is disabled 
      Base           ( const Base& ) ; ///< copy constructor is disabled 
      // assignement operator is disabled 
      Base& operator=( const Base& ) ; ///< assignement operator is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// execute the code 
      StatusCode executeCode () ;
      // ======================================================================
    private: 
      // ======================================================================
      mutable bool             m_pyInit   ;
      // flag to display the prepared code 
      bool                     m_showCode ; ///< flag to display the prepared code 
      // ======================================================================
    protected : // use python as factroy for LOK-functors ?
      // ======================================================================  
      /// use python as factroy for LOKI-functors ?
      bool m_use_python ;           // use python as factory for LOKI-functors ?
      /// use LoKi functor cache
      bool m_use_cache ;            // use LoKi functor cache ?
      // ======================================================================
    protected : // some stuff to deal with generation of C++ code 
      // ======================================================================
      // make c++ code ?
      bool                           m_makeCpp    ; ///< make C++ code ?
      // file name for C++ code 
      std::string                    m_cppname    ; ///< file name for C++ code 
      // header lines for C++ code 
      std::vector<std::string>       m_cpplines   ; ///< header lines for C++ code 
      /// 
      // information about the created functors 
      typedef std::pair<std::string,std::string>  PAIR       ;
      typedef std::map<std::string,PAIR>          FUNCTIONS  ;
      typedef std::map<std::string,FUNCTIONS>     ALLFUNCS   ;
      ALLFUNCS                       m_allfuncs   ;
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                            end of namespace LoKi::Hybrid 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// helper method to decrease the typing
// ============================================================================
template <class TYPE>
inline void LoKi::Hybrid::Base::_set ( TYPE*& local , const TYPE& right ) 
{ 
  if ( 0 != local ) 
  { 
    if ( msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "setCut/Fun(): Existing 'Function/Predicate' is substituted !" ).ignore() ; } ;
    delete local;
    local = 0 ;
  }
  // clone it! 
  local = right.clone() ;
  // debug printput:
  if ( msgLevel ( MSG::DEBUG ) ) 
  {
    debug() << "The 'cut' is set to be '" << (*local) << "' = '"
            << System::typeinfoName( typeid( *local) ) << endmsg  ;
  } ;
  //
}
// ============================================================================
template <class TYPE1,class TYPE2>
inline 
StatusCode LoKi::Hybrid::Base::_get_
( const std::string&                                            code    , 
  LoKi::Functor<TYPE1,TYPE2>*&                                  local   , 
  typename LoKi::Assignable<LoKi::Functor<TYPE1,TYPE2> >::Type& output  ) 
{
  // ==========================================================================
  //
  // 1) clear the placeholder, if needed
  //
  if ( NULL != local ) { delete local ; local = NULL ; }
  //
  // 2') look for cached functors:
  typedef LoKi::CacheFactory< LoKi::Functor<TYPE1,TYPE2> > cache_t;
  LoKi::Functor<TYPE1,TYPE2>* created = 
    ( !this->m_use_cache ? NULL :
      cache_t::Factory::create ( cache_t::id ( LoKi::Cache::makeHash ( code ) ) ) );
  //
  if ( created ) 
  {
    local  = created ; 
    output = *local ;
    //
    this->counter("# loaded from CACHE" ) += 1 ;
    //
    delete local ; local = NULL ;
    //
    return StatusCode::SUCCESS ;    // RETURN
  } 
  // 
  if ( !this->m_use_python ) { return StatusCode::FAILURE ; }
  //
  // 2") execute the code 
  //
  const StatusCode sc = this->executeCode ( code ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Error from LoKi::Hybrid::Base::executeCode", sc  ) ; } // RETURN 
  if ( NULL == local  ) 
  { return Error ( "Invalid object for the code"                     ) ; } // RETURN 
  // assign the result 
  output = *local ;                                                        // ASSIGN
  //
  this->counter("# loaded from PYTHON") += 1 ;
  //
  delete local ; local = NULL ;
  //
  if ( this->m_makeCpp ) 
  {
    const std::string funtype = System::typeinfoName ( typeid ( LoKi::Functor<TYPE1,TYPE2> ) ) ;
    const std::string cppcode = Gaudi::Utils::toCpp           ( output )   ;
    const std::string pytype  = Gaudi::Utils::toString        ( output )   ;
    m_allfuncs[ funtype ][ code ] = std::make_pair ( cppcode , pytype ) ;
    //
  }
  //
  return sc ;
  // =========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKIHYBRID_HYBRIDTOOL_H
// ============================================================================
