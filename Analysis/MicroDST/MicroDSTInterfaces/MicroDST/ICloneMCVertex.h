// $Id: ICloneMCVertex.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
#ifndef MICRODST_ICLONEMCVERTEX_H 
#define MICRODST_ICLONEMCVERTEX_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class MCVertex;
}

static const InterfaceID IID_ICloneMCVertex ( "ICloneMCVertex", 1, 0 );

/** @class ICloneMCVertex ICloneMCVertex.h MicroDST/ICloneMCVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class ICloneMCVertex : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneMCVertex; }

  virtual LHCb::MCVertex* operator() (const LHCb::MCVertex* mcVertex) = 0;

};
#endif // MICRODST_ICLONEMCVERTEX_H
