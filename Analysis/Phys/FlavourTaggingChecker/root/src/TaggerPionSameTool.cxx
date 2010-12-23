#include "TaggerPionSameTool.h"

TaggerPionSameTool::TaggerPionSameTool() {

  declareProperty( "PionSame_Pt_cut", m_Pt_cut_pionS  = 0.75 *GeV );
  declareProperty( "PionSame_P_cut",  m_P_cut_pionS   = 5.0 *GeV );
  declareProperty( "PionSame_IPs_cut",m_IPs_cut_pionS = 3.5 );
  declareProperty( "PionSame_LCS_cut",   m_lcs_cut       = 5.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS   = 2.5 *GeV);
  declareProperty( "PionSame_dQ_extra_cut", m_dQcut_extra_pionS = 1.5 *GeV);
  declareProperty( "PionSame_ghost_cut",  m_ghost_cut_pS     = -999.0);
  declareProperty( "PionSame_ipPU_cut", m_ipPU_cut_pS      = 3.0 );
  declareProperty( "PionSame_distPhi_cut", m_distPhi_cut_pS= 0.005 );
  declareProperty( "PionSame_PIDNoK_cut", m_PionSame_PIDNoK_cut = 3.0);
  declareProperty( "PionSame_PIDNoP_cut", m_PionSame_PIDNoP_cut = 10.0);
  declareProperty( "PionProbMin",     m_PionProbMin   = 0.53);
  declareProperty( "PionSame_P0_Cal",  m_P0_Cal_pionS   = 0.428 ); 
  declareProperty( "PionSame_P1_Cal",  m_P1_Cal_pionS   = 0.79 ); 
  declareProperty( "PionSame_Eta_Cal", m_Eta_Cal_pionS  = 0.411 ); 

  NNetTool_MLP nnet;
  tpionS = new Tagger();

  hcut_pS_ippu= new TH1F("hcut_pS_ippu","hcut_pS_ippu",100, 0, 20);
  hcut_pS_distphi= new TH1F("hcut_pS_distphi","hcut_pS_distphi",100, 0, 0.09);
  hcut_pS_pt  = new TH1F("hcut_pS_pt",  "hcut_pS_pt",  100, 0, 10);
  hcut_pS_p   = new TH1F("hcut_pS_p",   "hcut_pS_p",   100, 0, 100);
  hcut_pS_lcs = new TH1F("hcut_pS_lcs", "hcut_pS_lcs", 100, 0, 10);
  hcut_pS_IPs = new TH1F("hcut_pS_IPs", "hcut_pS_IPs", 100, 0, 20);
  hcut_pS_gho = new TH1F("hcut_pS_gho", "hcut_pS_gho", 100, -50, 0);
  hcut_pS_dq  = new TH1F("hcut_pS_dq",  "hcut_pS_dq",  100, 0, 3);
  hcut_pS_dqe = new TH1F("hcut_pS_dqe", "hcut_pS_dqe", 100, 0, 3);

}

///////////////////////////////////////////////
Tagger* TaggerPionSameTool::tag(Event& event) {
  tpionS->reset();

  verbose()<<"--Pion SS Tagger--"<<endreq;

  TLorentzVector ptotB = event.signalB()->momentum();
  double B0mass = ptotB.M();

  Particle* ipionS=NULL;
  double ptmaxpS = -99.0, ncand=0;
  Particles::iterator ipart, jpart;

  Particles vtags = event.particles();
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    if(!checkPIDhypo(Particle::pion, *ipart)) continue;
    verbose()<<" Pion PIDk="<< (*ipart)->PIDk() <<endreq;

    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_pionS )  continue;
    double P  = (*ipart)->p();
    if( P  < m_P_cut_pionS )  continue;
    verbose()<<" Pion P="<< P <<" Pt="<< Pt <<endreq;

    double lcs = (*ipart)->LCS();
    if( lcs > m_lcs_cut ) continue;
    if( (*ipart)->type() != Particle::Long && 
        (*ipart)->type() != Particle::Matched ) continue;

    double tsa = (*ipart)->likelihood();
    if(tsa < m_ghost_cut_pS) continue;
    verbose() << " Pion lcs="<< lcs <<" tsa="<<tsa<<endmsg;

    double IPsig = (*ipart)->IPs();
    if(IPsig > m_IPs_cut_pionS)  continue;

    double dQ = (ptotB+(*ipart)->momentum()).M() - B0mass;
    if(dQ > m_dQcut_pionS ) continue;
    verbose() << " Pion IPs="<< IPsig <<" dQ="<<dQ<<endmsg;

    //ip and phi wrt PU
    double ipPU  = (*ipart)->IPPU();
    if(ipPU < m_ipPU_cut_pS ) continue;
    double distPhi = (*ipart)->distPhi();
    if (distPhi < m_distPhi_cut_pS) continue;
  
    ncand++;
 
    hcut_pS_pt  ->Fill(Pt);
    hcut_pS_p   ->Fill(P);
    hcut_pS_lcs ->Fill((*ipart)->LCS());
    hcut_pS_IPs ->Fill(IPsig);
    hcut_pS_gho ->Fill(tsa);
    hcut_pS_dq  ->Fill(dQ);

    if( Pt <= ptmaxpS ) continue;//equal sign in "<=" is used to kill duplicates
 
    //accept candidate
    ipionS = (*ipart);
    ptmaxpS = Pt;
 
  }
  if( !ipionS ) return tpionS;

  double extra_dQ = (ptotB+ipionS->momentum()).M() - B0mass;
  if( extra_dQ > m_dQcut_extra_pionS ) return tpionS;
  verbose() << " Pion dQExtra="<< extra_dQ <<endmsg;

  hcut_pS_dqe ->Fill(extra_dQ);

  //calculate omega
  double B0the= ptotB.Theta();
  double B0phi= ptotB.Phi();
  double ang = asin((ipionS->pt())/(ipionS->p()));
  double deta= log(tan(B0the/2.))-log(tan(ang/2.));
  double dphi= std::min(fabs(ipionS->momentum().Phi()-B0phi), 
                        6.283-fabs(ipionS->momentum().Phi()-B0phi));
  double dQ  = ((ptotB+ ipionS->momentum() ).M() - B0mass);

  std::vector<double> NNinputs(10);
  NNinputs.at(0) = vtags.size();
  NNinputs.at(1) = event.signalB()->pt()/GeV;
  NNinputs.at(2) = ipionS->p()/GeV;
  NNinputs.at(3) = ipionS->pt()/GeV;
  NNinputs.at(4) = ipionS->IPs();
  NNinputs.at(5) = deta;
  NNinputs.at(6) = dphi;
  NNinputs.at(7) = dQ/GeV;
  NNinputs.at(8) = event.pileup();
  NNinputs.at(9) = ncand;

  double pn = nnet.MLPpS( NNinputs );
  verbose() << " Pion pn="<< pn <<endmsg;

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  pn = 1 - m_P0_Cal_pionS - m_P1_Cal_pionS * ( (1-pn)-m_Eta_Cal_pionS);
  debug() << " Pion pn="<< pn <<" w="<<1-pn<<endmsg;

  if( pn < m_PionProbMin ) return tpionS;

  int tagdecision = ipionS->charge()>0 ? 1: -1;
  if( event.isBu() ) tagdecision = -tagdecision; //is Bu

  tpionS->setDecision( tagdecision );
  tpionS->setRawNNetOutput( nnet.m_rnet_pS );
  tpionS->setOmega( 1-pn );
  tpionS->setType( Tagger::SS_Pion );
  tpionS->addToTaggerParts(ipionS);

  return tpionS;
}
