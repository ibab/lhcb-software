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
  declareProperty( "NeuralNetName", m_NeuralNetName   = "NNetTool_MLP" );
  declareProperty( "Kaon_Pt_cut",   m_Pt_cut_kaon        = 0.6 *GeV );
  declareProperty( "Kaon_P_cut",    m_P_cut_kaon         = 4.0 *GeV );
  declareProperty( "Kaon_IPs_cut",  m_IPs_cut_kaon       = 3.8 );
  declareProperty( "Kaon_IP_cut",   m_IP_cut_kaon        = 1.5 );

  declareProperty( "Kaon_LongTrack_LCS_cut",    m_lcs_kl = 1.6 );
  declareProperty( "Kaon_upstreamTrack_LCS_cut",m_lcs_ku = 1.6 );

  declareProperty( "Kaon_PIDkp_extracut", m_PIDkp_extracut= -1.0 );
  declareProperty( "Kaon_ghost_cut",      m_ghost_cut     =-14.0 );

  declareProperty( "Kaon_LongTrack_IP_cut",     m_IP_kl  = 999.0 );
  declareProperty( "Kaon_upstreamTrack_IP_cut", m_IP_ku  = 999.0 );
  declareProperty( "AverageOmega", m_AverageOmega = 0.33 );
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

  verbose()<<"allVtx.size()="<< allVtx.size() << endreq;

  //select kaon opposite tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaon=0;
  double ptmaxk = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    if( (*ipart)->particleID().abspid() != 321 ) continue;

    if((*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 )
       - (*ipart)->proto()->info( ProtoParticle::CombDLLp, -1000.0 ) 
       < m_PIDkp_extracut ) continue;

    double tsa= (*ipart)->proto()->track()->likelihood();
    if(tsa < m_ghost_cut) continue;

    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_kaon )  continue;
    double P = (*ipart)->p();
    if( P < m_P_cut_kaon )  continue;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    verbose() << " Kaon P="<< P <<" Pt="<< Pt << " IPsig=" << IPsig 
              << " IP=" << IP <<endreq;

    if(IPsig > m_IPs_cut_kaon ) if(fabs(IP)< m_IP_cut_kaon)  {
      const Track* track = (*ipart)->proto()->track();
      double lcs = track->chi2PerDoF();

      if( ( track->type()==Track::Long 
            && lcs<m_lcs_kl && fabs(IP)<m_IP_kl ) 
          || 
          ( track->type()==Track::Upstream 
            && lcs<m_lcs_ku && fabs(IP)<m_IP_ku ) ) {
        
        ncand++;
        if( Pt > ptmaxk ) { 
          ikaon = (*ipart);
          ptmaxk = Pt;
        }
      }
    }
  } 
  if( ! ikaon ) return tkaon;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    double IP, IPerr;
    m_util->calcIP(ikaon, RecVert, IP, IPerr);

    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = ikaon->p()/GeV;
    NNinputs.at(3) = ikaon->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
//    NNinputs.at(8) = m_util->getNvtx();
    NNinputs.at(8) = allVtx.size();
    NNinputs.at(9) = ncand;

    pn = m_nnet->MLPk( NNinputs );

  }

  tkaon.setOmega( 1-pn );
  tkaon.setDecision(ikaon->charge()>0 ? -1: 1);
  tkaon.setType( Tagger::OS_Kaon ); 
  tkaon.addToTaggerParts(ikaon);

  return tkaon;
}

//==========================================================================
StatusCode TaggerKaonOppositeTool::finalize() { 
  return StatusCode::SUCCESS; 
}

