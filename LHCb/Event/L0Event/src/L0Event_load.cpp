// $Id: L0Event_load.cpp,v 1.1 2001-06-27 10:07:06 ocallot Exp $

/** 
   L0Event_load.cpp

    Package    : L0Event
  
    Description: Implementation of <Package>_load routine. This routine 
                 is needed for forcing the linker to load all the components
                 of the library.. 
                                                                        
*/
//
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"
#include "GaudiKernel/IToolFactory.h"
//
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/ObjectList.h"
// Level0PileUpVeto
#include "L0Event/Level0PileUpVeto.h"
_ImplementDataObjectFactory( Level0PileUpVeto ) ;  



#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();
#define DLL_DECL_ALGTOOL(x)    extern const IToolFactory& x##Factory; x##Factory.addRef();


void L0Event_load() {

  DLL_DECL_OBJECTFACTORY( Level0PileUpVeto );

};

extern "C" void L0Event_loadRef()     {  L0Event_load(); }









