// $Id: RichDigiQC_load.cpp,v 1.2 2003-09-20 15:45:18 jonrob Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichDigiQC ) {

  DECLARE_ALGORITHM( RichDigitQC );
  DECLARE_ALGORITHM( RichDigiDataObjVerifier );

}
