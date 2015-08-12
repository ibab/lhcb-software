// $Id: ICombineTaggersTool.h,v 1.3 2009-07-29 09:39:39 jpalac Exp $
#ifndef ICOMBINETAGGERSTOOL_H 
#define ICOMBINETAGGERSTOOL_H 1

#include <string>
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/ITagger.h" 

#include "Event/FlavourTag.h"

static const InterfaceID IID_ICombineTaggersTool("ICombineTaggersTool", 1 , 0); 

/** @class ICombineTaggersTool ICombineTaggersTool.h 
 *  
 *  v1.2
 *  @author Marco Musy (Milano)
 *  @date   2004-12-14
 */
class ICombineTaggersTool : virtual public IAlgTool {
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ICombineTaggersTool; };
 
  virtual int combineTaggers( LHCb::FlavourTag&, std::vector<LHCb::Tagger*>& , int, bool, bool ) =0; // new interface


};
#endif // ICOMBINETAGGERSTOOL_H
