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
DECLARE_TOOL_FACTORY( TaggerMuonTool )

//====================================================================
  TaggerMuonTool::TaggerMuonTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent ) :
    GaudiTool ( type, name, parent ), m_eventSvc(0)
{

  declareInterface<ITagger>(this);

  declareProperty( "CombTech",           m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName",      m_NeuralNetName        = "NNetTool_MLP" );

  declareProperty( "Muon_Pt_cut",        m_Pt_cut_muon  = 1.1 *GeV );
  declareProperty( "Muon_P_cut",         m_P_cut_muon   = 0.0 *GeV );
  declareProperty( "Muon_lcs_cut",       m_lcs_cut_muon = 3. );
  declareProperty( "Muon_IPs_cut",       m_IPs_cut_muon = 0.0 );
  declareProperty( "Muon_PIDm_cut",      m_PIDm_cut     = -100. );
  declareProperty( "Muon_ipPU_cut",      m_ipPU_cut_muon= 3.0 );
  declareProperty( "Muon_distPhi_cut",   m_distPhi_cut_muon= 0.005 );

  declareProperty( "Muon_ghostprob_cut", m_ghostprob_cut   = 0.4);
  declareProperty( "Muon_PIDNNm_cut",    m_PIDNNm_cut_muon = 0.35);
  declareProperty( "Muon_PIDNNpi_cut",   m_PIDNNpi_cut_muon= 0.8);
  declareProperty( "Muon_PIDNNe_cut",    m_PIDNNe_cut_muon = 0.8);
  declareProperty( "Muon_PIDNNk_cut",    m_PIDNNk_cut_muon = 0.8);
  declareProperty( "Muon_PIDNNp_cut",    m_PIDNNp_cut_muon = 0.8);

  declareProperty( "Muon_P0_Cal",        m_P0_Cal_muon   = 0. );
  declareProperty( "Muon_P1_Cal",        m_P1_Cal_muon   = 1. );
  declareProperty( "Muon_Eta_Cal",       m_Eta_Cal_muon  = 0. );
  declareProperty( "Muon_ProbMin",       m_ProbMin_muon  = 0. ); //no cut
  declareProperty( "Muon_AverageOmega",  m_AverageOmega  = 0.33 );
  declareProperty( "Personality",        m_personality = "Reco14");
  m_nnet = 0;
  m_util = 0;
  m_descend = 0;
}

TaggerMuonTool::~TaggerMuonTool() {}

//=====================================================================
StatusCode TaggerMuonTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Mu calib ctt: P0_Cal "<<m_P0_Cal_muon
            <<", P1_Cal "<<m_P1_Cal_muon<<endreq;

  sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() )
  {
    fatal() << " Unable to locate Event Data Service" << endreq;
    return sc;
  }

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);


  // register (multiple) personalities of tag
  m_tag.registerPersonality("Reco12",this, &TaggerMuonTool::tagReco12);
  m_tag.registerPersonality("Reco14",this, &TaggerMuonTool::tagReco14);

  // select personality
  // (this throws an exception if the personality chosen by the user is not
  // known)
  try {
      m_tag.setPersonality(m_personality);
  } catch (const std::exception& e) {
      error() << "Caught exception while setting TaggerMuonTool::tag "
	  "personality: " << e.what() << endmsg;
      return StatusCode::FAILURE;
  };

  return sc;
}
//================================================================================
Tagger TaggerMuonTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                            std::vector<const Vertex*>& allVtx,
                            Particle::ConstVector& vtags )
{
  return m_tag(AXB0, RecVert, allVtx, vtags); 
}
//================================================================================
Tagger TaggerMuonTool::tagReco12( const Particle* AXB0, const RecVertex* RecVert,
                                  std::vector<const Vertex*>& allVtx,
                                  Particle::ConstVector& vtags )
{
  Tagger tmu;
  if(!RecVert) return tmu;

  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"--Muon Tagger--for Reco12"<<endreq;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select muon tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* imuon=0;
  double ptmaxm = -99.0, ncand=0;
  for( Particle::ConstVector::const_iterator ipart = vtags.begin(); 
       ipart != vtags.end(); ++ipart )
  {

    const Particle* axp = (*ipart);
    const ProtoParticle* proto = axp->proto();

    const int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    if( muonNSH != 0 )                               continue;

    if(!proto->info(ProtoParticle::MuonPIDStatus,0)) continue;

    const double pidm=proto->info( ProtoParticle::CombDLLmu, -1000.0 );
    if(pidm < m_PIDm_cut )                           continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Muon PIDm="<< pidm <<" muonNSH="<<muonNSH<<endreq;

    const double Pt = axp->pt();
    if( Pt < m_Pt_cut_muon )                         continue;

    const double P = axp->p();
    if( P  < m_P_cut_muon )                          continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Muon P="<< P <<" Pt="<< Pt <<endreq;

    const double cloneDist = proto->track()->info(LHCb::Track::CloneDist, -1.);
    if ( cloneDist > -0.9 )                          continue;

    const double lcs = proto->track()->chi2PerDoF();
    if(lcs>m_lcs_cut_muon)                           continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Muon lcs="<< lcs <<endreq;

    //calculate signed IP wrt RecVert
    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = IP/IPerr;
    if(fabs(IPsig) < m_IPs_cut_muon) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " IPsig="<< IPsig <<endreq;

    const double ippu=(*ipart)->info(LHCb::Particle::LastGlobal+1,100000.);
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " ippu="<< ippu <<endreq;
    if(ippu < m_ipPU_cut_muon) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_muon ) continue;

    if ( msgLevel(MSG::VERBOSE) )
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
  if(m_CombinationTechnique == "NNet")
  {

    double IP(0), IPerr(0);
    m_util->calcIP(imuon, RecVert, IP, IPerr); //calculate IP

    if ( msgLevel(MSG::DEBUG) )
      debug()<<"IP/IPerr: "<<IP/IPerr<<endmsg;

    std::vector<double> NNinputs(10);
    //    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = imuon->p()/GeV;
    NNinputs.at(3) = imuon->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
    NNinputs.at(8) = allVtx.size();

    pn = m_nnet->MLPm( NNinputs );
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Muon pn="<< pn <<endreq;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_muon - m_P1_Cal_muon * ( (1-pn)-m_Eta_Cal_muon);
    if ( msgLevel(MSG::DEBUG) )
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
//=====================================================================
Tagger TaggerMuonTool::tagReco14( const Particle* AXB0, const RecVertex* RecVert,
                                  std::vector<const Vertex*>& allVtx, 
                                  Particle::ConstVector& vtags ){
  Tagger tmu;
  if(!RecVert) return tmu;
  
  if ( msgLevel(MSG::VERBOSE) ) verbose()<<"--Muon Tagger--for Reco14"<<endreq;
  
 //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select muon tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* imuon=0;
  Float_t ptmaxm = -99.0, ncand=0;
  Float_t save_IPPU=-10, save_IPs=-10, save_PIDNNm=-10.;
  //save_pidm=-10,save_PIDNNpi=-10.;

  
  for( Particle::ConstVector::const_iterator ipart = vtags.begin(); 
       ipart != vtags.end(); ++ipart )
  {
    const Particle*      axp   = (*ipart);
    const ProtoParticle* proto = axp->proto();
    const Track* track = (*ipart)->proto()->track();

    if(track->type() != LHCb::Track::Long)           continue;
    
    int muonNSH = (int) proto->info( ProtoParticle::MuonNShared, -1.0 );
    if( muonNSH != 0 )                               continue;

    if(proto->muonPID() == 0)                        continue;
    if(proto->muonPID()->IsMuon() == 0 )             continue; 

    if(!proto->info(ProtoParticle::MuonPIDStatus,0)) continue;

    const double pidm=proto->info( ProtoParticle::CombDLLmu, -1000.0 );
    if(pidm < m_PIDm_cut )                           continue;
    
    const double Pt = axp->pt();
    if( Pt < m_Pt_cut_muon )                         continue;

    const double P = axp->p();
    if( P  < m_P_cut_muon )                          continue;

    const double lcs = track->chi2PerDoF();
    if(lcs>m_lcs_cut_muon)                           continue;

    if(axp->info(LHCb::Particle::LastGlobal+2,-1.) > 0 )continue; // already tagged

    if(track->ghostProbability() > m_ghostprob_cut )  continue; 

    const double PIDNNm = proto->info( ProtoParticle::ProbNNmu, -1000.0);    
    if(PIDNNm <  m_PIDNNm_cut_muon )                 continue;    
    const double PIDNNpi = proto->info( ProtoParticle::ProbNNpi,-1000.0);    
    if(PIDNNpi >  m_PIDNNpi_cut_muon )                 continue;    
    const double PIDNNe = proto->info( ProtoParticle::ProbNNe, -1000.0);    
    if(PIDNNe >  m_PIDNNe_cut_muon )                 continue;    
    const double PIDNNp = proto->info( ProtoParticle::ProbNNp, -1000.0);    
    if(PIDNNp >  m_PIDNNp_cut_muon )                 continue;    
    const double PIDNNk = proto->info( ProtoParticle::ProbNNk, -1000.0);    
    if(PIDNNk >  m_PIDNNk_cut_muon )                 continue;    

    //calculate signed IP wrt RecVert
    double IP(0.), IPerr(0.);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr)                                       continue;
    double IPsig = IP/IPerr;
    if(fabs(IPsig) < m_IPs_cut_muon)                 continue;
    
    const double ippu=(*ipart)->info(LHCb::Particle::LastGlobal+1,10000.);
    if(ippu < m_ipPU_cut_muon)                       continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi)) continue;
    if( distphi < m_distPhi_cut_muon )               continue;

    ++ncand;

    Particle* c = const_cast<Particle*>(*ipart);
    c->addInfo(LHCb::Particle::LastGlobal+2,13); // store the information that the muon tagger is found // new
    
    if( Pt > ptmaxm ) { //Pt ordering
      imuon = axp;
      ptmaxm = Pt;
      save_IPs = IPsig;
      //save_pidm = pidm;
      save_IPPU = ippu;
      save_PIDNNm = PIDNNm;      
      //save_PIDNNpi = PIDNNpi;      
    }
  }
  if( ! imuon ) return tmu;

  //calculate omega
  double pn = 1. - m_AverageOmega;
  double sign=1.;
  std::list<std::pair<std::string, Float_t> > NNinputList;    
  if(m_CombinationTechnique == "NNet") {
    if ( msgLevel(MSG::DEBUG) ) debug()<< allVtx.size()<< endreq;

    NNinputList.push_back(std::pair<std::string, Float_t>("mult",   m_util->countTracks(vtags)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partP",  log(imuon->p()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partPt", log(imuon->pt()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("ptB",    log(AXB0->pt()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("IPs",    log(fabs(save_IPs))));
    NNinputList.push_back(std::pair<std::string, Float_t>("partlcs",log(imuon->proto()->track()->chi2PerDoF())));
    NNinputList.push_back(std::pair<std::string, Float_t>("PIDNNm", save_PIDNNm));
    NNinputList.push_back(std::pair<std::string, Float_t>("ghostProb",log(imuon->proto()->track()->ghostProbability())));
    NNinputList.push_back(std::pair<std::string, Float_t>("IPPU",   log(save_IPPU)));    

    pn = m_nnet->MLPmTMVA( NNinputList); 

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1. - m_P0_Cal_muon - m_P1_Cal_muon * ( (1.-pn)-m_Eta_Cal_muon);

    if( pn < 0 || pn > 1 ) { 
       if ( msgLevel(MSG::INFO) )
         info()<<" ===> Something wrong with MUON Training "<<pn<<endmsg;
       return tmu;
    }
    if( pn < m_ProbMin_muon ) return tmu;
  
    if(pn < 0.5){
      pn = 1. - pn;
      sign = -1.;
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug()<<" TaggerMuon: "<<sign*imuon->charge()<<" omega="<<1-pn;
    
    typedef std::list<std::pair<std::string, Float_t> > ParListType;
    for(ParListType::const_iterator p = NNinputList.begin(); p != NNinputList.end(); p++) {
      debug() << p->second<<" ";
    }
    debug()<<endreq;
  }
  
  

  tmu.setOmega( 1 - pn );
  tmu.setDecision(sign*imuon->charge()>0 ? -1: 1);
  tmu.setType( Tagger::OS_Muon ); 
  tmu.addToTaggerParts(imuon);

  return tmu;
}
