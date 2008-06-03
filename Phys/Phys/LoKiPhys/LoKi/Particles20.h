// $Id: Particles20.h,v 1.6 2008-06-03 15:47:08 cattanem Exp $
// ============================================================================
#ifndef LOKI_PARTICLES20_H 
#define LOKI_PARTICLES20_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Particles1.h"
#include "LoKi/Particles4.h"
#include "LoKi/Particles16.h"
#include "LoKi/Particles19.h"
#include "LoKi/PhysSources.h"
#include "LoKi/Vertices5.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
/** @file
 *  Collection of "context-dependent" functors, needed for the 
 *  new framework "CombineParticles", developed by Juan PALACIOS, 
 *   Patrick KOPPENBURG and Gerhard RAVEN.
 * 
 *  Essentially all these functord depends on "event-data" and 
 *  get the nesessary "context-dependent" data from Algorithm Context Service
 *
 *  The basic ingredients here:
 *   - LoKi Service 
 *   - Algorithm Context Service
 *   - PhysDesktop
 *   - LoKi::getPhysDesktop
 *   - Gaudi::Utils::getDValgorithm
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-16
 */
namespace LoKi 
{
  namespace Particles
  {
    // ========================================================================
    /** @class CosineDirectionAngleWithTheBestPV
     *  The special version of LoKi::Particles::CosineDirectionAngle functor,
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @see LoKi::Cuts::BPVDIRA
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::CosineDirectionAngle
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class CosineDirectionAngleWithTheBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      /// the default constructor, creates the object in invalid state 
      CosineDirectionAngleWithTheBestPV () ;
      /// MANDATORY: virtual destructor 
      virtual ~CosineDirectionAngleWithTheBestPV () {} ;
      /// MANDATORY: the clone method ("virtual constructor")
      virtual  CosineDirectionAngleWithTheBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
    private:
      // the actual function evaluator 
      LoKi::Particles::CosineDirectionAngle m_dira ; ///< the actual function evaluator 
    } ;
    // ======================================================================== 
    /** @class ImpParWithTheBestPV
     *  The special version of LoKi::Particles::ImpPar functor
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @see LoKi::Cuts::BPVIP
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::ImpPar
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class ImpParWithTheBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      /** the "default" constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      ImpParWithTheBestPV ( const std::string& geo = "" ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpParWithTheBestPV () {} ;
      /// MANDATORY: the clone method ("virtual constructor")
      virtual  ImpParWithTheBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
    public:
      // set the tool
      void setTool ( const IDistanceCalculator* t ) const 
      { m_ip.setTool ( t ) ; }      
      // set the tool
      void setTool ( const LoKi::Interface<IDistanceCalculator>& t ) const 
      { m_ip.setTool ( t ) ; }
    public:
      /// get the tool
      const LoKi::Vertices::ImpactParamTool& tool() const { return m_ip ; }
    public:
      // ======================================================================
      /// cast to
      operator const LoKi::Vertices::ImpactParamTool& () const { return m_ip ; }  
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functor 
      LoKi::Particles::ImpPar   m_ip  ; // the actual functor 
      /// the nick name or type name of the IDistanceCalculator
      std::string               m_geo ;
      // ======================================================================
    };
    // ========================================================================
    /** @class ImpParChi2WithTheBestPV
     *  The special version of LoKi::Particles::ImpParChi2 functor
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @see LoKi::Cuts::BPVIPCHI2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::ImpParChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class ImpParChi2WithTheBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      // ======================================================================
      /** the "default" constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      ImpParChi2WithTheBestPV ( const std::string& geo = "" ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ImpParChi2WithTheBestPV () {} ;
      /// MANDATORY: the clone method ("virtual constructor")
      virtual  ImpParChi2WithTheBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      // set the tool
      void setTool ( const IDistanceCalculator* t ) const 
      { m_ip.setTool ( t ) ; }      
      // set the tool
      void setTool ( const LoKi::Interface<IDistanceCalculator>& t ) const 
      { m_ip.setTool ( t ) ; }
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool
      const LoKi::Vertices::ImpactParamTool& tool() const { return m_ip ; }
      // ======================================================================
    public:
      // ======================================================================
      /// cast to
      operator const LoKi::Vertices::ImpactParamTool& () const { return m_ip ; } 
      // ======================================================================
    private:
      /// the actual functor 
      LoKi::Particles::ImpParChi2   m_ip  ; // the actual functor 
      /// the nick name or type name of the IDistanceCalculator
      std::string                   m_geo ;
    };
    // ========================================================================
    /** @class MinImpParWithSource
     *  The special version of LoKi::Particles::MinImpPar functor
     *  which gets all the primary vertices form the "source"
     *
     *  @see LoKi::Cuts::MIPSOURCE
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class MinImpParWithSource
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source _Source ;
      // ======================================================================
    public:
      // ======================================================================
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from the source and nickname or full type/name of 
       *  IDistanceCalculator tool
       *  @see DVAlgorithm::distanceCalculator 
       *  @param source the source 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParWithSource 
      ( const _Source&     source      , 
        const std::string& geo    = "" ) ;
      /// MANDATORY: virtual destructor
      virtual ~MinImpParWithSource() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParWithSource* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    public:
      /// get the tool
      // ======================================================================
      const LoKi::Vertices::ImpactParamTool& tool() const { return m_mip.impPar()  ; }
      /// cast to the tool 
      operator const LoKi::Vertices::ImpactParamTool& () const { return m_mip.impPar()  ; } 
      /// get the source 
      const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& source () const 
      { return m_source ; }
      /// cast to the source 
      operator const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& () const 
      { return m_source ; }
      // ======================================================================
    public:
      // get the nickname of full type/name of IDistanceCalculator tool
      // ======================================================================
      const std::string& geo() const { return m_geo ; }
    private:
      // ======================================================================
      // no default constructor 
      MinImpParWithSource () ; ///< no default constructor
      // ======================================================================
    private:
      // ======================================================================
      /// the underlying functor 
      mutable LoKi::Particles::MinImpPar m_mip    ; // the underlying functor 
      /// the source 
      LoKi::Assignable<_Source>::Type    m_source ; // the source 
      /// the nickname or type/name of IDistanceCalculator tool 
      std::string                        m_geo    ; // IDistanceCalculator tool
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinImpParDV
     *  The special version of LoKi::Particles::MinImpParWithSource functor
     *  which gets all the primary vertices from the Desktop
     *
     *  @see LoKi::Cuts::MIPDV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *  @see LoKi::Particles::MinImpParWithSource
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class MinImpParDV 
      : public LoKi::Particles::MinImpParWithSource 
    {
    public:
      // ======================================================================
      /** the "default" constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParDV  ( const std::string& geo = "" ) ;
      /** the constructor form the vertex selection functot and 
       *  the name/nickname of IDistanceCalculator tool from DVAlgorithm
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParDV  
      ( const LoKi::PhysTypes::VCuts& cuts     , 
        const std::string&            geo = "" ) ;
      /** the constructor form the vertex selection functot and 
       *  the name/nickname of IDistanceCalculator tool from DVAlgorithm
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParDV  
      ( const std::string&            geo  , 
        const LoKi::PhysTypes::VCuts& cuts ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpParDV () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParDV* clone () const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinImpParTES
     *  The special version of LoKi::Particles::MinImpParWithSource functor
     *  which gets all the primary vertices from the Transient Event Store
     *
     *  @see LoKi::Cuts::MIPTES
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpPar
     *  @see LoKi::Particles::MinImpParWithSource
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class MinImpParTES 
      : public LoKi::Particles::MinImpParWithSource 
    {
    public:
      // ======================================================================
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const std::string& path      , 
        const std::string& geo  = "" ) ;      
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const std::vector<std::string>& path     , 
        const std::string&              geo = "" ) ;      
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const LoKi::PhysTypes::VCuts&   cuts     ,
        const std::vector<std::string>& path     , 
        const std::string&              geo = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const LoKi::PhysTypes::VCuts& cuts      ,
        const std::string&            path      , 
        const std::string&            geo  = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const std::vector<std::string>& path     , 
        const LoKi::PhysTypes::VCuts&   cuts     ,
        const std::string&              geo = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const std::string&            path      , 
        const LoKi::PhysTypes::VCuts& cuts      ,
        const std::string&            geo  = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const std::vector<std::string>& path , 
        const std::string&              geo  , 
        const LoKi::PhysTypes::VCuts&   cuts ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParTES
      ( const std::string&            path , 
        const std::string&            geo  ,
        const LoKi::PhysTypes::VCuts& cuts ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpParTES () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParTES* clone () const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      // the list of TES locations 
      std::vector<std::string> m_path ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinImpParChi2WithSource
     *  The special version of LoKi::Particles::MinImpParChi2 functor
     *  which gets all the primary vertoices form "source"
     *
     *  @see LoKi::Cuts::MIPCHI2SOURCE
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class MinImpParChi2WithSource
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source _Source ;
      // ======================================================================
    public:
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from the source and nickname or full type/name of 
       *  IDistanceCalculator tool
       *  @see DVAlgorithm::distanceCalculator 
       *  @param source the source 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2WithSource 
      ( const _Source&     source      , 
        const std::string& geo    = "" ) ;
      /// MANDATORY: virtual destructor
      virtual ~MinImpParChi2WithSource() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParChi2WithSource* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    public:
      // ======================================================================
      /// get the tool
      const LoKi::Vertices::ImpactParamTool& tool() const { return m_mip.impParChi2()  ; }
      /// cast to the tool 
      operator const LoKi::Vertices::ImpactParamTool& () const { return m_mip.impParChi2()  ; } 
      /// get the source 
      const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& source () const 
      { return m_source ; }
      /// cast to the source 
      operator const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& () const 
      { return m_source ; }
      // ======================================================================
    public:
      // ======================================================================
      // get the nickname of full type/name of IDistanceCalculator tool
      const std::string& geo() const { return m_geo ; }
      // ======================================================================
    private:
      // ======================================================================
      // no default constructor 
      MinImpParChi2WithSource () ; ///< no default constructor
      // ======================================================================
    private:
      // ======================================================================
      /// the underlying functor 
      mutable LoKi::Particles::MinImpParChi2 m_mip ; // the underlying functor 
      /// the source 
      LoKi::Assignable<_Source>::Type    m_source ; // the source 
      /// the nickname or type/name of IDistanceCalculator tool 
      std::string                        m_geo    ; // IDistanceCalculator tool
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinImpParChi2DV
     *  The special version of LoKi::Particles::MinImpParChi2WithSource functor
     *  which gets all the primary vertices from the Desktop
     *
     *  @see LoKi::Cuts::MIPCHI2DV
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *  @see LoKi::Particles::MinImpParChi2WithSource
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class MinImpParChi2DV 
      : public LoKi::Particles::MinImpParChi2WithSource 
    {
    public:
      // ======================================================================
      /** the "default" constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2DV  ( const std::string& geo = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2DV  
      ( const LoKi::PhysTypes::VCuts& cuts     , 
        const std::string&            geo = "" ) ;
      /** the  constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2DV  
      ( const std::string&            geo  , 
        const LoKi::PhysTypes::VCuts& cuts ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpParChi2DV () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParChi2DV* clone () const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinImpParChi2TES
     *  The special version of LoKi::Particles::MinImpParChi2WithSource functor
     *  which gets all the primary vertices from the Transient Event Store
     *
     *  @see LoKi::Cuts::MIPCHI2TES
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::MinImpParChi2
     *  @see LoKi::Particles::MinImpParChi2WithSource
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-16
     */
    class MinImpParChi2TES 
      : public LoKi::Particles::MinImpParChi2WithSource 
    {
    public:
      // ======================================================================
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const std::string& path      , 
        const std::string& geo  = "" ) ;      
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const std::vector<std::string>& path     , 
        const std::string&              geo = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const LoKi::PhysTypes::VCuts& cuts      , 
        const std::string&            path      , 
        const std::string&            geo  = "" ) ;      
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const LoKi::PhysTypes::VCuts&   cuts     , 
        const std::vector<std::string>& path     , 
        const std::string&              geo = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const std::string&            path      , 
        const LoKi::PhysTypes::VCuts& cuts      , 
        const std::string&            geo  = "" ) ;      
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const std::vector<std::string>& path     , 
        const LoKi::PhysTypes::VCuts& cuts      , 
        const std::string&              geo = "" ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the location of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const std::string&            path , 
        const std::string&            geo  , 
        const LoKi::PhysTypes::VCuts& cuts ) ;
      /** the constructor,
       *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
       *  by full type/name
       *  @see DVAlgorithm::distanceCalculator 
       *  @param path the locations of vertices in TES 
       *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
       */
      MinImpParChi2TES
      ( const std::vector<std::string>& path , 
        const std::string&              geo  , 
        const LoKi::PhysTypes::VCuts&   cuts ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinImpParChi2TES () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinImpParChi2TES* clone () const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the list of TES locations 
      std::vector<std::string> m_path ; // the list of TES locations 
      // ======================================================================
    } ; 
    // ========================================================================
    /** @class VertexDistanceDV 
     *  The special version of LoKi::Particles::VertexDistance functor
     *  which gets "the best primary vertex" from the Desktop
     *
     *  @see LoKi::Cuts::BPVVD
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexDistance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class VertexDistanceDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the default constructor 
      VertexDistanceDV  () ;
      /// MANDATORY: virtual destructor
      virtual ~VertexDistanceDV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexDistanceDV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      // the actual evaluator 
      LoKi::Particles::VertexDistance m_fun ; ///< the actual evaluator
      // ======================================================================
    } ;
    // ========================================================================
    /** @class VertexSignedDistanceDV 
     *  The special version of LoKi::Particles::VertexSignedDistance functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *
     *  @see LoKi::Cuts::BPVVDSIGN
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexSignedDistance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class VertexSignedDistanceDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the default constructor 
      VertexSignedDistanceDV  () ;
      /// MANDATORY: virtual destructor
      virtual ~VertexSignedDistanceDV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexSignedDistanceDV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::VertexSignedDistance m_fun ; // the actual evaluator
      // ======================================================================
    } ;
    // ========================================================================
    /** @class VertexDotDistanceDV 
     *  The special version of LoKi::Particles::VertexDotDistance functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *
     *  @see LoKi::Cuts::BPVVDDOT
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexDotDistance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class VertexDotDistanceDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the default constructor 
      VertexDotDistanceDV  () ;
      /// MANDATORY: virtual destructor
      virtual ~VertexDotDistanceDV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexDotDistanceDV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::VertexDotDistance m_fun ; // the actual evaluator
      // ======================================================================
    } ;
    // ========================================================================
    /** @class VertexChi2DistanceDV 
     *  The special version of LoKi::Particles::VertexChi2Distance functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *
     *  @see LoKi::Cuts::BPVVDCHI2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::VertexChi2Distance
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class VertexChi2DistanceDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the default constructor 
      VertexChi2DistanceDV  () ;
      /// MANDATORY: virtual destructor
      virtual ~VertexChi2DistanceDV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexChi2DistanceDV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::VertexChi2Distance m_fun ; // the actual evaluator
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LifeTimeDV
     *  The special version of LoKi::Particles::LifeTime functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from GaudiAlgorithm 
     *
     *  @see LoKi::Cuts::BPVLTIME
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTime
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the "default" constructor
      LifeTimeDV () ;
      /// the constructor form tool type/name 
      LifeTimeDV ( const std::string& fit ) ;
      /// MANDATORY: virtual destructor
      virtual ~LifeTimeDV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeDV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::LifeTime m_fun ; // the actual evaluator
      /// the tool type/name 
      std::string               m_fit ;  // the tool type/name 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LifeTimeChi2DV
     *  The special version of LoKi::Particles::LifeTimeChi2 functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from GaudiAlgorithm 
     *
     *  @see LoKi::Cuts::BPVLTCHI2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTimeChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeChi2DV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the "default" constructor
      LifeTimeChi2DV () ;
      /// the constructor form tool type/name 
      LifeTimeChi2DV ( const std::string& fit ) ;
      /// MANDATORY: virtual destructor
      virtual ~LifeTimeChi2DV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeChi2DV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::LifeTimeChi2 m_fun ;  // the actual evaluator
      /// the tool type/name 
      std::string                   m_fit ;  // the tool type/name 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LifeTimeSignedChi2DV
     *  The special version of LoKi::Particles::LifeTimeChi2 functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from GaudiAlgorithm 
     *
     *  @see LoKi::Cuts::BPVLTSIGNCHI2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTimeSignedChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeSignedChi2DV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the "default" constructor
      LifeTimeSignedChi2DV () ;
      /// the constructor form tool type/name 
      LifeTimeSignedChi2DV ( const std::string& fit ) ;
      /// MANDATORY: virtual destructor
      virtual ~LifeTimeSignedChi2DV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeSignedChi2DV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::LifeTimeSignedChi2 m_fun ; // the actual evaluator
      /// the tool type/name 
      std::string                         m_fit ; // the tool type/name 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class LifeTimeFitChi2DV
     *  The special version of LoKi::Particles::LifeTimeFitChi2 functor
     *  which gets "the best primary vertex" from IPhysDesktop
     *  and ILifetimeFitter from DVAlgorithm 
     *
     *  @see LoKi::Cuts::BPVLTFITCHI2
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Particles::LifeTimeSignedChi2
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class LifeTimeFitChi2DV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the "default" constructor
      LifeTimeFitChi2DV () ;
      /// the constructor form tool type/name 
      LifeTimeFitChi2DV ( const std::string& fit ) ;
      /// MANDATORY: virtual destructor
      virtual ~LifeTimeFitChi2DV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  LifeTimeFitChi2DV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator 
      LoKi::Particles::LifeTimeFitChi2 m_fun ; // the actual evaluator
      /// the tool type/name 
      std::string                      m_fit ; // the tool type/name 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class VertexZDistanceWithTheBestPV
     *  The functor which evaluates Delta(Z) for the end-vertex of the 
     *  particle and "the best primary vertex" from the Desktop
     *
     *  @see LoKi::Cuts::BPVVDZ 
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *
     *  The concept and the name come 
     *     from Sean BRISBANE s.brisbane1@physics.ox.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class VertexZDistanceWithTheBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the default constructor 
      VertexZDistanceWithTheBestPV  () ;
      /// MANDATORY: virtual destructor
      virtual ~VertexZDistanceWithTheBestPV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexZDistanceWithTheBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class VertexRhoDistanceWithTheBestPV
     *  The functor which evaluates Delta(Z) for the end-vertex of the 
     *  particle and "the best primary vertex" from the Desktop
     *
     *  @see LoKi::Cuts::BPVVDR
     *  @see LoKi::Cuts::BPVVDRHO
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *
     *  The concept and the name come 
     *     from Sean BRISBANE s.brisbane1@physics.ox.ac.uk
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-01-17
     */
    class VertexRhoDistanceWithTheBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
      , public virtual LoKi::AuxDesktopBase
    {
    public:
      // ======================================================================
      /// the default constructor 
      VertexRhoDistanceWithTheBestPV  () ;
      /// MANDATORY: virtual destructor
      virtual ~VertexRhoDistanceWithTheBestPV() {} 
      /// MANDATORY: clone method ("virtual constructor")
      virtual  VertexRhoDistanceWithTheBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexDistanceWithSource
     *  The simple functor which evaluates the minimal distance 
     *  between the vertex and vertices from the "source"
     *  @see LoKi::Vertices::MinVertexDistanceWithSource  
     *  @see LoKi::Vertices::MinVertexDistance
     *  @see LoKi::Cuts::MINVDSOURCE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexDistanceWithSource
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
      /// the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source _Source ;
      // ======================================================================
    public:
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the source 
      MinVertexDistanceWithSource ( const _Source& source ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexDistanceWithSource () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexDistanceWithSource* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const ; 
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexDistanceWithSource() ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// get the underlying functor 
      mutable LoKi::Vertices::MinVertexDistanceWithSource m_fun ; // the evaluator 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexDistanceDV
     *  The special functor
     *  which gets all the primary vertices from the Desktop
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::MINVDDV 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexDistanceDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// the default constructor 
      MinVertexDistanceDV () ;
      /// the constructor from the vertex filter
      MinVertexDistanceDV ( const LoKi::PhysTypes::VCuts& cut ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexDistanceDV () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexDistanceDV* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const ; 
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functor 
      LoKi::Vertices::MinVertexDistanceDV m_fun ; // the actual functor 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexDistanceTES
     *  The functor
     *  which gets all the primary vertices from Transient Event Store 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::MINVDTES
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexDistanceTES
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// the constructor from the TES location  
      MinVertexDistanceTES ( const std::string& path ) ;
      /// the constructor from the TES locations 
      MinVertexDistanceTES ( const std::vector<std::string>& path ) ;
      /// the constructor from the TES location  & selector 
      MinVertexDistanceTES 
      ( const std::string&            path , 
        const LoKi::PhysTypes::VCuts& cut  ) ;
      /// the constructor from the TES locations and the vertex selector 
      MinVertexDistanceTES
      ( const std::vector<std::string>& path ,
        const LoKi::PhysTypes::VCuts&   cut  ) ;
      /// the constructor from the TES location  & selector 
      MinVertexDistanceTES 
      ( const LoKi::PhysTypes::VCuts& cut  ,
        const std::string&            path ) ;
      /// the constructor from the TES locations and the vertex selector 
      MinVertexDistanceTES
      ( const LoKi::PhysTypes::VCuts&   cut  ,
        const std::vector<std::string>& path ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexDistanceTES () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexDistanceTES* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const ; 
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexDistanceTES() ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// The actual functor 
      LoKi::Vertices::MinVertexDistanceTES m_fun ; // The actual functor 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexChi2DistanceWithSource
     *  The simple functor which evaluates the minimal distance 
     *  between the vertex and vertices from the "source"
     *  @see LoKi::Vertices::MinVertexChi2DistanceWithSource  
     *  @see LoKi::Vertices::MinVertexChi2Distance
     *  @see LoKi::Cuts::MINVDCHI2SOURCE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexChi2DistanceWithSource
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
      // ======================================================================
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source _Source ;
      // ======================================================================
    public:
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the source 
      MinVertexChi2DistanceWithSource ( const _Source& source ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexChi2DistanceWithSource () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexChi2DistanceWithSource* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const ; 
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexChi2DistanceWithSource() ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// get the underlying functor 
      mutable LoKi::Vertices::MinVertexChi2DistanceWithSource m_fun ; // the evaluator 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexChi2DistanceDV
     *  The special functor
     *  which gets all the primary vertices from the Desktop
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::MINVDCHI2DV 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexChi2DistanceDV 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// the default constructor 
      MinVertexChi2DistanceDV () ;
      /// the constructor from the vertex filter
      MinVertexChi2DistanceDV ( const LoKi::PhysTypes::VCuts& cut ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexChi2DistanceDV () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexChi2DistanceDV* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const ; 
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functor 
      LoKi::Vertices::MinVertexChi2DistanceDV m_fun ; // the actual functor 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexChi2DistanceTES
     *  The functor
     *  which gets all the primary vertices from Transient Event Store 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::MINVDCHI2TES 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexChi2DistanceTES
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public:
      // ======================================================================
      /// the constructor from the TES location  
      MinVertexChi2DistanceTES ( const std::string& path ) ;
      /// the constructor from the TES locations 
      MinVertexChi2DistanceTES ( const std::vector<std::string>& path ) ;
      /// the constructor from the TES location  & selector 
      MinVertexChi2DistanceTES 
      ( const std::string&            path , 
        const LoKi::PhysTypes::VCuts& cut  ) ;
      /// the constructor from the TES locations and the vertex selector 
      MinVertexChi2DistanceTES
      ( const std::vector<std::string>& path ,
        const LoKi::PhysTypes::VCuts&   cut  ) ;
      /// the constructor from the TES location  & selector 
      MinVertexChi2DistanceTES 
      ( const LoKi::PhysTypes::VCuts& cut  ,
        const std::string&            path ) ;
      /// the constructor from the TES locations and the vertex selector 
      MinVertexChi2DistanceTES
      ( const LoKi::PhysTypes::VCuts&   cut  ,
        const std::vector<std::string>& path ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexChi2DistanceTES () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexChi2DistanceTES* clone () const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const ; 
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexChi2DistanceTES() ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// The actual functor 
      LoKi::Vertices::MinVertexChi2DistanceTES m_fun ; // The actual functor 
      // ======================================================================
    } ;
  } // end of namespace LoKi::Particles    
} // end of namespace LoKi   
// ============================================================================
// The end 
// ============================================================================
#endif // LOKI_PARTICLES20_H
// ============================================================================
