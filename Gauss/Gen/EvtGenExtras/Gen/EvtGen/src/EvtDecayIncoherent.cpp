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
// Module: EvtGen/EvtDecayIncoherent.cc
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------

#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtDecayIncoherent.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenBase/EvtPDL.hh"


void EvtDecayIncoherent::makeDecay(EvtParticle* p){

  int i;
  //initialize this the hard way..
  //Lange June 26, 2000
  for (i=0; i<MAX_DAUG; i++ ) { spinDensitySet[i]=0;}

  decay(p);

  EvtSpinDensity rho;

  rho.SetDiag(p->getSpinStates());

  p->setSpinDensityBackward(rho);

  if (getPHOTOS() || EvtRadCorr::alwaysRadCorr()) {
    EvtRadCorr::doRadCorr(p);
  }

  //Now decay the daughters.
  for(i=0;i<p->getNDaug();i++){
    //Need to set the spin density of the daughters to be
    //diagonal.
    rho.SetDiag(p->getDaug(i)->getSpinStates());
    //if (p->getDaug(i)->getNDaug()==0){
      //only do this if the user has not already set the 
      //spin density matrix herself.
     //Lange June 26, 2000
      if ( isDaughterSpinDensitySet(i)==0 ) { 
	p->getDaug(i)->setSpinDensityForward(rho);
      }
      else{
	//report(INFO,"EvtGen") << "spinDensitymatrix already set!!!\n";
	EvtSpinDensity temp=p->getDaug(i)->getSpinDensityForward();
	//	report(INFO,"EvtGen") <<temp<<std::endl;
      }
      //Now decay the daughter.  Really!
      p->getDaug(i)->decay();
      //}
  } 
			    
}







