// $Id: ICombineTaggersTool.h,v 1.2 2006-10-24 10:16:44 jpalac Exp $
#ifndef ICOMBINETAGGERSTOOL_H 
#define ICOMBINETAGGERSTOOL_H 1

#include <string>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/ITagger.h" 

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
 
  virtual int combineTaggers( LHCb::FlavourTag&, std::vector<LHCb::Tagger*>& ) =0;

};
#endif // ICOMBINETAGGERSTOOL_H
