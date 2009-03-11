// $Id: Particles25.h,v 1.1 2009-03-11 17:24:12 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES25_H 
#define LOKI_PARTICLES25_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/Particles16.h"
// ============================================================================
/** @file LoKi/Particles25.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  The file contains the functions, requested by Rob Lambert
 *  (see <a href="http://savannah.cern.ch/bugs/?47745">LoKi's Savannah portal #47745</a>)
 * 
 *  All these functors are to be used within CombineParticles framework only
 *  and surely completely useless for the regular Loki or Bender application
 *
 *  @attention There are no direct needs to use this "Context" 
 *             functor inside the native LoKi-based C++ code, 
 *             there are more efficient, transparent, 
 *             clear and safe analogues...
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-10 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class MaxCosineDirectionAngleWithSource 
     *  Simple functor which evaluates the maximum cosine of the angle between 
     *  the particle momentum and the flight direction from the primary to the 
     *  secondary vertex. The maximum is looked through all primary vertices.
     *  The primary vertices are extracted from "source"
     *  
     *  @see LoKi::Cuts::MAXDIRASOURCE 
     *  @see LoKi::Cuts::DIRA 
     *  @see LoKi::Cuts::BPVDIRA 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-11
     */
    class MaxCosineDirectionAngleWithSource 
      : public LoKi::Particles::CosineDirectionAngle 
    {
    public:
      // ======================================================================
      // the source of vertices 
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Source  Source ;
      // ======================================================================
    public:
      // ======================================================================
      /// the constructor with the source 
      MaxCosineDirectionAngleWithSource ( const Source& source ) ;
      /// copy constructor 
      MaxCosineDirectionAngleWithSource 
      ( const MaxCosineDirectionAngleWithSource& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxCosineDirectionAngleWithSource (){}
      /// MANDATORY: clone method ("virtual contructor")
      virtual  MaxCosineDirectionAngleWithSource* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // =====================================================================
    private:
      // =====================================================================
      /// the default constructor is disabled 
      MaxCosineDirectionAngleWithSource() ; // default constructor is disabled 
      // =====================================================================
    public:
      // =====================================================================
      /// access to the source 
      const Source& source() const { return m_source.func() ; }
      // ======================================================================
    protected:
      // ======================================================================
      const LHCb::VertexBase::ConstVector& vertices() const 
      { return m_vertices ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the source 
      LoKi::Assignable<Source>::Type        m_source   ; // the source 
      /// container of vertices 
      mutable LHCb::VertexBase::ConstVector m_vertices ; // container of vertices 
      // ======================================================================
    };
    // ========================================================================
    /** @class MaxCosineDirectionAngleDV 
     *  Simple functor which evaluates the maximum cosine of the angle between 
     *  the particle momentum and the flight direction from the primary to the 
     *  secondary vertex. The maximum is looked through all primary vertices.
     *  The primary vertices are extracted from DaVinci desktop
     *  
     *  @see LoKi::Cuts::MAXDIRADV
     *  @see LoKi::Cuts::MAXDIRASOURCE 
     *  @see LoKi::Cuts::DIRA 
     *  @see LoKi::Cuts::BPVDIRA 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-11
     */
    class MaxCosineDirectionAngleDV 
      : public LoKi::Particles::CosineDirectionAngle
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      // ======================================================================
      /// default constructor 
      MaxCosineDirectionAngleDV () ;
      /// constructor with the vertex selection 
      MaxCosineDirectionAngleDV ( const LoKi::Types::VCuts& vcut ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxCosineDirectionAngleDV() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxCosineDirectionAngleDV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the source 
      LoKi::Types::VCut m_vcut ; // the source 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MaxCosineDirectionAngleTES
     *  Simple functor which evaluates the maximum cosine of the angle between 
     *  the particle momentum and the flight direction from the primary to the 
     *  secondary vertex. The maximum is looked through all primary vertices.
     *  The primary vertices are extracted from TES
     *  
     *  @see LoKi::Cuts::MAXDIRATES
     *  @see LoKi::Cuts::MAXDIRADV
     *  @see LoKi::Cuts::MAXDIRASOURCE 
     *  @see LoKi::Cuts::DIRA 
     *  @see LoKi::Cuts::BPVDIRA 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-11
     */
    class MaxCosineDirectionAngleTES
      : public LoKi::Particles::MaxCosineDirectionAngleWithSource 
    {
    public:
      // ======================================================================
      /// constructor from the path 
      MaxCosineDirectionAngleTES ( const std::string& path ) ;
      /// constructor with the vertex selection 
      MaxCosineDirectionAngleTES ( const std::string&        path , 
                                   const LoKi::Types::VCuts& vcut ) ;
      /// constructor with the vertex selection 
      MaxCosineDirectionAngleTES ( const LoKi::Types::VCuts& vcut ,
                                   const std::string&        path ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MaxCosineDirectionAngleTES() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxCosineDirectionAngleTES* clone() const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the path 
      std::string      m_path ; // the path
      /// the source 
      LoKi::Types::VCut m_vcut ; // the source 
      // ======================================================================
    } ; 
    // ========================================================================
  } // end of namespace LoKi::Particles
  // ==========================================================================
} //end of namespace LoKi
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef MAXDIRASOURCE 
     *  Simple functor which evaluates the maximum cosine of the angle between 
     *  the particle momentum and the flight direction from the primary to the 
     *  secondary vertex. The maximum is looked through all primary vertices.
     *  The primary vertices are extracted from "source"
     *  
     *  @see LoKi::Particles::MaxCosineDirectionAngleWithSource
     *  @see LoKi::Cuts::MAXDIRADV 
     *  @see LoKi::Cuts::MAXDIRATES
     *  @see LoKi::Cuts::DIRA 
     *  @see LoKi::Cuts::BPVDIRA 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-11
     */
    typedef LoKi::Particles::MaxCosineDirectionAngleWithSource  MAXDIRASOURCE ;
    // ========================================================================
    /** @typedef MAXDIRADV 
     *  Simple functor which evaluates the maximum cosine of the angle between 
     *  the particle momentum and the flight direction from the primary to the 
     *  secondary vertex. The maximum is looked through all primary vertices.
     *  The primary vertices are extracted from DaVinci desktop
     *  
     *  @see LoKi::Particles::MaxCosineDirectionAngleDV 
     *  @see LoKi::Cuts::MAXDIRASOURCE 
     *  @see LoKi::Cuts::MAXDIRATES
     *  @see LoKi::Cuts::DIRA 
     *  @see LoKi::Cuts::BPVDIRA 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-11
     */
    typedef LoKi::Particles::MaxCosineDirectionAngleDV              MAXDIRADV ;
    // ========================================================================
    /** @typedef MAXDIRATES 
     *  Simple functor which evaluates the maximum cosine of the angle between 
     *  the particle momentum and the flight direction from the primary to the 
     *  secondary vertex. The maximum is looked through all primary vertices.
     *  The primary vertices are extracted from TES
     *
     *  @see LoKi::Particles::MaxCosineDirectionAngleTES
     *  @see LoKi::Cuts::MAXDIRADV
     *  @see LoKi::Cuts::MAXDIRASOURCE 
     *  @see LoKi::Cuts::DIRA 
     *  @see LoKi::Cuts::BPVDIRA 
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-03-11
     */
    typedef LoKi::Particles::MaxCosineDirectionAngleTES            MAXDIRATES ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi   
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES25_H
// ============================================================================
