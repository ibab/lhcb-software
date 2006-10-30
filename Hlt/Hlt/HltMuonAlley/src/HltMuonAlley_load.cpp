#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( HltMuonAlley ) {

  DECLARE_ALGORITHM( MatchPatVeloSpaceL0 );
  DECLARE_ALGORITHM( MatchPatVeloRL0 );
  DECLARE_ALGORITHM( MatchPatVeloRMuonHits );
  DECLARE_ALGORITHM( MatchPatVeloSpaceMuonHits );
  DECLARE_ALGORITHM(HltMuonPreTriggerSingle);
  DECLARE_ALGORITHM(HltMuonTriggerSingle);
  DECLARE_ALGORITHM(HltMuonPreTriggerDiMuons);
  DECLARE_ALGORITHM(HltMuonTriggerDiMuons);

}
