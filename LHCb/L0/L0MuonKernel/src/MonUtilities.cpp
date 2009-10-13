// $Id: MonUtilities.cpp,v 1.6 2009-10-13 08:10:57 jucogan Exp $
// Include files 

#include <sstream>

// local
#include "L0MuonKernel/MonUtilities.h"

void L0Muon::MonUtilities::makePads(std::vector<LHCb::MuonTileID>& strips,std::vector<LHCb::MuonTileID>& pads)
{

  pads.clear();

  std::vector<LHCb::MuonTileID>::iterator i1;
  std::vector<LHCb::MuonTileID>::iterator i2;
  for (i1=strips.begin(); i1<strips.end(); ++i1){
    if (i1->station()==0) {
      pads.push_back(*i1);// M1 contains pads already
    } else if ((i1->region()==0) & (i1->station()==3 || i1->station()==4)){
      pads.push_back(*i1);// M4 and M5 contains pads already in R1
    } else {
      for (i2=i1+1; i2<strips.end(); ++i2){
        LHCb::MuonTileID pad = (*i1).intercept(*i2);
        if (pad.isValid()){
          pads.push_back(pad);
        }
      }
    }
  }

}

void L0Muon::MonUtilities::filterTilesWithStation(std::vector<LHCb::MuonTileID>& original,
                                            std::vector<LHCb::MuonTileID>& filtered, unsigned int station)
{
  filtered.clear();
  
  std::vector<LHCb::MuonTileID>::iterator it;
  for (it=original.begin();it<original.end();++it){
    if (it->station()==station) filtered.push_back(*it);
  }

}


void L0Muon::MonUtilities::flipCoord(int &x, int&y, const int qua)
{
  switch ( qua ) {   
  case 0: 
    break;
  case 1: 
    y=-y;
    break;
  case 2: 
    x=-x;
    y=-y;
    break;
  case 3: 
    x=-x;
    break;
  };
}
  
void L0Muon::MonUtilities::offsetCoord(double &x, double&y, const int qua)
{
  switch ( qua ) {   
  case 0: 
    x+=0.5;
    y+=0.5;
    break;
  case 1: 
    x+=0.5;
    y-=0.5;
    break;
  case 2: 
    x-=0.5;
    y-=0.5;
    break;
  case 3: 
    x-=0.5;
    y+=0.5;
    break;
  };
}
  
std::string  L0Muon::MonUtilities::channelTypeName(Channel_type e)
{ 
  switch ( e ) {   
  case Pad    : return "Pad";
  case StripH : return "StripH";
  case StripV : return "StripV";
  default : return "Unknown";
  };
};
  
std::string  L0Muon::MonUtilities::quarterName(int qua)
{ 
  switch ( qua ) {   
  case 0    : return "Q1";
  case 1    : return "Q2";
  case 2    : return "Q3";
  case 3    : return "Q4";
  default : return "Unknown";
  };
};
std::string  L0Muon::MonUtilities::boardName(int board)
{
  std::stringstream name("");
  name<<"PB";
  name<<board;
  return name.str();
};
  
std::string  L0Muon::MonUtilities::puName(int pu)
{ 
  std::stringstream name("");
  name<<"PU";
  name<<pu;
  return name.str();
};
  
std::string  L0Muon::MonUtilities::regionName(int reg)
{ 
  switch ( reg ) {   
  case 0    : return "R1";
  case 1    : return "R2";
  case 2    : return "R3";
  case 3    : return "R4";
  default : return "Unknown";
  };
};
  
std::string  L0Muon::MonUtilities::stationName(int sta)
{ 
  switch ( sta ) {   
  case 0    : return "M1";
  case 1    : return "M2";
  case 2    : return "M3";
  case 3    : return "M4";
  case 4    : return "M5";
  default : return "Unknown";
  };
};

std::string  L0Muon::MonUtilities::errorName(int type)
{ 
  switch ( type ) {   
  case 0 : return "Overflow";
  case 1 : return "Internal";
  case 2 : return "Decoding";
  case 3 : return "Decoding_data";
  default : return "Unknown";
  };
};

std::string  L0Muon::MonUtilities::timeSlot(int bx)
{
  std::string ts;
  switch (bx) {
  case -7 : return "Prev7/";
  case -6 : return "Prev6/";
  case -5 : return "Prev5/";
  case -4 : return "Prev4/";
  case -3 : return "Prev3/";
  case -2 : return "Prev2/";
  case -1 : return "Prev1/";
  case  0 : return "";
  case  1 : return "Next1/";
  case  2 : return "Next2/";
  case  3 : return "Next3/";
  case  4 : return "Next4/";
  case  5 : return "Next5/";
  case  6 : return "Next6/";
  case  7 : return "Next7/";
  default : return "Unknown";
  };
};

L0Muon::MonUtilities::Channel_type operator++( L0Muon::MonUtilities::Channel_type &rs, int )
{
  return rs = (L0Muon::MonUtilities::Channel_type)(rs + 1);
};


std::string L0Muon::MonUtilities::hname_info_error(std::string toolname)  {return toolname+"Error_and_overflow";}

std::string L0Muon::MonUtilities::hname_info_bid(std::string toolname)  {return toolname+"L0_B_Id";}

std::string L0Muon::MonUtilities::hname_info_bid2(std::string toolname)  {return toolname+"L0_B_Id_coarse";}

std::string L0Muon::MonUtilities::hname_info_evt(std::string toolname)  {return toolname+"EventNumber";}

std::string L0Muon::MonUtilities::hname_pads_map(int sta,std::string toolname)  {
  return toolname+stationName(sta);
}

std::string L0Muon::MonUtilities::hname_pads_map_reg(int sta, int reg, std::string toolname)  {
  return toolname+stationName(sta)+regionName(reg);
}

std::string L0Muon::MonUtilities::hname_pads_multiBx(int sta,std::string toolname)  {
  return toolname+"multiplicity_per_time_slot_"+stationName(sta);
}

std::string L0Muon::MonUtilities::hname_pads_multi(int sta,std::string toolname)  {
  return toolname+"multiplicity_"+stationName(sta);
}

std::string L0Muon::MonUtilities::hname_pads_multiBx_region(int sta, int reg,std::string toolname)  {
  return toolname+"multiplicity_per_time_slot_"+stationName(sta)+"_"+regionName(reg);
}

std::string L0Muon::MonUtilities::hname_error_gen(int type, std::string toolname)  {
  return toolname+errorName(type);
}

std::string L0Muon::MonUtilities::hname_error_quarter(int type, int qua, std::string toolname)  {
  return toolname+errorName(type)+"_"+quarterName(qua);
}

std::string L0Muon::MonUtilities::hname_error_board(int type, int qua, int board, std::string toolname)  {
  return toolname+errorName(type)+"_"+quarterName(qua)+"_"+boardName(board);
}

std::string L0Muon::MonUtilities::hname_error_pu(int qua, int board, int pu, std::string toolname)  {
  return toolname+"LinkError_"+quarterName(qua)+"_"+boardName(board)+"_"+puName(pu) ;
}

std::string L0Muon::MonUtilities::hname_error_multi_opt(int qua,  std::string toolname)  {
  return toolname+"Nb_of_opt_link_error"+quarterName(qua) ;
}
std::string L0Muon::MonUtilities::hname_error_multi_ser(int qua,  std::string toolname)  {
  return toolname+"Nb_of_ser_link_error"+quarterName(qua) ;
}
std::string L0Muon::MonUtilities::hname_error_multi_par(int qua,  std::string toolname)  {
  return toolname+"Nb_of_par_link_error"+quarterName(qua) ;
}

std::string L0Muon::MonUtilities::hname_channels_hist(int qua,Channel_type type, int sta, int reg,std::string toolname)    {
  return toolname+"Hits_"+quarterName(qua)+"_"+stationName(sta)
    +"_"+regionName(reg)+"_"+channelTypeName(type);
}

std::string L0Muon::MonUtilities::hname_channels_hist2D(int qua,Channel_type type, int sta, int reg,std::string toolname)  {
  return hname_channels_hist(qua,type,sta,reg,toolname)+"2D";
}

std::string L0Muon::MonUtilities::hname_channels_histBX(int qua,Channel_type type, int sta, int reg,std::string toolname)  {
  return hname_channels_hist(qua,type,sta,reg,toolname)+"-VS-BX";
}

std::string L0Muon::MonUtilities::hname_channels_histDT(int qua,Channel_type type, int sta, int reg,std::string toolname)  {
  return hname_channels_hist(qua,type,sta,reg,toolname)+"-wrt-Muon";
}

std::string L0Muon::MonUtilities::hname_optlinks_evt(std::string toolname)    {
  return toolname+"evt_in_error";
}

std::string L0Muon::MonUtilities::hname_optlinks_multitot(std::string toolname)    {
  return toolname+"OLs_multiplicity";
}

std::string L0Muon::MonUtilities::hname_optlinks_multi(int qua,std::string toolname)    {
  return toolname+"OLs_multiplicity_"+quarterName(qua);
}

std::string L0Muon::MonUtilities::hname_optlinks_hist(int qua, int sta, int reg,std::string toolname)    {
  return toolname+"OLs_"+quarterName(qua)+"_"+stationName(sta)+"_"+regionName(reg);
}

std::string L0Muon::MonUtilities::hname_optlinks_hist2D(int qua, int sta, int reg,std::string toolname)  {
  return hname_optlinks_hist(qua,sta,reg,toolname)+"2D";
}

std::string L0Muon::MonUtilities::hname_optlinks_histBX(int qua, int sta, int reg,std::string toolname)  {
  return hname_optlinks_hist(qua,sta,reg,toolname)+"-VS-BX";
}

std::string L0Muon::MonUtilities::hname_optlinks_histDT(int qua, int sta, int reg,std::string toolname)  {
  return hname_optlinks_hist(qua,sta,reg,toolname)+"-wrt-Muon";
}

std::string L0Muon::MonUtilities::hname_cand_pt(std::string toolname)  {
  return toolname+"PT";
}

std::string L0Muon::MonUtilities::hname_cand_encodedpt(std::string toolname)  {
  return toolname+"encodedPT";
}

std::string L0Muon::MonUtilities::hname_cand_pos(int sta, std::string toolname)  {
  return toolname+"Pos_"+stationName(sta);
}

std::string L0Muon::MonUtilities::hname_cand_number(std::string toolname)  {
  return toolname+"number_of_candidates_per_ts";
}

std::string L0Muon::MonUtilities::hname_cand_sum(std::string toolname)  {
  return toolname+"number_of_candidates";
}

std::string L0Muon::MonUtilities::hname_cand_bid(std::string toolname)  {
  return toolname+"bid_with_l0muon_candidates";
}

std::string L0Muon::MonUtilities::hname_cand_evtbid(std::string toolname)  {
  return toolname+"bid_for_evt_with_l0muon_candidates";
}
