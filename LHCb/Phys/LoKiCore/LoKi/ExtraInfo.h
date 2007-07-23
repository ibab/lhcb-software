// $Id: ExtraInfo.h,v 1.1 2007-07-23 17:07:37 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_EXTRAINFO_H 
#define LOKI_EXTRAINFO_H 1
// ============================================================================
// Include files
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
      /// constructor from the index and default value 
      GetInfo ( const int    index , 
                const double value ) 
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
      virtual result_type operator() ( argument a )  const
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
      virtual result_type operator() ( argument a )  const
      { return LoKi::ExtraInfo::hasInfo ( a , m_index ) ; }
      /** OPTIONAL: the nice printout 
       *  @attention: it is worth to redefine for each type
       */
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return s <<  this->objType() << "(" << m_index << ")" ; }
    private:
      // the default contructot is disabled 
      CheckInfo() ; ///< the default contructor is disabled
    public:
      /// get the index 
      int index() const { return m_index ; }
    private:
      // index to be searched 
      int    m_index   ; ///< index to be searched 
    };
  } // end of namespace LoKi::ExtraInfo
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_EXTRAINFO_H
// ============================================================================

