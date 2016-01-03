// Include files 

#include <iostream>

// local
#include "MuonDet/MuonODEBoard.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonODEBoard
//
// 2004-01-12 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonODEBoard::MuonODEBoard(  ) {
  for(int i=0;i<4;i++){
    m_quadrant[i]=0;    
  }  
}
//=============================================================================
// Destructor
//=============================================================================
MuonODEBoard::~MuonODEBoard() {}

//=============================================================================
StatusCode MuonODEBoard::update(long tslayx,long tslayy, long tsnumb, 
                    std::vector<long> gridx, std::vector<long> gridy,
                    std::vector<long> quadrant) {
  m_TSLayoutX=tslayx;
  m_TSLayoutY=tslayy;
  m_TSNumber=tsnumb;
  m_TSGridX=gridx;
  m_TSGridY=gridy;
  m_TSQuadrant=quadrant;
  
  return StatusCode::SUCCESS;
  

}

StatusCode MuonODEBoard::addTSName(std::string name)
{
  if((int)m_TSName.size()<= m_TSNumber){
    m_TSName.push_back(name);    
    return StatusCode::SUCCESS;    
  }else{
    return StatusCode::FAILURE;
    
  }  
}

void MuonODEBoard::initialize(long number,long region){
  m_region=region;
  m_ODENumber=number;
}


void MuonODEBoard::setQuadrants(){
  for(int i=0;i<m_TSNumber;i++){
    unsigned int a=m_TSQuadrant[i];
//std::cout<<" ODE set "<<a<<" "<<i<<std::endl;	
    m_quadrant[a]=1;    
  } 
}
                   
bool MuonODEBoard::isQuadrantContained(long quadrant){
  if(m_quadrant[quadrant]>0){
    return true;
  }else {
    return false;
  }  
}

bool MuonODEBoard::isTSContained(LHCb::MuonTileID TSTile){
  long quadrant=(long)TSTile.quarter();
  long gridx=(long)TSTile.nX();
  long gridy=(long)TSTile.nY();
  for (int i=0;i< m_TSNumber;i++){
//	std::cout<<" ts in ode "<<i<<" "<<
//	m_TSQuadrant[i]<< " "<<
//	m_TSGridX[i]<<" "<<
//	 m_TSGridY[i]<<std::endl;
    if(m_TSQuadrant[i]==quadrant){      
      if(m_TSGridX[i]==gridx){
        if(m_TSGridY[i]==gridy){
          return true;          
        }
      }
    }    
  }
  return false;
  
}
