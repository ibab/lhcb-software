// $Id: ProcUtilities.h,v 1.2 2007-08-27 09:39:25 jucogan Exp $

#ifndef L0MUONKERNEL_PROCUTILITIES_H
#define L0MUONKERNEL_PROCUTILITIES_H     1

/** @class  ProcUtilities ProcUtilities.h L0MuonKernel/ProcUtilities.h

Functions with behaviour that may change from an emulator version to an other.

*/

#include <iostream>
#include "Kernel/MuonTileID.h"
#include "L0MuonKernel/MuonCandidate.h"

namespace L0Muon {
  
  // Used to search hits in M1 (Tower)
  int pendulumM1(int icol, int procVersion);
  
  // Used to search hits in M2 (Tower)
  int pendulumM2(int icol, int procVersion);
  
  // Used for ordering candidate (Tower). The first and the last are selected in CoreUnit.
  void candidatesOrdering(std::vector<L0Muon::PMuonCandidate> *cands, int procVersion);

  // Used when looking for a candididate (Tower) and at decoding stage 
  // when retrieving the pads of a candididate (MuonCandidate::pads)
  const int extrapolationM1(int i,int procVersion=-1);

  // Used when looking for a candididate (Tower) 
  int offsetM1(int offM1, int procVersion=-1);

  // Used for retrieving pads (L0MuonOutputs::l0muoncandidate)
  std::vector<LHCb::MuonTileID> add2pads(int quarter,int board, int pu, 
                                                 int colM3, int rowM3, int offM2, int offM1,
                                                 int procVersion, bool debug=false);
  int addM1Offset(int M1nX, int offM1, int procVersion=-1);

  // Used when computing PT (Tower and L0MuonOutputs::l0muoncandidate)
  void xyFromPad(LHCb::MuonTileID pad, double& x, double& y,int procVersion=-1);

  // Used to compute PT (Tower and L0MuonOutputs::l0muoncandidate)
  std::vector<double> kine(LHCb::MuonTileID p1, LHCb::MuonTileID p2,int procVersion=-1, bool debug=false);
  std::vector<double> kineV0(LHCb::MuonTileID p1, LHCb::MuonTileID p2, bool debug=false);

}; // namespace L0Muon

#endif      // L0MUONKERNEL_PROCUTILITIESH_H
