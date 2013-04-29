// $Id$
// ============================================================================
#ifndef LOKI_GENSOURCES_H 
#define LOKI_GENSOURCES_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKErnel
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
// ============================================================================
// GenEvent
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/GenTypes.h"
// ============================================================================
/** @file LoKi/GenSources.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace GenParticles 
  {
    // ========================================================================
    /** @class SourceTES 
     *  @see LoKi::Cuts::GSOURCE
     *  simple "source for the generator particles"
     *  @author Vanya BELYAEV ibelyav@physics.syr.edu
     *  @date 2006-12-07
     */
    class SourceTES 
      : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Source 
    { 
      // =====================================================================
      typedef LoKi::BasicFunctors<const HepMC::GenParticle*>::Source _Source;
      // =====================================================================
    public:
      // =====================================================================
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const std::string&           path = LHCb::HepMCEventLocation::Default , 
        IDataProviderSvc*            svc  = 0                                 , 
        const LoKi::GenTypes::GCuts& cuts = 
        LoKi::BasicFunctors<const HepMC::GenParticle*>::BooleanConstant(true) ) ; 
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::string&           path     , 
        const LoKi::GenTypes::GCuts& cuts     ,  
        IDataProviderSvc*            svc  = 0 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::GenTypes::GCuts& cuts                                     ,  
        const std::string&           path = LHCb::HepMCEventLocation::Default ,
        IDataProviderSvc*            svc  = 0                                 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::GenTypes::GCuts& cuts                                     ,  
        IDataProviderSvc*            svc                                      ,  
        const std::string&           path = LHCb::HepMCEventLocation::Default ) ;
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
      // ======================================================================
    public:
      // ======================================================================
      /// get the path 
      const std::string& path() const { return m_path ; }
      /// get the service
      const LoKi::Interface<IDataProviderSvc>& dataSvc() const 
      { return m_dataSvc ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// set the  path 
      void setPath ( const std::string& value ) { m_path = value ; }
      /// set the  service 
      void setPath ( const                 IDataProviderSvc*  value ) 
      { m_dataSvc = value ; }
      /// set the  service 
      void setPath ( const LoKi::Interface<IDataProviderSvc>& value ) 
      { m_dataSvc = value ; }
      // ======================================================================
    private:
      // ======================================================================
      // TES location of HepMC-events 
      std::string m_path ;              ///< TES location of HepMC events 
      /// data provder service 
      mutable LoKi::Interface<IDataProviderSvc>  m_dataSvc ;
      /// 'on-flight' filter
      LoKi::GenTypes::GCut m_cut ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                      end of namespace LoKi::GenParticles  
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef GSOURCE 
     *  The simlpe ``source'' of HepMC-particles 
     *  @author Vanya BELAYEV Ivan.BElyaev@cern.ch
     *  @date 2006-12-07
     */
    typedef LoKi::GenParticles::SourceTES GSOURCE ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENSOURCES_H
// ============================================================================
