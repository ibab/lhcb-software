#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( HltMuonAlley ) {

//  DECLARE_ALGORITHM( HltMuonAlleyDecision );
  //DECLARE_ALGORITHM( HltPreMuonAlleyDecision );
  DECLARE_ALGORITHM( MatchPatVeloSpaceL0 );
  DECLARE_ALGORITHM( MatchPatVeloRL0 );
//  DECLARE_ALGORITHM( TrgL0MuonProvider );
  DECLARE_ALGORITHM( MatchPatVeloRMuonHits );
  DECLARE_ALGORITHM( MatchPatVeloSpaceMuonHits );
  DECLARE_ALGORITHM(HltMuonPreTriggerSingle);
  //DECLARE_ALGORITHM( HltMuonAlleyMonitor );
  //DECLARE_ALGORITHM( HltMuonPrint );

}
