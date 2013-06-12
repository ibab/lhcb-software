// Include files 
#include "TaggerElectronTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerElectronTool
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerElectronTool )

//====================================================================
  TaggerElectronTool::TaggerElectronTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{

    declareInterface<ITagger>(this);

    declareProperty( "CombTech",          m_CombinationTechnique = "NNet" );
    declareProperty( "NeuralNetName",     m_NeuralNetName   = "NNetTool_MLP" );

    declareProperty( "Ele_Pt_cut",        m_Pt_cut_ele    = 1.1 * GeV );
    declareProperty( "Ele_P_cut",         m_P_cut_ele     = 0.0 * GeV );
    declareProperty( "Ele_lcs_cut",       m_lcs_cut_ele   = 3. );
    declareProperty( "Ele_IPs_cut",       m_IPs_cut_ele   = 3.5 );
    declareProperty( "Ele_ghost_cut",     m_ghost_cut_ele = -999 );
    declareProperty( "Ele_VeloChargeMin", m_VeloChMin     = 0.0 );
    declareProperty( "Ele_VeloChargeMax", m_VeloChMax     = 1.4 );
    declareProperty( "Ele_EoverPmax",     m_EoverPmax     = 2. );
    declareProperty( "Ele_EoverP",        m_EoverP        = 0.85 );
    declareProperty( "Ele_PIDe_cut",      m_PIDe_cut      = -1. );
    declareProperty( "Ele_ipPU_cut",      m_ipPU_cut_ele  = 4.0 );
    declareProperty( "Ele_distPhi_cut",   m_distPhi_cut_ele= 0.005 );
 
    declareProperty( "Ele_PIDNNe_cut",    m_PIDNNe_cut_ele     = 0.1 ); 
    declareProperty( "Ele_PIDNNk_cut",    m_PIDNNk_cut_ele     = 0.8 );
    declareProperty( "Ele_PIDNNp_cut",    m_PIDNNp_cut_ele     = 0.8 );
    declareProperty( "Ele_PIDNNpi_cut",   m_PIDNNpi_cut_ele    = 0.8 );
    declareProperty( "Ele_PIDNNepi_cut",  m_PIDNNepi_cut_ele   = -0.8 );
    declareProperty( "Ele_ghostProb_cut", m_ghostProb_ele      = 0.4);

    declareProperty( "Ele_P0_Cal",        m_P0_Cal_ele    = 0. );
    declareProperty( "Ele_P1_Cal",        m_P1_Cal_ele    = 1. );
    declareProperty( "Ele_Eta_Cal",       m_Eta_Cal_ele   = 0. );
    declareProperty( "Ele_ProbMin",       m_ProbMin_ele   = 0. ); //no cut
    declareProperty( "Ele_AverageOmega",  m_AverageOmega  = 0.33 );
    declareProperty( "Personality",       m_personality   = "Reco14");
    m_nnet = 0;
    m_util = 0;
    m_descend = 0;
    m_electron = 0;
  }

TaggerElectronTool::~TaggerElectronTool() {}

//=====================================================================
StatusCode TaggerElectronTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Ele calib ctt: P0_Cal "<<m_P0_Cal_ele
            << ", P1_Cal "<<m_P1_Cal_ele<<endreq;

  m_nnet = tool<INNetTool> ( m_NeuralNetName, this);

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  m_electron = tool<ICaloElectron>( "CaloElectron");

 // register (multiple) personalities of tag
  m_tag.registerPersonality("Reco12",this, &TaggerElectronTool::tagReco12);
  m_tag.registerPersonality("Reco14",this, &TaggerElectronTool::tagReco14);

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
Tagger TaggerElectronTool::tag( const Particle* AXB0, const RecVertex* RecVert,
                                std::vector<const Vertex*>& allVtx,
                                Particle::ConstVector& vtags )
{
  return m_tag(AXB0, RecVert, allVtx, vtags);
}

//=====================================================================
Tagger TaggerElectronTool::tagReco12( const Particle* AXB0, const RecVertex* RecVert,
                                      std::vector<const Vertex*>& allVtx,
                                      Particle::ConstVector& vtags )
{
  
  Tagger tele;
  if(!RecVert) return tele;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"-- Electron Tagger --"<<endreq;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select electron tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle::ConstVector vele(0);
  const Particle* iele=0;
  double ptmaxe = -99.0, ncand=0;
  for( Particle::ConstVector::const_iterator ipart = vtags.begin();
       ipart != vtags.end(); ++ipart ) 
  {

    const bool inHcalACC = 0 != (*ipart)->proto()->info(ProtoParticle::InAccHcal,0);
    if(!inHcalACC) continue;

    const double pide=(*ipart)->proto()->info( ProtoParticle::CombDLLe, -1000.0 );
    if(pide < m_PIDe_cut ) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Ele PIDe="<< pide <<endmsg;

    const double Pt = (*ipart)->pt();
    if( Pt < m_Pt_cut_ele )  continue;

    const double P = (*ipart)->p();
    if( P < m_P_cut_ele )  continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Ele P="<< P <<" Pt="<<Pt<<endmsg;
    
    const Track* track = (*ipart)->proto()->track();
    const double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut_ele ) continue;
    if(track->type() != Track::Long) continue;

    const double tsa = track->likelihood();
    if(tsa < m_ghost_cut_ele) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Ele lcs="<< lcs <<" tsa="<<tsa<<endmsg;

    //calculate signed IP wrt RecVert
    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr) continue;
    const double IPsig = IP/IPerr;
    if(fabs(IPsig) < m_IPs_cut_ele) continue;

    const double ippu=(*ipart)->info(LHCb::Particle::LastGlobal+1,100000.);
    if(ippu < m_ipPU_cut_ele) continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_ele ) continue;

    double eOverP  = -999;
    if( m_electron->set(*ipart) ) eOverP  = m_electron->eOverP();    

    if(eOverP > m_EoverP || eOverP<-100)
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " Elec E/P=" << eOverP <<endreq;

      const double veloch = (*ipart)->proto()->info( ProtoParticle::VeloCharge, 0.0 );
      if( veloch > m_VeloChMin && veloch < m_VeloChMax ) 
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " Elec veloch=" << veloch << endreq;

        ++ncand;

        if( Pt > ptmaxe ) 
        {
          iele = (*ipart);
          ptmaxe = Pt;
        }
      }
    }
  }
  if( !iele ) return tele;

  //calculate omega
  double pn = 1-m_AverageOmega;
  if(m_CombinationTechnique == "NNet") 
  {

    double IP(0), IPerr(0);
    m_util->calcIP(iele, RecVert, IP, IPerr);

    std::vector<double> NNinputs(10);
    //    NNinputs.at(0) = m_util->countTracks(vtags);
    NNinputs.at(1) = AXB0->pt()/GeV;;
    NNinputs.at(2) = iele->p()/GeV;
    NNinputs.at(3) = iele->pt()/GeV;
    //    NNinputs.at(4) = fabs(IP/IPerr);
    NNinputs.at(4) = IP/IPerr;
    NNinputs.at(8) = allVtx.size();

    pn = m_nnet->MLPe( NNinputs );
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Elec pn=" << pn << endreq;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_ele - m_P1_Cal_ele * ( (1-pn)-m_Eta_Cal_ele);
    if ( msgLevel(MSG::DEBUG) )
      debug() << " Elec pn="<< pn <<" w="<<1-pn<<endmsg;
    if( pn < 0 || pn > 1 ) return tele;
    if( pn < m_ProbMin_ele ) return tele;

  }

  tele.setOmega( 1-pn );
  tele.setDecision(iele->charge()>0 ? -1: 1);
  tele.addToTaggerParts(iele);
  tele.setType( Tagger::OS_Electron );

  return tele;
}

//====================================================================
Tagger TaggerElectronTool::tagReco14( const Particle* AXB0, const RecVertex* RecVert,
                                      std::vector<const Vertex*>& allVtx,
                                      Particle::ConstVector& vtags )
{
  Tagger tele;
  if(!RecVert) return tele;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"-- Electron Tagger --"<<endreq;
  
  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select electron tagger(s)
  //if more than one satisfies cuts, take the highest Pt one
  Particle::ConstVector vele(0);
  const Particle* iele=0;
  Float_t ptmaxe = -99.0,  save_IPs=-10, save_IPPU=-10., 
    //save_PIDe=-10.,   save_PIDNNe=-10.,  save_PIDNNk=-10., 
    //save_PIDNNp=-10., save_PIDNNpi=-10., save_veloch = -10., 
    save_eOverP=-10,    save_ghostProb=-10.;
  
  for( Particle::ConstVector::const_iterator ipart = vtags.begin();
       ipart != vtags.end(); ++ipart ) 
  {
    const Particle*      axp   = (*ipart);
    const ProtoParticle* proto = (*ipart)->proto();
    const Track*         track = (*ipart)->proto()->track();
    
    bool inHcalACC= proto->info(ProtoParticle::InAccHcal, false);
    if(!inHcalACC)                                         continue;

    if(track->type() != Track::Long)                       continue;

    const double lcs = track->chi2PerDoF();
    if( lcs > m_lcs_cut_ele )                              continue;

    const double tsa = track->likelihood();
    if(tsa < m_ghost_cut_ele)                              continue;

    if(track->ghostProbability() >  m_ghostProb_ele )      continue;    

    if(proto->muonPID() != 0) 
      if(proto->muonPID()->IsMuon() != 0 )                 continue;
    
    if(axp->info(LHCb::Particle::LastGlobal+2,-1.) > 0 )    continue; // already tagger by muon or kaon
    
    const double PIDNNpi = proto->info( ProtoParticle::ProbNNpi, -1000.0);    
    if(PIDNNpi > m_PIDNNpi_cut_ele  )                      continue;

    const double PIDNNp = proto->info( ProtoParticle::ProbNNp, -1000.0);    
    if(PIDNNp >  m_PIDNNp_cut_ele )                        continue;     

    const double PIDNNk = proto->info( ProtoParticle::ProbNNk, -1000.0);    
    if(PIDNNk >  m_PIDNNk_cut_ele )                        continue;     

    const double PIDNNe = proto->info( ProtoParticle::ProbNNe, -1000.0);    
    if(PIDNNe <  m_PIDNNe_cut_ele )                        continue;

    if(PIDNNe-PIDNNpi <  m_PIDNNepi_cut_ele )              continue;

    const double pide=proto->info( ProtoParticle::CombDLLe, -1000.0 );
    if(pide < m_PIDe_cut )                                 continue;

    const double Pt = axp->pt();
    if( Pt < m_Pt_cut_ele )                                continue;

    const double P = axp->p();
    if( P < m_P_cut_ele )                                  continue;

    //calculate signed IP wrt RecVert
    double IP(0.), IPerr(0.);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    if(!IPerr)                                             continue;
    const double IPsig = IP/IPerr;
    if(fabs(IPsig) < m_IPs_cut_ele)                        continue;

    const double ippu=(*ipart)->info(LHCb::Particle::LastGlobal+1,10000.);
    if(ippu < m_ipPU_cut_ele)                              continue;
    //distphi
    if( m_util->isinTree( *ipart, axdaugh, distphi ) )     continue ;//exclude signal
    if( distphi < m_distPhi_cut_ele )                      continue;

    double eOverP  = -999;
    if( m_electron->set(*ipart) ) eOverP  = m_electron->eOverP();    
 
    if(eOverP > m_EoverPmax)                               continue;
    if(eOverP > m_EoverP) {// || eOverP<-100) {
      if ( msgLevel(MSG::VERBOSE) ) 
        verbose() << " Elec E/P=" << eOverP <<endreq;

      double veloch = proto->info( ProtoParticle::VeloCharge, 0.0 );
      if( veloch > m_VeloChMin && veloch < m_VeloChMax ) {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " Elec veloch=" << veloch << endreq;

         Particle* c = const_cast<Particle*>(*ipart);
         c->addInfo(LHCb::Particle::LastGlobal+2,11); // store the info that this particle pass the tagger selection
         
         if( Pt > ptmaxe ) { 
           iele = (*ipart);
           ptmaxe = Pt;
           save_IPs = IPsig;
           save_IPPU = ippu;
           //save_PIDe = pide; 
           //save_PIDNNe = PIDNNe; 
           //save_PIDNNk = PIDNNk; 
           //save_PIDNNp = PIDNNp; 
           //save_PIDNNpi = PIDNNpi; 
           save_eOverP = eOverP;
           //save_veloch = veloch;
           save_ghostProb = track->ghostProbability();
           
         }
      }
    }
  }
  if( !iele ) return tele;

  //calculate omega
  double sign = 1.;  
  double pn = 1-m_AverageOmega;
    std::list<std::pair<std::string, Float_t> > NNinputList;    
  if(m_CombinationTechnique == "NNet") {
    if ( msgLevel(MSG::DEBUG) ) debug()<< allVtx.size()<< endreq;

    NNinputList.push_back(std::pair<std::string, Float_t>("mult",   m_util->countTracks(vtags)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partP",  log(iele->p()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partPt", log(iele->pt()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("ptB",    log(AXB0->pt()/GeV)));
    NNinputList.push_back(std::pair<std::string, Float_t>("IPs",    log(save_IPs)));
    NNinputList.push_back(std::pair<std::string, Float_t>("partlcs",log(iele->proto()->track()->chi2PerDoF())));
    NNinputList.push_back(std::pair<std::string, Float_t>("eOverP", save_eOverP));
    NNinputList.push_back(std::pair<std::string, Float_t>("ghostProb",log(save_ghostProb)));
    NNinputList.push_back(std::pair<std::string, Float_t>("IPPU",   log(save_IPPU)));

    
    pn = m_nnet->MLPeTMVA( NNinputList ); 

    if(msgLevel(MSG::VERBOSE)) verbose() << " Elec pn=" << pn << endreq;

    //Calibration (w=1-pn) w' = p0 + p1(w-eta)
    pn = 1 - m_P0_Cal_ele - m_P1_Cal_ele * ( (1-pn)-m_Eta_Cal_ele);

    if( pn < 0 || pn > 1 ) {
      if ( msgLevel(MSG::DEBUG) )      
        debug()<<" ===> Something wrong with MUON Training "<<pn<<endmsg;
      return tele;
    }
    
    if( pn < m_ProbMin_ele ) return tele;
    if(pn < 0.5){
      pn = 1. - pn;
      sign = -1.;
    }

  }

  if ( msgLevel(MSG::DEBUG) )
  {    
    debug()<<" TaggerElectron: "<<sign*iele->charge()<<" omega="<<1-pn<<" NNinputs:";
    
    typedef std::list<std::pair<std::string, Float_t> > ParListType;
    for(ParListType::const_iterator p = NNinputList.begin(); p != NNinputList.end(); p++) {
      debug() << p->second<<" ";
    }  
    debug()<<endmsg;
  }
  

  tele.setOmega( 1-pn );
  tele.setDecision(sign*iele->charge()>0 ? -1: 1);
  tele.addToTaggerParts(iele);
  tele.setType( Tagger::OS_Electron );

  return tele;
}
