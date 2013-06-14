// $Id: ICloneCaloCluster.h,v 1.1 2010-08-13 11:25:05 jpalac Exp $
#ifndef MICRODST_ICLONECALOCLUSTER_H
#define MICRODST_ICLONECALOCLUSTER_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb
{
  class CaloCluster;
}

/** @class ICloneCaloCluster MicroDST/ICloneCaloCluster.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */
class GAUDI_API ICloneCaloCluster : virtual public MicroDST::ICloner<LHCb::CaloCluster>
{

 public:

  /// Interface ID
  DeclareInterfaceID(ICloneCaloCluster, 1, 0 );

  /// Destructor
  virtual ~ICloneCaloCluster() { }

};

#endif // MICRODST_ICLONECALOCLUSTER_H
