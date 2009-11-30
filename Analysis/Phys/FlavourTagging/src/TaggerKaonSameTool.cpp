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
  declareProperty( "KaonSPID_extracut", m_KaonSPID_extracut = 5.0 );

  declareProperty( "AverageOmega",    m_AverageOmega = 0.356 );

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
  double B0mass= ptotB.M();
  double B0the = ptotB.Theta();
  double B0phi = ptotB.Phi();

  //select kaonS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaonS=0;
  double ptmaxkS = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 321 ) continue;

    if((*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 )
       - (*ipart)->proto()->info( ProtoParticle::CombDLLp, -1000.0 ) 
       < m_KaonSPID_extracut ) continue;

    double Pt = (*ipart)->pt();
    double P  = (*ipart)->p();
    if( Pt < m_Pt_cut_kaonS )  continue;
    if( P < m_P_cut_kaonS )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);

    if(IPsig < m_IP_cut_kaonS) {
      double deta  = fabs(log(tan(B0the/2.)/tan(asin(Pt/P)/2.)));
      double dphi  = fabs((*ipart)->momentum().Phi() - B0phi); 
      if(dphi>3.1416) dphi=6.2832-dphi;
      double dQ    = (ptotB+(*ipart)->momentum()).M() - B0mass;

      if(dphi > m_phicut_kaonS) continue;
      if(deta > m_etacut_kaonS) continue;
      if(dQ   > m_dQcut_kaonS ) continue;

      const Track* track = (*ipart)->proto()->track();
      double lcs = track->chi2PerDoF();
      if( lcs > m_lcs_cut ) continue;

      ncand++;
      if( Pt > ptmaxkS ) { 
        ikaonS  = (*ipart);
        ptmaxkS = Pt;
        verbose()<< " KaoS P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
                 << " deta="<<deta << " dphi="<<dphi << " dQ="<<dQ <<endreq;
      }
    }
  } 
  if( !ikaonS ) return tkaonS;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    double IP, IPerr;
    double B0the= ptotB.Theta();
    double B0phi= ptotB.Phi();
    double ang = asin((ikaonS->pt()/GeV)/(ikaonS->p()/GeV));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= std::min(fabs(ikaonS->momentum().Phi()-B0phi), 
                          6.283-fabs(ikaonS->momentum().Phi()-B0phi));
    double dQ  = ((ptotB+ikaonS->momentum()).M() - B0mass)/GeV;
    m_util->calcIP(ikaonS, RecVert, IP, IPerr);

    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = ikaonS->p()/GeV;
    NNinputs.at(3) = ikaonS->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
    NNinputs.at(5) = deta;
    NNinputs.at(6) = dphi;
    NNinputs.at(7) = dQ;
//    NNinputs.at(8) = m_util->getNvtx();
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

