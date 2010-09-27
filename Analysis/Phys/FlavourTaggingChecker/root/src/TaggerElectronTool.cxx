#include "TaggerElectronTool.h"

TaggerElectronTool::TaggerElectronTool() {

  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele = 1.1 * GeV );
  declareProperty( "Ele_P_cut",    m_P_cut_ele  = 0.0 * GeV );
  declareProperty( "Ele_lcs_cut",  m_lcs_cut_ele   = 5 );
  declareProperty( "Ele_ghost_cut",m_ghost_cut_ele = -999.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin  = 0.0 );
  declareProperty( "VeloChargeMax",m_VeloChMax  = 1.3 );
  declareProperty( "EoverP",       m_EoverP     = 0.85 );
  declareProperty( "Ele_PIDe_cut", m_PIDe_cut   = 4.0 );
  declareProperty( "ProbMin_ele",  m_ProbMin_ele = 0. ); //no cut

  NNetTool_MLP nnet;
  tele = new Tagger();
  hcut_ele_ippu= new TH1F("hcut_ele_ippu","hcut_ele_ippu",100, 0, 20);
  hcut_ele_N   = new TH1F("hcut_ele_N",   "hcut_ele_N",100, 0, 100);
  hcut_ele_ips = new TH1F("hcut_ele_ips", "hcut_ele_ips",100, 0, 20);
  hcut_ele_pidk= new TH1F("hcut_ele_pidk","hcut_ele_pidk",100, -10, 50);
  hcut_ele_pt  = new TH1F("hcut_ele_pt",  "hcut_ele_pt",  100, 0, 5);
  hcut_ele_p   = new TH1F("hcut_ele_p",   "hcut_ele_p",   100, 0, 100);
  hcut_ele_lcs = new TH1F("hcut_ele_lcs", "hcut_ele_lcs", 100, 0, 5);
  hcut_ele_gho = new TH1F("hcut_ele_gho", "hcut_ele_gho", 100, -40, 0);
  hcut_ele_EoP = new TH1F("hcut_ele_EoP", "hcut_ele_EoP", 100,  0, 2);
  hcut_ele_velo= new TH1F("hcut_ele_velo","hcut_ele_velo",100, 0, 2);

}

////////////////////////////////////////////////
Tagger* TaggerElectronTool::tag(Event& event) {
  tele->reset();

  Particle* iele=NULL;
  double ptmaxe=-999, ncand=0;
  Particles::iterator ipart;
  Particles vtags = event.particles();
  for (ipart=vtags.begin(); ipart!=vtags.end(); ++ipart) {

    if(!checkPIDhypo(Particle::electron, *ipart)) continue;

    Particle* axp = (*ipart);

    double Pt = axp->pt();
    if( Pt < m_Pt_cut_ele )  continue;

    double P = axp->p();
    if( P < m_P_cut_ele )  continue;

    if( axp->LCS() > m_lcs_cut_ele ) continue;

    if( axp->type() != Particle::Long && 
        axp->type() != Particle::Matched && 
        axp->type() != Particle::Upstream ) continue;

    double tsa = axp->likelihood();
    if(tsa < m_ghost_cut_ele) continue;

    double eOverP = axp->eOverP();
    if(eOverP > m_EoverP || eOverP<-100) {
  
      double veloch = axp->VeloCharge();

      if( veloch > m_VeloChMin && veloch < m_VeloChMax ) {
        
        ncand++;

        verbose()<<" Electron cand, Pt="<<Pt<<endmsg;
        hcut_ele_pidk->Fill(axp->PIDe());
        hcut_ele_pt  ->Fill(Pt);
        hcut_ele_p   ->Fill(P);
        hcut_ele_lcs ->Fill(axp->LCS());
        hcut_ele_gho ->Fill(tsa);
        hcut_ele_EoP ->Fill(eOverP);
        hcut_ele_velo->Fill(veloch);
        hcut_ele_ippu->Fill(axp->IPPU());
        hcut_ele_N   ->Fill(event.multiplicity());
        hcut_ele_ips ->Fill(axp->IPs());

        if( Pt > ptmaxe ) { 
          iele = (*ipart);
          ptmaxe = Pt;
        }
      }
    }
  }
  if(!iele) return tele;

  //calculate omega
  std::vector<double> NNinputs(10);
  NNinputs.at(0) = vtags.size();
  NNinputs.at(1) = event.signalB()->pt();
  NNinputs.at(2) = iele->p()/GeV;
  NNinputs.at(3) = iele->pt()/GeV;
  NNinputs.at(4) = iele->IPs();
  NNinputs.at(8) = event.pileup();
  NNinputs.at(9) = ncand;

  double pn = nnet.MLPe( NNinputs );

  if( pn < m_ProbMin_ele ) return tele;

  iele->setID(-11*iele->charge());

  tele->setOmega( 1-pn );
  tele->setRawNNetOutput( nnet.m_rnet_ele );
  tele->setDecision(iele->charge()>0 ? -1: 1);
  tele->addToTaggerParts(iele);
  tele->setType( Tagger::OS_Electron );

  return tele;
}
