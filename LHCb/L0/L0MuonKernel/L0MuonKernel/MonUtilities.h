// $Id: MonUtilities.h,v 1.2 2008-07-25 14:42:15 jucogan Exp $
#ifndef COMPONENT_MONUTILITIES_H 
#define COMPONENT_MONUTILITIES_H 1

#include <vector>

#include "Kernel/MuonTileID.h"

namespace L0Muon
{
  
  namespace MonUtilities
  {
    enum Channel_type {Pad, StripH, StripV , nb_channel_types};
    enum Stations {M1=0,M2,M3,M4,M5,NStations};
    enum Regions  {R1=0,R2,R3,R4,NRegions};
    enum Quarters {Q1=0,Q2,Q3,Q4,NQuarters};
    enum Errors {Overflow=0,Internal,Decoding,Decoding_data,NErrors};
    enum BoardTypes {Proc=0,Ctrl,NBoardTypes};
    
    void makePads(std::vector<LHCb::MuonTileID>& tiles, std::vector<LHCb::MuonTileID>& pads);
    void filterTilesWithStation(std::vector<LHCb::MuonTileID>& original,
                                std::vector<LHCb::MuonTileID>& filtered,
                                unsigned int station);
    void flipCoord(int &x, int&y, const int qua);
    std::string channelTypeName(Channel_type e);
    std::string quarterName(int qua);
    std::string boardName(int board);
    std::string regionName(int reg);
    std::string stationName(int sta);
    std::string errorName(int type);
    std::string timeSlot(int bx);
  
    std::string hname_info_error(std::string toolname="");
    std::string hname_info_bid(std::string toolname="")  ;
    std::string hname_info_bid2(std::string toolname="") ;
    std::string hname_info_evt(std::string toolname="") ;

    std::string hname_pads_map(int sta,std::string toolname="") ;
    std::string hname_pads_multiBx(int sta,std::string toolname="") ;
    std::string hname_pads_multi(int sta,std::string toolname="") ;
    std::string hname_pads_multiBx_region(int sta, int reg,std::string toolname="") ;

    std::string hname_error_ctrl1(int type, std::string toolname="") ;
    std::string hname_error_ctrl2(int type, int qua, std::string toolname="") ;
    std::string hname_error_proc(int type, int qua, int board, std::string toolname="") ;

    std::string hname_channels_hist(int qua,Channel_type type, int sta, int reg,std::string toolname="") ;
    std::string hname_channels_hist2D(int qua,Channel_type type, int sta, int reg,std::string toolname="") ;
    std::string hname_channels_histBX(int qua,Channel_type type, int sta, int reg,std::string toolname="") ;
    std::string hname_channels_histDT(int qua,Channel_type type, int sta, int reg,std::string toolname="") ;

    std::string hname_cand_pt(std::string toolname="") ;
    std::string hname_cand_encodedpt(std::string toolname="") ;
    std::string hname_cand_pos(int sta, std::string toolname="") ;
    std::string hname_cand_number(std::string toolname="") ;
    std::string hname_cand_sum(std::string toolname="") ;

  } // namespace MonUtilities
} // namespace L0Muon

  
L0Muon::MonUtilities::Channel_type operator++(L0Muon::MonUtilities::Channel_type &rs, int );

#endif // COMPONENT_MONUTILITIES_H
