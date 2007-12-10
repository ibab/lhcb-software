// $Id: ICloneVertex.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
#ifndef ICLONEVERTEX_H 
#define ICLONEVERTEX_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class Vertex;
}

static const InterfaceID IID_ICloneVertex ( "ICloneVertex", 1, 0 );

/** @class ICloneVertex ICloneVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class ICloneVertex : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneVertex; }

  virtual LHCb::Vertex* operator() (const LHCb::Vertex* vertex) = 0;

};
#endif // ICLONEVERTEX_H
