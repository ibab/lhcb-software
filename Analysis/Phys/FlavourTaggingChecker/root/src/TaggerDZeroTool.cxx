#include "TaggerDZeroTool.h"

/// WARNING: CHEATING for study purposes //

TaggerDZeroTool::TaggerDZeroTool() {

  NNetTool_MLP nnet;
  tDZ = new Tagger();

  hD_doca_r = new TH1F("hD_doca_r",   "hD_doca_r",   10, 0, 10);
  hD_doca_w = new TH1F("hD_doca_w",   "hD_doca_w",   100, 0, .5);
  hD_ipSV_r = new TH1F("hD_ipSV_r",   "hD_ipSV_r",   100, 0, 2);
  hD_ipSV_w = new TH1F("hD_ipSV_w",   "hD_ipSV_w",   100, 0, 2);
  hD_IPs_r  = new TH1F("hD_IPs_r",    "hD_IPs_r",    100, 0, 20);
  hD_IPs_w  = new TH1F("hD_IPs_w",    "hD_IPs_w",    100, 0, 20);
  hD_mass   = new TH1F("hD_mass",     "hD_mass",     100, 0.5, 2.5);
  hD_mass_r = new TH1F("hD_mass_r",   "hD_mass_r",   100, .5, 2.5);
}

///////////////////////////////////////////////////
Tagger* TaggerDZeroTool::tag(Event& event) {

  tDZ->reset();
  if(msgLevel(MSG::VERBOSE)) verbose()<<"--DZeroTool Tagger--"<<endreq;

  Particles parts = event.particles();
  Particles::iterator i, j;
  Particles Pfit; Pfit.clear();

  //--- Inclusive Secondary Vertex ---
  //look for a secondary Vtx due to opposite B
  //use DaVinci processed information
  int nD=0;
  for (i=parts.begin(); i!=parts.end(); ++i) {
    if((*i)->ComesFromSVtx()) Pfit.push_back(*i);

    if( (*i)->type()== Particle::Long || 
	(*i)->type()== Particle::Matched) {
      if(abs((*i)->motherID())==421 ){
	nD++;
	if( checkPIDhypo(Particle::kaon_opposite, *i) ){

	  if((*i)->fromB()) {
	    //      if((*i)->charge()==event.TrueTag()) {
	    hD_ipSV_r->Fill((*i)->IPSV());
	    hD_IPs_r->Fill((*i)->IPs());
	  } else {
	    hD_doca_w->Fill((*i)->docaSV());
	    hD_ipSV_w->Fill((*i)->IPSV());
	    hD_IPs_w->Fill((*i)->IPs());
	  }

	}
      }
    }
  }
	    hD_doca_r->Fill(nD);
	    if(nD!=2)  return tDZ;

  //if Vertex does not contain any daughters, exit
  if(Pfit.empty()) return tDZ;
  debug()<<"--- SVTOOL buildVertex returns vertex"
         <<" with "<<Pfit.size()<<"tracks"<<endreq;

  //calculate 
  for(i=Pfit.begin(); i!=Pfit.end(); i++) { 
    // if( (*i)->type()!= Particle::Long && (*i)->type()!= Particle::Matched) continue;
    // if((*i)->IPs()<2) continue;
    // if((*i)->pt()<.3) continue;
    bool iisK = checkPIDhypo(Particle::kaon_opposite, *i);
    bool iisP = checkPIDhypo(Particle::pion, *i);

    for(j=i+1; j!=Pfit.end(); j++) { 
      // if( (*j)->type()!= Particle::Long && (*j)->type()!= Particle::Matched) continue;
      if( (*i)->charge() * (*j)->charge() >0 ) continue; 
      bool jisK = checkPIDhypo(Particle::kaon_opposite, *j);
      bool jisP = checkPIDhypo(Particle::pion, *j);
       if( !(jisK && iisP || iisK && jisP) ) continue;
      // if((*j)->IPs()<2) continue;
      // if((*j)->pt()<.3) continue;

      TLorentzVector sum = (*i)->momentum() +(*j)->momentum(); 
      //1864.83
      hD_mass->Fill(sum.M());
      if(abs((*i)->motherID())==421 && abs((*j)->motherID())==421) hD_mass_r->Fill(sum.M());
    }

  }

  // //calculate omega
  // std::vector<double> NNinputs(10);
  // NNinputs.at(0) = parts.size();
  // NNinputs.at(1) = event.pileup();
  // NNinputs.at(2) = event.signalB()->pt();
  // NNinputs.at(3) = vflagged;
  // NNinputs.at(4) = Vptmin;
  // NNinputs.at(5) = Vipsmin;
  // NNinputs.at(6) = Vdocamax;
  // NNinputs.at(7) = maxprobf;
  // NNinputs.at(8) = Vflaglong/(vflagged? vflagged:1);
  // NNinputs.at(9) = fabs(DZ);

  // omega = 1 - nnet.MLPvtx( NNinputs );

 
  // tDZ->setDecision( DZ>0 ? -1 : 1 );
  // tDZ->setRawNNetOutput( nnet.m_rnet_vtx );
  // tDZ->setOmega( omega );
  // tDZ->setType( Tagger::VtxCharge ); 
  // for(ip=Pfit.begin(); ip!=Pfit.end(); ip++) {
  //   tDZ->addToTaggerParts(*ip);
  // }
  
  return tDZ;
}
