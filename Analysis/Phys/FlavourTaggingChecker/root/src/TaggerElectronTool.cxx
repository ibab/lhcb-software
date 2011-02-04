#include "TaggerElectronTool.h"

TaggerElectronTool::TaggerElectronTool() {

  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele    = 1000. );
  declareProperty( "Ele_P_cut",    m_P_cut_ele     = 0.0 );
  declareProperty( "Ele_lcs_cut",  m_lcs_cut_ele   = 5. );
  declareProperty( "Ele_IPs_cut",  m_IPs_cut_ele   = 2. );
  declareProperty( "Ele_ghost_cut",m_ghost_cut_ele = -999 );
  declareProperty( "VeloChargeMin",m_VeloChMin     = 0.0 );
  declareProperty( "VeloChargeMax",m_VeloChMax     = 1.6 );
  declareProperty( "EoverP",       m_EoverP        = 0.6 );
  declareProperty( "Ele_PIDe_cut", m_PIDe_cut      = 4. );
  declareProperty( "Ele_ipPU_cut", m_ipPU_cut_ele      = 3.0 );
  declareProperty( "Ele_distPhi_cut", m_distPhi_cut_ele= 0.03 );
  declareProperty( "ProbMin_ele",  m_ProbMin_ele   = 0. ); //no cut
  declareProperty( "Ele_P0_Cal",  m_P0_Cal_ele   = 0.291 ); 
  declareProperty( "Ele_P1_Cal",  m_P1_Cal_ele   = 1.47 ); 
  declareProperty( "Ele_Eta_Cal", m_Eta_Cal_ele  = 0.341 ); 

  NNetTool_MLP nnet;
  tele = new Tagger();
  hcut_ele_ippu= new TH1F("hcut_ele_ippu","hcut_ele_ippu",100, 0, 20);
  hcut_ele_distphi= new TH1F("hcut_ele_distphi","hcut_ele_distphi",100, 0, 0.09);
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

  if(msgLevel(MSG::VERBOSE)) verbose()<<"-- Electron Tagger --"<<endreq;

  Particle* iele=NULL;
  double ptmaxe=-999, ncand=0;
  Particles::iterator ipart;
  Particles vtags = event.particles();
  for (ipart=vtags.begin(); ipart!=vtags.end(); ++ipart) {

    Particle* axp = (*ipart);

    if(!checkPIDhypo(Particle::electron, axp)) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " Ele PIDe="<< axp->PIDe() <<endmsg;

    double Pt = axp->pt();
    if( Pt < m_Pt_cut_ele*0.001 )  continue;//to have GeV

    double P = axp->p();
    if( P < m_P_cut_ele*0.001 )  continue;//to have GeV
    if(msgLevel(MSG::VERBOSE)) verbose() << " Ele P="<< P <<" Pt="<<Pt<<endmsg;

    if( axp->LCS() > m_lcs_cut_ele ) continue;

    if( axp->type() != Particle::Long && 
        axp->type() != Particle::Matched && //old FTC long and matched are trtyop=7
        axp->type() != Particle::Upstream ) continue;

    double tsa = axp->likelihood();
    if(tsa < m_ghost_cut_ele) continue;

    if(msgLevel(MSG::VERBOSE)) verbose() << " Ele lcs="<< axp->LCS() <<" tsa="<<tsa<<endmsg;
    //calculate signed IP wrt RecVert
    double IPsig = axp->IPs();
    if(IPsig < m_IPs_cut_ele ) continue;

    //ip and phi wrt PU
    double ipPU  = axp->IPPU();
    if(ipPU < m_ipPU_cut_ele ) continue;
    double distPhi = axp->distPhi();
    if (distPhi < m_distPhi_cut_ele) continue;

    double eOverP = axp->eOverP();
    if(eOverP > m_EoverP || eOverP<-100) {
      if(msgLevel(MSG::VERBOSE)) verbose() << " Elec E/P=" << eOverP <<endreq;

      double veloch = axp->VeloCharge();
      if( veloch > m_VeloChMin && veloch < m_VeloChMax ) {
        if(msgLevel(MSG::VERBOSE)) verbose() << " Elec veloch=" << veloch << endreq;

        ncand++;

        hcut_ele_pidk   ->Fill(axp->PIDe());
        hcut_ele_pt     ->Fill(Pt);
        hcut_ele_p      ->Fill(P);
        hcut_ele_lcs    ->Fill(axp->LCS());
        hcut_ele_gho    ->Fill(tsa);
        hcut_ele_EoP    ->Fill(eOverP);
        hcut_ele_velo   ->Fill(veloch);
        hcut_ele_ippu   ->Fill(axp->IPPU());
        hcut_ele_distphi->Fill(distPhi);
        hcut_ele_N      ->Fill(event.multiplicity());
        hcut_ele_ips    ->Fill(IPsig);

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
  if(msgLevel(MSG::VERBOSE)) verbose() << " Elec pn=" << pn << endreq;

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  pn = 1 - m_P0_Cal_ele - m_P1_Cal_ele * ( (1-pn)-m_Eta_Cal_ele);
  debug() << " Elec pn=" << pn <<" w="<<1-pn<< endreq;

  if( pn < m_ProbMin_ele ) return tele;

  iele->setID(-11*iele->charge());

  tele->setOmega( 1-pn );
  tele->setRawNNetOutput( nnet.m_rnet_ele );
  tele->setDecision(iele->charge()>0 ? -1: 1);
  tele->addToTaggerParts(iele);
  tele->setType( Tagger::OS_Electron );

  return tele;
}
