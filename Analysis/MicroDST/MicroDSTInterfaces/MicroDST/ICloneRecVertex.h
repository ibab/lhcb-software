// $Id: ICloneRecVertex.h,v 1.5 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONERECVERTEX_H 
#define MICRODST_ICLONERECVERTEX_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

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
class GAUDI_API ICloneRecVertex : virtual public MicroDST::ICloner<LHCb::RecVertex>,
                                  virtual public MicroDST::ICloner<LHCb::VertexBase> {
public: 

  DeclareInterfaceID(ICloneRecVertex, 2, 0 );

};
#endif // MICRODST_ICLONERECVERTEX_H
