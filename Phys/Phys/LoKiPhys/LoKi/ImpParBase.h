// $Id: ImpParBase.h,v 1.2 2006-11-27 12:01:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/02/19 21:49:11  ibelyaev
//  restructirisation + new funtions
//
// ============================================================================
#ifndef LOKI_IMPPARBASE_H 
#define LOKI_IMPPARBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/VertexHolder.h"
#include "LoKi/ImpactParamTool.h"
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
    /** @class ImpParBase
     *  Helper class for evaluation 
     *  impact parameters, distances, and so on.
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpParBase : 
      public LoKi::Vertices::ImpactParamTool , 
      public LoKi::Vertices::VertexHolder    
    {
    public:
      /// constructor 
      ImpParBase 
      ( const LHCb::Vertex*    vertex ,   
        IGeomDispCalculator*   tool   ) ;
      /// constructor 
      ImpParBase 
      ( const LoKi::Point3D&   point  ,   
        IGeomDispCalculator*   tool   ) ;
      /// constructor 
      ImpParBase 
      ( const VertexHolder&    holder ,   
        IGeomDispCalculator*   tool   ) ;
      /// constructor 
      ImpParBase 
      ( const LHCb::Vertex*    vertex ,   
        const ImpactParamTool& tool   ) ;
      /// constructor 
      ImpParBase 
      ( const LoKi::Point3D&   point  ,   
        const ImpactParamTool& tool   ) ;
      /// constructor 
      ImpParBase 
      ( const VertexHolder&    holder ,   
        const ImpactParamTool& tool   ) ;
      /// copy constructor 
      ImpParBase 
      ( const ImpParBase&      base   ) ;
      /// the destructor is virtual 
      virtual ~ImpParBase () ;
    private:
      /// default constructor is private 
      ImpParBase();
    };

  } // end of namespace LoKi::Vertices;
} // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMPPARBASE_H
// ============================================================================
