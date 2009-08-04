// $Id: IContextTool.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_ICONTEXTTOOL_H 
#define DAVINCIKERNEL_ICONTEXTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/RecVertex.h"

// DaVinci
class IVertexFit;
class IPhysDesktop;
class IGeomDispCalculator;
class IDistanceCalculator;

/** @class IContextTool IContextTool.h Kernel/IContextTool.h
 *  
 *  Tool that tells the application if one runs in On or Offline Mode
 *  Now uses context. Can only be used as private tool.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class GAUDI_API IContextTool : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IContextTool, 2, 0);
  
  /// Get primary vertices
  virtual const LHCb::RecVertex::Container* primaryVertices() const = 0 ;
  
  /// Get geom tool
  virtual const IGeomDispCalculator* geomTool() const = 0 ;

  /// Get distance tool
  virtual const IDistanceCalculator* distanceTool() const = 0 ;

  /// Get default vertex fitter
  virtual const IVertexFit* vertexFitter() const = 0 ;

  /// Get physDesktop if available
  virtual IPhysDesktop* desktop() const = 0 ;


};
#endif // DAVINCIKERNEL_ICONTEXTTOOL_H
