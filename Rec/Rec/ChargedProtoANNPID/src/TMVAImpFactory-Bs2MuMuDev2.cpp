
// local
#include "TMVAImpFactory.h"

// So the TMVA networks pick up the std:: functions
using namespace std;

namespace ANNGlobalPID
{

 /** @namespace Bs2MuMuTuneBDTDev2 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of Bs2MuMuTuneBDTDev2 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2014-06-26
   */
  namespace Bs2MuMuTuneBDTDev2
  {
#include "tmva/Bs2MuMuTuneBDTDev2/GlobalPID_Muon_Long_TMVA.class.C"
  }

 /** @namespace Bs2MuMuTuneMLPDev2 TMVAImpFactory.cpp
   *
   *  Namespace for C++ implementations of Bs2MuMuTuneMLPDev2 TMVA networks.
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   2014-06-26
   */
  namespace Bs2MuMuTuneMLPDev2
  {
#include "tmva/Bs2MuMuTuneMLPDev2/GlobalPID_Muon_Long_TMVA.class.C"
  }

  void TMVAImpFactory::addBs2MuMuDev2( )
  {
    // BDT - IsMuon as pre-sel
    {
      const std::string tune = "Bs2MuMuTuneBDTDev2";
      add<Bs2MuMuTuneBDTDev2::ReadMuon_Long_TMVA> ( tune, "muon", "Long" );
    }
    // MLP-CE - IsMuon as pre-sel
    {
      const std::string tune = "Bs2MuMuTuneMLPDev2";
      add<Bs2MuMuTuneMLPDev2::ReadMuon_Long_TMVA> ( tune, "muon", "Long" );
    } 
  }

}
