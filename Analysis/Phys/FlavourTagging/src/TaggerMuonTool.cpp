// Include files 
#include "TaggerMuonTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerMuonTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerMuonTool );

//====================================================================
TaggerMuonTool::TaggerMuonTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent ) :
  GaudiTool ( type, name, parent ), m_eventSvc(0) {

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",     m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",m_NeuralNetName        = "NNetTool_MLP" );
  declareProperty( "Muon_AverageOmega", m_AverageOmega         = 0.33 );

  declareProperty( "Muon_Pt_cut",  m_Pt_cut_muon  = 1.2 *GeV );
  declareProperty( "Muon_P_cut",   m_P_cut_muon   = 0.0 *GeV );
  declareProperty( "Muon_IPs_cut", m_IPs_cut_muon = 0.0 );
  declareProperty( "Muon_lcs_cut", m_lcs_cut_muon = 3.2 );
  declareProperty( "Muon_PIDm_cut",m_PIDm_cut     = 2.5 );
  declareProperty( "Muon_ipPU_cut", m_ipPU_cut_muon      = 3.0 );
  declareProperty( "Muon_distPhi_cut", m_distPhi_cut_muon= 0.005 );
  declareProperty( "Muon_P0_Cal",  m_P0_Cal_muon   = 0.309 ); 
  declareProperty( "Muon_P1_Cal",  m_P1_Cal_muon   = 1.1953 ); 
  declareProperty( "Muon_Eta_Cal", m_Eta_Cal_muon  = 0.304 ); 

  declareProperty( "Muon_ProbMin", m_ProbMin_muon = 0. ); //no cut

  m_nnet = 0;
  m_util = 0;
  m_descend = 0;
}
TaggerMuonTool::~TaggerMuonTool() {}; 

//=====================================================================
StatusCode TaggerMuonTool::initialize() { 

  warning() << "Mu calib ctt: P0_Cal "<<m_P0_Cal_muon<<", P1_Cal "<<m_P1_Cal_muon<<endreq;

  StatusCode sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() ) {
    fatal() << " Unable to locate Event Data Service" << endreq;
    return sc;
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
  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//=====================================================================
Tagger TaggerMuonTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                            std::vector<const Vertex*>& allVtx, 
                            Particle::ConstVector& vtags ){
  Tagger tmu;
  if(!RecVert) return tmu;
  
  verbose()<<"--Muon Tagger--"<<endreq;
  
 //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select muon tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* imuon=0;
  double ptmaxm = -99.0, ncand=0;
  Particle::ConstVector::const_iterator ipart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ++ipart ) {

    const Particle* axp = (*ipart);

    const ProtoParticle* proto = axp->proto();
    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    if( muonNSH != 0 ) continue;
    if(!proto->info(ProtoParticle::MuonPIDStatus,0)) continue;
    double pidm=proto->info( ProtoParticle::CombDLLmu, -1000.0 );
    if(pidm < m_PIDm_cut ) continue;
    verbose() << " Muon PIDm="<< pidm <<" muonNSH="<<muonNSH<<endreq;
    
    double Pt = axp->pt();
    if( Pt < m_Pt_cut_muon ) continue;

    double P = axp->p();
    if( P  < m_P_cut_muon ) continue;
    verbose() << " Muon P="<< P <<" Pt="<< Pt <<endreq;

    double cloneDist = proto->track()->info(LHCb::Track::CloneDist, -1.);
    if (cloneDist!=-1) continue;

    double lcs = proto->track()->chi2PerDoF();
    if(lcs>m_lcs_cut_muon) continue;
    verbose() << " Muon lcs="<< lcs <<endreq;
  
    //calculate signed IP wrt RecVert
    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = IP/IPerr;
    if(fabs(IPsig) < m_IPs_cut_muon) continue;
    verbose() << " IPsig="<< IPsig <<endreq;

    double ippu=(*ipart)->info(1,100000.);
    verbose() << " ippu="<< ippu <<endreq;
    if(ippu < m_ipPU_cut_muon) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_muon ) continue;
    verbose()<<" distPhi="<<distphi<<endreq;

    ++ncand;

    if( Pt > ptmaxm ) { //Pt ordering
      imuon = axp;
      ptmaxm = Pt;
    }
  }
  if( ! imuon ) return tmu;

  //calculate omega
  double pn = 1 - m_AverageOmega;
  if(m_CombinationTechnique == "NNet") {

    double IP, IPerr;
    m_util->calcIP(imuon, RecVert, IP, IPerr); //calculate IP

    debug()<<"IP/IPerr: "<<IP/IPerr<<endmsg;
    
    std::vector<double> NNinputs(10);
    //    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = imuon->p()/GeV;
    NNinputs.at(3) = imuon->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
    NNinputs.at(8) = allVtx.size();
    
    pn = m_nnet->MLPm( NNinputs );
    verbose() << " Muon pn="<< pn <<endreq;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_muon - m_P1_Cal_muon * ( (1-pn)-m_Eta_Cal_muon);
    debug() << " Muon pn="<< pn <<" w="<<1-pn<<endmsg;
    if( pn < 0 || pn > 1 ) return tmu;
    if( pn < m_ProbMin_muon ) return tmu;

  }

  tmu.setOmega( 1-pn );
  tmu.setDecision(imuon->charge()>0 ? -1: 1);
  tmu.setType( Tagger::OS_Muon ); 
  tmu.addToTaggerParts(imuon);

  return tmu;
}

//====================================================================
StatusCode TaggerMuonTool::finalize() { return StatusCode::SUCCESS; }

