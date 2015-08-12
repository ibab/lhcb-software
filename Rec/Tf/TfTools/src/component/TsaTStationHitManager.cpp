//-----------------------------------------------------------------------------
/** @file TsaTStationHitManager.cpp
 *
 *  Implemetnation file for class : Tf::Tsa::TStationHitManager
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

// local
#include "TsaKernel/TsaTStationHitManager.h"

//-----------------------------------------------------------------------------

using namespace Tf::Tsa;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TStationHitManager )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Tf::Tsa::TStationHitManager::TStationHitManager( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
  : Tf::TStationHitManager<Tf::Tsa::SeedingHit> ( type, name , parent )
{
  declareInterface<Tf::Tsa::TStationHitManager>(this);
}

//=============================================================================
// Destructor
//=============================================================================
TStationHitManager::~TStationHitManager() {}


//=============================================================================

// Not sure why this helps here, but it does suppress the warning!
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:279)  // BOOST_ASSERT controlling expression is constant
#endif
