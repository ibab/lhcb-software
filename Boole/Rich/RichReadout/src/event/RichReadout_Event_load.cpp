
//-----------------------------------------------------------------------------
/** @file RichReadout_Event_load.cpp
 *
 *  Implementation file for classes in RichEvent
 *
 *  CVS Log :-
 *  $Id: RichReadout_Event_load.cpp,v 1.1 2005-06-18 11:36:05 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    08/05/2002 
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ContainerFactoryDefs.h"

#include "Event/MCRichDeposit.h"
_ImplementContainedObjectFactory(MCRichDeposit)
_ImplementDataObjectFactory(MCRichDeposits)

#include "Event/MCRichSummedDeposit.h"
_ImplementContainedObjectFactory(MCRichSummedDeposit)
_ImplementDataObjectFactory(MCRichSummedDeposits)

/// =======================================================

void RichReadout_Event_load() 
{

  DLL_DECL_OBJECTFACTORY( MCRichDeposit );
  DLL_DECL_OBJECTFACTORY( MCRichDeposits );   

  DLL_DECL_OBJECTFACTORY( MCRichSummedDeposit );
  DLL_DECL_OBJECTFACTORY( MCRichSummedDeposits );  
  
}

extern "C" void RichReadout_Event_loadRef() 
{
  RichReadout_Event_load();
}
