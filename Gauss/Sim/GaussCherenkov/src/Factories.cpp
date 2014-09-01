// Include files 

#include "GaudiKernel/DeclareFactoryEntries.h"


#include "GaussCherenkov/CkvSensDet.h"
#include "GaussCherenkov/CkvStdSensDet.h"
#include "GaussCherenkov/CkvGrandSensDet.h"


#include "GaussCherenkov/GetMCCkvHitsAlg.h"
#include "GaussCherenkov/GetMCCkvOpticalPhotonsAlg.h"
#include "GaussCherenkov/GetMCCkvSegmentsAlg.h"
#include "GaussCherenkov/GetMCCkvTracksAlg.h"


// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( CkvSensDet );
DECLARE_TOOL_FACTORY( CkvStdSensDet );
DECLARE_TOOL_FACTORY( CkvGrandSensDet );

DECLARE_ALGORITHM_FACTORY( GetMCCkvHitsAlg );
DECLARE_ALGORITHM_FACTORY( GetMCCkvOpticalPhotonsAlg );
DECLARE_ALGORITHM_FACTORY( GetMCCkvSegmentsAlg );
DECLARE_ALGORITHM_FACTORY( GetMCCkvTracksAlg );




