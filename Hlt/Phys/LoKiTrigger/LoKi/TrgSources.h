// $Id$
// ============================================================================
#ifndef LOKI_TRSOURCES_H 
#define LOKI_TRSOURCES_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiTrack 
// ============================================================================
#include "LoKi/TrackSources.h"
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/TrackTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace RecVertices 
  {
    // ========================================================================
    /** @class SourceTES 
     * 
     *  @see LoKi::Cuts::RVSOURCE
     *  simple "source for the reconstructed vertices from TES "
     *  @author Vanya BELYAEV ibelyav@physics.syr.edu
     *  @date 2006-12-07
     */
    class SourceTES 
      : public LoKi::BasicFunctors<LHCb::RecVertex*>::Source 
    { 
      typedef LoKi::BasicFunctors<LHCb::RecVertex*>::Source _Source;
    public:
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const std::string&              path                    , 
        IDataProviderSvc*               svc  = 0                , 
        const LoKi::TrackTypes::RVCuts& cuts = 
        LoKi::BasicFunctors<LHCb::RecVertex>::BooleanConstant(true) ) ; 
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const std::vector<std::string>& path                    , 
        IDataProviderSvc*               svc  = 0                , 
        const LoKi::TrackTypes::RVCuts& cuts = 
        LoKi::BasicFunctors<LHCb::RecVertex>::BooleanConstant(true) ) ; 
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::string&              path     , 
        const LoKi::TrackTypes::RVCuts& cuts     ,  
        IDataProviderSvc*               svc  = 0 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::vector<std::string>& path     , 
        const LoKi::TrackTypes::RVCuts& cuts     ,  
        IDataProviderSvc*               svc  = 0 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::TrackTypes::RVCuts& cuts        ,  
        const std::string&              path        ,
        IDataProviderSvc*               svc  = 0    ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::TrackTypes::RVCuts& cuts        ,  
        const std::vector<std::string>& path        ,
        IDataProviderSvc*               svc  = 0    ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::TrackTypes::RVCuts& cuts  ,  
        IDataProviderSvc*               svc   ,  
        const std::string&              path  ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const LoKi::TrackTypes::RVCuts& cuts  ,  
        IDataProviderSvc*               svc   ,  
        const std::vector<std::string>& path  ) ;
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
    private:
      // the default constructor is disabled 
      SourceTES() ; ///< the default constructor is disabled 
    public:
      /// get the path 
      const std::vector<std::string>& paths() const { return m_path ; }
      /// get the service
      const LoKi::Interface<IDataProviderSvc>& dataSvc() const 
      { return m_dataSvc ; }      
    public:
      /// set the  paths
      void setPaths  ( const std::vector<std::string>& value ) { m_path = value ; }
      /// set the  paths
      void addToPath ( const std::string& value ) { m_path.push_back( value ) ; }
      /// set the  service 
      void setDataSvc ( const                 IDataProviderSvc*  value ) 
      { m_dataSvc = value ; }
      /// set the  service 
      void setDataSvc ( const LoKi::Interface<IDataProviderSvc>& value ) 
      { m_dataSvc = value ; }
    private:
      /// get the particles from the certain  TES location 
      std::size_t get 
      ( const std::string&   location , 
        LHCb::RecVertex::Vector& output   ) const ;
    private:
      // ======================================================================
      // TES locations of Particles 
      std::vector<std::string>  m_path ; ///< TES locations of HepMC events 
      // data provder service 
      mutable LoKi::Interface<IDataProviderSvc>  m_dataSvc ; ///< data provder service 
      // 'on-flight' filter
      LoKi::TrackTypes::RVCut   m_cut ; ///< 'on-flight' filter
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::RecVertices
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRSOURCES_H
// ============================================================================
