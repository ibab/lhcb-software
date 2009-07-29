// $Id: ICloneMCVertex.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
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

/** @class ICloneMCVertex ICloneMCVertex.h MicroDST/ICloneMCVertex.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
class GAUDI_API ICloneMCVertex : virtual public IAlgTool {
public: 

  DeclareInterfaceID(ICloneMCVertex, 2, 0 );

  virtual LHCb::MCVertex* operator() (const LHCb::MCVertex* mcVertex) = 0;

};
#endif // MICRODST_ICLONEMCVERTEX_H
