// $Id: ImpactParamTool.h,v 1.7 2008-01-25 14:42:22 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IMPACTPARAMTOOL_H 
#define LOKI_IMPACTPARAMTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Interface.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IGeomDispCalculator.h"
// ============================================================================
/** @file
 *
 * Implementation file for class LoKi::Vertices::ImpParBase
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================
namespace LoKi
{
  /** @namespace LoKi::Vertices 
   *  collection of vertex-releated objects and functions 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-16 
   */
  namespace Vertices 
  {
    // ========================================================================
    /** @class ImpactParamTool 
     *  Helper class which allows safe manipulations with 
     *  IGeomDispCalculator tool 
     *  @see IGeomDicpCalculator
     *  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
     *  @dte 2006-02-19
     */
    class ImpactParamTool : virtual public LoKi::AuxFunBase 
    {    
    public:
      /// accessor to the tool itself 
      inline IGeomDispCalculator*  tool () const { return getTool() ; }
      /// accessor to the tool itself 
      inline const LoKi::Interface<IGeomDispCalculator>& getTool () const { return m_tool ; }
    public:
      /// constructor 
      ImpactParamTool 
      ( const IGeomDispCalculator* tool  ) ;
      /// copy constructor 
      ImpactParamTool     
      ( const ImpactParamTool&     tool  ) ;
      /// virtual destructor 
      virtual ~ImpactParamTool();
    public:
      // set the tool
      void setTool ( const IGeomDispCalculator* t ) const { m_tool = t ; }
      // set the tool
      void setTool ( const LoKi::Interface<IGeomDispCalculator>& t ) const 
      { m_tool = t ; }      
    public:
      /// cast 
      operator const LoKi::Interface<IGeomDispCalculator>& () const { return m_tool ; }
    private:
      /// default constructor is private 
      ImpactParamTool();
    private:
      // the local pointer to the tool 
      mutable LoKi::Interface<IGeomDispCalculator> m_tool ;
    } ;
    // ========================================================================
  } // end of namespace LoKi::Vertices;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMPACTPARAMTOOL_H
// ============================================================================
