// $Id: ICloneRecVertex.h,v 1.1.1.1 2007-12-10 08:23:45 jpalac Exp $
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
  class RecVertex;
}

static const InterfaceID IID_ICloneRecVertex ( "ICloneRecVertex", 1, 0 );

/** @class ICloneRecVertex ICloneRecVertex.h MicroDST/ICloneRecVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class ICloneRecVertex : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneRecVertex; }

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex) = 0;

protected:

private:

};
#endif // MICRODST_ICLONERECVERTEX_H
