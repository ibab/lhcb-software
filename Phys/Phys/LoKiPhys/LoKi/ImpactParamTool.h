// $Id: ImpactParamTool.h,v 1.1 2006-02-19 21:49:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_IMPACTPARAMTOOL_H 
#define LOKI_IMPACTPARAMTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Functions.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
class IGeomDispCalculator ;
// ============================================================================

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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */

namespace LoKi
{
  namespace Vertices 
  {
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
      inline IGeomDispCalculator*  tool () const { return m_tool ; }
    protected:
      /// constructor 
      ImpactParamTool 
      ( IGeomDispCalculator*   tool  ) ;
      /// copy constructor 
      ImpactParamTool     
      ( const ImpactParamTool& tool  ) ;
      /// virtual destructor 
      virtual ~ImpactParamTool();
      /// assignement 
      ImpactParamTool& 
      operator=( const ImpactParamTool& right ) ;
    private:
      /// default constructor is private 
      ImpactParamTool();
    private:
      // the local pointer to the tool 
      IGeomDispCalculator* m_tool;
    } ;

  }; // end of namespace LoKi::Vertices;
}; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMPACTPARAMTOOL_H
// ============================================================================
