// $Id: ICloneMCVertex.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONEMCVERTEX_H 
#define MICRODST_ICLONEMCVERTEX_H 1
// from MicroDST
#include "MicroDST/ICloner.h"

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
class GAUDI_API ICloneMCVertex : virtual public MicroDST::ICloner<LHCb::MCVertex> {
public:
  DeclareInterfaceID(ICloneMCVertex, 2, 0 );
};
#endif // MICRODST_ICLONEMCVERTEX_H
