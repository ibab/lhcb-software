// $Id: RichRecQC_load.cpp,v 1.1.1.1 2003-06-30 16:51:04 jonesc Exp $
// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( RichRecQC ) {

  DECLARE_ALGORITHM( RichPIDQC );
  DECLARE_ALGORITHM( RichRecoQC );

}
