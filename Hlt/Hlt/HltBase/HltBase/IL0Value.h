// $Id: IL0Value.h,v 1.1 2007-06-20 12:08:40 hernando Exp $
#ifndef HLTBASE_IL0VALUE_H 
#define HLTBASE_IL0VALUE_H 1

// Include files
// from STL
#include <string>
#include <map>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IL0Value ( "IL0Value", 1, 0 );

/** @class IL0Value IL0Value.h HltBase/IL0Value.h
 *  
 *  @author Jose Angel Hernando Morata
 *  @date   2006-07-20
 */
class IL0Value : virtual public IAlgTool {

public:

  // enum the value labels
  enum Type {EtTotal,SPDMult,PuHits,PuPeak2,EtHad,EtEle,EtGamma,EtPi0Local,EtPi0Global,PtMu,PtDiMu};
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0Value; }

  // clear of the containers
  virtual double value(int itype) = 0;

};
#endif // HLTBASE_IL0VALUE_H
