// Include files 
#include "TaggerPionSameTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerPionSameTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerPionSameTool );

//====================================================================
TaggerPionSameTool::TaggerPionSameTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "CombTech", m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_MLP" );
  declareProperty( "ProbMin", m_ProbMin               = 0.55);
  declareProperty( "PionSame_Pt_cut", m_Pt_cut_pionS  = 0.2 *GeV );
  declareProperty( "PionSame_P_cut",  m_P_cut_pionS   = 2.0 *GeV );
  declareProperty( "PionSame_IPs_cut",m_IPs_cut_pionS = 3.0 );
  declareProperty( "PionS_LCS_cut",   m_lcs_cut       = 3.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS   = 3.0 *GeV);
  declareProperty( "PionSame_dQ_extra_cut", m_dQcut_extra_pionS = 1.5 *GeV);
  declareProperty( "AverageOmega",    m_AverageOmega  = 0.41 );

  declareProperty( "Pion_ghost_cut", m_ghost_cut = -25.0);
  declareProperty( "PionSame_PIDNoK_cut",m_PionSame_PIDNoK_cut= 5.0);
  declareProperty( "PionSame_PIDNoP_cut",m_PionSame_PIDNoP_cut= 10.0);


  m_nnet = 0;
  m_util = 0;
}
TaggerPionSameTool::~TaggerPionSameTool() {}; 

//=====================================================================
StatusCode TaggerPionSameTool::initialize() { 

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerPionSameTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                                std::vector<const Vertex*>& allVtx, 
                                Particle::ConstVector& vtags ){
  Tagger tpionS;
  if(!RecVert) return tpionS;
  const Vertex * SecVert= 0;
  if(!allVtx.empty()) SecVert = allVtx.at(0);

  Gaudi::LorentzVector ptotB = AXB0->momentum();
  double B0mass = ptotB.M();

  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ipionS=0;
  double ptmaxpS = -99.0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    if( (*ipart)->particleID().abspid() != 211 ) continue;

    double Pt = (*ipart)->pt();
    if( Pt <= ptmaxpS ) continue;//equal sign in "<=" is used to kill duplicates
    
    if( Pt < m_Pt_cut_pionS )  continue;
    double P  = (*ipart)->p();
    if( P  < m_P_cut_pionS )  continue;

    const ProtoParticle* proto = (*ipart)->proto();

    int myID=211;
    double PIDm= proto->info( ProtoParticle::CombDLLmu, -1000.0 );
    double PIDe= proto->info( ProtoParticle::CombDLLe,  -1000.0 );
    double PIDk= proto->info( ProtoParticle::CombDLLk,  -1000.0 );
    double PIDp= proto->info( ProtoParticle::CombDLLp,  -1000.0 );
    int iflag_m = false;
    if( proto->muonPID() ) if(proto->muonPID()->IsMuon()) iflag_m=true;
    bool iflag_e = proto->info(ProtoParticle::InAccEcal, false);
    bool iflag_k = proto->info(ProtoParticle::RichPIDStatus, false);
    float PID_m_cut  = 0.0;
    float PID_e_cut  = 5.0;
    float PID_k_cut  = 8.0, PID_kp_cut = -1.0;
    if( iflag_e && PIDe>PID_e_cut ) myID = 11;
	  if( iflag_k && PIDk>PID_k_cut &&  PIDk-PIDp>PID_kp_cut) myID = 321;
	  if( iflag_m && PIDm>PID_m_cut ) myID = 13 ;
    if(myID != 211 ) continue;

    bool pidpass=false;
    if( PIDk==0 ) pidpass=true;
    if( PIDk!=0 ) if(PIDk < m_PionSame_PIDNoK_cut &&
                     PIDp < m_PionSame_PIDNoP_cut) pidpass=true;
    if(!pidpass) continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_pionS)  continue;

    double dQ = (ptotB+(*ipart)->momentum()).M() - B0mass;
    if(dQ > m_dQcut_pionS ) continue;

    const Track* track = proto->track();
    double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;
    if( track->type() != Track::Long ) continue;

    double tsa = track->likelihood();
    if(tsa < m_ghost_cut) continue;

    //accept candidate
    ipionS = (*ipart);
    ptmaxpS = Pt;
    debug() << " PioS P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
            << " IP=" << IP << " dQ=" << dQ<<endreq;
  } 
  if( ipionS  ) {
    double extra_dQ = (ptotB+ipionS->momentum()).M() - B0mass;
    if( extra_dQ > m_dQcut_extra_pionS ) return tpionS;
  }
  if( !ipionS ) return tpionS;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    double IP, IPerr, ip, iperr, IPT=0.;
    double B0the= ptotB.Theta();
    double B0phi= ptotB.Phi();
    double ang = asin((ipionS->pt()/GeV)/(ipionS->p()/GeV));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= std::min(fabs(ipionS->momentum().Phi()-B0phi), 
                          6.283-fabs(ipionS->momentum().Phi()-B0phi));
    double dQ  = ((ptotB+ipionS->momentum()).M() - B0mass)/GeV;
    m_util->calcIP(ipionS, RecVert, IP, IPerr);
    if(SecVert) {
      m_util->calcIP(ipionS, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    std::vector<double> NNinputs(8);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->p()/GeV;
    NNinputs.at(2) = ipionS->p()/GeV;
    NNinputs.at(3) = ipionS->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
    NNinputs.at(5) = deta;
    NNinputs.at(6) = dphi;
    NNinputs.at(7) = dQ;

    pn = m_nnet->MLPpS( NNinputs );

    if( pn < m_ProbMin ) return tpionS;
  }

  int tagdecision = ipionS->charge()>0 ? 1: -1;
  if( AXB0->particleID().hasUp() ) tagdecision = -tagdecision; //is Bu

//   if(pn<0.5){
//     pn = 1-pn;
//     tpionS.setDecision( -tagdecision );
//     tpionS.setOmega( 1-pn );
//   } else {
    tpionS.setDecision( tagdecision );
    tpionS.setOmega( 1-pn );
//   }
  tpionS.setType( Tagger::SS_Pion ); 
  tpionS.addTaggerPart(ipionS);

  return tpionS;
}
//==========================================================================
StatusCode TaggerPionSameTool::finalize() { return StatusCode::SUCCESS; }

