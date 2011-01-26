#include "TaggerKaonOppositeTool.h"

TaggerKaonOppositeTool::TaggerKaonOppositeTool() {

  declareProperty( "Kaon_Pt_cut",   m_Pt_cut_kaon   = 800  );
  declareProperty( "Kaon_P_cut",    m_P_cut_kaon    = 5875 );
  declareProperty( "Kaon_IPs_cut",  m_IPs_cut_kaon  = 4.05 );
  declareProperty( "Kaon_IP_cut",   m_IP_cut_kaon   = 1.25 );
  declareProperty( "Kaon_LCS_cut",  m_lcs_kaon      = 2.125 );
  declareProperty( "Kaon_PIDk",     m_PID_k_cut     =  6.5);
  declareProperty( "Kaon_PIDkp",    m_PIDkp_cut     = -3.5 );
  declareProperty( "Kaon_ghost_cut",m_ghost_cut_kaon     = -999.0 );
  declareProperty( "Kaon_ipPU_cut", m_ipPU_cut_kaon      = 4.72 );
  declareProperty( "Kaon_distPhi_cut", m_distPhi_cut_kaon= -0.005 );
  declareProperty( "ProbMin_kaon",  m_ProbMin_kaon  = 0.51 ); //no cut
  declareProperty( "Kaon_P0_Cal",  m_P0_Cal_kaon   = 0.359 ); 
  declareProperty( "Kaon_P1_Cal",  m_P1_Cal_kaon   = 0.89 ); 
  declareProperty( "Kaon_Eta_Cal", m_Eta_Cal_kaon  = 0.359 ); 

  NNetTool_MLP nnet;
  tkaon = new Tagger();
  hcut_ko_ippu= new TH1F("hcut_ko_ippu","hcut_ko_ippu",100, 0, 20);
  hcut_ko_distphi= new TH1F("hcut_ko_distphi","hcut_ko_distphi",100, 0, 0.09);
  hcut_ko_N   = new TH1F("hcut_ko_N",   "hcut_ko_N"   ,100, 0, 100);
  hcut_ko_pidk= new TH1F("hcut_ko_pidk","hcut_ko_pidk",100, -10, 90);
  hcut_ko_pidp= new TH1F("hcut_ko_pidp","hcut_ko_pidp",100, -10, 90);
  hcut_ko_pt  = new TH1F("hcut_ko_pt", "hcut_ko_pt",  100, 0, 8);
  hcut_ko_p   = new TH1F("hcut_ko_p",  "hcut_ko_p",   100, 0, 100);
  hcut_ko_lcs = new TH1F("hcut_ko_lcs","hcut_ko_lcs", 100, 0, 5);
  hcut_ko_gho = new TH1F("hcut_ko_gho","hcut_ko_gho", 100, -40, 0);
  hcut_ko_IPs = new TH1F("hcut_ko_IPs","hcut_ko_IPs", 100, 0, 20);
  hcut_ko_IP  = new TH1F("hcut_ko_IP ","hcut_ko_IP",  100, 0, 5);

}

///////////////////////////////////////////////////
Tagger* TaggerKaonOppositeTool::tag(Event& event) {
  tkaon->reset();

  if(msgLevel(MSG::VERBOSE)) verbose() << "--Kaon Oppsite Tagger--"<<endmsg;

  Particle* ikaon=NULL;
  double ptmaxk=-999, ncand=0;
  Particles::iterator ipart;
  Particles vtags = event.particles();
  for (ipart=vtags.begin(); ipart!=vtags.end(); ++ipart) {

    Particle* axp = (*ipart);
    double pidk= axp->PIDk();
    double pidproton = axp->PIDp();

    if(!checkPIDhypo(Particle::kaon_opposite, axp)) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " Kaon PID pass"<<endreq;

    double Pt = axp->pt();
    if( Pt < m_Pt_cut_kaon*0.001 )  continue;//GeV
 
    double P = axp->p();
    if( P < m_P_cut_kaon*0.001 )  continue;//GeV
 
    if(msgLevel(MSG::VERBOSE)) verbose() << " Kaon P="<< P <<" Pt="<<Pt<<endmsg;
    double lcs = axp->LCS();
    if(lcs > m_lcs_kaon) continue;
 
    double tsa = axp->likelihood();
    if( tsa < m_ghost_cut_kaon ) continue;
    if(msgLevel(MSG::VERBOSE)) verbose() << " Kaon lcs="<< lcs <<" tsa="<<tsa<<endmsg;

    //calculate signed IP wrt RecVert
    double IPsig = axp->IPs();
    double IP    = axp->IP();
    double ipPU  = axp->IPPU();
    if(msgLevel(MSG::VERBOSE)) verbose() << " Kaon IPs="<< IPsig <<" IP="<<fabs(IP)<<" ipPU="<<ipPU<<endmsg;
    if(IPsig < m_IPs_cut_kaon ) continue;
    if(fabs(IP) > m_IP_cut_kaon ) continue;
    if(ipPU < m_ipPU_cut_kaon ) continue;

    double distPhi = axp->distPhi();
    if (distPhi < m_distPhi_cut_kaon) continue;

    ncand++;
    
    hcut_ko_ippu->Fill(ipPU);
    hcut_ko_N   ->Fill(event.multiplicity());
    hcut_ko_pidk->Fill(pidk);
    hcut_ko_pidp->Fill(pidproton);
    hcut_ko_pt  ->Fill(Pt);
    hcut_ko_p   ->Fill(P);
    hcut_ko_lcs ->Fill(lcs);
    hcut_ko_gho ->Fill(tsa);
    hcut_ko_IPs ->Fill(IPsig);
    hcut_ko_IP  ->Fill(IP);
    hcut_ko_distphi->Fill(distPhi);

    if( Pt > ptmaxk ) { 
      ikaon = (*ipart);
      ptmaxk = Pt;
    }
  }

  if( ! ikaon ) return tkaon;

  //calculate omega
  std::vector<double> NNinputs(10); 
  NNinputs.at(0) = vtags.size();
  NNinputs.at(1) = event.signalB()->pt();
  NNinputs.at(2) = ikaon->p()/GeV;
  NNinputs.at(3) = ikaon->pt()/GeV;
  NNinputs.at(4) = ikaon->IPs();
  NNinputs.at(8) = event.pileup();
  NNinputs.at(9) = ncand;

  double pn = nnet.MLPk( NNinputs );
  if(msgLevel(MSG::DEBUG)) debug() << " Kaon pn="<< pn<<endmsg;

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  pn = 1 - m_P0_Cal_kaon - m_P1_Cal_kaon * ( (1-pn)-m_Eta_Cal_kaon);
  debug() << " Kaon pn="<< pn<<" w="<<1-pn<<endmsg;

  if( pn < m_ProbMin_kaon ) return tkaon;

  ikaon->setID(321*ikaon->charge());

  tkaon->setOmega( 1-pn );
  tkaon->setRawNNetOutput( nnet.m_rnet_k );
  tkaon->setDecision(ikaon->charge()>0 ? -1: 1);
  tkaon->setType( Tagger::OS_Kaon ); 
  tkaon->addToTaggerParts(ikaon);

  return tkaon;
}
