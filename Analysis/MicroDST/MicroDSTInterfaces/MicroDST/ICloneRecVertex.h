// $Id: ICloneRecVertex.h,v 1.6 2010-08-11 12:45:22 jpalac Exp $
#ifndef MICRODST_ICLONERECVERTEX_H
#define MICRODST_ICLONERECVERTEX_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb
{
  class RecVertex;
}

/** @class ICloneRecVertex MicroDST/ICloneRecVertex.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class GAUDI_API ICloneRecVertex : virtual public MicroDST::ICloner<LHCb::RecVertex>
{

 public:

  /// Interface ID
  DeclareInterfaceID(ICloneRecVertex, 3, 0 );

  /// Destructor
  virtual ~ICloneRecVertex() { }

};

#endif // MICRODST_ICLONERECVERTEX_H
