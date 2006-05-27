// $Id: MEPMaker_load.cpp,v 1.2 2006-05-27 17:46:10 niko Exp $
//====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MEPMaker) 
{
//  DECLARE_OBJECT( L1MEP );
  DECLARE_OBJECT( MEPMaker );
  DECLARE_OBJECT( DAQdummy );
  DECLARE_OBJECT( RootTune );
} 
