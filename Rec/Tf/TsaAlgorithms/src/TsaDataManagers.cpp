
#include "GaudiKernel/ToolFactory.h"

#include "TsaKernel/TsaSeedingHit.h"

#include "TfKernel/ITStationHitManager.h"
#include "TfKernel/OTStationHitManager.h"
#include "TfKernel/TTStationHitManager.h"

// CRJ : Need to try and find a way to do this automatically ?

// Instantiate an instance of the IT station hit manager for the Tsa extended hits
template class Tf::ITStationHitManager<Tf::Tsa::SeedingHit>;
typedef Tf::ITStationHitManager<Tf::Tsa::SeedingHit> TsaITStationHitManager;
DECLARE_TOOL_FACTORY( TsaITStationHitManager );

// Instantiate an instance of the OT station hit manager for the Tsa extended hits
template class Tf::OTStationHitManager<Tf::Tsa::SeedingHit>;
typedef Tf::OTStationHitManager<Tf::Tsa::SeedingHit> TsaOTStationHitManager;
DECLARE_TOOL_FACTORY( TsaOTStationHitManager );

// Instantiate an instance of the TT station hit manager for the Tsa extended hits
template class Tf::TTStationHitManager<Tf::Tsa::SeedingHit>;
typedef Tf::TTStationHitManager<Tf::Tsa::SeedingHit> TsaTTStationHitManager;
DECLARE_TOOL_FACTORY( TsaTTStationHitManager );
