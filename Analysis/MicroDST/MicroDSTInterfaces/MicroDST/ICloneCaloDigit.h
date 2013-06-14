// $Id: ICloneCaloDigit.h,v 1.1 2010-08-13 11:25:05 jpalac Exp $
#ifndef MICRODST_ICLONECALODIGIT_H 
#define MICRODST_ICLONECALODIGIT_H 1

// from MicroDST
#include "MicroDST/ICloner.h"

// Forward declarations
namespace LHCb 
{
  class CaloDigit;
}

/** @class ICloneCaloDigit MicroDST/ICloneCaloDigit.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */
class GAUDI_API ICloneCaloDigit : virtual public MicroDST::ICloner<LHCb::CaloDigit>
{

public: 

  /// Interface ID
  DeclareInterfaceID(ICloneCaloDigit, 1, 0 );

  /// Destructor
  virtual ~ICloneCaloDigit() { }

};

#endif // MICRODST_ICLONECALODIGIT_H
