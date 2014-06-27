// Include files 

// local
#include "TMVAImpFactory.h"

// So the TMVA networks pick up the std:: functions
using namespace std;

namespace ANNGlobalPID
{

  /** @namespace MC12TuneV2 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of MC12TuneV2 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2013-04-26
   */
  namespace MC12TuneV2
  {
    // Long tracks
#include "tmva/MC12TuneV2/GlobalPID_Electron_Long_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Muon_Long_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Pion_Long_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Kaon_Long_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Proton_Long_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Ghost_Long_TMVA.class.C"
    // Downstream
#include "tmva/MC12TuneV2/GlobalPID_Electron_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Muon_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Pion_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Kaon_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Proton_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Ghost_Downstream_TMVA.class.C"
    // Upstream
#include "tmva/MC12TuneV2/GlobalPID_Electron_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Muon_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Pion_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Kaon_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Proton_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV2/GlobalPID_Ghost_Upstream_TMVA.class.C"
  }

  /** @namespace MC12TuneV3 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of MC12TuneV3 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2014-06026
   */
  namespace MC12TuneV3
  {
    // Long tracks
#include "tmva/MC12TuneV3/GlobalPID_Electron_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Muon_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Pion_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Kaon_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Proton_Long_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Ghost_Long_TMVA.class.C"
    // Downstream
#include "tmva/MC12TuneV3/GlobalPID_Electron_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Muon_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Pion_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Kaon_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Proton_Downstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Ghost_Downstream_TMVA.class.C"
    // Upstream
#include "tmva/MC12TuneV3/GlobalPID_Electron_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Muon_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Pion_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Kaon_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Proton_Upstream_TMVA.class.C"
#include "tmva/MC12TuneV3/GlobalPID_Ghost_Upstream_TMVA.class.C"
  }

  // Standard constructor
  TMVAImpFactory::TMVAImpFactory()
  {
    // Add the MC12TuneV2 implementations to the factory
    {
      // Long
      add<MC12TuneV2::ReadElectron_Long_TMVA>       ( "MC12TuneV2", "electron", "Long" );
      add<MC12TuneV2::ReadMuon_Long_TMVA>           ( "MC12TuneV2", "muon",     "Long" );
      add<MC12TuneV2::ReadPion_Long_TMVA>           ( "MC12TuneV2", "pion",     "Long" );
      add<MC12TuneV2::ReadKaon_Long_TMVA>           ( "MC12TuneV2", "kaon",     "Long" );
      add<MC12TuneV2::ReadProton_Long_TMVA>         ( "MC12TuneV2", "proton",   "Long" );
      add<MC12TuneV2::ReadGhost_Long_TMVA>          ( "MC12TuneV2", "ghost",    "Long" );
      // Downstream
      add<MC12TuneV2::ReadElectron_Downstream_TMVA> ( "MC12TuneV2", "electron", "Downstream" );
      add<MC12TuneV2::ReadMuon_Downstream_TMVA>     ( "MC12TuneV2", "muon",     "Downstream" );
      add<MC12TuneV2::ReadPion_Downstream_TMVA>     ( "MC12TuneV2", "pion",     "Downstream" );
      add<MC12TuneV2::ReadKaon_Downstream_TMVA>     ( "MC12TuneV2", "kaon",     "Downstream" );
      add<MC12TuneV2::ReadProton_Downstream_TMVA>   ( "MC12TuneV2", "proton",   "Downstream" );
      add<MC12TuneV2::ReadGhost_Downstream_TMVA>    ( "MC12TuneV2", "ghost",    "Downstream" );
      // Upstream
      add<MC12TuneV2::ReadElectron_Upstream_TMVA>   ( "MC12TuneV2", "electron", "Upstream" );
      add<MC12TuneV2::ReadMuon_Upstream_TMVA>       ( "MC12TuneV2", "muon",     "Upstream" );
      add<MC12TuneV2::ReadPion_Upstream_TMVA>       ( "MC12TuneV2", "pion",     "Upstream" );
      add<MC12TuneV2::ReadKaon_Upstream_TMVA>       ( "MC12TuneV2", "kaon",     "Upstream" );
      add<MC12TuneV2::ReadProton_Upstream_TMVA>     ( "MC12TuneV2", "proton",   "Upstream" );
      add<MC12TuneV2::ReadGhost_Upstream_TMVA>      ( "MC12TuneV2", "ghost",    "Upstream" );
    }
    // Add the MC12TuneV3 implementations to the factory
    {
      // Long
      add<MC12TuneV3::ReadElectron_Long_TMVA>       ( "MC12TuneV3", "electron", "Long" );
      add<MC12TuneV3::ReadMuon_Long_TMVA>           ( "MC12TuneV3", "muon",     "Long" );
      add<MC12TuneV3::ReadPion_Long_TMVA>           ( "MC12TuneV3", "pion",     "Long" );
      add<MC12TuneV3::ReadKaon_Long_TMVA>           ( "MC12TuneV3", "kaon",     "Long" );
      add<MC12TuneV3::ReadProton_Long_TMVA>         ( "MC12TuneV3", "proton",   "Long" );
      add<MC12TuneV3::ReadGhost_Long_TMVA>          ( "MC12TuneV3", "ghost",    "Long" );
      // Downstream
      add<MC12TuneV3::ReadElectron_Downstream_TMVA> ( "MC12TuneV3", "electron", "Downstream" );
      add<MC12TuneV3::ReadMuon_Downstream_TMVA>     ( "MC12TuneV3", "muon",     "Downstream" );
      add<MC12TuneV3::ReadPion_Downstream_TMVA>     ( "MC12TuneV3", "pion",     "Downstream" );
      add<MC12TuneV3::ReadKaon_Downstream_TMVA>     ( "MC12TuneV3", "kaon",     "Downstream" );
      add<MC12TuneV3::ReadProton_Downstream_TMVA>   ( "MC12TuneV3", "proton",   "Downstream" );
      add<MC12TuneV3::ReadGhost_Downstream_TMVA>    ( "MC12TuneV3", "ghost",    "Downstream" );
      // Upstream
      add<MC12TuneV3::ReadElectron_Upstream_TMVA>   ( "MC12TuneV3", "electron", "Upstream" );
      add<MC12TuneV3::ReadMuon_Upstream_TMVA>       ( "MC12TuneV3", "muon",     "Upstream" );
      add<MC12TuneV3::ReadPion_Upstream_TMVA>       ( "MC12TuneV3", "pion",     "Upstream" );
      add<MC12TuneV3::ReadKaon_Upstream_TMVA>       ( "MC12TuneV3", "kaon",     "Upstream" );
      add<MC12TuneV3::ReadProton_Upstream_TMVA>     ( "MC12TuneV3", "proton",   "Upstream" );
      add<MC12TuneV3::ReadGhost_Upstream_TMVA>      ( "MC12TuneV3", "ghost",    "Upstream" );
    }
  }

  TMVAImpFactory::~TMVAImpFactory( )
  {
    for ( Map::iterator i = m_map.begin(); i != m_map.end(); ++i )
    { delete i->second; }
  }

  // Method to get a static instance of the factory
  const TMVAImpFactory& tmvaFactory()
  {
    static TMVAImpFactory factory;
    return factory;
  }

}

