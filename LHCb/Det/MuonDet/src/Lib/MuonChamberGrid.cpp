// $Id: MuonChamberGrid.cpp,v 1.12 2007-11-28 08:00:23 cattanem Exp $
// Include files 

// local
#include "GaudiKernel/IToolSvc.h"  

#include "MuonDet/MuonChamberGrid.h"
#include "MuonDet/MuonFrontEndID.h"
#include <iostream>
#include <vector>
#include <math.h>

//-----------------------------------------------------------------------------
// Implementation file for class : MuonChamberGrid
//
// 2004-01-07 : Alessia(o) Sat(r)ta(i)
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonChamberGrid::MuonChamberGrid() {
  m_number_of_grid=0;
  m_x_pad_rdout1.clear();
  m_y_pad_rdout1.clear();
  m_x_pad_rdout2.clear();
  m_y_pad_rdout2.clear();
  m_readoutType.clear();
  m_mapType.clear();
}
//=============================================================================
// Destructor
//=============================================================================
MuonChamberGrid::~MuonChamberGrid() {}; 

//=============================================================================
StatusCode MuonChamberGrid::initialize(){

  char patt[400]; int Ngrid(0);
  sprintf(patt,"%s",(this->name()).c_str());
  sscanf(patt,"/G%d",&Ngrid);
  m_number_of_grid = Ngrid;
  
  //  std::cout<< "Gn: "<< m_number_of_grid<<" "
  //     <<(this->name()).data()<<std::endl;

  m_x_pad_rdout1     = param< std::vector<double> >("xrd1");
  m_y_pad_rdout1     = param< std::vector<double> >("yrd1");
  m_x_pad_rdout2     = param< std::vector<double> >("xrd2");
  m_y_pad_rdout2     = param< std::vector<double> >("yrd2");
  m_readoutType      = param< std::vector<int> >("grrd");
  m_mapType          = param< std::vector<int> >("mapT");


  return StatusCode::SUCCESS;
}

std::vector< std::pair< MuonFrontEndID,std::vector<float> > > 
MuonChamberGrid::listOfPhysChannels(double x_enter,double y_enter,
                                    double x_exit,double y_exit) {

  using GaudiUtils::operator<<; // for streaming std::vector

  std::vector< std::pair< MuonFrontEndID,std::vector<float> > > keepTemporary;
  std::pair< MuonFrontEndID,std::vector<float> > tmpPair;
  std::vector<double> x_rdout;  
  std::vector<double> y_rdout;
  std::vector<float> myBoundary; myBoundary.resize(4);

  bool debug = false;
  bool parallelFlag = false; 

  double slopeY(0),intercept(0);
  if(fabs(x_exit-x_enter)>0.001){
    slopeY =(y_exit - y_enter)/(x_exit - x_enter);
    intercept = y_exit - slopeY * x_exit ;
  } else {
    parallelFlag = true; 
  }

  //The choice on the readout numbers is taken
  //looking for dummy grids
  int TstRead =  m_x_pad_rdout2.size();
  int RdN = 1;
  if(TstRead > 1) RdN = 2;

  if(debug) std::cout<< "Returning List of Phys Channels for grid G"
		     << m_number_of_grid<<" . A Details. Vectors rd1x: "
		     << m_x_pad_rdout1<<" rd1y: "
		     << m_y_pad_rdout1<<" rd2x: "
		     << m_x_pad_rdout2<<" rd2y: "
		     << m_y_pad_rdout2<<std::endl;
  
  //Questa parte sui readout va vista bene
  for(int iRd=0;iRd<RdN;iRd++){

    int PhNx,PhNy;

    //Readout choice
    if(iRd) {
      x_rdout = m_x_pad_rdout2;
      y_rdout = m_y_pad_rdout2;
    } else {
      x_rdout = m_x_pad_rdout1;
      y_rdout = m_y_pad_rdout1;
    }

    //Physical channels settings related to the grid/readout
    PhNx = x_rdout.size();
    PhNy = y_rdout.size();

    unsigned int nxChaEntry, nxChaExit;
    int tmpNxChaEntry(0), tmpNxChaExit(20), inxLo, inyLo;
    
    if(debug) std::cout<< "Looping on readout n. "<<iRd<< " whith vct dim nX: "
                       << PhNx<<" nY: "
                       << PhNy<<". Entry/Exit of hit. x_e: "
                       << x_enter<<" y_e: "
                       << y_enter<<" x_x:  "
                       << x_exit<<" y_x: "
                       << y_exit<<" "<<std::endl;
    
    double tmpXLenght(0);
    for(inxLo = 0; inxLo < PhNx; inxLo++) {
      if(x_enter - tmpXLenght > 0.0001) {
        tmpXLenght += x_rdout[inxLo];
        tmpNxChaEntry =  inxLo;
        //	tmpNxChaEntry =  inxLo+1;
      } else {
        break;
      }
    }
    tmpXLenght = 0;
    for(inxLo = 0; inxLo < PhNx; inxLo++) {
      if(x_exit - tmpXLenght > 0.0001) {
        tmpXLenght += x_rdout[inxLo];
        tmpNxChaExit =  inxLo;
        //	tmpNxChaExit =  inxLo+1;
      } else {
        break;
      }
    }
    double xstart,xstop;
    
    if(debug) std::cout<< "************************************* "<<std::endl;
    
    if(debug) std::cout<< "Returning List of Phys Channels. TmpCha_entry: "
                       << tmpNxChaEntry<<" TmpCha_exit: "
                       << tmpNxChaExit<<std::endl;
    
    if(tmpNxChaEntry<=tmpNxChaExit){
      // normal order								
      if( tmpNxChaEntry<0)tmpNxChaEntry=0;
      if( tmpNxChaExit>=(int)PhNx) tmpNxChaExit=(int)PhNx-1 ;	
      
      xstart =	x_enter;      xstop  =	x_exit;			
      nxChaEntry = (unsigned int) tmpNxChaEntry;
      nxChaExit  = (unsigned int) tmpNxChaExit;										
                  
    } else {
      // inverse order					
      if( tmpNxChaExit<0)tmpNxChaExit=0;
      if( tmpNxChaEntry>=(int)PhNx) tmpNxChaEntry=(int)PhNx-1 ;	
      xstop  =	x_enter;      xstart =	x_exit;				
      nxChaEntry = (unsigned int) tmpNxChaExit;
      nxChaExit  = (unsigned int) tmpNxChaEntry;										
    }   

    unsigned int nyBegin ;    unsigned int nyEnd ;					
    double xBegin(0.),xEnd(0.);       double yBegin(0.), yEnd(0.); 
    
    if(debug) std::cout<< "Returning List of Phys Channels. Cha_entry: "
                       << nxChaEntry<<" Cha_exit: "
                       << nxChaExit<<std::endl;
    
    for (unsigned int Xloop=nxChaEntry;Xloop<=nxChaExit;Xloop++){
      if(Xloop==nxChaEntry){
        xBegin= xstart;
      } else {
        xBegin = retLenght(Xloop,x_rdout);
      }
      if(Xloop==nxChaExit){						 
        xEnd= xstop;
      }else{
        xEnd = retLenght(Xloop+1,x_rdout);
      }	
      
      if(debug) std::cout<< "Linear Parameters bef p flag. Xbeg "
                         <<xBegin<<"; xEnd: "
                         <<xEnd<<" Ybeg "
                         <<yBegin<<"; yEnd: "
                         <<yEnd<<" int: "
                         <<intercept<<" slope: "
                         <<slopeY<<std::endl;
      
      
      if(parallelFlag){	
        yBegin=y_enter;	yEnd= y_exit;
      } else {					
        yBegin=intercept+slopeY*xBegin ;
        yEnd=intercept+slopeY*xEnd ;
      }
      
      if(debug) std::cout<< "Computing Linear Parameters. Xbeg "
                         <<xBegin<<"; xEnd: "
                         <<xEnd<<" Ybeg "
                         <<yBegin<<"; yEnd: "
                         <<yEnd<<" int: "
                         <<intercept<<" slope: "
                         <<slopeY<<std::endl;
      
      double xinit,yinit,xend,yend;
      int tmpYBegin(0),tmpYEnd(0);
      double tmpYLenght(0);
      
      for(inyLo = 0; inyLo < PhNy; inyLo++) {
        if(yBegin - tmpYLenght > 0.0001) {
          tmpYLenght += y_rdout[inyLo];
          //	  tmpYBegin =  inyLo+1;
          tmpYBegin =  inyLo;
        } else {
          break;
        }
      }
      tmpYLenght = 0;
      for(inyLo = 0; inyLo < PhNy; inyLo++) {
        if(yEnd - tmpYLenght > 0.0001) {
          tmpYLenght += y_rdout[inyLo];
          //	  tmpYEnd =  inyLo+1;
          tmpYEnd =  inyLo;
        } else {
          break;
        }
      }
      
      if(debug) std::cout<< "Debugging tmpY_beg: "
                         <<tmpYBegin<<"; tmpY_end: "
                         <<tmpYEnd<<std::endl;
      
      if(tmpYBegin<=tmpYEnd){
        if( tmpYBegin<0)tmpYBegin=0;
        if( tmpYEnd>=(int)PhNy)tmpYEnd=(int)PhNy-1 ;	
        
        nyBegin=tmpYBegin; nyEnd=tmpYEnd;	
        
        xinit=xBegin;	yinit=yBegin;
        xend=xEnd;	yend=yEnd;
        
      } else {
        
        if( tmpYEnd<0)tmpYEnd=0;
        if( tmpYBegin>=(int)PhNy)tmpYBegin=(int)PhNy-1 ;
        
        nyBegin=tmpYEnd; nyEnd=tmpYBegin;
        
        xinit=xEnd;	yinit=yEnd;
        xend=xBegin;	yend=yBegin;
        
      }
      
      if(debug) std::cout<< "Debugging Y_beg: "
                         <<nyBegin<<"; Y_end: "
                         <<nyEnd<<std::endl;
      
      for (unsigned int Yloop=nyBegin;Yloop<=nyEnd;Yloop++){
        // Compute distance from the boundaries
        // of the physical channel
        double myX(0),myY(0);
        if(nyBegin==nyEnd){
          myX = (xinit+xend)/2;
          myY = (yinit+yend)/2;
        } else if(Yloop==nyBegin&&Yloop!=nyEnd){
          myX = (xinit + (retLenght(Yloop+1,y_rdout)-intercept)/slopeY)/2;
          myY = (yinit + retLenght(Yloop+1,y_rdout))/2;
        } else if(Yloop!=nyBegin&&Yloop==nyEnd){
          myX = (((retLenght(Yloop,y_rdout)-intercept)/slopeY)+xend)/2;
          myY = (retLenght(Yloop,y_rdout)+yend)/2;
        } else {
          myX = ((retLenght(Yloop,y_rdout)-intercept)/slopeY 
                 + (retLenght(Yloop+1,y_rdout)-intercept)/slopeY)/2;
          myY = (retLenght(Yloop,y_rdout) + retLenght(Yloop+1,y_rdout))/2;
        }
        
        myBoundary.at(0) = myX - retLenght(Xloop,x_rdout);
        myBoundary.at(1) = myY - retLenght(Yloop,y_rdout);		
        myBoundary.at(2) = retLenght(Xloop+1,x_rdout) - myX;
        myBoundary.at(3) = retLenght(Yloop+1,y_rdout) - myY;
        
        MuonFrontEndID* inputPointer = new  MuonFrontEndID;
        
        inputPointer->setFEGridX(PhNx);
        inputPointer->setFEGridY(PhNy);
        inputPointer->setFEIDX(Xloop);
        inputPointer->setFEIDY(Yloop);								
        inputPointer->setReadout(m_readoutType[iRd]);
        
        if(debug) std::cout<< "Hit processing.  RT:: "
                           <<m_readoutType[iRd]<<" ; Xl = "
                           <<" "<<Xloop<<" ; Bd = "
                           <<" "<<myBoundary.at(0)<<" ; Bd2 = "
                           <<" "<<myBoundary.at(1)<<" ; Bd3 = "
                           <<" "<<myBoundary.at(2)<<" ; Bd4 = "
                           <<" "<<myBoundary.at(3)<<" ; Yl = "
                           <<Yloop<<" "<<std::endl;
        
        tmpPair = std::pair< MuonFrontEndID,std::vector<float> >
          (*inputPointer,myBoundary);
        keepTemporary.push_back(tmpPair);
        delete inputPointer;
      }
    }
  }
  
  return keepTemporary;
}

double MuonChamberGrid::retLenght(int nLx,  std::vector<double> my_list){
  double lenght(0);
  int VctSize = my_list.size();
  if(nLx > VctSize) {
    nLx = VctSize;
    std::cout<<"MuonChamberGrid:: Vector index Out Of Range."<<std::endl;
  }
  for(int inxLo = 0; inxLo < nLx; inxLo++) {
    lenght += my_list[inxLo];
  }
  return lenght;
}

StatusCode MuonChamberGrid::getPCCenter(MuonFrontEndID fe,
                                        double& xcenter, double& ycenter){
  std::vector<double> x_rdout;
  std::vector<double> y_rdout;
  int readout=fe.getReadout();
  
  int TstRead =  m_x_pad_rdout2.size();
  int RdN = 1;
  if(TstRead > 1) RdN = 2;
  
  //get the correct grid
  for(int iRd=0;iRd<RdN;iRd++){
    if(m_readoutType[iRd]==readout){      
      if(iRd) {
        x_rdout = m_x_pad_rdout2;
        y_rdout = m_y_pad_rdout2;
      } else {
        x_rdout = m_x_pad_rdout1;
        y_rdout = m_y_pad_rdout1;
      }
      break;      
    }
  }
  unsigned int chx=fe.getFEIDX();
  unsigned int chy=fe.getFEIDY();
  if(chx>0){
    xcenter=(x_rdout[chx]+x_rdout[chx-1])/2;
  }else  xcenter=(x_rdout[chx])/2;
  
  if(chy>0){
    ycenter=(y_rdout[chy]+y_rdout[chy-1])/2;
  }else  ycenter=(y_rdout[chy])/2;
  
return StatusCode::SUCCESS;
}
