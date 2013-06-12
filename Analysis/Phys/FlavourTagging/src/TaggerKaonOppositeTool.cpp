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
DECLARE_TOOL_FACTORY( TaggerKaonOppositeTool )

//====================================================================
  TaggerKaonOppositeTool::TaggerKaonOppositeTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
    declareInterface<ITagger>(this);

    declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
    declareProperty( "NeuralNetName", m_NeuralNetName    = "NNetTool_MLP" );

    declareProperty( "Kaon_Pt_cut",      m_Pt_cut_kaon   = 0.7 *GeV );
    declareProperty( "Kaon_P_cut",       m_P_cut_kaon    = 2.0 *GeV );
    declareProperty( "Kaon_IPs_cut",     m_IPs_cut_kaon  = 4.0 );
    declareProperty( "Kaon_IP_cut",      m_IP_cut_kaon   = 1.6 );
    declareProperty( "Kaon_lcs_cut",     m_lcs_kaon      = 3.0 );
    declareProperty( "Kaon_PIDk_cut",    m_PIDk_cut      = -100.);
    declareProperty( "Kaon_PIDkp_cut",   m_PIDkp_cut     = -300. );
    declareProperty( "Kaon_ghost_cut",   m_ghost_cut     = -999.0 );
    declareProperty( "Kaon_ipPU_cut",    m_ipPU_cut_kaon = 6.0 );
    declareProperty( "Kaon_distPhi_cut", m_distPhi_cut_kaon = 0.005 );
    declareProperty( "Kaon_PIDNNk_cut",  m_PIDNNk_cut_kaon  = 0.25 );
    declareProperty( "Kaon_PIDNNm_cut",  m_PIDNNm_cut_kaon  = 0.8 );
    declareProperty( "Kaon_PIDNNe_cut",  m_PIDNNe_cut_kaon  = 0.8 );
    declareProperty( "Kaon_PIDNNpi_cut", m_PIDNNpi_cut_kaon = 0.8 );
    declareProperty( "Kaon_PIDNNp_cut",  m_PIDNNp_cut_kaon  = 0.8 );
    declareProperty( "Kaon_PIDNNkp_cut", m_PIDNNkp_cut_kaon = 0. );
    declareProperty( "Kaon_PIDNNkpi_cut",m_PIDNNkpi_cut_kaon = -0.6 );
    declareProperty( "Kaon_ghostProb_cut",m_ghostProb_kaon  = 0.35);
    declareProperty( "Kaon_P0_Cal",      m_P0_Cal_kaon   = 0. );
    declareProperty( "Kaon_P1_Cal",      m_P1_Cal_kaon   = 1. );
    declareProperty( "Kaon_Eta_Cal",     m_Eta_Cal_kaon  = 0. );
    declareProperty( "Kaon_AverageOmega",m_AverageOmega  = 0.33 );
    declareProperty( "Kaon_ProbMin",     m_ProbMin_kaon  = 0. ); //no cut
    declareProperty( "Personality",      m_personality   = "Reco14");
    m_nnet = 0;
    m_util = 0;
    m_descend = 0;

  }

TaggerKaonOppositeTool::~TaggerKaonOppositeTool() {}

//=====================================================================
StatusCode TaggerKaonOppositeTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "KOS calib ctt: P0_Cal "<<m_P0_Cal_kaon
            << ", P1_Cal "<<m_P1_Cal_kaon<<endreq;

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this );

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  // register (multiple) personalities of tag
  m_tag.registerPersonality("Reco12",this, &TaggerKaonOppositeTool::tagReco12);
  m_tag.registerPersonality("Reco14",this, &TaggerKaonOppositeTool::tagReco14);
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
Tagger TaggerKaonOppositeTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                            std::vector<const Vertex*>& allVtx,
                            Particle::ConstVector& vtags )
{
  return m_tag(AXB0, RecVert, allVtx, vtags); 
}
//=====================================================================
Tagger TaggerKaonOppositeTool::tagReco12( const Particle* AXB0,
                                          const RecVertex* RecVert,
                                          std::vector<const Vertex*>& allVtx,
                                          Particle::ConstVector& vtags )
{
  Tagger tkaon;
  if(!RecVert) return tkaon;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--Kaon OppositeSide Tagger--"<<endreq;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaon opposite tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaon=0;
  double ptmaxk = -99.0, ncand=0;
  for( Particle::ConstVector::const_iterator ipart = vtags.begin(); 
       ipart != vtags.end(); ++ipart )
  {

    const double pidk = (*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    if(pidk < m_PIDk_cut ) continue;
    if(pidk==0 || pidk==-1000.0) continue;

    const double pidproton = (*ipart)->proto()->info(ProtoParticle::CombDLLp, -1000.0);
    if( pidk - pidproton < m_PIDkp_cut ) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon PIDk="<< pidk <<" Dkp="<<pidk - pidproton<<endmsg;

    const double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_kaon )  continue;

    const double P = (*ipart)->p();
    if( P < m_P_cut_kaon )  continue;

    const double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    if (cloneDist!=-1) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon P="<< P <<" Pt="<<Pt<<endmsg;

    const Track* track = (*ipart)->proto()->track();
    const double lcs = track->chi2PerDoF();
    if(lcs > m_lcs_kaon) continue;

    const double tsa = (*ipart)->proto()->track()->likelihood();
    if( tsa < m_ghost_cut ) continue;
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon lcs="<< lcs <<" tsa="<<tsa<<endmsg;

    //calculate signed IP wrt RecVert
    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = fabs(IP/IPerr);
    if (fabs(IP) > m_IP_cut_kaon) continue;
    if (IPsig < m_IPs_cut_kaon) continue;

    const double ippu=(*ipart)->info(LHCb::Particle::LastGlobal+1,100000.);

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon IPs="<< IPsig <<" IP="<<fabs(IP)<<" IPPU="<<ippu<<endmsg;

    if(ippu < m_ipPU_cut_kaon) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_kaon ) continue;

    ++ncand;

    if( Pt > ptmaxk )
    {
      ikaon = (*ipart);
      ptmaxk = Pt;
    }
  }

  if( ! ikaon ) return tkaon;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") 
  {

    double IP(0), IPerr(0);
    m_util->calcIP(ikaon, RecVert, IP, IPerr);

    std::vector<double> NNinputs(10);
    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = ikaon->p()/GeV;
    NNinputs.at(3) = ikaon->pt()/GeV;
    NNinputs.at(4) = IP/IPerr;
    NNinputs.at(8) = allVtx.size();

    pn = m_nnet->MLPk( NNinputs );

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Kaon input="<< NNinputs <<endmsg;
      verbose() << " Kaon pn="<< pn <<endmsg;
    }

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_kaon - m_P1_Cal_kaon * ( (1-pn)-m_Eta_Cal_kaon);
    if ( msgLevel(MSG::DEBUG) )
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
Tagger TaggerKaonOppositeTool::tagReco14( const Particle* AXB0, 
                                          const RecVertex* RecVert,
                                          std::vector<const Vertex*>& allVtx, 
                                          Particle::ConstVector& vtags ){
  Tagger tkaon;
  if(!RecVert) return tkaon;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--Kaon OppositeSide Tagger--"<<endreq;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaon opposite tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  const Particle* ikaon=0;
  Float_t ptmaxk = -99.0, save_IPs=-10, 
    save_IPPU = -10., //save_PIDk=-10., save_PIDpk = -10, 
    save_PIDNNp = -10., save_PIDNNpi = -10., save_PIDNNk =-10. ;
  for( Particle::ConstVector::const_iterator ipart = vtags.begin(); 
       ipart != vtags.end(); ++ipart )
  {
    const Particle*      axp   = (*ipart);
    const ProtoParticle* proto = (*ipart)->proto();
    const Track*         track = (*ipart)->proto()->track();
    
    if(track->type() != Track::Long)                        continue;
    const double lcs = track->chi2PerDoF();
    if(lcs > m_lcs_kaon)                                    continue;
    
    if(axp->info(LHCb::Particle::LastGlobal+2,-1.) > 0 )    continue; // already tagger by muon   // new
    if(proto->muonPID() ) if(proto->muonPID()->IsMuon()!= 0)continue; // new
  
    const double tsa = track->likelihood();
    if( tsa < m_ghost_cut )                                 continue;

    if(track->ghostProbability() >  m_ghostProb_kaon )      continue;  
    
    const double pidk=proto->info( ProtoParticle::CombDLLk, -1000.0 );
    if(pidk < m_PIDk_cut )                                 continue;
    if(pidk==0 || pidk==-1000.0)                            continue;

    const double pidproton = proto->info(ProtoParticle::CombDLLp, -1000.0);
    if(pidk - pidproton < m_PIDkp_cut )                    continue;

    const double PIDNNm = proto->info( ProtoParticle::ProbNNmu, -1000.0);
    if(PIDNNm >  m_PIDNNm_cut_kaon )                       continue;   

    const double PIDNNe = proto->info( ProtoParticle::ProbNNe, -1000.0);
    if(PIDNNe >  m_PIDNNe_cut_kaon )                       continue;   

    const double PIDNNpi = proto->info( ProtoParticle::ProbNNpi, -1000.0);
    if(PIDNNpi >  m_PIDNNpi_cut_kaon )                     continue;   

    const double PIDNNp = proto->info( ProtoParticle::ProbNNp, -1000.0);     
    if(PIDNNp >  m_PIDNNp_cut_kaon )                       continue;    

    const double PIDNNk = proto->info( ProtoParticle::ProbNNk, -1000.0);     
    if(PIDNNk <  m_PIDNNk_cut_kaon )                       continue;    
    
    if(PIDNNk - PIDNNp <  m_PIDNNkp_cut_kaon )             continue;    
    if(PIDNNk - PIDNNpi <  m_PIDNNkpi_cut_kaon )           continue;   
    
    const double Pt = axp->pt();
    if( Pt < m_Pt_cut_kaon )                               continue;

    const double P = axp->p();
    if( P < m_P_cut_kaon )                                 continue;


    double IP(0.), IPerr(0.);
    m_util->calcIP(axp, RecVert, IP, IPerr);
    if(!IPerr)                                              continue;
    const double IPsig = fabs(IP/IPerr);
    if (fabs(IP) > m_IP_cut_kaon)                           continue;
    if (IPsig < m_IPs_cut_kaon)                             continue;
 
    const double ippu=axp->info(LHCb::Particle::LastGlobal+1,10000.);
    if(ippu < m_ipPU_cut_kaon)                              continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon IPs="<< IPsig <<" IP="<<fabs(IP)<<" IPPU="<<ippu<<endmsg;

    //distphi
    if( m_util->isinTree( axp, axdaugh, distphi ) )         continue ;//exclude signal
    if( distphi < m_distPhi_cut_kaon )                      continue;

    Particle* c = const_cast<Particle*>(*ipart);
    c->addInfo(LHCb::Particle::LastGlobal+2,321);      // store the information that the kaon tagger is found    // new
	
    if( Pt > ptmaxk ) { 
      ikaon = axp;
      ptmaxk = Pt;
      save_IPs = IPsig;
      save_IPPU = ippu;
      //save_PIDpk = pidproton - pidk; 
      //save_PIDk = pidk; 
      save_PIDNNp = PIDNNp; 
      save_PIDNNpi = PIDNNpi; 
      save_PIDNNk = PIDNNk;
    }
  }
  if( ! ikaon ) return tkaon;

  //calculate omega
  double pn = 1-m_AverageOmega;
  double sign = 1.;
  
  std::list<std::pair<std::string, Float_t> > NNinputList;    
  if(m_CombinationTechnique == "NNet") {
    
    NNinputList.push_back(std::pair<std::string, Float_t>("mult",   m_util->countTracks(vtags)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partP",  log(ikaon->p()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partPt", log(ikaon->pt()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("nnkrec", allVtx.size()));
    NNinputList.push_back(std::pair<std::string, Float_t>("ptB",    log(AXB0->pt()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("IPs",    log(save_IPs)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partlcs",log(ikaon->proto()->track()->chi2PerDoF())));
    NNinputList.push_back(std::pair<std::string, Float_t>("PIDNNk", save_PIDNNk));
    NNinputList.push_back(std::pair<std::string, Float_t>("PIDNNpi",save_PIDNNpi));
    NNinputList.push_back(std::pair<std::string, Float_t>("PIDNNp", save_PIDNNp));
    NNinputList.push_back(std::pair<std::string, Float_t>("ghostProb",log(ikaon->proto()->track()->ghostProbability())));
    NNinputList.push_back(std::pair<std::string, Float_t>("IPPU",   log(save_IPPU)));
  
    pn = m_nnet->MLPkaonTMVA( NNinputList );

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon pn="<< pn <<endmsg;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_kaon - m_P1_Cal_kaon * ( (1-pn)-m_Eta_Cal_kaon);
    debug() << " Kaon pn="<< pn <<" w="<<1-pn<<endmsg;
    if( pn < 0 || pn > 1 ) { 
    if ( msgLevel(MSG::DEBUG) )
      debug()<<" ===> Something wrong with KAON Training "<<pn<<endmsg;
      return tkaon;
    }
    
    if( pn < m_ProbMin_kaon ) return tkaon;
    if(pn < 0.5){
      pn = 1. - pn;
      sign = -1.;
    }
  }
  
  if ( msgLevel(MSG::DEBUG) )
  {
    
    debug()<<" TaggerKaon: "<<sign*ikaon->charge()<<" omega="<<1-pn;
    
    typedef std::list<std::pair<std::string, Float_t> > ParListType;
    for(ParListType::const_iterator p = NNinputList.begin(); p != NNinputList.end(); p++) {
    debug() << p->second<<" ";
    }
    debug()<<endreq;
  }
  
  tkaon.setOmega( 1-pn );
  tkaon.setDecision(sign*ikaon->charge()>0 ? -1: 1);
  tkaon.setType( Tagger::OS_Kaon ); 
  tkaon.addToTaggerParts(ikaon);

  return tkaon;
}
