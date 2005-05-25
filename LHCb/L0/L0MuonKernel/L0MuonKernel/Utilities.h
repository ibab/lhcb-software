#ifndef L0MUONKERNEL_UTILITIES
#define L0MUONKERNEL_UTILITIES 1

#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/MuonLayout.h"
#include "L0mConf/L0MTile.h"
#include <vector>
#include <stdio.h>
#include <boost/dynamic_bitset.hpp>
#include "ProcessorKernel/TileRegister.h"

namespace L0Muon {
  /// Read the external file describing the register and return the L0MTileList
  /// type is 0 for L0Buffer and 1 for optical link
  std::vector<L0MTile> tileListFromMap(const MuonTileID & pu, int type, std::string path);

  /// Generic method to extract a tile list from the map
  std::vector<L0MTile> readTileListFromMap(FILE *file,int max);

  ///
  void L0Muon::splitTileListInfo(const std::vector<L0MTile> & l0mtilelist,
				 TileRegister * reg);

  ///
  void L0Muon::splitTileListInfo(const std::vector<L0MTile> & l0mtilelist,
				 std::vector<MuonTileID> & muontilelist,
				 boost::dynamic_bitset<> & tilestag,
				 boost::dynamic_bitset<> & stripstag);

};  // namespace L0Muon

#endif // L0MUONKERNEL_UTILITIES
