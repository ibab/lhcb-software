// $Id: MuonChamberGrid.cpp,v 1.1 2005-10-25 06:54:28 asarti Exp $
// Include files 

// local
#include "GaudiKernel/IToolSvc.h"  

#include "MuonDet/MuonChamberGrid.h"
#include "MuonDet/MuonFrontEndID.h"
#include <iostream>
#include <vector>

#include "DetDescCnv/XmlUserConditionCnv.h"

typedef XmlUserConditionCnv<MuonChamberGrid>       XmlMuonGridCnv;
static CnvFactory<XmlMuonGridCnv>           s_XmlMuonGridCnvFactory ;
const ICnvFactory&  XmlMuonGridCnvFactory = s_XmlMuonGridCnvFactory ;

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
}
//=============================================================================
// Destructor
//=============================================================================
MuonChamberGrid::~MuonChamberGrid() {}; 

//=============================================================================
StatusCode MuonChamberGrid::initialize(){

  char patt[400]; int Ngrid(0);
  sprintf(patt,"%s",(this->name()).data());
  sscanf(patt,"/G%d",&Ngrid);
  m_number_of_grid = Ngrid;
  
  //  std::cout<< "Gn: "<< m_number_of_grid<<" "
  //	   <<(this->name()).data()<<std::endl;

  m_x_pad_rdout1     = param< std::vector<double> >("xrd1");
  m_y_pad_rdout1     = param< std::vector<double> >("yrd1");
  m_x_pad_rdout2     = param< std::vector<double> >("xrd2");
  m_y_pad_rdout2     = param< std::vector<double> >("yrd2");
  m_readoutType      = param< std::vector<double> >("grrd");


  return StatusCode::SUCCESS;
}

std::vector<MuonFrontEndID> MuonChamberGrid::listOfPhysChannels(double x_enter,double y_enter,double x_exit,double y_exit) {

  std::vector<MuonFrontEndID> keepTemporary;
  std::vector<double> x_rdout;  
  std::vector<double> y_rdout;

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

  bool debug = false;
  
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
      if(x_enter > tmpXLenght) {
	tmpXLenght += x_rdout[inxLo];
	tmpNxChaEntry =  inxLo;
	//	tmpNxChaEntry =  inxLo+1;
      } else {
	break;
      }
    }
    tmpXLenght = 0;
    for(inxLo = 0; inxLo < PhNx; inxLo++) {
      if(x_exit > tmpXLenght) {
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
    double xBegin,xEnd;       double yBegin, yEnd; 

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


      int tmpYBegin(0),tmpYEnd(0);
      double tmpYLenght(0);
      
      for(inyLo = 0; inyLo < PhNy; inyLo++) {
	if(yBegin > tmpYLenght) {
	  tmpYLenght += y_rdout[inyLo];
	  //	  tmpYBegin =  inyLo+1;
	  tmpYBegin =  inyLo;
	} else {
	  break;
	}
      }
      tmpYLenght = 0;
      for(inyLo = 0; inyLo < PhNy; inyLo++) {
	if(yEnd > tmpYLenght) {
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
      } else {

	if( tmpYEnd<0)tmpYEnd=0;
	if( tmpYBegin>=(int)PhNy)tmpYBegin=(int)PhNy-1 ;

	nyBegin=tmpYEnd; nyEnd=tmpYBegin;
      }

      if(debug) std::cout<< "Debugging Y_beg: "
			 <<nyBegin<<"; Y_end: "
			 <<nyEnd<<std::endl;

      for (unsigned int Yloop=nyBegin;Yloop<=nyEnd;Yloop++){

	MuonFrontEndID* inputPointer = new  MuonFrontEndID  ;				

	inputPointer->setFEGridX(PhNx);
	inputPointer->setFEGridY(PhNy);
	inputPointer->setFEIDX(Xloop);
	inputPointer->setFEIDY(Yloop);								
	inputPointer->setReadout(m_readoutType[iRd]);

	if(debug) std::cout<< "Hit processing.  RT:: "
			   <<m_readoutType[iRd]<<" ; Xl = "
			   <<" "<<Xloop<<" ; Yl = "
			   <<Yloop<<" "<<std::endl;
	
	keepTemporary.push_back(*inputPointer);
	delete inputPointer;
      }
    }
  }

  return keepTemporary;
}

double MuonChamberGrid::retLenght(int nLx,  std::vector<double> my_list){
  double lenght(0);
  for(int inxLo = 0; inxLo < nLx; inxLo++) {
    lenght += my_list[inxLo];
  }
  return lenght;
}

