#include "TaggerMuonTool.h"

TaggerMuonTool::TaggerMuonTool( ) {

  declareProperty( "Muon_Pt_cut",  m_Pt_cut_muon  = 1.1 *GeV );
  declareProperty( "Muon_P_cut",   m_P_cut_muon   = 0.0 *GeV );
  declareProperty( "Muon_lcs_cut", m_lcs_cut_muon = 2.2 );
  declareProperty( "Muon_IPs_cut", m_IPs_cut_muon = 0.0 );
  declareProperty( "Muon_PIDm_cut",m_PIDm_cut     = 2.0 );
  declareProperty( "Muon_ipPU_cut", m_ipPU_cut_muon      = 3.0 );
  declareProperty( "Muon_distPhi_cut", m_distPhi_cut_muon= 0.005 );
  declareProperty( "ProbMin_muon", m_ProbMin_muon = 0. ); //no cut
  declareProperty( "Muon_P0_Cal",  m_P0_Cal_muon   = 0.319 ); 
  declareProperty( "Muon_P1_Cal",  m_P1_Cal_muon   = 1.21 ); 
  declareProperty( "Muon_Eta_Cal", m_Eta_Cal_muon  = 0.321 ); 

  NNetTool_MLP nnet;
  tmu = new Tagger();
  hcut_mu_ippu= new TH1F("hcut_mu_ippu","hcut_mu_ippu",100, 0, 20);
  hcut_mu_distphi= new TH1F("hcut_mu_distphi","hcut_mu_distphi",100, 0, 0.09);
  hcut_mu_N   = new TH1F("hcut_mu_N",   "hcut_mu_N",   100, 0, 100);
  hcut_mu_pid = new TH1F("hcut_mu_pid","hcut_mu_pid", 100, -10, 40);
  hcut_mu_pt  = new TH1F("hcut_mu_pt", "hcut_mu_pt",  100, 0, 10);
  hcut_mu_p   = new TH1F("hcut_mu_p",  "hcut_mu_p",   100, 0, 100);
  hcut_mu_lcs = new TH1F("hcut_mu_lcs","hcut_mu_lcs", 100, 0, 10);

}

/////////////////////////////////////////////
Tagger* TaggerMuonTool::tag(Event& event) {
  tmu->reset();

  if(msgLevel(MSG::VERBOSE)) verbose()<<"--Muon Tagger--"<<endreq;

  Particle* imuon=NULL;
  double ptmaxm=-999, ncand=0;
  Particles::iterator ipart;
  Particles vtags = event.particles();
  for (ipart=vtags.begin(); ipart!=vtags.end(); ++ipart) {

    Particle* axp = (*ipart);

    if(!checkPIDhypo(Particle::muon, axp)) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " Muon PIDm"<< axp->PIDm()<<endreq;

    double Pt = axp->pt();
    if( Pt < m_Pt_cut_muon ) continue;

    double P = axp->p();
    if( P  < m_P_cut_muon ) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " Muon P="<< P <<" Pt="<< Pt <<endreq;

    double lcs = axp->LCS();
    if( lcs > m_lcs_cut_muon) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " Muon lcs="<< lcs <<endreq;

    //calculate signed IP wrt RecVert
    double IPsig = axp->IPs();
    if(IPsig < m_IPs_cut_muon ) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " IPsig="<< IPsig <<endreq;

    //ip and phi wrt PU
    double ipPU  = axp->IPPU();
    if(msgLevel(MSG::VERBOSE)) verbose() << " ipPU="<< ipPU <<endreq;
    if(ipPU < m_ipPU_cut_muon ) continue;
    double distPhi = axp->distPhi();
    if (distPhi < m_distPhi_cut_muon) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " distPhi="<< distPhi <<endreq;

    ncand++;

    hcut_mu_ippu   ->Fill(axp->IPPU());
    hcut_mu_distphi->Fill(distPhi);
    hcut_mu_N      ->Fill(event.multiplicity());
    hcut_mu_pid    ->Fill(axp->PIDm());
    hcut_mu_pt     ->Fill(Pt);
    hcut_mu_p      ->Fill(P);
    hcut_mu_lcs    ->Fill(axp->LCS());

    if( Pt > ptmaxm ) { //Pt ordering
      imuon = (*ipart);
      ptmaxm = Pt;
    }

  }  
  if(!imuon) return tmu;

  //calculate omega
  vector<double> NNinputs(10); 
  NNinputs.at(0) = vtags.size();
  NNinputs.at(1) = event.signalB()->pt();
  NNinputs.at(2) = imuon->p();
  NNinputs.at(3) = imuon->pt();
  NNinputs.at(4) = imuon->IPs();
  NNinputs.at(8) = event.pileup();
  NNinputs.at(9) = ncand;

  double pn = nnet.MLPm( NNinputs );
  if(msgLevel(MSG::VERBOSE)) verbose() << " Muon pn="<< pn <<endreq;

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  pn = 1 - m_P0_Cal_muon - m_P1_Cal_muon * ( (1-pn)-m_Eta_Cal_muon);
  debug() << " Muon pn="<< pn <<" w="<<1-pn<<endreq;

  if( pn < m_ProbMin_muon ) return tmu;

  imuon->setID(-13*imuon->charge());

  tmu->setOmega( 1-pn );
  tmu->setRawNNetOutput( nnet.m_rnet_mu );
  tmu->setDecision(imuon->charge() > 0 ? -1: 1);
  tmu->setType( Tagger::OS_Muon ); 
  tmu->addToTaggerParts(imuon);

  return tmu;
}
