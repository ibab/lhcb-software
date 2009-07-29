// $Id: ICloneFlavourTag.h,v 1.2 2009-07-29 16:46:46 jpalac Exp $
#ifndef MICRODST_ICLONEFLAVOURTAG_H 
#define MICRODST_ICLONEFLAVOURTAG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb
{
  class FlavourTag;
}

/** @class ICloneFlavourTag ICloneFlavourTag.h MicroDST/ICloneFlavourTag.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-08
 */
class GAUDI_API ICloneFlavourTag : virtual public IAlgTool {
public:

  DeclareInterfaceID(ICloneFlavourTag, 2, 0);
  
  virtual LHCb::FlavourTag* operator() (const LHCb::FlavourTag* tag) = 0;

};
#endif // MICRODST_ICLONEFLAVOURTAG_H
