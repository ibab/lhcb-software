#include "GaudiKernel/DeclareFactoryEntries.h"

// Implement the relations factories
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Relations/RelationMACROs.h"

IMPLEMENT_RelationW2D( Track, MCParticle, float );

DECLARE_FACTORY_ENTRIES(TrackAssociators) {

  DECLARE_RelationW2D(Track, MCParticle, float);

  DECLARE_ALGORITHM(TrackAssociator);

}
