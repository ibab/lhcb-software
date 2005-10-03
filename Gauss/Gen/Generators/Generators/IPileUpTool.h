// $Id: IPileUpTool.h,v 1.1 2005-10-03 09:50:06 robbep Exp $
#ifndef GENERATORS_IPILEUPTOOL_H 
#define GENERATORS_IPILEUPTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"


/** @class IPileUpTool IPileUpTool.h Generators/IPileUpTool.h
 *  
 *  Abstract interface to pile up tools
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-17
 */

static const InterfaceID IID_IPileUpTool( "IPileUpTool" , 1 , 0 ) ;

class IPileUpTool : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() { return IID_IPileUpTool ; }

  virtual unsigned int numberOfPileUp( double & currentLuminosity ) = 0 ;

  virtual void printPileUpCounters( ) = 0 ;
};
#endif // GENERATORS_IPILEUPTOOL_H
