
//-----------------------------------------------------------------------------
/** @file TsaTStationHitManager.cpp
 *
 *  Implemetnation file for class : Tf::Tsa::TStationHitManager
 *
 *  $Id: TsaTStationHitManager.cpp,v 1.1 2007-10-23 17:21:38 smenzeme Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TsaKernel/TsaTStationHitManager.h"

//-----------------------------------------------------------------------------

using namespace Tf::Tsa;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TStationHitManager );

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
// Method that controls what happens when an extended hit is made from an OTHit
//=============================================================================
Tf::Tsa::SeedingHit *
Tf::Tsa::TStationHitManager::createHit( const Tf::OTHit & othit ) const
{
  Tf::Tsa::SeedingHit * hit
    = Tf::TStationHitManager<Tf::Tsa::SeedingHit>::createHit(othit);
  // Matt, do whaver you want to your hit here ...
  return hit;
}

//=============================================================================
