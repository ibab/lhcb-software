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
// Module: EvtGen/EvtDecayAmp.cc
//
// Description:
//
// Modification history:
//
//    DJL/RYD     August 11, 1998         Module created
//
//------------------------------------------------------------------------


#include "EvtGen/EvtDecayBase.hh"
#include "EvtGen/EvtDecayAmp.hh"
#include "EvtGen/EvtParticle.hh"
#include "EvtGen/EvtPDL.hh"
#include "EvtGen/EvtRandom.hh"
#include "EvtGen/EvtPHOTOS.hh"
#include "EvtGen/EvtAmp.hh"
#include "EvtGen/EvtReport.hh"


void EvtDecayAmp::makeDecay(EvtParticle* p){

  int ntimes=10000;

  int i,more;

  EvtSpinDensity rho;
  double prob,prob_max;

  _amp2.init(p->getId(),getNDaug(),getDaugs());

  do{
    _weight = 1.0;
    decay(p);

    rho=_amp2.getSpinDensity();
    prob=p->getSpinDensityForward().NormalizedProb(rho);
    if (prob<0.0) {

      report(ERROR,"EvtGen")<<"Negative prob:"<<p->getId().getId()
			    <<" "<<p->getChannel()<<endl;

      report(ERROR,"EvtGen") << "rho_forward:"<<endl;
      report(ERROR,"EvtGen") << p->getSpinDensityForward();
      report(ERROR,"EvtGen") << "rho decay:"<<endl;
      report(ERROR,"EvtGen") << rho <<endl;
    }

    if (prob!=prob) {

      report(DEBUG,"EvtGen") << "Forward density matrix:"<<endl;
      report(ERROR,"EvtGen") << p->getSpinDensityForward();

      report(DEBUG,"EvtGen") << "Decay density matrix:"<<endl;
      report(ERROR,"EvtGen") << rho;

      report(DEBUG,"EvtGen") << "prob:"<<prob<<endl;
      
      report(DEBUG,"EvtGen") << "Particle:"
			     <<EvtPDL::name(p->getId())<<endl;
      report(DEBUG,"EvtGen") << "channel        :"<<p->getChannel()<<endl;

      if( p->getParent()!=0){
	report(DEBUG,"EvtGen") << "parent:"
			       <<EvtPDL::name(
				p->getParent()->getId())<<endl;
	report(DEBUG,"EvtGen") << "parent channel        :"
			       <<p->getParent()->getChannel()<<endl;

        int i;
	report(DEBUG,"EvtGen") << "parent daughters  :";
        for (i=0;i<p->getParent()->getNDaug();i++){
	  report(DEBUG,"") << EvtPDL::name(
			    p->getParent()->getDaug(i)->getId())
				 << " ";
        }
	report(DEBUG,"") << endl;

      }
    }


    prob/=_weight;


    prob_max = getProbMax(prob);

    //cout << "Prob,prob_max,weight:"<<prob<<" "<<prob_max<<" "<<_weight<<endl;

    more=prob<EvtRandom::Flat(prob_max);

    ntimes--;

  }while(ntimes&&more);


  if (ntimes==0){
    report(DEBUG,"EvtGen") << "Tried accept/reject:10000"
			   <<" times, and rejected all the times!"<<endl;
    report(DEBUG,"")<<p->getSpinDensityForward()<<endl;
    report(DEBUG,"EvtGen") << "Is therefore accepting the last event!"<<endl;
    report(DEBUG,"EvtGen") << "Decay of particle:"<<
      EvtPDL::name(p->getId())<<"(channel:"<<
      p->getChannel()<<") with mass "<<p->mass()<<endl;
    
    int ii;
    for(ii=0;ii<p->getNDaug();ii++){
      report(DEBUG,"EvtGen") <<"Daughter "<<ii<<":"<<
	EvtPDL::name(p->getDaug(ii)->getId())<<" with mass "<<
	p->getDaug(ii)->mass()<<endl;
    }				   
  }


  EvtSpinDensity rho_list[10];

  rho_list[0]=p->getSpinDensityForward();

  EvtAmp ampcont;

  if (_amp2._pstates!=1){
    ampcont=_amp2.contract(0,p->getSpinDensityForward());
  }
  else{
    ampcont=_amp2;
  }

  for(i=0;i<p->getNDaug();i++){

    rho.SetDim(_amp2.dstates[i]);

    if (_amp2.dstates[i]==1) {
      rho.Set(0,0,EvtComplex(1.0,0.0));
    }
    else{
      rho=ampcont.contract(_amp2._dnontrivial[i],_amp2);
    }
    
    if (!rho.Check()) {
      
      report(ERROR,"EvtGen") << "-------start error-------"<<endl;
      report(ERROR,"EvtGen")<<"forward rho failed Check"<<
	p->getId().getId()<<" "<<p->getChannel()<<" "<<i<<endl;
      
      report(ERROR,"EvtGen") << rho;
      int ii; 
      _amp2.dump();
      for(ii=0;ii<i+1;ii++){
	report(ERROR,"EvtGen") << rho_list[ii];
      }
      report(ERROR,"EvtGen") << "-------Done with error-------"<<endl;  
    }

    p->getDaug(i)->setSpinDensityForward(rho);
    p->getDaug(i)->decay();
    
    rho_list[i+1]=p->getDaug(i)->getSpinDensityBackward();
  
    if (_amp2.dstates[i]!=1){
      ampcont=ampcont.contract(_amp2._dnontrivial[i],rho_list[i+1]);
    }
  

  }

  p->setSpinDensityBackward(_amp2.getBackwardSpinDensity(rho_list));


  if (!p->getSpinDensityBackward().Check()) {

    report(ERROR,"EvtGen")<<"rho_backward failed Check"<<
      p->getId().getId()<<" "<<p->getChannel()<<endl;

    report(ERROR,"EvtGen") << p->getSpinDensityBackward();

  }

  if (getPHOTOS()) EvtPHOTOS::PHOTOS(p);


}










