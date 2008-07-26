// $Id: IMagFieldTool.h,v 1.1 2008-07-26 18:04:53 cattanem Exp $
#ifndef IMAGFIELDTOOL_H 
#define IMAGFIELDTOOL_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMagFieldTool ( "IMagFieldTool", 1, 0 );

/** @class IMagFieldTool IMagFieldTool.h
 *  Tool to which MagneticFieldSvc delegates reading of field map
 *
 *  @author Marco Cattaneo
 *  @date   2008-07-26
 */
class IMagFieldTool : virtual public IAlgTool {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMagFieldTool; }

  /// Return the field vector fvec at the point xyz
  virtual void fieldVector( const Gaudi::XYZPoint&  xyz,
                                  Gaudi::XYZVector& fvec ) const = 0;

  /// Update the cached field from the files theFiles scaled by scaleFactor
  virtual StatusCode updateMap( const std::vector<std::string>& theFiles,
                                const double& scaleFactor ) = 0;
  
protected:

private:

};
#endif // IMAGFIELDTOOL_H
