// $Id: Sources.h,v 1.1 2007-11-28 13:56:33 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_SOURCES_H 
#define LOKI_SOURCES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Services.h"
// ============================================================================
namespace LoKi 
{
  namespace Filters 
  {
    /** @class Sources Sources.h LoKi/Sources.h
     *
     *  The concept belongs to the Gerhard "The Great" Raven.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-28
     */
    template <class TYPE,class TYPE2=std::vector<TYPE*> >
    class Source: public LoKi::Functor<void,TYPE2>
    {
    private:
      typedef  LoKi::Functor<void,TYPE2> Self      ;
      typedef typename TYPE::Container   Container ;
    public:
      /// constructor from the service and path in TES 
      Source 
      ( IDataProviderSvc*  svc   , 
        const std::string& path  )
        : Self() 
        , m_svc  ( svc   ) 
        , m_path ( path  )
      {}
      /// constructor from the service and path in TES 
      Source 
      ( const std::string& path     , 
        IDataProviderSvc*  svc  = 0 )
        : Self() 
        , m_svc  ( svc   ) 
        , m_path ( path  )
      {}
      /// copy constructor
      Source ( const Source& right ) 
        : LoKi::AuxFunBase ( right ) 
        , Self   ( right         ) 
        , m_svc  ( right.m_svc   ) 
        , m_path ( right.m_path  )
      {}
      /// MANDATORY: virtual desctrutor
      virtual ~Source() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Source* clone() const { return new Source ( *this ) ; }
      /// MANATORY: the only one essenial method 
      virtual typename Self::result_type operator() 
        ( typename Self::argument /* a */ ) const 
      {
        // locate the service if needed:
        if ( !m_svc.validPointer() ) 
        {
          const LoKi::Services::Services& svc = LoKi::Services::instance() ;
          IDataProviderSvc* evtSvc = svc.evtSvc() ;
          Assert ( 0 != evtSvc , "Could not extract EventDataService!" ) ;
          m_svc = evtSvc ;
        }
        // get the data from TES:
        SmartDataPtr<Container> data ( m_svc , m_path ) ;
        // check the validity:
        Assert ( !(!data) , "No valid data is found at '" + m_path + "'" ) ;
        // return the valid data 
        return Self::result_type( data->begin() , data->end() ) ;
      }
    public:
      /// get the service
      const LoKi::Interface<IDataProviderSvc>& evtSvc() const { return m_svc ; }
    private:
      // the data provider service 
      LoKi::Interface<IDataProviderSvc> m_svc ; ///< data provider service
      // TES location of the data 
      std::string                       m_path ; ///< TES location of data 
    } ;
  } // end of namespace LoKi::Filters
  // ==========================================================================
  /** simple "source" function
   *
   *  @code
   *
   *
   *   std::vector<TYPE*> out = source<TYPE>
   *
   *   // some 'tee'-action:
   *   const LoKi::Functor<TYPE,TYPE2>& fun = ...; 
   *
   *   std::vector<TYPE> out = in >> tee ( fun ) ;
   *
 
   *  @endcode 
   *
   *  The concept belongs to the Gerhard "The Great" Raven.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-11-28
   */
  template <class TYPE>
  inline 
  std::vector<TYPE*>
  source ( const std::string& path     , 
           IDataProviderSvc*  svc  = 0 )
  {
    return LoKi::Filters::Source<TYPE>( path , svc ) ;
  }
  
  
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_SOURCES_H
// ============================================================================
