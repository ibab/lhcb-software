// local
#include "Event/L0MuonError.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonError
//
// 2008-07-22 : Julien Cogan
//-----------------------------------------------------------------------------

void LHCb::L0MuonError::index( int & quarter, int & board, int & pu ) const{

  LHCb::MuonTileID mid=key();
  

  quarter = mid.quarter();
  

  MuonLayout lay=mid.layout();
  
  int xgrid=lay.xGrid();
  
  int ygrid=lay.yGrid();
  

  // CU & SU
  if (xgrid==0 && ygrid==0) {
    
    board=15;
    
    pu=-1;
    
    return;
    
  }
  

  // BCSU
  if (xgrid==1 && ygrid==1) {
    
    board=mid.region()*3+mid.nY()*2+mid.nX()-1;
    
    pu=5;

    return;
    
  }
  

  // PU
  if (xgrid==2 && ygrid==2) {
    
    LHCb::MuonTileID mid_board=MuonLayout(1,1).contains(mid);
    
    board=mid_board.region()*3+mid_board.nY()*2+mid_board.nX()-1;
    
    pu=(mid.nY()%2)*2+(mid.nX()%2);

    //if (pu==4) std::cout<<"%%%% pu=4 %%%% should be <4 "<<mid.toString()<<std::endl;
    
    return;
    
  }
  

  // Unknown
  quarter=-1;
  
  board=-1;
  
  pu=-1;
  

  return;
  


}
