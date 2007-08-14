// $Id: ExtraInfo.h,v 1.4 2007-08-14 20:50:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_EXTRAINFO_H 
#define LOKI_EXTRAINFO_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <limits>
#include <climits>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Info.h"
// ============================================================================
namespace LoKi 
{
  namespace ExtraInfo 
  {
    // ========================================================================
    /** @var defaultValue
     *  the default value for missing keys 
     *  @author Vanya BELAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-14
     */
    const double defaultValue = -1*std::numeric_limits<double>::max() ;
    // ========================================================================
    /** @class GetInfo
     *  simple function which return the value of "extra info", 
     *  for classes equipped with "extraInfo" methods, e.g. 
     *  LHCb::Particle, LCHb::Vertex, LHCb::Track
     *  @author Vanya BELYAEV ibelyaev@physics.sye.edu
     *  @date 2007-06-14
     */
    template <class TYPE>
    class GetInfo : public LoKi::Function<TYPE>
    {
    public:
      /// constructor from the index and the default value 
      GetInfo ( const int    index                , 
                const double value = defaultValue ) 
        : LoKi::Function<TYPE>() 
        , m_index   ( index )  
        , m_default ( value ) 
      {} ;
      /// copy constructor 
      GetInfo ( const GetInfo& right ) 
        : LoKi::AuxFunBase     ( right           )  
        , LoKi::Function<TYPE> ( right           ) 
        , m_index              ( right.m_index   )  
        , m_default            ( right.m_default ) 
      {} ;
      /// MANDATORY: virtual destructor 
      virtual ~GetInfo() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GetInfo* clone() const { return new GetInfo(*this); }
      /// MANDATORY: the only one essential method
      virtual typename LoKi::Function<TYPE>::result_type operator() 
      ( typename LoKi::Function<TYPE>::argument a )  const
      { return LoKi::ExtraInfo::info ( a , m_index , m_default ) ; }
      /** OPTIONAL: the nice printout 
       *  @attention: it is worth to redefine it for each type
       */
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s << this->objType() << "(" << m_index << "," << m_default << ")" ; }
    private:
      // the default contructot is disabled 
      GetInfo() ; ///< the default contructor is disabled
    public:
      /// get the index 
      int     index  () const { return m_index   ; }
      double  value  () const { return m_default ; }
    private:
      // index to be searched 
      int    m_index   ; ///< index to be searched 
      // default value for non-existing index 
      double m_default ; ///< default value for non-existing index 
    };
    // ==========================================================================
    /** @class CheckInfo
     *  simple predicate to check the existence of "key" in the "extraInfo"
     *  for classes equipped with "extraInfo" methods 
     *  @author Vanya BELYAEV ibelyaev@physics.sye.edu
     *  @date 2007-06-14
     */
    template <class TYPE>
    class CheckInfo : public LoKi::Predicate<TYPE>
    {
    public:
      /// constructor fomr the index and default value 
      CheckInfo ( const int    index ) 
        : LoKi::Predicate<TYPE>() 
        , m_index   ( index )  
      {} ;
      /// copy constructor 
      CheckInfo ( const CheckInfo& right ) 
        : LoKi::AuxFunBase      ( right         )  
        , LoKi::Predicate<TYPE> ( right         ) 
        , m_index               ( right.m_index )  
      {} ;
      /// MANDATORY: virtual destructor 
      virtual ~CheckInfo () {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  CheckInfo* clone() const { return new CheckInfo (*this); }
      /// MANDATORY: the only one essential method
      virtual typename LoKi::Predicate<TYPE>::result_type operator() 
      ( typename LoKi::Predicate<TYPE>::argument a )  const
      { return LoKi::ExtraInfo::hasInfo ( a , m_index ) ; }
      /** OPTIONAL: the nice printout 
       *  @attention: it is worth to redefine for each type
       */
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s <<  this->objType() << "(" << m_index << ")" ; }
    private:
      // the default contructor is disabled 
      CheckInfo() ; ///< the default contructor is disabled
    public:
      /// get the index 
      int index() const { return m_index ; }
    private:
      // index to be searched 
      int    m_index   ; ///< index to be searched 
    };
    // ========================================================================
    /** @class GetSmartInfo
     *
     *  Simple class which checks the "exxtraInfo" field of the 
     *  argument
     *  for the given key, and return the corresponding value.
     *  For the missing keys the infomation is evaluated using the 
     *  supplied function and (optionally) inserted into "extraInfo"
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-14
     */
    template <class TYPE>
    class GetSmartInfo : public LoKi::Function<TYPE>
    {
    public:
      /// contructor from ID, function and the flag 
      GetSmartInfo 
      ( const int                   index          ,
        const LoKi::Function<TYPE>& fun            , 
        const bool                  update = false ) 
        : LoKi::Function<TYPE> () 
        , m_fun    ( fun    ) 
        , m_index  ( index  )
        , m_update ( update ) 
      {} ;
      /// copy constructor 
      GetSmartInfo ( const GetSmartInfo& right ) 
        : LoKi::AuxFunBase     ( right ) 
        , LoKi::Function<TYPE> ( right ) 
        , m_fun    ( right.m_fun    ) 
        , m_index  ( right.m_index  )
        , m_update ( right.m_update ) 
      {} ;
      /// MANDATORY: virtual destructor
      virtual ~GetSmartInfo() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  GetSmartInfo* clone() const { return new GetSmartInfo (*this); }
      /// MANDATORY: the only one essential method
      virtual typename LoKi::Function<TYPE>::result_type operator() 
      ( typename LoKi::Function<TYPE>::argument a )  const
      {
        // check extra info:
        if ( LoKi::ExtraInfo::hasInfo ( a , m_index ) ) 
        { return LoKi::ExtraInfo::info ( a , m_index , defaultValue  ) ; }   // RETURN 
        // evaluate the function 
        const double  result = m_fun ( a ) ;
        // update info (if needed) 
        if ( m_update ) { LoKi::ExtraInfo::addInfo( a , m_index , result ) ; }
        //
        return result ;                                             // RETURN 
      }
      /** OPTIONAL: the nice printout 
       *  @attention: it is worth to redefine for each type
       */
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { 
        s <<  this->objType() << "(" << m_fun << "," << m_index << "," ;
        return s <<  ( m_update ? "True" : "False"  ) << ")" ;
      }
      /** unique function ID (hash); see LoKi::genericID 
       *  @attention the key is used and funtion-ID
       *  @see LoKi::AuxFunBase
       *  @return the function-ID
       */
      virtual std::size_t         id     () const { return m_index ; }
    public:
      /// get the index 
      int                         index  () const { return m_index  ; }
      /// get the function 
      const LoKi::Function<TYPE>& fun    () const { return m_fun    ; }
      /// get the flag 
      bool                        update () const { return m_update ; }
    private:
      // no default constructor 
      GetSmartInfo() ; ///< no default constructor 
    private:
      // the function to be evaluated (if needed) 
      LoKi::FunctionFromFunction<TYPE> m_fun ; ///< the function to be evaluated (if needed) 
      // the index ("ID") in "extraInfo" table 
      int   m_index; ///< the index in "extraInfo" table
      // "update" -flag (indicate the insertion of "extraInfo" for missing data
      bool  m_update ; ///< "update flag"
    } ;  
  } // end of namespace LoKi::ExtraInfo
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTRAINFO_H
// ============================================================================

