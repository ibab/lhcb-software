
#include "TsaKernel/TsaSeedingHit.h"

#include "TfKernel/TTStationHitManager.h"

// CRJ : Need to try and find a way to do this automatically ?

// Instantiate an instance of the TT station hit manager for the Tsa extended hits
template class Tf::TTStationHitManager<Tf::Tsa::SeedingHit>;
typedef Tf::TTStationHitManager<Tf::Tsa::SeedingHit> TsaTTStationHitManager;
DECLARE_TOOL_FACTORY( TsaTTStationHitManager )
