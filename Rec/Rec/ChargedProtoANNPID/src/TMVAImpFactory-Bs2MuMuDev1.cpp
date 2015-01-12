
// local
#include "TMVAImpFactory.h"

// So the TMVA networks pick up the std:: functions
using namespace std;

namespace ANNGlobalPID
{

  /** @namespace Bs2MuMuTuneBDTDev1 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of Bs2MuMuTuneBDTDev1 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2014-06-26
   */
  namespace Bs2MuMuTuneBDTDev1
  {
#include "tmva/Bs2MuMuTuneBDTDev1/GlobalPID_Muon_Long_TMVA.class.C"
#include "tmva/Bs2MuMuTuneBDTDev1/GlobalPID_Pion_Long_TMVA.class.C"
#include "tmva/Bs2MuMuTuneBDTDev1/GlobalPID_Kaon_Long_TMVA.class.C"
  }

  /** @namespace Bs2MuMuTuneMLPDev1 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of Bs2MuMuTuneMLPDev1 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2014-06-26
   */
  namespace Bs2MuMuTuneMLPDev1
  {
#include "tmva/Bs2MuMuTuneMLPDev1/GlobalPID_Muon_Long_TMVA.class.C"
#include "tmva/Bs2MuMuTuneMLPDev1/GlobalPID_Pion_Long_TMVA.class.C"
#include "tmva/Bs2MuMuTuneMLPDev1/GlobalPID_Kaon_Long_TMVA.class.C"
  }

  void TMVAImpFactory::addBs2MuMuDev1( )
  {
    // BDT - IsMuon as Input
    {
      const std::string tune = "Bs2MuMuTuneBDTDev1";
      add<Bs2MuMuTuneBDTDev1::ReadMuon_Long_TMVA> ( tune, "muon", "Long" );
      add<Bs2MuMuTuneBDTDev1::ReadPion_Long_TMVA> ( tune, "pion", "Long" );
      add<Bs2MuMuTuneBDTDev1::ReadKaon_Long_TMVA> ( tune, "kaon", "Long" );
    }
    // MLP-CE - IsMuon as Input
    {
      const std::string tune = "Bs2MuMuTuneMLPDev1";
      add<Bs2MuMuTuneMLPDev1::ReadMuon_Long_TMVA> ( tune, "muon", "Long" );
      add<Bs2MuMuTuneMLPDev1::ReadPion_Long_TMVA> ( tune, "pion", "Long" );
      add<Bs2MuMuTuneMLPDev1::ReadKaon_Long_TMVA> ( tune, "kaon", "Long" );
    }
  }

}
