// $Id: ICloneMuonPID.h,v 1.1 2010-08-12 15:54:53 jpalac Exp $
#ifndef MICRODST_ICLONEMUONPID_H
#define MICRODST_ICLONEMUONPID_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb
{
  class MuonPID;
}

/** @class ICloneMuonPID MicroDST/ICloneMuonPID.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-12
 */
class GAUDI_API ICloneMuonPID : virtual public MicroDST::ICloner<LHCb::MuonPID>
{

 public:

  /// Interface ID
  DeclareInterfaceID(ICloneMuonPID, 1, 0 );

  /// Destructor
  virtual ~ICloneMuonPID() { }

};

#endif // MICRODST_ICLONEMUONPID_H
