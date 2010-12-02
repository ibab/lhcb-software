#include "SVertexOneSeedTool.h"

SVertexOneSeedTool::SVertexOneSeedTool() {

  declareProperty( "MinSeedProbability", m_maxprobf = 0.4 );

  //declare histos:///////////////////
  hvprobf2 = new TH1F("hvprobf2","probf",        100, 0, 1.01);
  hvprobf0 = new TH1F("hvprobf0","probf",        100, 0, 1.01);
  hvChi0   = new TH1F("hvChi0","hvChi0", 100, 0, 15);
  hvChi2   = new TH1F("hvChi2","hvChi2", 100, 0, 15);
  hz0      = new TH1F("hz0","hz0", 50, 0, 10);
  hz2      = new TH1F("hz2","hz2", 50, 0, 10);
  hvpmin_2 = new TH1F("hvpmin_2","hvpmin_2", 100, 0, 2.5);
  hvpmin_0 = new TH1F("hvpmin_0","hvpmin_0", 100, 0, 2.5);
  hvipmin2 = new TH1F("hvipmin2","hvipmin2", 100, 0, 2);
  hvipmin0 = new TH1F("hvipmin0","hvipmin0", 100, 0, 2);
  hvipmax2 = new TH1F("hvipmax2","hvipmax2", 100, 0, 5);
  hvipmax0 = new TH1F("hvipmax0","hvipmax0", 100, 0, 5);
  hvipsmin2= new TH1F("hvipsmin2","hvipsmin2", 100, 0, 40);
  hvipsmin0= new TH1F("hvipsmin0","hvipsmin0", 100, 0, 40);
  hvdphi2  = new TH1F("hvdphi2","hvdphi2", 100, 0, 3);
  hvdphi0  = new TH1F("hvdphi0","hvdphi0", 100, 0, 3);
  hvmass2  = new TH1F("hvmass2","hvmass2", 100, 0, 3);
  hvmass0  = new TH1F("hvmass0","hvmass0", 100, 0, 3);
  hvreson2 = new TH1F("hvreson2","hvreson2", 100, 0, 3);
  hvreson0 = new TH1F("hvreson0","hvreson0", 100, 0, 3);
  hmaxprobf2 = new TH1F("hmaxprobf2","maxprobf", 100, 0, 1.01);
  hmaxprobf0 = new TH1F("hmaxprobf0","maxprobf", 100, 0, 1.01);
}

//////////////////////////////////////////////////////////
Vertices SVertexOneSeedTool::buildVertex(Particles& vtags, 
                                         Event* event) {
  Vertices vtxvect; 

  if( ! event->hasSecondaryVertexSeed() ) return vtxvect;

  //loop to find seed -----------------------------------
  // this has been done already in DaVinci...
  // just use 2-track seed formed there:

  Vertex* Vfit = event->getSeeds().at(0);

  Particles pp = Vfit->outgoingParticles();
  Particle* p1 = pp.at(0); 
  Particle* p2 = pp.at(1);

  debug()<<"SVertexOneSeedTool outgoingParticles "<<pp.size()<<endl;

  //evaluate likelihood

  double prob_chi2  = pol(Vfit->chi2PerDoF(), 0.615074, -0.081797, 0.00421188);

  double prob_ptmin = pol(std::min(p1->pt(), p2->pt()) /GeV, 0.0662687, 1.10754, -0.350278);

  double prob_ipmax = pol(std::max(p1->IP(), p2->IP()), 0.763837, -0.0822829, -0.0154407);

  double ipsmin     = std::min(p1->IPs(), p2->IPs());
  if(ipsmin < 2.5) return vtxvect; //////////// CUT
  double prob_ipsmin = 0;
  if(ipsmin<6)  prob_ipsmin = pol(ipsmin, -0.642335, 0.356381, -0.0239819);
  else prob_ipsmin = pol(ipsmin, 0.536929, 0.0254873, -0.000439594);
  
  double dphi= fabs(p1->Phi()-p2->Phi()); 
  if(dphi>3.1416) dphi= 2*3.1416-dphi;
  double prob_deltaphi = pol(dphi, 0.699251, -0.19263, 0.00319839);

  double prob_rdist;
  TVector3 SVpoint = Vfit->position();
  double rdist = sqrt( SVpoint.x()*SVpoint.x()
		       +SVpoint.y()*SVpoint.y()
		       +SVpoint.z()*SVpoint.z()*.074*.074 );
  if(rdist<1) prob_rdist= pol(rdist, 9.61771e-05, 0.936598, -0.433183);
  else        prob_rdist= pol(rdist, 0.44296, 0.0956002, -0.0130237);

  // debug()<<prob_chi2<< " "<<  prob_ptmin<<  " "<<    prob_ipmax<< " "
  // 	 << prob_ipsmin<<  " "<< prob_deltaphi<<  " "<< prob_rdist<<endmsg;

  double probf = combine(prob_chi2,   prob_ptmin,    prob_ipmax,
			 prob_ipsmin, prob_deltaphi, prob_rdist);

  //plots
  bool twofromB = (p1->fromB()==1) && (p2->fromB()==1);
  if(twofromB) hvprobf2->Fill(probf); else hvprobf0->Fill(probf);

  if( probf < m_maxprobf ) return vtxvect; //////////// CUT

  //save likelihood
  Vfit->setLikelihood(probf);
  debug()<<"maxprobf="<<probf<<endreq;

  //add iteratively other tracks to the seed ----------------------
  Particles::iterator jpp;
  for ( jpp = vtags.begin(); jpp != vtags.end(); jpp++ ) {
    if( (*jpp)->pt() == p1->pt() ) continue;
    if( (*jpp)->pt() == p2->pt() ) continue;

    if((*jpp)->cloneDist()!=-1) continue;
    if((*jpp)->LCS()>=3) continue;

    double ip=(*jpp)->IP(), ipe=(*jpp)->IPerr();
    if( ipe==0 ) continue;
    if( ip/ipe < 3.5 ) continue; //cut
    if( ip < 0.1 ) continue;     //cut

    double ipSV=(*jpp)->IPSV();
    if( ipSV > 0.7 ) continue;//cut

    if((*jpp)->DOCA() > 0.2 ) continue;//cut
    Vfit->addToOutgoingParticles(*jpp);

    debug() << " added to SV === pt=" << (*jpp)->pt() <<endreq;
  }

  double smass= (p1->momentum()+p2->momentum()).M();
  if(msgLevel(MSG::VERBOSE)) verbose()<<"smass"<<smass<<endreq;
  if(smass<0.2) return vtxvect; //////////// CUT

  if(twofromB && //p1->motherID()==p2->motherID() && p1->motherID() && 
     p1->charge()*p2->charge()<0) hvreson2->Fill(smass); 
  else hvreson0->Fill(smass);

  //if(smass>0.48 && smass<0.51 &&
  if(smass>0.490 && smass<0.505 &&
     p1->charge()*p2->charge()<0) return vtxvect; //////////// CUT KShort

  Vfit->setType(Vertex::Secondary);

  vtxvect.push_back(Vfit);

  //fill some more histos!
  if(twofromB) hmaxprobf2->Fill(probf); else hmaxprobf0->Fill(probf);
  if(twofromB) hz2->Fill(rdist); else hz0->Fill(rdist);
  if(twofromB) hvChi2->Fill(Vfit->chi2PerDoF());
  else         hvChi0->Fill(Vfit->chi2PerDoF());
  if(twofromB) hvpmin_2->Fill(std::min(p1->pt(), p2->pt()) /GeV);
  else         hvpmin_0->Fill(std::min(p1->pt(), p2->pt()) /GeV);
  if(twofromB) hvipmax2->Fill(std::max(p1->IP(), p2->IP()));
  else         hvipmax0->Fill(std::max(p1->IP(), p2->IP()));
  if(twofromB) hvipsmin2->Fill(ipsmin);
  else         hvipsmin0->Fill(ipsmin);
  if(twofromB) hvdphi2->Fill(dphi);
  else         hvdphi0->Fill(dphi);
  if(twofromB) hvmass2->Fill(smass);
  else         hvmass0->Fill(smass);
  event->setSecondaryVertices(vtxvect);
  
  return vtxvect;
}

//////////////////////////////////////////////////////////////////
double SVertexOneSeedTool::combine(double p1, double p2, double p3,
				   double p4, double p5, double p6){
  double probs = p1*p2*p3*p4*p5*p6;
  double probb = (1-p1)*(1-p2)*(1-p3)*(1-p4)*(1-p5)*(1-p6);
  double probf = probs / ( probs + probb );
  if(probf>1) probf=1;
  return probf;
}
