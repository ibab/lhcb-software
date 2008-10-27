// $Id: IContextTool.h,v 1.3 2008-10-27 15:19:33 jpalac Exp $
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

static const InterfaceID IID_IContextTool ( "IContextTool", 1, 1 );

/** @class IContextTool IContextTool.h Kernel/IContextTool.h
 *  
 *  Tool that tells the application if one runs in On or Offline Mode
 *  Now uses context. Can only be used as private tool.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class IContextTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IContextTool; }

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
