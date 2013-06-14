// $Id: $
#ifndef MICRODST_ICLONEWEIGHTSVECTOR_H 
#define MICRODST_ICLONEWEIGHTSVECTOR_H 1
// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb 
{
  class WeightsVector;
}

/** @class ICloneWeightsVector MicroDST/ICloneWeightsVector.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2010-10-03
 */
class GAUDI_API ICloneWeightsVector : virtual public MicroDST::ICloner<LHCb::WeightsVector>
{

 public: 

  /// Interface ID
  DeclareInterfaceID(ICloneWeightsVector, 1, 0 );

  /// Destructor
  virtual ~ICloneWeightsVector() { }

};

#endif // MICRODST_ICLONEWEIGHTSVECTOR_H
