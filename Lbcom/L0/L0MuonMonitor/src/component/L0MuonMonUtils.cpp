// $Id: L0MuonMonUtils.cpp,v 1.2 2008-05-06 12:19:27 jucogan Exp $
// Include files 



// local
#include "L0MuonMonUtils.h"

void L0MuonMonUtils::makePads(std::vector<LHCb::MuonTileID>& strips,std::vector<LHCb::MuonTileID>& pads)
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

void L0MuonMonUtils::filterTilesWithStation(std::vector<LHCb::MuonTileID>& original,
                                            std::vector<LHCb::MuonTileID>& filtered, int station)
{
  filtered.clear();
  
  std::vector<LHCb::MuonTileID>::iterator it;
  for (it=original.begin();it<original.end();++it){
    if (it->station()==station) filtered.push_back(*it);
  }

}


void L0MuonMonUtils::flipCoord(int &x, int&y, const int qua)
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
  
  
std::string  L0MuonMonUtils::channelTypeName(Channel_type e)
{ 
  switch ( e ) {   
  case Pad    : return "Pad";
  case StripH : return "StripH";
  case StripV : return "StripV";
  default : return "Unknown";
  };
};
  
std::string  L0MuonMonUtils::quarterName(int qua)
{ 
  switch ( qua ) {   
  case 0    : return "Q1";
  case 1    : return "Q2";
  case 2    : return "Q3";
  case 3    : return "Q4";
  default : return "Unknown";
  };
};
  
std::string  L0MuonMonUtils::regionName(int reg)
{ 
  switch ( reg ) {   
  case 0    : return "R1";
  case 1    : return "R2";
  case 2    : return "R3";
  case 3    : return "R4";
  default : return "Unknown";
  };
};
  
std::string  L0MuonMonUtils::stationName(int sta)
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
std::string  L0MuonMonUtils::timeSlot(int bx)
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

L0MuonMonUtils::Channel_type operator++( L0MuonMonUtils::Channel_type &rs, int )
{
  return rs = (L0MuonMonUtils::Channel_type)(rs + 1);
};

