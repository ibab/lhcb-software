// $Id: ICloneVertex.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef ICLONEVERTEX_H 
#define ICLONEVERTEX_H 1
// from MicroDST
#include "MicroDST/ICloner.h"

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
class GAUDI_API ICloneVertex : virtual public MicroDST::ICloner<LHCb::Vertex> 
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneVertex, 2, 0 );

  /// Destructor
  virtual ~ICloneVertex() { }

};

#endif // ICLONEVERTEX_H
