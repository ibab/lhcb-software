// Include files 
#include "TaggerKaonSameTool.h"

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
  declareProperty( "KaonSame_Pt_cut", m_Pt_cut_kaonS = 0.45 *GeV );
  declareProperty( "KaonSame_P_cut",  m_P_cut_kaonS  = 4.0 *GeV );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS = 3.0 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS = 1.1 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS = 1.0 );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS  = 1.6 *GeV);
  declareProperty( "KaonS_LCS_cut",   m_lcs_cut      = 2.0 );

  declareProperty( "KaonSPID_kS_cut",  m_KaonSPID_kS_cut   =  1.0 );
  declareProperty( "KaonSPID_kpS_cut", m_KaonSPID_kpS_cut  = -1.0 );

  declareProperty( "AverageOmega",    m_AverageOmega = 0.33 );

  m_nnet = 0;
  m_util = 0;
}
TaggerKaonSameTool::~TaggerKaonSameTool() {}; 

//=====================================================================
StatusCode TaggerKaonSameTool::initialize() { 

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

  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerKaonSameTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                                std::vector<const Vertex*>& allVtx, 
                                Particle::ConstVector& vtags ){
  Tagger tkaonS;
  if(!RecVert) return tkaonS;

  verbose()<<"allVtx.size()="<< allVtx.size() << endreq;

  Gaudi::LorentzVector ptotB = AXB0->momentum();

  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaonS=0;
  double ptmaxkS = -99.0, ncand=0;
  double save_dphi=0, save_dQ=0, save_IPsig=0;

  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    if(! (*ipart)->proto()->info(ProtoParticle::InAccEcal, false) ) continue;
    double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );
    double pidp=(*ipart)->proto()->info( ProtoParticle::CombDLLp, -1000.0 ); 

    debug()<<(*ipart)->particleID().abspid()<<" candidate kaonS p="<<(*ipart)->p()/GeV
	   <<"  PIDk="<<pidk<<"  PIDp="<<pidp <<endreq;

    if(pidk==0) continue;
    if(pidk < m_KaonSPID_kS_cut ) continue;
    if(pidk - pidp < m_KaonSPID_kpS_cut ) continue;

    debug()<<"           candidate accepted"<<(*ipart)->p()/GeV <<endreq;


    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_kaonS )  continue;

    double P  = (*ipart)->p();
    if( P < m_P_cut_kaonS )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = IP/IPerr;
    if(fabs(IPsig) > m_IP_cut_kaonS) continue;

    double deta  = fabs(log(tan(ptotB.Theta()/2.)/tan(asin(Pt/P)/2.)));
    if(deta > m_etacut_kaonS) continue;

    double dphi  = fabs((*ipart)->momentum().Phi() - ptotB.Phi()); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    if(dphi > m_phicut_kaonS) continue;

    Gaudi::LorentzVector pm  = (*ipart)->momentum();
    double E = sqrt(pm.P() * pm.P()+ 493.677*MeV * 493.677*MeV);
    Gaudi::LorentzVector pmK ( pm.Px(),pm.Py(),pm.Pz(), E);

    double dQ    = (ptotB+pmK).M() - ptotB.M();

    debug()<<"kS dQ="<<dQ<<"  "<<((ptotB+(*ipart)->momentum()).M())
	   <<"  "<< ptotB.M()<<endreq;

    if(dQ > m_dQcut_kaonS ) continue;

    const Track* track = (*ipart)->proto()->track();
    double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;

    ncand++;
    if( Pt > ptmaxkS ) { 
      ikaonS  = (*ipart);
      ptmaxkS = Pt;
      save_dphi=dphi;
      save_dQ=dQ;
      save_IPsig=IPsig;
      debug()<< " KaoS P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
	     << " deta="<<deta << " dphi="<<dphi << " dQ="<<dQ <<endreq;
    }

  } 
  if( !ikaonS ) return tkaonS;

  //calculate omega
  double pn = 1-m_AverageOmega; 
  if(m_CombinationTechnique == "NNet") {

    double ang = asin(ikaonS->pt()/ikaonS->p());
    double deta= log(tan(ptotB.Theta()/2))-log(tan(ang/2));

    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;
    NNinputs.at(2) = ikaonS->p()/GeV;
    NNinputs.at(3) = ikaonS->pt()/GeV;
    NNinputs.at(4) = save_IPsig;
    NNinputs.at(5) = deta;
    NNinputs.at(6) = save_dphi;
    NNinputs.at(7) = save_dQ/GeV;
    NNinputs.at(8) = allVtx.size();
    NNinputs.at(9) = ncand;

    pn = m_nnet->MLPkS( NNinputs );

  }

  tkaonS.setOmega( 1-pn );
  tkaonS.setDecision(ikaonS->charge()>0 ? 1: -1);
  tkaonS.setType( Tagger::SS_Kaon ); 
  tkaonS.addToTaggerParts(ikaonS);

  return tkaonS;
}
//==========================================================================
StatusCode TaggerKaonSameTool::finalize() { return StatusCode::SUCCESS; }

