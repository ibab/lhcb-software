// $Id: ICloneFlavourTag.h,v 1.3 2009-07-29 21:29:01 jpalac Exp $
#ifndef MICRODST_ICLONEFLAVOURTAG_H
#define MICRODST_ICLONEFLAVOURTAG_H 1

#include "MicroDST/ICloner.h"
namespace LHCb
{
  class FlavourTag;
}

/** @class ICloneFlavourTag MicroDST/ICloneFlavourTag.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-08
 */
class GAUDI_API ICloneFlavourTag : virtual public MicroDST::ICloner<LHCb::FlavourTag>
{

 public:

  /// Interface ID
  DeclareInterfaceID(ICloneFlavourTag, 2, 0);

  /// Destructor
  virtual ~ICloneFlavourTag() { }

};

#endif // MICRODST_ICLONEFLAVOURTAG_H
