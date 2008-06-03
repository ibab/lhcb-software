// $Id: ImpactParamTool.h,v 1.9 2008-06-03 15:47:08 cattanem Exp $
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
#include "Kernel/IDistanceCalculator.h"
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
  // ==========================================================================
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
     *  Essentially it is some kind of "smart" reference to 
     *  IDistanceCalculatro interface 
     *  @see IDistanceCalculator
     *  @author Vanya BELYAEV ibelyaev@physcis.syr.edu
     *  @date 2006-02-19
     */
    class ImpactParamTool : virtual public LoKi::AuxFunBase 
    {    
    public:
      // ======================================================================
      /// accessor to the tool itself 
      inline const IDistanceCalculator* tool () const { return getTool() ; }
      /// accessor to the tool itself 
      inline const LoKi::Interface<IDistanceCalculator>& getTool () const
      { return m_tool ; }
      // ======================================================================
    public:
      // ======================================================================
      /// constructor 
      ImpactParamTool 
      ( const IDistanceCalculator* tool  ) ;
      /// constructor 
      ImpactParamTool 
      ( const LoKi::Interface<IDistanceCalculator>& tool  ) ;
      /// copy constructor 
      ImpactParamTool     
      ( const ImpactParamTool&     tool  ) ;
      /// virtual destructor 
      virtual ~ImpactParamTool();
      // ======================================================================
    public:
      // ======================================================================
      /// set the tool
      void setTool ( const IDistanceCalculator* t ) const { m_tool = t ; }
      /// set the tool
      void setTool ( const LoKi::Interface<IDistanceCalculator>& t ) const 
      { m_tool = t ; }      
      // ======================================================================
    public:
      // ======================================================================
      /// cast 
      operator const LoKi::Interface<IDistanceCalculator>& () const 
      { return getTool() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      ImpactParamTool();
      // ======================================================================
    private:
      // ======================================================================
      // the local pointer to the tool 
      mutable LoKi::Interface<IDistanceCalculator> m_tool ;
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Vertices;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMPACTPARAMTOOL_H
// ============================================================================
