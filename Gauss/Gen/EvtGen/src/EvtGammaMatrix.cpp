//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtGammaMatrix.cc
//
// Description: Make gamma matrices availible for the calc. of amplitudes, etc.
//
// Modification history:
//
//    DJL/RYD     September 25, 1996         Module created
//
//------------------------------------------------------------------------
// 
#include <iostream>
#include <math.h>
#include "EvtGen/EvtComplex.hh"
#include "EvtGen/EvtGammaMatrix.hh"
#include "EvtGen/EvtDiracSpinor.hh"
#include "EvtGen/EvtReport.hh"


EvtGammaMatrix::EvtGammaMatrix(){
  int i,j;

  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      gamma[i][j]=EvtComplex(0.0,0.0);
    }
  }
}

EvtGammaMatrix operator*(const EvtComplex& c,const EvtGammaMatrix& g){
  int i,j;

  EvtGammaMatrix temp;

  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      temp.gamma[i][j]=g.gamma[i][j]*c;
    }
  }

  return temp;

}


ostream& operator<<(ostream& s, const EvtGammaMatrix& g){


  s<<"["<<g.gamma[0][0]<<","<<g.gamma[0][1]<<","<<g.gamma[0][2]<<","<<g.gamma[0][3]<<"]"<<endl;
  s<<"["<<g.gamma[1][0]<<","<<g.gamma[1][1]<<","<<g.gamma[1][2]<<","<<g.gamma[1][3]<<"]"<<endl;
  s<<"["<<g.gamma[2][0]<<","<<g.gamma[2][1]<<","<<g.gamma[2][2]<<","<<g.gamma[2][3]<<"]"<<endl;
  s<<"["<<g.gamma[3][0]<<","<<g.gamma[3][1]<<","<<g.gamma[3][2]<<","<<g.gamma[3][3]<<"]"<<endl;

  return s;

}



EvtGammaMatrix::EvtGammaMatrix(const EvtGammaMatrix& gm){
  int i,j;
  
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      gamma[i][j]=gm.gamma[i][j];
    }
  }
}

EvtGammaMatrix::~EvtGammaMatrix() {}

EvtGammaMatrix& EvtGammaMatrix::operator=(const EvtGammaMatrix& gm){
  int i,j;
  
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      gamma[i][j]=gm.gamma[i][j];
    }
  }
  return *this;
}

void EvtGammaMatrix::init(){
  int i,j;
  
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      gamma[i][j]=EvtComplex(0.0,0.0);
    }
  }
}

const EvtGammaMatrix& EvtGammaMatrix::va0(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    g.gamma[0][0]=EvtComplex(1.0,0.0);
    g.gamma[0][1]=EvtComplex(0.0,0.0);
    g.gamma[0][2]=EvtComplex(-1.0,0.0);
    g.gamma[0][3]=EvtComplex(0.0,0.0);
    g.gamma[1][0]=EvtComplex(0.0,0.0);
    g.gamma[1][1]=EvtComplex(1.0,0.0);
    g.gamma[1][2]=EvtComplex(0.0,0.0);
    g.gamma[1][3]=EvtComplex(-1.0,0.0);
    g.gamma[2][0]=EvtComplex(-1.0,0.0);
    g.gamma[2][1]=EvtComplex(0.0,0.0);
    g.gamma[2][2]=EvtComplex(1.0,0.0);
    g.gamma[2][3]=EvtComplex(0.0,0.0);
    g.gamma[3][0]=EvtComplex(0.0,0.0);
    g.gamma[3][1]=EvtComplex(-1.0,0.0);
    g.gamma[3][2]=EvtComplex(0.0,0.0);
    g.gamma[3][3]=EvtComplex(1.0,0.0);
  }

  return g;

}


const EvtGammaMatrix& EvtGammaMatrix::va1(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    g.gamma[0][0]=EvtComplex(0.0,0.0);
    g.gamma[0][1]=EvtComplex(-1.0,0.0);
    g.gamma[0][2]=EvtComplex(0.0,0.0);
    g.gamma[0][3]=EvtComplex(1.0,0.0);
    g.gamma[1][0]=EvtComplex(-1.0,0.0);
    g.gamma[1][1]=EvtComplex(0.0,0.0);
    g.gamma[1][2]=EvtComplex(1.0,0.0);
    g.gamma[1][3]=EvtComplex(0.0,0.0);
    g.gamma[2][0]=EvtComplex(0.0,0.0);
    g.gamma[2][1]=EvtComplex(1.0,0.0);
    g.gamma[2][2]=EvtComplex(0.0,0.0);
    g.gamma[2][3]=EvtComplex(-1.0,0.0);
    g.gamma[3][0]=EvtComplex(1.0,0.0);
    g.gamma[3][1]=EvtComplex(0.0,0.0);
    g.gamma[3][2]=EvtComplex(-1.0,0.0);
    g.gamma[3][3]=EvtComplex(0.0,0.0);
  }

  return g;

}



const EvtGammaMatrix& EvtGammaMatrix::va2(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    g.gamma[0][0]=EvtComplex(0.0,0.0);
    g.gamma[0][1]=EvtComplex(0.0,1.0);
    g.gamma[0][2]=EvtComplex(0.0,0.0);
    g.gamma[0][3]=EvtComplex(0.0,-1.0);
    g.gamma[1][0]=EvtComplex(0.0,-1.0);
    g.gamma[1][1]=EvtComplex(0.0,0.0);
    g.gamma[1][2]=EvtComplex(0.0,1.0);
    g.gamma[1][3]=EvtComplex(0.0,0.0);
    g.gamma[2][0]=EvtComplex(0.0,0.0);
    g.gamma[2][1]=EvtComplex(0.0,-1.0);
    g.gamma[2][2]=EvtComplex(0.0,0.0);
    g.gamma[2][3]=EvtComplex(0.0,1.0);
    g.gamma[3][0]=EvtComplex(0.0,1.0);
    g.gamma[3][1]=EvtComplex(0.0,0.0);
    g.gamma[3][2]=EvtComplex(0.0,-1.0);
    g.gamma[3][3]=EvtComplex(0.0,0.0);
  }

  return g;

}




const EvtGammaMatrix& EvtGammaMatrix::va3(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    g.gamma[0][0]=EvtComplex(-1.0,0.0);
    g.gamma[0][1]=EvtComplex(0.0,0.0);
    g.gamma[0][2]=EvtComplex(1.0,0.0);
    g.gamma[0][3]=EvtComplex(0.0,0.0);
    g.gamma[1][0]=EvtComplex(0.0,0.0);
    g.gamma[1][1]=EvtComplex(1.0,0.0);
    g.gamma[1][2]=EvtComplex(0.0,0.0);
    g.gamma[1][3]=EvtComplex(-1.0,0.0);
    g.gamma[2][0]=EvtComplex(1.0,0.0);
    g.gamma[2][1]=EvtComplex(0.0,0.0);
    g.gamma[2][2]=EvtComplex(-1.0,0.0);
    g.gamma[2][3]=EvtComplex(0.0,0.0);
    g.gamma[3][0]=EvtComplex(0.0,0.0);
    g.gamma[3][1]=EvtComplex(-1.0,0.0);
    g.gamma[3][2]=EvtComplex(0.0,0.0);
    g.gamma[3][3]=EvtComplex(1.0,0.0);
  }

  return g;
  
}





const EvtGammaMatrix& EvtGammaMatrix::g0(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){

    first=0;

    int i,j;
  
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][0]=EvtComplex(1.0,0.0);
    g.gamma[1][1]=EvtComplex(1.0,0.0);
    g.gamma[2][2]=EvtComplex(-1.0,0.0);
    g.gamma[3][3]=EvtComplex(-1.0,0.0);
  }

  return g;

}




const EvtGammaMatrix& EvtGammaMatrix::g1(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    first=0;
    int i,j;
    
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][3]=EvtComplex(1.0,0.0);
    g.gamma[1][2]=EvtComplex(1.0,0.0);
    g.gamma[2][1]=EvtComplex(-1.0,0.0);
    g.gamma[3][0]=EvtComplex(-1.0,0.0);
  }
  
  return g;

}




const EvtGammaMatrix& EvtGammaMatrix::g2(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    first=0;
    int i,j;
    
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][3]=EvtComplex(0.0,-1.0);
    g.gamma[1][2]=EvtComplex(0.0,1.0);
    g.gamma[2][1]=EvtComplex(0.0,1.0);
    g.gamma[3][0]=EvtComplex(0.0,-1.0);
  }
  
  return g;

}





const EvtGammaMatrix& EvtGammaMatrix::g3(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
    first=0;
    int i,j;
    
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][2]=EvtComplex(1.0,0.0);
    g.gamma[1][3]=EvtComplex(-1.0,0.0);
    g.gamma[2][0]=EvtComplex(-1.0,0.0);
    g.gamma[3][1]=EvtComplex(1.0,0.0);
  }

  return g;

}




const EvtGammaMatrix& EvtGammaMatrix::g5(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){

    int i,j;
    
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][2]=EvtComplex(1.0,0.0);
    g.gamma[1][3]=EvtComplex(1.0,0.0);
    g.gamma[2][0]=EvtComplex(1.0,0.0);
    g.gamma[3][1]=EvtComplex(1.0,0.0);
  }

  return g;

}



const EvtGammaMatrix& EvtGammaMatrix::v0(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){

    int i,j;
  
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][0]=EvtComplex(1.0,0.0);
    g.gamma[1][1]=EvtComplex(1.0,0.0);
    g.gamma[2][2]=EvtComplex(1.0,0.0);
    g.gamma[3][3]=EvtComplex(1.0,0.0);
  }

  return g;

}





const EvtGammaMatrix& EvtGammaMatrix::v1(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){

    int i,j;
    
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][3]=EvtComplex(1.0,0.0);
    g.gamma[1][2]=EvtComplex(1.0,0.0);
    g.gamma[2][1]=EvtComplex(1.0,0.0);
    g.gamma[3][0]=EvtComplex(1.0,0.0);
  }

  return g;

}




const EvtGammaMatrix& EvtGammaMatrix::v2(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){
  
    int i,j;

    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][3]=EvtComplex(0.0,-1.0);
    g.gamma[1][2]=EvtComplex(0.0,1.0);
    g.gamma[2][1]=EvtComplex(0.0,-1.0);
    g.gamma[3][0]=EvtComplex(0.0,1.0);
  }

  return g;

}




const EvtGammaMatrix& EvtGammaMatrix::v3(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){

    int i,j;
  
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][2]=EvtComplex(1.0,0.0);
    g.gamma[1][3]=EvtComplex(-1.0,0.0);
    g.gamma[2][0]=EvtComplex(1.0,0.0);
    g.gamma[3][1]=EvtComplex(-1.0,0.0);
  }

  return g;

}





const EvtGammaMatrix& EvtGammaMatrix::id(){

  static EvtGammaMatrix g;
  static int first=1;

  if (first){

    int i,j;
    
    for(i=0;i<4;i++){
      for(j=0;j<4;j++){
	g.gamma[i][j]=EvtComplex(0.0,0.0);
      }
    }
    
    g.gamma[0][0]=EvtComplex(1.0,0.0);
    g.gamma[1][1]=EvtComplex(1.0,0.0);
    g.gamma[2][2]=EvtComplex(1.0,0.0);
    g.gamma[3][3]=EvtComplex(1.0,0.0);
  }

  return g;

}




EvtGammaMatrix& EvtGammaMatrix::operator+=(const EvtGammaMatrix &g){

  int i,j;
  
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      gamma[i][j]+=g.gamma[i][j];
    }
  }
  return *this;
}





EvtGammaMatrix& EvtGammaMatrix::operator-=(const EvtGammaMatrix &g){

  int i,j;
  
  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      gamma[i][j]-=g.gamma[i][j];
    }
  }
  return *this;
}



EvtGammaMatrix& EvtGammaMatrix::operator*=(const EvtGammaMatrix &g){

  int i,j,k;
  EvtGammaMatrix temp;

  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
      temp.gamma[i][j]=EvtComplex(0.0,0.0);
      for(k=0;k<4;k++){
	temp.gamma[i][j]+=gamma[i][k]*g.gamma[k][j];
      }
    }
  }

  for(i=0;i<4;i++){
    for(j=0;j<4;j++){
       gamma[i][j]=temp.gamma[i][j];
    }
  }

  return *this;
}


EvtDiracSpinor operator*(const EvtGammaMatrix& g,const EvtDiracSpinor& d){

  int i,j;
  EvtDiracSpinor temp;
  
   for(i=0;i<4;i++){
     temp.set_spinor(i,EvtComplex(0.0,0.0));
     for(j=0;j<4;j++){
       temp.set_spinor(i,temp.get_spinor(i)+g.gamma[i][j]*d.get_spinor(j));      
     }
   }
   
   return temp;
}  

EvtComplex operator*(const EvtDiracSpinor& d,const EvtDiracSpinor& dp){

  int i;
  EvtComplex temp;
  
  temp=EvtComplex(0.0,0.0);
  
  for(i=0;i<4;i++){
    temp+=::conj(d.get_spinor(i))*dp.get_spinor(i);
  }
  return temp;
}  

