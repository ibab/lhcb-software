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
  declareProperty( "NeuralNetName", m_NeuralNetName    = "NNetTool_MLP" );

  declareProperty( "Kaon_Pt_cut",   m_Pt_cut_kaon   = 0.7 *GeV );
  declareProperty( "Kaon_P_cut",    m_P_cut_kaon    = 5.0 *GeV );
  declareProperty( "Kaon_IPs_cut",  m_IPs_cut_kaon  = 4.3 );
  declareProperty( "Kaon_IP_cut",   m_IP_cut_kaon   = 1.45 );
  declareProperty( "Kaon_lcs_cut",  m_lcs_kaon      = 2.45 );
  declareProperty( "Kaon_PIDk_cut",     m_PID_k_cut     =  0.75);
  declareProperty( "Kaon_PIDkp_cut",    m_PIDkp_cut     = -3. );
  declareProperty( "Kaon_ghost_cut",m_ghost_cut     = -999.0 );
  declareProperty( "Kaon_ipPU_cut", m_ipPU_cut_kaon      = 7.5 );
  declareProperty( "Kaon_distPhi_cut", m_distPhi_cut_kaon= 0.005 );
  declareProperty( "Kaon_P0_Cal",  m_P0_Cal_kaon   = 0.393 ); 
  declareProperty( "Kaon_P1_Cal",  m_P1_Cal_kaon   = 0.706 ); 
  declareProperty( "Kaon_Eta_Cal", m_Eta_Cal_kaon  = 0.354 ); 
  declareProperty( "Kaon_AverageOmega",  m_AverageOmega     = 0.33 );
  declareProperty( "Kaon_ProbMin",  m_ProbMin_kaon  = 0.54 ); //no cut

  m_nnet = 0;
  m_util = 0;
  m_descend = 0;
  
}
TaggerKaonOppositeTool::~TaggerKaonOppositeTool() {}; 

//=====================================================================
StatusCode TaggerKaonOppositeTool::initialize() { 

  warning() << "KOS calib ctt: P0_Cal "<<m_P0_Cal_kaon<<", P1_Cal "<<m_P1_Cal_kaon<<endreq;

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this );
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
Tagger TaggerKaonOppositeTool::tag( const Particle* AXB0, 
                                    const RecVertex* RecVert,
                                    std::vector<const Vertex*>& allVtx, 
                                    Particle::ConstVector& vtags ){
  Tagger tkaon;
  if(!RecVert) return tkaon;

  verbose()<<"--Kaon OppositeSide Tagger--"<<endreq;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaon opposite tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaon=0;
  double ptmaxk = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ++ipart ) {
    
    double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    if(pidk < m_PID_k_cut ) continue;
    if(pidk==0 || pidk==-1000.0) continue;

    double pidproton = (*ipart)->proto()->info(ProtoParticle::CombDLLp, -1000.0);
    if( pidk - pidproton < m_PIDkp_cut ) continue;
    verbose() << " Kaon PIDk="<< pidk <<" Dkp="<<pidk - pidproton<<endmsg;
    
    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_kaon )  continue;

    double P = (*ipart)->p();
    if( P < m_P_cut_kaon )  continue;

    double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    if (cloneDist!=-1) continue;

    verbose() << " Kaon P="<< P <<" Pt="<<Pt<<endmsg;
    const Track* track = (*ipart)->proto()->track();
    double lcs = track->chi2PerDoF();
    if(lcs > m_lcs_kaon) continue;

    double tsa = (*ipart)->proto()->track()->likelihood();
    if( tsa < m_ghost_cut ) continue;
    verbose() << " Kaon lcs="<< lcs <<" tsa="<<tsa<<endmsg;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    if (fabs(IP) > m_IP_cut_kaon) continue;
    if (IPsig < m_IPs_cut_kaon) continue;
 
    double ippu=(*ipart)->info(1,100000.);
    verbose() << " Kaon IPs="<< IPsig <<" IP="<<fabs(IP)<<" IPPU="<<ippu<<endmsg;
    if(ippu < m_ipPU_cut_kaon) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_kaon ) continue;

    ++ncand;
	
    if( Pt > ptmaxk ) { 
      ikaon = (*ipart);
      ptmaxk = Pt;
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
    NNinputs.at(8) = allVtx.size();

    pn = m_nnet->MLPk( NNinputs );

    verbose() << " Kaon input="<< NNinputs <<endmsg;
    verbose() << " Kaon pn="<< pn <<endmsg;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_kaon - m_P1_Cal_kaon * ( (1-pn)-m_Eta_Cal_kaon);
    debug() << " Kaon pn="<< pn <<" w="<<1-pn<<endmsg;
    if( pn < 0 || pn > 1 ) return tkaon;
    if( pn < m_ProbMin_kaon ) return tkaon;

  }

  tkaon.setOmega( 1-pn );
  tkaon.setDecision(ikaon->charge()>0 ? -1: 1);
  tkaon.setType( Tagger::OS_Kaon ); 
  tkaon.addToTaggerParts(ikaon);

  return tkaon;
}

//==========================================================================
StatusCode TaggerKaonOppositeTool::finalize() { return StatusCode::SUCCESS; }

