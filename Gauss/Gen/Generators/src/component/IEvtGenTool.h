#ifndef GENERATORS_IEVTGENTOOL_H
#define GENERATORS_IEVTGENTOOL_H

#include <string>
//from Gaudi
#include "GaudiKernel/IAlgTool.h"
// from EvtGen
#include "EvtGen/EvtGen.hh"

/** @class IEvtGenTool IEvtGenTool.h "Generators/IEvtGenTool.h"
  *
  * Interface for containter tool for EvtGen
  *
  * @author Alex Shires
  * @date 2011-10-28
*/

static const InterfaceID IID_IEvtGenTool( "IEvtGenTool", 1, 0 ) ;

class IEvtGenTool : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID() { return IID_IEvtGenTool ; }

    /* return the pointer to EvtGen
    */
    virtual EvtGen * getEvtGen() = 0 ; 

    /* set the EVtGen pointer
    */
    virtual void setEvtGen( EvtGen * ) = 0 ; 

    /* return the initialisation status of
     * EvtGen
    */
    virtual bool isInit() = 0 ;

} ; 

#endif // GENERATORS_IEVTGENTOOL_H
