// Include files 
#include "TaggerKaonSameTool.h"

#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerKaonSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerKaonSameTool );

//====================================================================
TaggerKaonSameTool::TaggerKaonSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName   = "NNetTool_MLP" );
  declareProperty( "KaonSame_AverageOmega",    m_AverageOmega   = 0.33 );
  
  declareProperty( "KaonSame_Pt_cut", m_Pt_cut_kaonS = 0.85 *GeV );
  declareProperty( "KaonSame_P_cut",  m_P_cut_kaonS  = 5.25 *GeV );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS = 4.125 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS = 0.825 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS = 0.6 );
  declareProperty( "KaonSame_dR_cut", m_dRcut_kaonS= 10. );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS  = 1.850 *GeV);
  declareProperty( "KaonSame_lcs_cut",   m_lcs_cut      = 3.75 );
  declareProperty( "KaonSame_ipPU_cut", m_ipPU_cut_kS      = 3.0 );
  declareProperty( "KaonSame_distPhi_cut", m_distPhi_cut_kS= 0.005 );
  declareProperty( "KaonSame_PIDk_cut", m_KaonSPID_kS_cut  =  3.5 );
  declareProperty( "KaonSame_PIDkp_cut",m_KaonSPID_kpS_cut = -8.5 );

  declareProperty( "KaonSame_ProbMin",   m_ProbMin_kaonS    = 0. ); //no cut
  // 1fb-1 paper tuning
  declareProperty( "KaonSame_P0_Cal",  m_P0_Cal_kaonS   = 0.350 );
  declareProperty( "KaonSame_P1_Cal",  m_P1_Cal_kaonS   = 0.51 );
  declareProperty( "KaonSame_Eta_Cal", m_Eta_Cal_kaonS  = 0.324 );

  m_nnet = 0;
  m_util = 0;
  m_descend = 0;
}
TaggerKaonSameTool::~TaggerKaonSameTool() {}; 

//=====================================================================
StatusCode TaggerKaonSameTool::initialize() { 
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  warning() << "KSS calib ctt: P0_Cal "<<m_P0_Cal_kaonS<<", P1_Cal "<<m_P1_Cal_kaonS<<endreq;

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  } 
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerKaonSameTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                                std::vector<const Vertex*>& allVtx, 
                                Particle::ConstVector& vtags ){
  Tagger tkaonS;
  if(!RecVert) return tkaonS;

  verbose()<<"--Kaon SS Tagger--"<<endreq;

  Gaudi::LorentzVector ptotB = AXB0->momentum();

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaonS=0;
  double ptmaxkS = -99.0, ncand=0;
  double save_dphi=0, save_dQ=0, save_IPsig=0;

  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );
    double pidp=(*ipart)->proto()->info( ProtoParticle::CombDLLp, -1000.0 );

    //verbose()<<"  KaonS PIDk="<<pidk<<" PIDp="<<pidp <<" pidk-p: "<<pidk-pidp<<endreq;
    if(pidk==0) continue;
    if(pidk < m_KaonSPID_kS_cut ) continue;
    if(pidk - pidp < m_KaonSPID_kpS_cut ) continue;
    verbose()<<"     KaonS PIDk="<<pidk<<" PIDp="<<pidp <<endreq;

    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_kaonS )  continue;

    double P  = (*ipart)->p();
    if( P < m_P_cut_kaonS )  continue;
    verbose()<<" KaonS P="<<P<<" Pt="<<Pt <<endreq;

    //double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    //if (cloneDist!=-1) continue;

    const Track* track = (*ipart)->proto()->track();
    double lcs = track->chi2PerDoF();
    verbose()<<" KaonS lcs1="<<lcs<<endreq;
    if( lcs > m_lcs_cut ) continue;
    verbose()<<" KaonS lcs2="<<lcs<<endreq;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = IP/IPerr;
    if(fabs(IPsig) > m_IP_cut_kaonS) continue;
    verbose()<<" KaonS IPs="<<IPsig<<endreq;

    double ippu=(*ipart)->info(1,100000.);
    if(ippu < m_ipPU_cut_kS) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    debug()<<" distPhi="<<distphi<<endreq;
    if( distphi < m_distPhi_cut_kS ) continue;

    double deta  = fabs(log(tan(ptotB.Theta()/2.)/tan(asin(Pt/P)/2.)));
    double dphi = fabs(TaggingHelpers::dphi(
		(*ipart)->momentum().Phi(), ptotB.Phi()));
    double dR = sqrt(deta*deta+dphi*dphi);

    if(deta > m_etacut_kaonS) continue;
    if(dphi > m_phicut_kaonS) continue;
    if(dR > m_dRcut_kaonS) continue;

    Gaudi::LorentzVector pm  = (*ipart)->momentum();
    double E = sqrt(pm.P() * pm.P()+ 493.677*MeV * 493.677*MeV);
    Gaudi::LorentzVector pmK ( pm.Px(),pm.Py(),pm.Pz(), E);

    double dQ    = (ptotB+pmK).M() - ptotB.M();
    verbose()<<" KaonS Deta="<<deta<<" KaonS dQ="<<dQ
             <<" Dphi="<<dphi<<endreq;

    if(dQ > m_dQcut_kaonS ) continue;

    ncand++;

    if( Pt > ptmaxkS ) { 
      ikaonS  = (*ipart);
      ptmaxkS = Pt;
      save_dphi=dphi;
      save_dQ=dQ;
      save_IPsig=IPsig;
    }

  } 
  if( !ikaonS ) return tkaonS;

  //calculate omega
  double pn = 1-m_AverageOmega; 
  if(m_CombinationTechnique == "NNet") {

    double ang = asin(ikaonS->pt()/ikaonS->p());
    double deta= log(tan(ptotB.Theta()/2))-log(tan(ang/2));
    double dphi = fabs(TaggingHelpers::dphi(
		ikaonS->momentum().Phi(), ptotB.Phi()));
    double dR = sqrt(deta*deta+dphi*dphi);

    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;
    //    NNinputs.at(2) = ikaonS->p()/GeV;
    NNinputs.at(3) = ikaonS->pt()/GeV;
    NNinputs.at(4) = save_IPsig;
    NNinputs.at(5) = deta;
    NNinputs.at(6) = save_dphi;
    NNinputs.at(7) = save_dQ/GeV;
    NNinputs.at(8) = allVtx.size();
    NNinputs.at(9) = dR;

    pn = m_nnet->MLPkS( NNinputs );
    verbose()<<" KaonS pn inputs="<<NNinputs<<endreq;
    verbose()<<" KaonS pn ="<<pn<<endreq;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_kaonS - m_P1_Cal_kaonS * ( (1-pn)-m_Eta_Cal_kaonS);
    debug() << " SS Kaon pn="<< pn <<" w="<<1-pn<<endmsg;

    if( pn < 0 || pn > 1 ) return tkaonS;
    if( pn < m_ProbMin_kaonS ) return tkaonS;

  }

  tkaonS.setOmega( 1-pn );
  tkaonS.setDecision(ikaonS->charge()>0 ? 1: -1);
  tkaonS.setType( Tagger::SS_Kaon ); 
  tkaonS.addToTaggerParts(ikaonS);

  return tkaonS;
}
//==========================================================================
StatusCode TaggerKaonSameTool::finalize() { return GaudiTool::finalize(); }

