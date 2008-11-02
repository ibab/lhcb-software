// $Id: Vertices5.h,v 1.3 2008-11-02 20:13:32 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTICES5_H 
#define LOKI_VERTICES5_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/Vertices1.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-03-28
 */
// ============================================================================
namespace LoKi 
{
  namespace Vertices 
  {
    // ========================================================================
    /** @class MinVertexDistanceWithSource
     *  The simple functor which evaluates the minimal distance 
     *  between the vertex and vertices from the "source"
     *  @see LoKi::Vertices::MinVertexDistance 
     *  @see LoKi::Cuts::VMINVDSOURCE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexDistanceWithSource
      : public LoKi::Vertices::MinVertexDistance 
    {
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source _Source ;
    public:
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
    public:
      /// constructor from the source 
      MinVertexDistanceWithSource ( const _Source& source ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexDistanceWithSource () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexDistanceWithSource* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const 
      { return minvdsource ( v ) ; }
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    public:
      // ======================================================================
      /// get the source 
      const Source& source() const { return m_source ; }
      // cast to the source 
      operator const Source&() const { return source() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructo is disabled 
      MinVertexDistanceWithSource() ; // the default constructo is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// make the actual evaluation
      result_type minvdsource ( argument v ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the source of the vertices 
      LoKi::Assignable<_Source>::Type           m_source ; // the source
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexDistanceDV
     *  The special version of LoKi::Vertices::MinVertexDistanceWithSource functor
     *  which gets all the primary vertices from the Desktop
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::VMINVDDV 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexDistanceDV 
      : public LoKi::Vertices::MinVertexDistanceWithSource 
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
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the access to the cut:
      const LoKi::PhysTypes::VCuts& cut() const { return m_cut ; }  
      // ======================================================================
    private:
      // ======================================================================
      /// The vertex selector 
      LoKi::PhysTypes::VCut m_cut ; // The vertex selector 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexDistanceTES
     *  The special version of LoKi::Vertices::MinVertexDistanceWithSource functor
     *  which gets all the primary vertices from Transient Event Store 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::VMINVDDV 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexDistanceTES
      : public LoKi::Vertices::MinVertexDistanceWithSource 
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
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexDistanceTES() ; // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the access to the cut:
      const LoKi::PhysTypes::VCuts&   cut  () const { return m_cut ; }
      /// get the access to the path
      const std::vector<std::string>& path () const { return m_path; }
      // ======================================================================
    private:
      // ======================================================================
      /// The vertex selector 
      LoKi::PhysTypes::VCut    m_cut  ; // The vertex selector 
      /// the TES locations 
      std::vector<std::string> m_path ; // the TES locations 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexChi2DistanceWithSource
     *  The simple functor which evaluates the minimal distance 
     *  between the vertex and vertices from the "source"
     *  @see LoKi::Vertices::MinVertexChi2Distance 
     *  @see LoKi::Cuts::VMINVDCHI2SOURCE
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexChi2DistanceWithSource
      : public LoKi::Vertices::MinVertexChi2Distance
    {
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source _Source ;
    public:
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
    public:
      /// constructor from the source 
      MinVertexChi2DistanceWithSource ( const _Source& source ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MinVertexChi2DistanceWithSource () {};
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MinVertexChi2DistanceWithSource* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument v ) const 
      { return minvdchi2source ( v ) ;}
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;      
      // ======================================================================
    public:
      // ======================================================================
      /// get the source 
      const Source& source() const { return m_source ; }
      // cast to the source 
      operator const Source&() const { return source() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexChi2DistanceWithSource() ; // the default constructo is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// make the actual evaluation
      result_type minvdchi2source ( argument v ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the source of the vertices 
      LoKi::Assignable<_Source>::Type           m_source ; // the source
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexChi2DistanceDV
     *  The special version of LoKi::Vertices::MinVertexChi2DistanceWithSource functor
     *  which gets all the primary vertices from the Desktop
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::VMINVDCHI2DV 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexChi2DistanceDV 
      : public LoKi::Vertices::MinVertexChi2DistanceWithSource 
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
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the access to the cut:
      const LoKi::PhysTypes::VCuts& cut() const { return m_cut ; }
      // ======================================================================
    private:
      // ======================================================================
      /// The vertex selector 
      LoKi::PhysTypes::VCut m_cut ; // The vertex selector 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MinVertexChi2DistanceTES
     *  The special version of LoKi::Vertices::MinVertexChi2DistanceWithSource functor
     *  which gets all the primary vertices from Transient Event Store 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @see LoKi::Cuts::VMINVDCHI2TES 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-28
     */
    class MinVertexChi2DistanceTES
      : public LoKi::Vertices::MinVertexChi2DistanceWithSource 
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
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MinVertexChi2DistanceTES() ; // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the access to the cut:
      const LoKi::PhysTypes::VCuts&   cut  () const { return m_cut ; }
      /// get the access to the path
      const std::vector<std::string>& path () const { return m_path; }
      // ======================================================================
    private:
      // ======================================================================
      /// The vertex selector 
      LoKi::PhysTypes::VCut    m_cut  ; // The vertex selector 
      /// the TES locations 
      std::vector<std::string> m_path ; // the TES locations 
      // ======================================================================
    } ;
  } // end of namespace LoKi::Vertices
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES5_H
// ============================================================================
