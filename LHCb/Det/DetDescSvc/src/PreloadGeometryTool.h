// $Id: PreloadGeometryTool.h,v 1.1 2006-07-27 20:39:16 cattanem Exp $
#ifndef PRELOADGEOMETRYTOOL_H 
#define PRELOADGEOMETRYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IGenericTool.h"            // Interface


/** @class PreloadGeometryTool PreloadGeometryTool.h
 *  Tool to traverse the full geometry tree and load it
 *
 *  @author Marco Cattaneo
 *  @date   2006-07-26
 */
class PreloadGeometryTool : public GaudiTool, virtual public IGenericTool {
public: 
  /// Standard constructor
  PreloadGeometryTool( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~PreloadGeometryTool( ); ///< Destructor

  void execute(); ///< Load the geometry tree
protected:

private:

};
#endif // PRELOADGEOMETRYTOOL_H
