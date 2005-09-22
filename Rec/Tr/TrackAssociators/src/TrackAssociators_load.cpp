#include "GaudiKernel/DeclareFactoryEntries.h"

// Implement the relations factories
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Relations/RelationMACROs.h"

IMPLEMENT_RelationW2D( Track, MCParticle, float );

/** @file TrackAssociators_load.cpp 
 * 
 *  Mandatory file to IMPLEMENT and DECLARE factories 
 *  for relation tables and tools
 *
 *  @author Marco Cattaneo
 *  @date   25 February 2004
 */

DECLARE_FACTORY_ENTRIES(TrackAssociators) {

  DECLARE_RelationW2D( Track, MCParticle, float );

  DECLARE_ALGORITHM( TrackAssociator );
}
