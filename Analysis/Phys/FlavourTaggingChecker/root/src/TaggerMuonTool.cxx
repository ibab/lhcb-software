#include "TaggerMuonTool.h"

TaggerMuonTool::TaggerMuonTool( ) {

  declareProperty( "Muon_Pt_cut",  m_Pt_cut_muon  = 1.1 *GeV );
  declareProperty( "Muon_P_cut",   m_P_cut_muon   = 0.0 *GeV );
  declareProperty( "Muon_lcs_cut", m_lcs_cut_muon = 5 );
  declareProperty( "Muon_PIDm_cut",m_PIDm_cut     = 2.0 );
  declareProperty( "ProbMin_muon", m_ProbMin_muon = 0. ); //no cut

  NNetTool_MLP nnet;
  tmu = new Tagger();
  hcut_mu_ippu= new TH1F("hcut_mu_ippu","hcut_mu_ippu",100, 0, 20);
  hcut_mu_N   = new TH1F("hcut_mu_N",   "hcut_mu_N",   100, 0, 100);
  hcut_mu_pid = new TH1F("hcut_mu_pid","hcut_mu_pid", 100, -10, 40);
  hcut_mu_pt  = new TH1F("hcut_mu_pt", "hcut_mu_pt",  100, 0, 10);
  hcut_mu_p   = new TH1F("hcut_mu_p",  "hcut_mu_p",   100, 0, 100);
  hcut_mu_lcs = new TH1F("hcut_mu_lcs","hcut_mu_lcs", 100, 0, 10);

}

/////////////////////////////////////////////
Tagger* TaggerMuonTool::tag(Event& event) {
  tmu->reset();

  Particle* imuon=NULL;
  double ptmaxm=-999, ncand=0;
  Particles::iterator ipart;
  Particles vtags = event.particles();
  for (ipart=vtags.begin(); ipart!=vtags.end(); ++ipart) {

    Particle* axp = (*ipart);

    if(!checkPIDhypo(Particle::muon, axp)) continue;
   
    double Pt = axp->pt();
    if( Pt < m_Pt_cut_muon ) continue;

    double P = axp->p();
    if( P  < m_P_cut_muon ) continue;

    if( axp->LCS() > m_lcs_cut_muon) continue;

    ncand++;

    debug()<<" Muon cand, Pt="<<Pt<<endmsg;
    hcut_mu_ippu->Fill(axp->IPPU());
    hcut_mu_N   ->Fill(event.multiplicity());
    hcut_mu_pid->Fill(axp->PIDm());
    hcut_mu_pt->Fill(Pt);
    hcut_mu_p->Fill(P);
    hcut_mu_lcs->Fill(axp->LCS());

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

  if( pn < m_ProbMin_muon ) return tmu;

  imuon->setID(-13*imuon->charge());

  tmu->setOmega( 1-pn );
  tmu->setRawNNetOutput( nnet.m_rnet_mu );
  tmu->setDecision(imuon->charge() > 0 ? -1: 1);
  tmu->setType( Tagger::OS_Muon ); 
  tmu->addToTaggerParts(imuon);

  return tmu;
}
