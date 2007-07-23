// $Id: ImpParBase.h,v 1.5 2007-07-23 17:35:41 ibelyaev Exp $
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
  namespace Vertices 
  {
    // ========================================================================
    /** @class ImpParBase
     *  Helper class for evaluation 
     *  impact parameters, distances, and so on.
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class ImpParBase 
      : public LoKi::Vertices::ImpactParamTool 
      , public LoKi::Vertices::VertexHolder    
    {
    public:
      /// constructor 
      ImpParBase 
      ( const LHCb::VertexBase*vertex ,   
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
      ( const LHCb::VertexBase*vertex ,   
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
    // ========================================================================
  } // end of namespace LoKi::Vertices;
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IMPPARBASE_H
// ============================================================================
