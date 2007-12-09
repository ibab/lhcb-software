// $Id: MCSources.h,v 1.1 2007-12-09 18:20:17 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCSOURCES_H 
#define LOKI_MCSOURCES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
// ============================================================================
// MCEvent
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi
{
  namespace MCParticles 
  {
    /** @class SourceTES 
     * 
     *  @see LoKi::Cuts::MCSOURCE
     *  simple "source for the Monte Carlo particles"
     *  @author Vanya BELYAEV ibelyav@physics.syr.edu
     *  @date 2006-12-07
     */
    class SourceTES 
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Source 
    { 
      typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Source _Source;
    public:
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const std::string&           path = LHCb::MCParticleLocation::Default , 
        IDataProviderSvc*            svc  = 0                                 , 
        const LoKi::MCTypes::MCCuts& cuts = 
        LoKi::BasicFunctors<const LHCb::MCParticle*>::BooleanConstant(true) ) ; 
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::string&           path     , 
        const LoKi::MCTypes::MCCuts& cuts     ,  
        IDataProviderSvc*            svc  = 0 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::MCTypes::MCCuts& cuts                                     ,  
        const std::string&           path = LHCb::MCParticleLocation::Default ,
        IDataProviderSvc*            svc  = 0                                 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::MCTypes::MCCuts& cuts                                     ,  
        IDataProviderSvc*            svc                                      ,  
        const std::string&           path = LHCb::MCParticleLocation::Default ) ;
      /// copy constructor
      SourceTES ( const SourceTES& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SourceTES() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SourceTES* clone() const { return new SourceTES(*this) ; }
      /// MANDATORY: the only essential method:
      virtual result_type operator() () const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& o ) const ;
    public:
      /// get the path 
      const std::string& path() const { return m_path ; }
      /// get the service
      const LoKi::Interface<IDataProviderSvc>& dataSvc() const 
      { return m_dataSvc ; }      
    public:
      /// set the  path 
      void setPath    ( const std::string& value ) { m_path = value ; }
      /// set the  service 
      void setDataSvc ( const                 IDataProviderSvc*  value ) 
      { m_dataSvc = value ; }
      /// set the  service 
      void setDataSvc ( const LoKi::Interface<IDataProviderSvc>& value ) 
      { m_dataSvc = value ; }
    private:
      // TES location of HepMC-events 
      std::string m_path ;              ///< TES location of HepMC events 
      /// data provder service 
      mutable LoKi::Interface<IDataProviderSvc>  m_dataSvc ;
      /// 'on-flight' filter
      LoKi::MCTypes::MCCut m_cut ;
    } ;
  } // end of namespace LoKi::GenParticles  
  // ==========================================================================
  namespace Cuts 
  {
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCSOURCES_H
// ============================================================================
