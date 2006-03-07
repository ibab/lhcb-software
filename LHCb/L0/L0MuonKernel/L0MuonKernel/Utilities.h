#ifndef L0MUONKERNEL_UTILITIES
#define L0MUONKERNEL_UTILITIES 1

#include "Kernel/MuonTileID.h"
#include <vector>
#include <stdio.h>

namespace L0Muon {

  void xyFromPad(LHCb::MuonTileID pad, double& x, double& y,
                         std::vector<double> ptparam);
  
  
  std::vector<double> ptcalc(LHCb::MuonTileID p1, LHCb::MuonTileID p2,
                        std::vector<double> ptparam, bool debug=false);

  
};  // namespace L0Muon

#endif // L0MUONKERNEL_UTILITIES
