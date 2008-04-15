// $Id: L0MuonMonUtils.h,v 1.2 2008-04-15 09:47:37 jucogan Exp $
#ifndef COMPONENT_L0MUONMONUTILS_H 
#define COMPONENT_L0MUONMONUTILS_H 1

#include <vector>

#include "Kernel/MuonTileID.h"

namespace L0MuonMonUtils
{
  enum Channel_type {Pad, StripH, StripV , nb_channel_types};
  enum Stations {M1=0,M2,M3,M4,M5,NStations};
  enum Regions  {R1=0,R2,R3,R4,NRegions};
  enum Quarters {Q1=0,Q2,Q3,Q4,NQuarters};

  void makePads(std::vector<LHCb::MuonTileID>& tiles, std::vector<LHCb::MuonTileID>& pads);
  void flipCoord(int &x, int&y, const int qua);
  std::string channelTypeName(Channel_type e);
  std::string quarterName(int qua);
  std::string regionName(int reg);
  std::string stationName(int sta);
  std::string timeSlot(int bx);
}

L0MuonMonUtils::Channel_type operator++( L0MuonMonUtils::Channel_type &rs, int );

#endif // COMPONENT_L0MUONMONUTILS_H
