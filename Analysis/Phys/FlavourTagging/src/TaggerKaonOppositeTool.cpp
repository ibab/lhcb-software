// Include files 
#include "TaggerKaonOppositeTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerKaonOppositeTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerKaonOppositeTool );

//====================================================================
TaggerKaonOppositeTool::TaggerKaonOppositeTool( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",  m_NeuralNetName   = "NNetTool_v1" );
  declareProperty( "Kaon_Pt_cut",   m_Pt_cut_kaon        = 0.5 );
  declareProperty( "Kaon_P_cut",    m_P_cut_kaon         = 3.0 );
  declareProperty( "Kaon_IP_cut",   m_IP_cut_kaon        = 3.5 );

  declareProperty( "Kaon_LongTrack_LCS_cut",    m_lcs_kl = 2.5 );
  declareProperty( "Kaon_upstreamTrack_LCS_cut",m_lcs_ku = 5.0 );

  declareProperty( "Kaon_LongTrack_IP_cut",     m_IP_kl  = 2.0 );
  declareProperty( "Kaon_upstreamTrack_IP_cut", m_IP_ku  = 2.0 );
  declareProperty( "AverageOmega", m_AverageOmega = 0.355 );
  m_nnet = 0;
  m_util = 0;
}
TaggerKaonOppositeTool::~TaggerKaonOppositeTool() {}; 

//=====================================================================
StatusCode TaggerKaonOppositeTool::initialize() { 

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
Tagger TaggerKaonOppositeTool::tag( const Particle* AXB0, 
				    const RecVertex* RecVert,
				    std::vector<const Vertex*>& allVtx, 
				    Particle::ConstVector& vtags ){
  Tagger tkaon;
  if(!RecVert) return tkaon;
  const Vertex * SecVert= 0;
  if(!allVtx.empty()) SecVert = allVtx.at(0);

  //select kaon opposite tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaon=0;
  double ptmaxk = -99.0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 321 ) continue;
    double Pt = (*ipart)->pt()/GeV;
    if( Pt < m_Pt_cut_kaon )  continue;
    double P = (*ipart)->p()/GeV;
    if( P < m_P_cut_kaon )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    debug() << " Kaon P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
            << " IP=" << IP <<endreq;

    if(IPsig > m_IP_cut_kaon ) {
      const Track* track = (*ipart)->proto()->track();
      double lcs = track->chi2PerDoF();

      if( ( track->type()==Track::Long 
	    && lcs<m_lcs_kl && fabs(IP)<m_IP_kl ) 
         || 
	  ( track->type()==Track::Upstream 
	    && lcs<m_lcs_ku && fabs(IP)<m_IP_ku ) ) {
        if( Pt > ptmaxk ) { 
          ikaon = (*ipart);
          ptmaxk = Pt;
        }
      }
    }
  } 
  if( ! ikaon ) return tkaon;

  tkaon.addTaggerPart(*ikaon);
  tkaon.setDecision(ikaon->charge()>0 ? -1: 1);

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {
    Gaudi::LorentzVector ptotB = AXB0->momentum();
    double B0the  = ptotB.Theta();
    double B0p    = ptotB.P()/GeV;
    double IP, IPerr, ip, iperr, IPT=0.;

    m_util->calcIP(ikaon, RecVert, IP, IPerr);
    if(SecVert) {
      m_util->calcIP(ikaon, SecVert, ip, iperr);
      if(!iperr) IPT = ip/iperr;
    } else IPT = -1000.; 

    std::vector<double> inputs(8);
    inputs.at(0) = B0p;
    inputs.at(1) = B0the;
    inputs.at(2) =  m_util->countTracks(vtags);
    inputs.at(3) = 100;
    inputs.at(4) = ikaon->p()/GeV;
    inputs.at(5) = ikaon->pt()/GeV;
    inputs.at(6) = IP/IPerr;
    inputs.at(7) = IPT;
    
    pn = m_nnet->MLPk( inputs );

  }
  tkaon.setOmega( 1-pn );
  tkaon.setType( Tagger::OS_Kaon ); 

  return tkaon;
}

//==========================================================================
StatusCode TaggerKaonOppositeTool::finalize() { return StatusCode::SUCCESS; }

