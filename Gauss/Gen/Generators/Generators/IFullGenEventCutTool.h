// $Id: IFullGenEventCutTool.h,v 1.1 2005-11-21 16:16:50 robbep Exp $
#ifndef GENERATORS_IFULLGENEVENTCUTTOOL_H 
#define GENERATORS_IFULLGENEVENTCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace HepMC {
  class GenEvent ;
}  
class HardInfo ;

/** @class IFullGenEventCutTool IFullGenEventCutTool.h 
 *   Generators/IFullGenEventCutTool.h
 *  
 *  Abstract interface to generator level cut on full event
 * 
 *  @author Patrick Robbe
 *  @date   2005-11-17
 */

static const InterfaceID IID_IFullGenEventCutTool( "IFullGenEventCutTool" , 
                                                   1 , 0 ) ;

class IFullGenEventCutTool : virtual public IAlgTool {
public:
  typedef std::vector< HepMCEvent * > EventVector ;
  typedef std::vector< HardInfo * >   HardVector  ;
  static const InterfaceID& interfaceID() { return IID_IFullGenEventCutTool ; }
  
  virtual bool studyFullEvent( EventVector & theEventVector ,
                               HardVector & theHardVector ) const = 0 ;
};
#endif // GENERATORS_ICUTTOOL_H
