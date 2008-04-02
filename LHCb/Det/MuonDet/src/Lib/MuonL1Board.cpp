// $Id: MuonL1Board.cpp,v 1.2 2008-04-02 11:47:48 asatta Exp $
// Include files 

#include <iostream>

// local
#include "MuonDet/MuonL1Board.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonL1Board
//
// 2004-01-05 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonL1Board::MuonL1Board(  ) {

}
//=============================================================================
// Destructor
//=============================================================================
MuonL1Board::~MuonL1Board() {}; 

//=============================================================================



void MuonL1Board::addODE(long number, std::string name) 
{
  m_ODENumberList.push_back(number);
  m_ODEName.push_back(name); 
}; 


void MuonL1Board::addLayout(long region,long X,long Y)
{
  m_regionLayout[region][0]=X;
  m_regionLayout[region][1]=Y;  
};

long MuonL1Board::getODEPosition(long number,bool hole)
{
  long no_hole=0;
  for(int i=0;i<24;i++){
//    std::cout<<"l1 search ODE "<<number<<" "<<m_linkConnection[i]<<" 
//"<<no_hole<<std::endl;
    if(m_linkConnection[i]==number){
      if(hole){return i;
      }else{
        return no_hole;
      }
    }
    if(m_linkConnection[i]>0)no_hole++;
      
        
  } 
std::cout<<" not found "<<number<<std::endl;
 for(int i=0;i<24;i++){
 std::cout<<"l1 search ODE "<<number<<
" "<<m_linkConnection[i]<<std::endl;
}
  return -1;
  
};

