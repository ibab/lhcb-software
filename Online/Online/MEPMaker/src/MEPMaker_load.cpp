// $Id: MEPMaker_load.cpp,v 1.1 2006-05-23 17:24:18 niko Exp $
//====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MEPMaker) 
{
  DECLARE_OBJECT( L1MEP );
  DECLARE_OBJECT( RawMEP );
  DECLARE_OBJECT( DAQdummy );
  DECLARE_OBJECT( RootTune );
} 
