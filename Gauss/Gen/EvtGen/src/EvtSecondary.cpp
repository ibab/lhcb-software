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
// Module: EvtSecondary.cc
//
// Description: Class to store the decays of the secondary particles.
//
// Modification history:
//
//    RYD       March 12, 1998       Module created
//
//------------------------------------------------------------------------
//
#include <iostream>
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtSecondary.hh"
#include "EvtGen/EvtReport.hh"


void EvtSecondary::init(){
  _npart=0;
}
  
int EvtSecondary::getNPart(){
  return _npart;
}

void EvtSecondary::createSecondary(int stdhepindex,EvtParticle* prnt){

  _stdhepindex[_npart]=stdhepindex;
  if (prnt->getNDaug()==0){
    _id1[_npart]=0;
    _id2[_npart]=0;
    _id3[_npart]=0;
    _npart++;
    return;
  }
  if (prnt->getNDaug()==1){
    _id1[_npart]=EvtPDL::getStdHep(prnt->getDaug(0)->getId());
    _id2[_npart]=0;
    _id3[_npart]=0;
    _npart++;
    return;
  }
  if (prnt->getNDaug()==2){
    _id1[_npart]=EvtPDL::getStdHep(prnt->getDaug(0)->getId());
    _id2[_npart]=EvtPDL::getStdHep(prnt->getDaug(1)->getId());
    _id3[_npart]=0;
    _npart++;
    return;
  }
  if (prnt->getNDaug()==3){
    _id1[_npart]=EvtPDL::getStdHep(prnt->getDaug(0)->getId());
    _id2[_npart]=EvtPDL::getStdHep(prnt->getDaug(1)->getId());
    _id3[_npart]=EvtPDL::getStdHep(prnt->getDaug(2)->getId());
    _npart++;
    return;
  }
  
  report(ERROR,"EvtGen") << 
    "More than 3 decay products in a secondary particle!"<<std::endl;


}
 

std::ostream& operator<<(std::ostream& s, const EvtSecondary& secondary){

  s <<std::endl;
  s << "Secondary decays:"<<std::endl;

  int i;
  for(i=0;i<secondary._npart;i++){

    report(INFO,"EvtGen") <<i<<" "
	 <<secondary._stdhepindex[i]<<" "
	 <<secondary._id1[i]<<" "
	 <<secondary._id2[i]<<" "
	 <<secondary._id3[i]<<std::endl;

  }
  
  s<<std::endl;
  
  return s;

}  

