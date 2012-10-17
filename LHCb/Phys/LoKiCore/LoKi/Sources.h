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
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GetData.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Services.h"
// ============================================================================
/** @file LoKi/Sources.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-17
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Functors
  {
    // ========================================================================
    /** @class Sources LoKi/Sources.h
     *
     *  The concept belongs to the Gerhard "The Great" Raven.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2007-11-28
     */
    template <class TYPE                            , 
              class TYPE2=std::vector<TYPE*>        , 
              class TYPE3= typename TYPE::Container >
    class Source: public LoKi::Functor<void,TYPE2>
    {
    private:
      // ======================================================================
      typedef  LoKi::Functor<void,TYPE2> Self      ;
      // ======================================================================
    public:
      // ======================================================================
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
        ( /* typename Self::argument */ ) const 
      {
        /// locate the service if needed:
        if ( !m_svc ) 
        {
          const LoKi::Services& svcs = LoKi::Services::instance() ;
          IDataProviderSvc* evtSvc = svcs.evtSvc() ;
          Assert ( 0 != evtSvc , "Could not extract EventDataService!" ) ;
          m_svc = evtSvc ;
        }
        /// get the data from TES:
        SmartDataPtr<TYPE3> data ( m_svc , m_path ) ;
        /// check the validity:
        Assert ( !(!data) , "No valid data is found at '" + m_path + "'" ) ;
        /// return the valid data 
        return typename Self::result_type ( data->begin() , data->end() ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      /// get the service
      const LoKi::Interface<IDataProviderSvc>& evtSvc() const { return m_svc ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the data provider service 
      mutable LoKi::Interface<IDataProviderSvc> m_svc ; // data service 
      /// TES location of the data 
      std::string                               m_path ; // TES location of data 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace LoKi::Functors
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
  LoKi::Functors::Source<TYPE>
  source ( const std::string& path     , 
           IDataProviderSvc*  svc  = 0 )
  {
    return LoKi::Functors::Source<TYPE>( path , svc ) ;
  }
  // ==========================================================================  
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_SOURCES_H
// ============================================================================
