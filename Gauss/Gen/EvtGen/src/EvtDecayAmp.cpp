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



#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGenBase/EvtDecayAmp.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtRandom.hh"
#include "EvtGenBase/EvtRadCorr.hh"
#include "EvtGenBase/EvtAmp.hh"
#include "EvtGenBase/EvtReport.hh"


void EvtDecayAmp::makeDecay(EvtParticle* p){

  int ntimes=10000;

  int i,more;

  EvtSpinDensity rho;
  double prob,prob_max;

  _amp2.init(p->getId(),getNDaug(),getDaugs());
  //report(INFO,"EvtGen") << "Decaying " << EvtPDL::name(p->getId()) << std::endl;
  do{
    _weight = 1.0;
    decay(p);

    rho=_amp2.getSpinDensity();
    prob=p->getSpinDensityForward().NormalizedProb(rho);
    if (prob<0.0) {

      report(ERROR,"EvtGen")<<"Negative prob:"<<p->getId().getId()
			    <<" "<<p->getChannel()<<std::endl;

      report(ERROR,"EvtGen") << "rho_forward:"<<std::endl;
      report(ERROR,"EvtGen") << p->getSpinDensityForward();
      report(ERROR,"EvtGen") << "rho decay:"<<std::endl;
      report(ERROR,"EvtGen") << rho <<std::endl;
    }

    if (prob!=prob) {

      report(DEBUG,"EvtGen") << "Forward density matrix:"<<std::endl;
      report(ERROR,"EvtGen") << p->getSpinDensityForward();

      report(DEBUG,"EvtGen") << "Decay density matrix:"<<std::endl;
      report(ERROR,"EvtGen") << rho;

      report(DEBUG,"EvtGen") << "prob:"<<prob<<std::endl;
      
      report(DEBUG,"EvtGen") << "Particle:"
			     <<EvtPDL::name(p->getId()).c_str()<<std::endl;
      report(DEBUG,"EvtGen") << "channel        :"<<p->getChannel()<<std::endl;
      report(DEBUG,"EvtGen") << "Momentum:" << p->getP4() << " " << p->mass() << std::endl;
      if( p->getParent()!=0){
	report(DEBUG,"EvtGen") << "parent:"
			       <<EvtPDL::name(
				p->getParent()->getId()).c_str()<<std::endl;
	report(DEBUG,"EvtGen") << "parent channel        :"
			       <<p->getParent()->getChannel()<<std::endl;

        int i;
	report(DEBUG,"EvtGen") << "parent daughters  :";
        for (i=0;i<p->getParent()->getNDaug();i++){
	  report(DEBUG,"") << EvtPDL::name(
			    p->getParent()->getDaug(i)->getId()).c_str()
				 << " ";
        }
	report(DEBUG,"") << std::endl;

	report(DEBUG,"EvtGen") << "daughters  :";
        for (i=0;i<p->getNDaug();i++){
	  report(DEBUG,"") << EvtPDL::name(
			    p->getDaug(i)->getId()).c_str()
				 << " ";
        }
	report(DEBUG,"") << std::endl;

	report(DEBUG,"EvtGen") << "daughter momenta  :" << std::endl;;
        for (i=0;i<p->getNDaug();i++){
	  report(DEBUG,"") << p->getDaug(i)->getP4() << " " << p->getDaug(i)->mass();
	  report(DEBUG,"") << std::endl;
        }

      }
    }


    prob/=_weight;


    prob_max = getProbMax(prob);

    //report(INFO,"EvtGen") << "Prob,prob_max,weight:"<<prob<<" "<<prob_max<<" "<<_weight<<std::endl;

    more=prob<EvtRandom::Flat(prob_max);

    ntimes--;

  }while(ntimes&&more);

  //report(INFO,"EvtGen") << "Done\n";

  if (ntimes==0){
    report(DEBUG,"EvtGen") << "Tried accept/reject:10000"
			   <<" times, and rejected all the times!"<<std::endl;
    report(DEBUG,"")<<p->getSpinDensityForward()<<std::endl;
    report(DEBUG,"EvtGen") << "Is therefore accepting the last event!"<<std::endl;
    report(DEBUG,"EvtGen") << "Decay of particle:"<<
      EvtPDL::name(p->getId()).c_str()<<"(channel:"<<
      p->getChannel()<<") with mass "<<p->mass()<<std::endl;
    
    int ii;
    for(ii=0;ii<p->getNDaug();ii++){
      report(DEBUG,"EvtGen") <<"Daughter "<<ii<<":"<<
	EvtPDL::name(p->getDaug(ii)->getId()).c_str()<<" with mass "<<
	p->getDaug(ii)->mass()<<std::endl;
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

  //report(INFO,"EvtGen") << "Found " << p->getNDaug() << " daughters\n";
  for(i=0;i<p->getNDaug();i++){

    rho.SetDim(_amp2.dstates[i]);

    if (_amp2.dstates[i]==1) {
      rho.Set(0,0,EvtComplex(1.0,0.0));
    }
    else{
      rho=ampcont.contract(_amp2._dnontrivial[i],_amp2);
    }
    
    if (!rho.Check()) {
      
      report(ERROR,"EvtGen") << "-------start error-------"<<std::endl;
      report(ERROR,"EvtGen")<<"forward rho failed Check:"<<
	EvtPDL::name(p->getId()).c_str()<<" "<<p->getChannel()<<" "<<i<<std::endl;

      report(ERROR,"EvtGen")<<"Parent:"<<EvtPDL::name(p->getParent()->getId()).c_str()<<std::endl;
      report(ERROR,"EvtGen")<<"GrandParent:"<<EvtPDL::name(p->getParent()->getParent()->getId()).c_str()<<std::endl;
      report(ERROR,"EvtGen")<<"GrandGrandParent:"<<EvtPDL::name(p->getParent()->getParent()->getParent()->getId()).c_str()<<std::endl;

      report(ERROR,"EvtGen") << rho;
      int ii; 
      _amp2.dump();
      for(ii=0;ii<i+1;ii++){
	report(ERROR,"EvtGen") << rho_list[ii];
      }
      report(ERROR,"EvtGen") << "-------Done with error-------"<<std::endl;  
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
      p->getId().getId()<<" "<<p->getChannel()<<std::endl;

    report(ERROR,"EvtGen") << p->getSpinDensityBackward();

  }

  if (getPHOTOS() || EvtRadCorr::alwaysRadCorr()) {
    EvtRadCorr::doRadCorr(p);
  }

}










