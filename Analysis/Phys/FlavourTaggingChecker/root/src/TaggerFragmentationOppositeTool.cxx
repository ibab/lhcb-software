#include "TaggerFragmentationOppositeTool.h"
#include "NNetTool_MLP.h"

TaggerFragmentationOppositeTool::TaggerFragmentationOppositeTool() {

  declareProperty( "Fragment_Pt_cut",   m_Pt_cut_frag        = 0.4 *GeV );
  declareProperty( "Fragment_P_cut",    m_P_cut_frag         = 3.0 *GeV );
  declareProperty( "Fragment_IPs_cut",  m_IPs_cut_frag       = 2 );
  declareProperty( "Fragment_IP_cut",   m_IP_cut_frag        = 1.5 );
  declareProperty( "Fragment_LCS_cut",  m_lcs_frag           = 5 );
  declareProperty( "Fragment_ghost_cut", m_ghost_cut         = -20.0 );

  declareProperty( "Fragment_PIDk",  m_PID_k_cut =  0.0);
  declareProperty( "Fragment_PIDkp", m_PIDkp_cut = -1.0 );

  declareProperty( "Fragment_LongTrack_IP_cut",     m_IP_kl  = 999.0 ); //no cut
  declareProperty( "Fragment_upstreamTrack_IP_cut", m_IP_ku  = 999.0 ); //no cut

  declareProperty( "ProbMin_frag", m_ProbMin_frag  = 0. ); //no cut

  //m_svtool= new SVertexOneSeedTool();

  NNetTool_MLP nnet;
  tfrag = new Tagger();
  hcut_frag_pidk= new TH1F("hcut_frag_pidk","hcut_frag_pidk",100, -10, 90);
  hcut_frag_pidp= new TH1F("hcut_frag_pidp","hcut_frag_pidp",100, -10, 90);
  hcut_frag_pt  = new TH1F("hcut_frag_pt", "hcut_frag_pt",  100, 0, 10);
  hcut_frag_p   = new TH1F("hcut_frag_p",  "hcut_frag_p",   100, 0, 100);
  hcut_frag_lcs = new TH1F("hcut_frag_lcs","hcut_frag_lcs", 100, 0, 10);
  hcut_frag_gho = new TH1F("hcut_frag_gho","hcut_frag_gho", 100, -50, 0);
  hcut_frag_IPs = new TH1F("hcut_frag_IPs","hcut_frag_IPs", 100, 0, 20);
  hcut_frag_IP  = new TH1F("hcut_frag_IP ","hcut_frag_IP",  100, 0, 100);

}

////////////////////////////////////////////////////////////
Tagger* TaggerFragmentationOppositeTool::tag(Event& event) {
  tfrag->reset();

  return tfrag;

  if(event.oppositeB()->absID()!= 531) return tfrag;
  TLorentzVector ptotB = event.oppositeB()->momentum(); //cheating

  Particle* ifrag=NULL;
  double ptmaxk=-999, ncand=0;
  Particles::iterator ipart;
  Particles vtags = event.particles();
  for (ipart=vtags.begin(); ipart!=vtags.end(); ++ipart) {

    Particle* axp = (*ipart);
    double pidk= axp->PIDk();
 
    if(pidk < m_PID_k_cut ) continue;
    if(pidk==0 || pidk==-1000.0) continue;
 
    double pidproton = axp->PIDp();
    if( pidk - pidproton < m_PIDkp_cut ) continue;
   
    double Pt = axp->pt();
    if( Pt < m_Pt_cut_frag )  continue;

    double P = axp->p();
    if( P < m_P_cut_frag )  continue;

    //calculate signed IP wrt RecVert
    double IPsig = axp->IPs();
    if(IPsig > m_IPs_cut_frag ) continue;

    double IP    = axp->IP();
    if(fabs(IP) > m_IP_cut_frag ) continue;

    double dphi  = fabs(axp->momentum().Phi() - ptotB.Phi()); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    if(dphi > 1) continue;

    ncand++;
    
    hcut_frag_pidk->Fill(pidk);
    hcut_frag_pidp->Fill(pidproton);
    hcut_frag_pt  ->Fill(Pt);
    hcut_frag_p   ->Fill(P);
    hcut_frag_IPs ->Fill(IPsig);
    hcut_frag_IP  ->Fill(IP);

    if( Pt > ptmaxk ) { 
      ifrag = (*ipart);
      ptmaxk = Pt;
    }

  }
  
  if( ! ifrag ) return tfrag;


  //calculate omega
  std::vector<double> NNinputs(10); 
  NNinputs.at(0) = vtags.size();
  NNinputs.at(1) = event.signalB()->pt();
  NNinputs.at(2) = ifrag->p()/GeV;
  NNinputs.at(3) = ifrag->pt()/GeV;
  NNinputs.at(4) = ifrag->IPs();
  NNinputs.at(8) = event.pileup();
  NNinputs.at(9) = ncand;

  double pn = nnet.MLPk( NNinputs );

  if( pn < m_ProbMin_frag ) return tfrag;

  ifrag->setID(321*ifrag->charge());

  tfrag->setOmega( 1-pn );
  tfrag->setRawNNetOutput( nnet.m_rnet_k );
  tfrag->setDecision(ifrag->charge()>0 ? -1: 1);
  tfrag->setType( Tagger::Topology ); 
  tfrag->addToTaggerParts(ifrag);

  return tfrag;
}
