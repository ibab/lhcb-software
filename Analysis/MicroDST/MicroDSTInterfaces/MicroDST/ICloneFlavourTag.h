// $Id: ICloneFlavourTag.h,v 1.1 2008-08-08 11:42:21 jpalac Exp $
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

static const InterfaceID IID_ICloneFlavourTag ( "ICloneFlavourTag", 1, 0 );

/** @class ICloneFlavourTag ICloneFlavourTag.h MicroDST/ICloneFlavourTag.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-08
 */
class ICloneFlavourTag : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICloneFlavourTag; }

  virtual LHCb::FlavourTag* operator() (const LHCb::FlavourTag* tag) = 0;

};
#endif // MICRODST_ICLONEFLAVOURTAG_H
