// $Id: ICloneVertexBase.h,v 1.1 2009-07-29 21:28:36 jpalac Exp $
#ifndef MICRODST_ICLONEVERTEXBASE_H 
#define MICRODST_ICLONEVERTEXBASE_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb 
{
  class VertexBase;
}

/** @class ICloneVertexBase ICloneVertexBase.h MicroDST/ICloneVertexBase.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-07-29
 */
class GAUDI_API ICloneVertexBase : virtual public MicroDST::ICloner<LHCb::VertexBase> {
public: 

  DeclareInterfaceID(ICloneVertexBase, 2, 0 );

};
#endif // MICRODST_ICLONEVERTEXBASE_H
