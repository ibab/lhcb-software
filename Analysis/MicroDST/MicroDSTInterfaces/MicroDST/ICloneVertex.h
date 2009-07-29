// $Id: ICloneVertex.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
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

/** @class ICloneVertex ICloneVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class GAUDI_API ICloneVertex : virtual public IAlgTool {
public: 

  DeclareInterfaceID(ICloneVertex, 2, 0 );
  
  virtual LHCb::Vertex* operator() (const LHCb::Vertex* vertex) = 0;

};
#endif // ICLONEVERTEX_H
