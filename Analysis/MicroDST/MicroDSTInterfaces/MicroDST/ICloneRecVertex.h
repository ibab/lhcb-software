// $Id: ICloneRecVertex.h,v 1.4 2009-07-29 16:46:46 jpalac Exp $
#ifndef MICRODST_ICLONERECVERTEX_H 
#define MICRODST_ICLONERECVERTEX_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb 
{
  class VertexBase;
  class RecVertex;
}

/** @class ICloneRecVertex ICloneRecVertex.h MicroDST/ICloneRecVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class GAUDI_API ICloneRecVertex : virtual public IAlgTool {
public: 

  DeclareInterfaceID(ICloneRecVertex, 2, 0 );
  
  virtual LHCb::VertexBase* operator() (const LHCb::VertexBase* vertex) = 0;

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex) = 0;

};
#endif // MICRODST_ICLONERECVERTEX_H
