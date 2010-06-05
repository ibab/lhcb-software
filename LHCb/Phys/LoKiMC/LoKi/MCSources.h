// $Id: MCSources.h,v 1.2 2010-06-05 20:15:57 ibelyaev Exp $
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
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
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
#include "LoKi/IMCDecay.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace MCParticles 
  {
    // ========================================================================
    /** @class SourceTES 
     * 
     *  @see LoKi::Cuts::MCSOURCE
     *  simple "source for the Monte Carlo particles"
     *  @author Vanya BELYAEV ibelyav@physics.syr.edu
     *  @date 2006-12-07
     */
    class SourceTES : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Source 
    { 
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::MCParticle*>::Source _Source;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const std::string&              path = LHCb::MCParticleLocation::Default ,
        IDataProviderSvc*               svc  = 0                                 ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::string&              path       , 
        const LoKi::MCTypes::MCCuts&    cuts       ,  
        IDataProviderSvc*               svc  = 0   ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::string&              path       , 
        const Decays::iNode&            node       ,  
        IDataProviderSvc*               svc  = 0   ) ;
      /// constructor from the service, TES location   
      SourceTES
      ( const std::string&              path       , 
        const Decays::IMCDecay::Finder& finder     ,  
        IDataProviderSvc*               svc  = 0   ) ;
      /// constructor from the service, TES location   
      SourceTES
      ( const std::string&              path       , 
        const Decays::IMCDecay::iTree&  finder     ,  
        IDataProviderSvc*               svc  = 0   ) ;
      /// constructor from the service, TES location   
      SourceTES
      ( const std::string&              path       , 
        const std::string&              descriptor ,  
        IDataProviderSvc*               svc  = 0   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SourceTES() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SourceTES* clone () const ; 
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
      void setPath    ( const std::string& value ) { m_path = value ; }
      /// set the  service 
      void setDataSvc ( const                 IDataProviderSvc*  value ) 
      { m_dataSvc = value ; }
      /// set the  service 
      void setDataSvc ( const LoKi::Interface<IDataProviderSvc>& value ) 
      { m_dataSvc = value ; }
      // ======================================================================
    private:
      // ======================================================================
      /// TES location of MCParticles
      std::string               m_path       ;  // TES location of HepMC events 
      /// data provder service 
      mutable LoKi::Interface<IDataProviderSvc>  m_dataSvc ;
      /// 'on-flight' filter
      LoKi::MCTypes::MCCut      m_cut        ;            // 'on-flight' filter
      /// use decay finder ? 
      bool                      m_use_finder ;            // use decay finder ?
      /// decay finder 
      Decays::IMCDecay::Finder  m_finder     ;            //       decay finder 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                       end of namespace LoKi::MCParticles  
  // ==========================================================================
  namespace MCVertices 
  {
    // ========================================================================
    /** @class SourceTES 
     * 
     *  @see LoKi::Cuts::MCVSOURCE
     *  simple "source for the Monte Carlo vertices "
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-06-04
     */
    class SourceTES : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Source 
    { 
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::MCVertex*>::Source _Source;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the service, TES location and cuts 
      SourceTES 
      ( const std::string&              path = LHCb::MCVertexLocation::Default , 
        IDataProviderSvc*               svc  = 0                               ) ;
      /// constructor from the service, TES location and cuts 
      SourceTES
      ( const std::string&              path       , 
        const LoKi::MCTypes::MCVCuts&   cuts       ,  
        IDataProviderSvc*               svc  = 0   ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SourceTES() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SourceTES* clone () const ;
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
      void setPath    ( const std::string& value ) { m_path = value ; }
      /// set the  service 
      void setDataSvc ( const                 IDataProviderSvc*  value ) 
      { m_dataSvc = value ; }
      /// set the  service 
      void setDataSvc ( const LoKi::Interface<IDataProviderSvc>& value ) 
      { m_dataSvc = value ; }
      // ======================================================================
    private:
      // ======================================================================
      /// TES location of MCVertices 
      std::string               m_path       ;    // TES location of MCVertices  
      /// data provder service 
      mutable LoKi::Interface<IDataProviderSvc>  m_dataSvc ;
      /// 'on-flight' filter
      LoKi::MCTypes::MCVCut     m_cut        ;            // 'on-flight' filter
      // ======================================================================
    } ;
    // ========================================================================
  } //                                       end of namespace LoKi::MCParticles  
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef MCSOURCE 
     *  simple ``source'' for MC-particles 
     * 
     *  @code 
     * 
     *   const MCSOURCE s1 = MCSOURCE ( "MC/Particles" ,   MCPT > 1 * GeV ) ;
     *   const MCSOURCE s2 = MCSOURCE ( "MC/Particles" , " D0 => K- pi+ " ) ;
     *
     *    const LHCb::MCParticle::ConstVector v1 = s1() ;
     *    const LHCb::MCParticle::ConstVector v2 = s2() ;
     *
     *  @endcode 
     *  
     *  @see LoKi::MCParticles::SourceTES 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     */
    typedef LoKi::MCParticles::SourceTES                             MCSOURCE ;
    // ========================================================================
    /** @typedef MCVSOURCE 
     *  simple ``source'' for MC-vertices 
     * 
     *  @code 
     * 
     *   const MCVSOURCE s = MCSOURCE ( "MC/Vertices" ,   MCVZ < 1 * meter ) ;
     *
     *   const LHCb::MCVertex::ConstVector v2 = s2() ;
     *
     *  @endcode 
     *  
     *  @see LoKi::MCParticles::SourceTES 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     */
    typedef LoKi::MCParticles::SourceTES                             MCSOURCE ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MCSOURCES_H
// ============================================================================
