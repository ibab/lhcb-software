
//-----------------------------------------------------------------------------
/** @file TsaOTStationHitManager.cpp
 *
 *  Implemetnation file for class : Tf::Tsa::OTStationHitManager
 *
 *  $Id: TsaOTStationHitManager.cpp,v 1.1 2007-08-16 12:55:55 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TsaOTStationHitManager.h"

//-----------------------------------------------------------------------------

using namespace Tf::Tsa;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( OTStationHitManager );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Tf::Tsa::OTStationHitManager::OTStationHitManager( const std::string& type,
                                                   const std::string& name,
                                                   const IInterface* parent )
  : Tf::OTStationHitManager<Tf::Tsa::SeedingHit> ( type, name , parent )
{
  declareInterface<Tf::Tsa::OTStationHitManager>(this);
}

//=============================================================================
// Destructor
//=============================================================================
OTStationHitManager::~OTStationHitManager() {}

//=============================================================================
// Method that controls what happens when an extended hit is made from an OTHit
//=============================================================================
Tf::Tsa::SeedingHit *
Tf::Tsa::OTStationHitManager::createHit( const Tf::OTHit & othit ) const
{
  Tf::Tsa::SeedingHit * hit 
    = Tf::OTStationHitManager<Tf::Tsa::SeedingHit>::createHit(othit);
  // Matt, do whaver you want to your hit here ...
  return hit;
}

//=============================================================================
