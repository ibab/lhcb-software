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
  declareProperty( "NeuralNetName",  m_NeuralNetName  = "NNetTool_v1" );
  declareProperty( "ProbMin", m_ProbMin               = 0.55);
  declareProperty( "PionSame_Pt_cut", m_Pt_cut_pionS  = 0.2 );
  declareProperty( "PionSame_P_cut",  m_P_cut_pionS   = 2.0 );
  declareProperty( "PionSame_IPs_cut",m_IPs_cut_pionS = 3.0 );
  declareProperty( "PionS_LCS_cut",   m_lcs_cut       = 3.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS   = 3.0 );
  declareProperty( "PionSame_dQ_extra_cut", m_dQcut_extra_pionS  = 1.0 );
  declareProperty( "AverageOmega", m_AverageOmega     = 0.44 );
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
  double B0mass = ptotB.M()/GeV;

  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ipionS=0;
  double ptmaxpS = -99.0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 211 ) continue;

    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_pionS )  continue;

    double P = (*ipart)->p()/GeV;
    if( P  < m_P_cut_pionS )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_pionS)  continue;   

    double dQ = (ptotB+(*ipart)->momentum()).M()/GeV - B0mass;
    if(dQ > m_dQcut_pionS ) continue;

    const ProtoParticle* proto = (*ipart)->proto();
    const Track* track = proto->track();
    double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;
    if( track->type() != Track::Long ) continue;

    if( Pt < ptmaxpS ) continue;

    bool dupli=false;
    for( jpart = vtags.begin(); jpart != vtags.end(); jpart++ ) {
      if((*jpart)->proto() == proto) if(ipart!=jpart) { dupli=true; break; }
    }
    if( dupli ) continue;

    //accept candidate
    ipionS = (*ipart);
    ptmaxpS = Pt;
    debug() << " PioS P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
	    << " IP=" << IP << " dQ=" << dQ<<endreq;
  } 
  if( ipionS  ) {
    double extra_dQ = (ptotB+ipionS->momentum()).M()/GeV - B0mass;
    if( extra_dQ > m_dQcut_extra_pionS ) return tpionS;
  }
  if( !ipionS ) return tpionS;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    double IP, IPerr, ip, iperr, IPT=0.;
    double B0p  = ptotB.P()/GeV;
    double B0the= ptotB.Theta();
    double B0phi= ptotB.Phi();
    double ang = asin((ipionS->pt()/GeV)/(ipionS->p()/GeV));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= std::min(fabs(ipionS->momentum().Phi()-B0phi), 
			  6.283-fabs(ipionS->momentum().Phi()-B0phi));
    double dQ  = (ptotB+ipionS->momentum()).M()/GeV - B0mass;
    m_util->calcIP(ipionS, RecVert, IP, IPerr);
    if(SecVert) {
      m_util->calcIP(ipionS, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    std::vector<double> inputs(12);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) = m_util->countTracks(vtags);
    inputs.at(3) = 100;
    inputs.at(4) = ipionS->p()/GeV;
    inputs.at(5) = ipionS->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;
    inputs.at(8) = 0.;
    inputs.at(9) = deta;
    inputs.at(10)= dphi;
    inputs.at(11)= dQ;
    
    pn = m_nnet->MLPpS( inputs );

    if( pn < m_ProbMin ) return tpionS;
  }

  int tagdecision = ipionS->charge()>0 ? 1: -1;
  if( AXB0->particleID().hasUp() ) tagdecision = -tagdecision; //is Bu
  tpionS.setDecision( tagdecision );
  tpionS.setOmega( 1-pn );
  tpionS.setType( Tagger::SS_Pion ); 
  tpionS.addTaggerPart(*ipionS);

  return tpionS;
}
//==========================================================================
StatusCode TaggerPionSameTool::finalize() { return StatusCode::SUCCESS; }

