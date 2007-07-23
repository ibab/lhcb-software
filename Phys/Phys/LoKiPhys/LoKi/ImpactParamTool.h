// $Id: ImpactParamTool.h,v 1.6 2007-07-23 17:35:41 ibelyaev Exp $
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
   *  collection of vertex-releated objects and fucntions 
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
      inline IGeomDispCalculator*  tool () const 
      { return m_tool.getObject() ; }
    protected:
      /// constructor 
      ImpactParamTool 
      ( IGeomDispCalculator*   tool  ) ;
      /// copy constructor 
      ImpactParamTool     
      ( const ImpactParamTool& tool  ) ;
      /// virtual destructor 
      virtual ~ImpactParamTool();
    private:
      /// default constructor is private 
      ImpactParamTool();
    private:
      // the local pointer to the tool 
      LoKi::Interface<IGeomDispCalculator> m_tool ;
    } ;
    // ========================================================================
  } // end of namespace LoKi::Vertices;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMPACTPARAMTOOL_H
// ============================================================================
