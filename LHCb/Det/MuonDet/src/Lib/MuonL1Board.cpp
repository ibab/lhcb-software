// $Id: MuonL1Board.cpp,v 1.1 2004-02-03 11:40:27 asatta Exp $
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

long MuonL1Board::getODEPosition(long number)
{
  for(int i=0;i<m_numberOfODE;i++){
//	std::cout<<"l1 search ODE "<<m_ODENumberList[i]<<std::endl;
    if(m_ODENumberList[i]==number)return i;
    
  } 
  return -1;
  
};

