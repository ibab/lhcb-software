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
// Module: cgFinder.cc
//
//  Description:
//

// Modification history:
//
//  August 11,2000
//------------------------------------------------------------------------
//
//
//
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "EvtGen/EvtOrthogVector.hh"
#include "EvtGen/EvtCGCoefSingle.hh"
#include "EvtGen/EvtdFunction.hh"

int main(int /*argc*/, char* /*argv*/[]){

  std::cout << "First test the Vector..." << std::endl;
  EvtVectorT<double> tempT;
  tempT.append(1.0);
  std::cout << tempT[0] << std::endl;
  tempT.append(2.2);
  std::cout << tempT[0] << " " << tempT[1]  << std::endl;
  tempT.append(3.0);
  std::cout << tempT[0] << " " << tempT[1] << " "<< tempT[2] << std::endl;

  std::cout << "EvtdFunction:"<<EvtdFunction::d(6,2,0,acos(0.9999))<<std::endl;
  std::cout << "EvtdFunction:"<<EvtdFunction::d(6,2,0,acos(0.5))<<std::endl;
  std::cout << "EvtdFunction:"<<EvtdFunction::d(6,2,0,acos(0.0))<<std::endl;


  double temp[5];
  int dim=5;

  temp[0]=1.0; temp[1]=2.0; temp[2]=1.0; temp[3]=0.0;temp[4]=3.0;
  EvtVectorT<double> vect0(dim,temp);
  temp[0]=3.0; temp[1]=2.0; temp[2]=1.0; temp[3]=0.0;temp[4]=1.0;
  EvtVectorT<double> vect1(dim,temp);
  temp[0]=6.0; temp[1]=7.0; temp[2]=8.0; temp[3]=5.0;temp[4]=1.0;
  EvtVectorT<double> vect2(dim,temp);
  temp[0]=3.0; temp[1]=1.0; temp[2]=2.0; temp[3]=1.0;temp[4]=0.0;
  EvtVectorT<double> vect3(dim,temp);

  EvtVectorT<double> vectors[4]={vect0,vect1,vect2,vect3};

  EvtOrthogVector getOrth(dim,vectors);

  EvtVectorT<double> orth=getOrth.getOrthogVector();
  std::cout << "orth 0="<<orth[0]<<std::endl;
  std::cout << "orth 1="<<orth[1]<<std::endl;
  std::cout << "orth 2="<<orth[2]<<std::endl;
  std::cout << "orth 3="<<orth[3]<<std::endl;
  std::cout << "orth 4="<<orth[4]<<std::endl;

  EvtCGCoefSingle cg1,cg2,cg3,cg4,cg5;
  
  cg1.init(4,4);
  std::cout <<"cg1.coef(6,6,4,4,4,2):"<<cg1.coef(6,6,4,4,4,2)<<std::endl;
  std::cout <<"cg1.coef(6,6,4,4,2,4):"<<cg1.coef(6,6,4,4,2,4)<<std::endl<<std::endl;

  std::cout <<"cg1.coef(4,4,4,4,4,0):"<<cg1.coef(4,4,4,4,4,0)<<std::endl;
  std::cout <<"cg1.coef(4,4,4,4,2,2):"<<cg1.coef(4,4,4,4,2,2)<<std::endl;
  std::cout <<"cg1.coef(4,4,4,4,0,4):"<<cg1.coef(4,4,4,4,0,4)<<std::endl<<std::endl<<std::endl;


  cg2.init(2,2);
  std::cout <<"cg2.coef(2,2,2,2,2,0):"<<cg2.coef(2,2,2,2,2,0)<<std::endl;
  std::cout <<"cg2.coef(2,2,2,2,0,2):"<<cg2.coef(2,2,2,2,0,2)<<std::endl<<std::endl<<std::endl;


  cg3.init(1,1);
  std::cout <<"cg3.coef(0,0,1,1,1,-1):"<<cg3.coef(0,0,1,1,1,-1)<<std::endl;
  std::cout <<"cg3.coef(0,0,1,1,-1,1):"<<cg3.coef(0,0,1,1,-1,1)<<std::endl<<std::endl<<std::endl;

  cg4.init(4,3);
  std::cout <<"cg4.coef(1,1,4,3,4,-3):"<<cg4.coef(1,1,4,3,4,-3)<<std::endl;
  std::cout <<"cg4.coef(1,1,4,3,2,-1):"<<cg4.coef(1,1,4,3,2,-1)<<std::endl;
  std::cout <<"cg4.coef(1,1,4,3,0,1):"<<cg4.coef(1,1,4,3,0,1)<<std::endl;
  std::cout <<"cg4.coef(1,1,4,3,-2,3):"<<cg4.coef(1,1,4,3,-2,3)<<std::endl<<std::endl<<std::endl;

  cg5.init(8,9);
  std::cout <<"cg5.coef(1,1,8,9,4,-3):"<<cg5.coef(1,1,8,9,4,-3)<<std::endl;

  return 1;
}






