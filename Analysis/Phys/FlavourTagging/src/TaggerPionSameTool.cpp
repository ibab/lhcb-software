// Include files 
#include "TaggerPionSameTool.h"

#include "dphi.h"

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

  declareProperty( "PionSame_Pt_cut",     m_Pt_cut_pionS  = 0.6 *GeV );
  declareProperty( "PionSame_P_cut",      m_P_cut_pionS   = 4.0 *GeV );
  declareProperty( "PionSame_IPs_cut",    m_IPs_cut_pionS = 9. );
  declareProperty( "PionSame_lcs_cut",    m_lcs_cut       = 5.0 );
  declareProperty( "PionSame_deta_max_cut", m_eta_max_cut_pionS   = 0.35);
  declareProperty( "PionSame_deta_min_cut", m_eta_min_cut_pionS   = -0.5);
  declareProperty( "PionSame_dphi_cut",   m_phi_cut_pionS   = 0.75);
  declareProperty( "PionSame_dR_cut",     m_dR_cut_pionS   = 0.75);
  declareProperty( "PionSame_dQ_cut",     m_dQcut_pionS   = 2.5 *GeV);
  declareProperty( "PionSame_dQ_extra_cut", m_dQcut_extra_pionS = 1.2 *GeV);
  declareProperty( "PionSame_ghost_cut",  m_ghost_cut     = -999.0);
  declareProperty( "PionSame_ipPU_cut",   m_ipPU_cut_pS      = 6.0 );
  declareProperty( "PionSame_distPhi_cut",m_distPhi_cut_pS= 0.005 );
  declareProperty( "PionSame_PIDNoK_cut", m_PionSame_PIDNoK_cut = 4.3);
  declareProperty( "PionSame_PIDNoP_cut", m_PionSame_PIDNoP_cut = 14.0);
  // 1fb-1 paper tuning
  declareProperty( "PionSame_P0_Cal",  m_P0_Cal_pionS   = 0.388 ); 
  declareProperty( "PionSame_P1_Cal",  m_P1_Cal_pionS   = 0.88 ); 
  declareProperty( "PionSame_P2_Cal",  m_P2_Cal_pionS   = -2.06 ); 
  declareProperty( "PionSame_Eta_Cal", m_Eta_Cal_pionS  = 0.361 );
 
  declareProperty( "PionSame_AverageOmega",  m_AverageOmega   = 0.40 );
  declareProperty( "PionSame_ProbMin",       m_PionProbMin   = 0.56);

  m_nnet = 0;
  m_util = 0;
  m_descend = 0;

}
TaggerPionSameTool::~TaggerPionSameTool() {}; 

//=====================================================================
StatusCode TaggerPionSameTool::initialize() { 
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  warning() << "PiSS calib ctt: P0_Cal "<<m_P0_Cal_pionS<<", P1_Cal "<<m_P1_Cal_pionS<<endreq;

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
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
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

  verbose()<<"--Pion SS Tagger--"<<endreq;

  Gaudi::LorentzVector ptotB = AXB0->momentum();
  double B0mass = ptotB.M();

 //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select pionS sameside tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ipionS=0;
  double ptmaxpS = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart, jpart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ++ipart ) {

    //PID cuts to select the pion
    const ProtoParticle* proto = (*ipart)->proto();
    double PIDk= proto->info( ProtoParticle::CombDLLk,  -1000.0 );
    double PIDp= proto->info( ProtoParticle::CombDLLp,  -1000.0 );
    bool pidpass=false;
    if( PIDk==0 ) pidpass=true;
    if( PIDk!=0 ) if(PIDk < m_PionSame_PIDNoK_cut &&
                     PIDp < m_PionSame_PIDNoP_cut) pidpass=true;
    if( (*ipart)->particleID().abspid() != 211 ) continue;
    if(!pidpass) continue;
    verbose()<<" Pion PIDk="<< PIDk <<endreq;
    
    double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_pionS )  continue;
    double P  = (*ipart)->p();
    if( P  < m_P_cut_pionS )  continue;
    verbose()<<" Pion P="<< P <<" Pt="<< Pt <<endreq;

    //double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    //if (cloneDist!=-1) continue;

    const Track* track = proto->track();
    double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut ) continue;
    if( track->type() != Track::Long ) continue;

    double tsa = track->likelihood();
    if(tsa < m_ghost_cut) continue;
    verbose() << " Pion lcs="<< lcs <<" tsa="<<tsa<<endmsg;


    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    if(IPsig > m_IPs_cut_pionS)  continue;


    double ippu=(*ipart)->info(1,100000.); // retrieve the stored information of IP significance wrt PU, saved in BTaggingTool.cpp    
    if(ippu < m_ipPU_cut_pS) continue;

    
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_pS ) continue;

    double deta  = fabs(log(tan(ptotB.Theta()/2.)/tan(asin(Pt/P)/2.)));
    double dphi  = fabs(TaggingHelpers::dphi(
		(*ipart)->momentum().Phi(), ptotB.Phi()));
    double dR = sqrt(deta*deta+dphi*dphi);
    if(deta > m_eta_max_cut_pionS) continue;
    if(deta < m_eta_min_cut_pionS) continue;
    if(dphi > m_phi_cut_pionS) continue;
    if(dR > m_dR_cut_pionS) continue; 

    double dQ = (ptotB+(*ipart)->momentum()).M() - B0mass;
    verbose() << " Pion IPs="<< IPsig <<" dQ="<<dQ<<endmsg;
    if(dQ > m_dQcut_pionS ) continue;

    ++ncand;

    if( Pt <= ptmaxpS ) continue;//equal sign in "<=" is used to kill duplicates

    //accept candidate
    ipionS = (*ipart);
    ptmaxpS = Pt;

  } 
  if( !ipionS ) return tpionS;

  double extra_dQ = (ptotB+ipionS->momentum()).M() - B0mass;
  if( extra_dQ > m_dQcut_extra_pionS ) return tpionS;
  verbose() << " Pion dQExtra="<< extra_dQ <<endmsg;
 
  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    //calculate omega  
    double IP, IPerr;
    m_util->calcIP(ipionS, RecVert, IP, IPerr);
    double B0the= ptotB.Theta();
    double B0phi= ptotB.Phi();
    double ang = asin((ipionS->pt())/(ipionS->p()));
    double deta= log(tan(B0the/2.))-log(tan(ang/2.));
    double dphi= fabs(TaggingHelpers::dphi(ipionS->momentum().Phi(), B0phi));
    double dQ  = ((ptotB+ ipionS->momentum() ).M() - B0mass);
    double dR = sqrt(deta*deta+dphi*dphi);
    
    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;
    NNinputs.at(3) = ipionS->pt()/GeV;
    NNinputs.at(4) = fabs(IP/IPerr);
    NNinputs.at(5) = dR;
    NNinputs.at(7) = dQ/GeV;
    NNinputs.at(8) = allVtx.size();

    //    std::cout<<" NNet inputs BC "<<NNinputs.at(0)<<" "<<NNinputs.at(1)<<" "<<NNinputs.at(3)<<" "<<NNinputs.at(4)<<" "<<NNinputs.at(5)<<" "<<NNinputs.at(7)<<" "<<NNinputs.at(8)<<" "<<std::endl;

    pn = m_nnet->MLPpS( NNinputs );
    verbose() << " Pion pn="<< pn <<endmsg;
    
    //    std::cout<<" NNet inputs AC "<<NNinputs.at(0)<<" "<<NNinputs.at(1)<<" "<<NNinputs.at(3)<<" "<<NNinputs.at(4)<<" "<<NNinputs.at(5)<<" "<<NNinputs.at(7)<<" "<<NNinputs.at(8)<<" "<<pn<<std::endl;
    
    
    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    //pn = 1 - m_P0_Cal_pionS - m_P1_Cal_pionS * ( (1-pn)-m_Eta_Cal_pionS);
    pn = 1 - m_P0_Cal_pionS - m_P1_Cal_pionS * ((1-pn)-m_Eta_Cal_pionS) - m_P2_Cal_pionS * ((1-pn)-m_Eta_Cal_pionS) * ((1-pn)-m_Eta_Cal_pionS);
    
    debug() << " PionS pn="<< pn <<" w="<<1-pn<<endmsg;

    if( pn < 0 || pn > 1 ) return tpionS;    
    if( pn < m_PionProbMin ) return tpionS;
  }

  int tagdecision = ipionS->charge()>0 ? 1: -1;
  if( AXB0->particleID().hasUp() ) tagdecision = -tagdecision; //is Bu

  tpionS.setDecision( tagdecision );
  tpionS.setOmega( 1-pn );
  tpionS.setType( Tagger::SS_Pion ); 
  tpionS.addToTaggerParts(ipionS);

  return tpionS;
}
//==========================================================================
StatusCode TaggerPionSameTool::finalize() { return GaudiTool::finalize(); }

