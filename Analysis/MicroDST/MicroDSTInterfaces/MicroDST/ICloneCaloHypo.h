// $Id: ICloneCaloHypo.h,v 1.1 2010-08-12 15:54:53 jpalac Exp $
#ifndef MICRODST_ICLONECALOHYPO_H 
#define MICRODST_ICLONECALOHYPO_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb 
{
  class CaloHypo;
}

/** @class ICloneCaloHypo MicroDST/ICloneCaloHypo.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-12
 */
class GAUDI_API ICloneCaloHypo : virtual public MicroDST::ICloner<LHCb::CaloHypo>
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneCaloHypo, 1, 0 );

  /// Destructor
  virtual ~ICloneCaloHypo() { }

};

#endif // MICRODST_ICLONECALOHYPO_H
