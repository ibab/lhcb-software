// $Id: OTDAQ_load.cpp,v 1.2 2004-09-03 12:12:25 jnardull Exp $
// Include files 
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"
#include "GaudiKernel/IToolFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(OTDAQ) {
  DECLARE_ALGORITHM( OTTimeCreator );
  DECLARE_ALGORITHM( OTFillBufferFromOTTime );
}
