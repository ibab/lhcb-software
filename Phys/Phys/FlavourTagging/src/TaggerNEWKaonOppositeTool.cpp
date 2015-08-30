// Include files 
#include "TaggerNEWKaonOppositeTool.h"
#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerNEWKaonOppositeTool
//
// Author: Katharina Kreplin
// 2013-01-23
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerNEWKaonOppositeTool )

//====================================================================
  TaggerNEWKaonOppositeTool::TaggerNEWKaonOppositeTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName", m_NeuralNetName    = "NNetTool_MLP" );

  // preselection
  declareProperty( "Kaon_PIDk_cut",     m_PID_k_cut        =  0.75);
  declareProperty( "Kaon_PIDkp_cut",    m_PIDkp_cut        = -3. );
  declareProperty( "Kaon_distPhi_cut",  m_distPhi_cut      = 0.005 );
  declareProperty( "Kaon_tsa_cut",      m_tsa_cut          = -999.0 );

  declareProperty( "Kaon_P0_Cal",        m_P0_Cal_kaon   = 0.4349 );
  declareProperty( "Kaon_P1_Cal",        m_P1_Cal_kaon   = 1.074 );
  declareProperty( "Kaon_AverageOmega",  m_AverageOmega  = 0.4350 );
  declareProperty( "Kaon_ProbMin",       m_ProbMin_kaon  = 0.5    ); // no cut

  declareProperty( "Kaon_ghost_cut",  m_ghost_cut          = 0.5 );
  declareProperty( "Kaon_ipPU_cut",   m_ipPU_cut_kaon      = 3.0 ); // no cut
  declareProperty( "Kaon_NN1_cut",    m_NN1_cut_kaon       = 0.6 ); // tightened
  declareProperty( "Personality",     m_personality        = "Reco14");

  m_nn_1      = 0.;
  m_nn_2      = 0.;
  m_nn_2_flip = 0.;

  m_util = 0;
  m_descend = 0;

}

TaggerNEWKaonOppositeTool::~TaggerNEWKaonOppositeTool() {}

//=====================================================================
StatusCode TaggerNEWKaonOppositeTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "NEW NN KOS calib ctt: P0_Cal "<<m_P0_Cal_kaon
            << ", P1_Cal "<<m_P1_Cal_kaon<<endmsg;

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
 
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  // register (multiple) personalities of tag
  m_tag.registerPersonality("Reco12",this, &TaggerNEWKaonOppositeTool::tagReco12);
  m_tag.registerPersonality("Reco14",this, &TaggerNEWKaonOppositeTool::tagReco14);
  // select personality
  // (this throws an exception if the personality chosen by the user is not
  // known)
  try {
    m_tag.setPersonality(m_personality);
  } catch (const std::exception& e) {
    error() << "Caught exception while setting TaggerNEWKaonOppositeTool::tag "
      "personality: " << e.what() << endmsg;
    return StatusCode::FAILURE;
  };

  //------ Reco 12 ------------------------------------

  std::vector<std::string> variable_names;
  variable_names.push_back("log(k_p)");
  variable_names.push_back("log(k_pt)");
  variable_names.push_back("log(abs(k_ipsig))");
  variable_names.push_back("log(abs(k_ip))");
  variable_names.push_back("log(k_lcs)");
  variable_names.push_back("k_diff_eta");
  variable_names.push_back("k_delta_phi");
  variable_names.push_back("log(B_pt)");
  variable_names.push_back("log(cands)");
  variable_names.push_back("no_vtx");

  mymc_reader = new MCReaderCompileWrapper(variable_names);

  variable_names.clear();

  variable_names.push_back("sign_tag[0]*rnet_opp[0]");
  variable_names.push_back("sign_tag[1]*rnet_opp[1]");
  variable_names.push_back("sign_tag[2]*rnet_opp[2]");
  variable_names.push_back("sign_tag[3]*rnet_opp[3]");
  variable_names.push_back("sign_tag[0]*pidk[0]");
  variable_names.push_back("sign_tag[1]*pidk[1]");
  variable_names.push_back("sign_tag[2]*pidk[2]");
  variable_names.push_back("sign_tag[3]*pidk[3]");
  variable_names.push_back("B_pt");
  variable_names.push_back("no_vtx");
  variable_names.push_back("cands");

  mydata_reader = new DataReaderCompileWrapper(variable_names);
  variable_names.clear();

  //------ Reco 14 ------------------------------------

  std::vector<std::string> variable_names_reco14;
  variable_names_reco14.push_back("log(k_p)");
  variable_names_reco14.push_back("log(k_pt)");
  variable_names_reco14.push_back("log(abs(k_ipsig))");
  variable_names_reco14.push_back("log(abs(k_ip))");
  variable_names_reco14.push_back("log(k_lcs)");
  variable_names_reco14.push_back("log(k_ghost)");
  variable_names_reco14.push_back("k_diff_eta");
  variable_names_reco14.push_back("k_delta_phi");
  variable_names_reco14.push_back("log(B_pt)");
  variable_names_reco14.push_back("log(cands)");
  variable_names_reco14.push_back("no_vtx");

  mymc_reader_reco14 = new MCReaderCompileWrapper_reco14(variable_names_reco14);

  variable_names_reco14.clear();

  variable_names_reco14.push_back("sign_tag[0]*rnet_opp[0]");
  variable_names_reco14.push_back("sign_tag[1]*rnet_opp[1]");
  variable_names_reco14.push_back("sign_tag[2]*rnet_opp[2]");
  variable_names_reco14.push_back("sign_tag[0]*k_pidNNk[0]");
  variable_names_reco14.push_back("sign_tag[1]*k_pidNNk[1]");
  variable_names_reco14.push_back("sign_tag[2]*k_pidNNk[2]");
  variable_names_reco14.push_back("B_pt");
  variable_names_reco14.push_back("no_vtx");
  variable_names_reco14.push_back("cands");

  mydata_reader_reco14 = new DataReaderCompileWrapper_reco14(variable_names_reco14);
  variable_names_reco14.clear();

  return sc;
}

//=====================================================================
Tagger TaggerNEWKaonOppositeTool::tag( const Particle* AXB0,
                                       const RecVertex* RecVert,
                                       const int nPV,
                                       Particle::ConstVector& vtags )
{
  return m_tag(AXB0, RecVert, nPV, vtags);
}
//=====================================================================
Tagger TaggerNEWKaonOppositeTool::tagReco12( const Particle* AXB0,
                                             const RecVertex* RecVert,
                                             const int nPV,
                                             Particle::ConstVector& vtags )
{

  Tagger tkaon;
  if(!RecVert) return tkaon;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--NEW NN Kaon OppositeSide Tagger on Reco 12--"<<endmsg;

  const double B_Pt     = AXB0->pt();
  const double B_eta    = AXB0->momentum().Eta();
  const double B_phi    = AXB0->momentum().Phi();
  const int    no_vtx   = nPV;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaon opposite tagger(s)
  Particle const * ikaon=0;
  int cands = 0;
  Particle::ConstVector::const_iterator ipart;
  //Particle::ConstVector::iterator ipart;

  Particle::ConstVector vtags_sel;

  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ )
  { // presel before NN1

    const double pidk = (*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    if(pidk < m_PID_k_cut ) continue;
    if(pidk==0 || pidk==-1000.0) continue;

    const double pidproton = (*ipart)->proto()->info(ProtoParticle::CombDLLp, -1000.0);
    if( pidk - pidproton < m_PIDkp_cut ) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon PIDk="<< pidk <<" Dkp="<<pidk - pidproton<<endmsg;

    const double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    if (cloneDist!=-1) continue;

    const double tsa = (*ipart)->proto()->track()->likelihood();
    if( tsa < m_tsa_cut ) continue;

    if( tsa > m_ghost_cut ) continue;
    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut ) continue;

    const int type =(*ipart)->proto()->track()->type();
    if( type != 3 )   continue;

    ////////////////////////////////
    vtags_sel.push_back(*ipart);         // store presel tagger candidate
    ////////////////////////////////

    ++cands;
  }

  double cands_nn_1 = cands;

  typedef std::pair <double, int> myPair;
  typedef std::pair <double, Particle::ConstVector::const_iterator > myPair_ev;
  std::vector<myPair> myMap;
  std::vector<myPair_ev> event_map;

  const int max_tracks = 50;

  std::tr1::array<double, max_tracks> pre_sign_tag;
  std::tr1::array<double, max_tracks> pre_rnet_opp;
  std::tr1::array<double, max_tracks> pre_pidk;

  std::tr1::array<double, max_tracks> pos_sign_tag;
  std::tr1::array<double, max_tracks> pos_rnet_opp;
  std::tr1::array<double, max_tracks> pos_pidk;

  int count = 0;

  for( ipart = vtags_sel.begin(); ipart != vtags_sel.end(); ipart++ ) { // do NN1 sel

    const double Pt = (*ipart)->pt();
    const double P = (*ipart)->p();
    const double lcs = (*ipart)->proto()->track()->chi2PerDoF();
    const double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    IP = fabs(IP);
    if(!IPerr) continue;
    const double IPsig = fabs(IP/IPerr);
    const double ippu=(*ipart)->info(LHCb::Particle::FlavourTaggingIPPUs,100000.);

    if(ippu < m_ipPU_cut_kaon)  continue;

    const double eta = (*ipart)->momentum().Eta();
    const double phi = (*ipart)->momentum().Phi();

    const double diff_eta = B_eta - eta;
    const double delta_phi = TaggingHelpers::dphi(B_phi, phi);

    std::vector<double> values;
    values.push_back(log(P)          );
    values.push_back(log(Pt)         );
    values.push_back(log(IPsig)      );
    values.push_back(log(IP)         );
    values.push_back(log(lcs)        );
    values.push_back(diff_eta        );
    values.push_back(delta_phi       );
    values.push_back(log(B_Pt)       );
    values.push_back(log(cands_nn_1) );
    values.push_back(no_vtx          );

    m_nn_1 = mymc_reader->GetMvaValue(values);

    if ( msgLevel(MSG::DEBUG)) {      
      debug()<<" TaggerNNetOSK NN1="<<m_nn_1<<" NNinputs:";
      for(unsigned int i=0; i<values.size(); ++i) debug() << values.at(i)<<" ";      
      debug()<<endmsg;
    }
    



    if(m_nn_1 < m_NN1_cut_kaon) continue;

    const myPair pair = std::make_pair(m_nn_1, count); // sort tracks by NN1 output, highest first
    myMap.push_back(pair);
    if ( msgLevel(MSG::DEBUG) )
      debug() << " " << pair.first << " " << pair.second << endmsg;

    pre_sign_tag.at(count) = (*ipart)->charge();
    pre_rnet_opp.at(count) = m_nn_1;
    pre_pidk.at(count)     = pidk;

    ++count;

    const myPair_ev ev_pair = std::make_pair(m_nn_1, ipart); // find highest nn1 track
    event_map.push_back(ev_pair);

  }

  count = 0;

  cands = myMap.size();
  double cands_nn_2 = cands;

  if ( msgLevel(MSG::DEBUG) )
    debug() << " after NN cut " << cands << endmsg;

  int    my_os_k_dec = 0;
  double my_os_k_eta = -0.1;

  if(cands == 0) return tkaon;

  else
  {
    std::stable_sort(myMap.begin(), myMap.end(), std::greater<myPair>());
    for(unsigned i =0; i < (unsigned)cands; i++)
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << " <map 1> " << myMap[i].first << " <map 2> " << myMap[i].second << endmsg;
      pos_rnet_opp.at(i) = pre_rnet_opp.at(myMap[i].second);
      pos_sign_tag.at(i) = pre_sign_tag.at(myMap[i].second);
      pos_pidk.at(i)     = pre_pidk.at(myMap[i].second);
    }

    if(cands < max_tracks)
    {
      for(int i=0; i < (max_tracks-cands); ++i)
      {
        pos_rnet_opp.at(cands+i) = 0.;
        pos_sign_tag.at(cands+i) = 0.;
        pos_pidk.at(cands+i)     = 0.;
      }
    }

    std::vector<double> values;
    std::vector<double> values_2;
    values.push_back(pos_sign_tag.at(0)*pos_rnet_opp.at(0));
    values.push_back(pos_sign_tag.at(1)*pos_rnet_opp.at(1));
    values.push_back(pos_sign_tag.at(2)*pos_rnet_opp.at(2));
    values.push_back(pos_sign_tag.at(3)*pos_rnet_opp.at(3));
    values.push_back(pos_sign_tag.at(0)*pos_pidk.at(0)    );
    values.push_back(pos_sign_tag.at(1)*pos_pidk.at(1)    );
    values.push_back(pos_sign_tag.at(2)*pos_pidk.at(2)    );
    values.push_back(pos_sign_tag.at(3)*pos_pidk.at(3)    );
    values.push_back(B_Pt                                 );
    values.push_back(no_vtx                               );
    values.push_back(cands_nn_2                           );

    values_2.push_back((-1.) * pos_sign_tag.at(0)*pos_rnet_opp.at(0));
    values_2.push_back((-1.) * pos_sign_tag.at(1)*pos_rnet_opp.at(1));
    values_2.push_back((-1.) * pos_sign_tag.at(2)*pos_rnet_opp.at(2));
    values_2.push_back((-1.) * pos_sign_tag.at(3)*pos_rnet_opp.at(3));
    values_2.push_back((-1.) * pos_sign_tag.at(0)*pos_pidk.at(0)    );
    values_2.push_back((-1.) * pos_sign_tag.at(1)*pos_pidk.at(1)    );
    values_2.push_back((-1.) * pos_sign_tag.at(2)*pos_pidk.at(2)    );
    values_2.push_back((-1.) * pos_sign_tag.at(3)*pos_pidk.at(3)    );
    values_2.push_back(B_Pt                                         );
    values_2.push_back(no_vtx                                       );
    values_2.push_back(cands_nn_2                                   );

    m_nn_2 = mydata_reader->GetMvaValue(values);

    m_nn_2_flip = mydata_reader->GetMvaValue(values_2);

    if ( msgLevel(MSG::DEBUG) ) {      
      debug()<<" TaggerNNetOSK NN2="<<m_nn_2<<" & "<<m_nn_2_flip<<" NNinputs:";
      for(unsigned int i=0; i<values_2.size(); ++i) debug() << values_2.at(i)<<" ";      
      debug()<<endmsg;
    }

	// Interpret the conditional probability right:
	// important and significant, if sample split in B or anti-B categories
    my_os_k_eta = (m_nn_2 + (1. - m_nn_2_flip)) / 2.;

    if ( msgLevel(MSG::DEBUG) )
      debug () << " NN 2     " << my_os_k_eta << endmsg;

    myMap.clear();

    // event_map is a vector, so let's sort it descending...
    std::stable_sort( event_map.rbegin(), event_map.rend() );
    ikaon = *event_map[0].second;

    event_map.clear();
  }

  if( ! ikaon ) return tkaon;

  if(my_os_k_eta > (1. - m_ProbMin_kaon))
    my_os_k_dec = 1;

  if(my_os_k_eta < m_ProbMin_kaon)
    my_os_k_dec = -1;

  if(my_os_k_eta > 0.5)
    my_os_k_eta = 1. - my_os_k_eta;

  // set tag decision before calibration:
  // same decision as used to determine the calibration parameters!
  my_os_k_eta = m_P0_Cal_kaon + m_P1_Cal_kaon * (my_os_k_eta  - m_AverageOmega);

  if( my_os_k_dec == 0 ) return tkaon;
  

  tkaon.setOmega( my_os_k_eta );
  tkaon.setDecision( my_os_k_dec );
  tkaon.setType( Tagger::OS_nnetKaon);

  for (int i=0; i<cands_nn_2; i++){    
    tkaon.addToTaggerParts(*event_map[i].second);
  }

  return tkaon;
}
//=====================================================================
Tagger TaggerNEWKaonOppositeTool::tagReco14( const Particle* AXB0,
                                             const RecVertex* RecVert,
                                             const int nPV,
                                             Particle::ConstVector& vtags )
{

  Tagger tkaon;
  if(!RecVert) return tkaon;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--NEW NN Kaon OppositeSide Tagger on Reco 14--"<<endmsg;

  const double B_Pt     = AXB0->pt();
  const double B_eta    = AXB0->momentum().Eta();
  const double B_phi    = AXB0->momentum().Phi();
  const int    no_vtx   = nPV;

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaon opposite tagger(s)
  Particle const * ikaon=0;
  int cands = 0;
  Particle::ConstVector::const_iterator ipart;
  //Particle::ConstVector::iterator ipart;

  Particle::ConstVector vtags_sel;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ )
  { // presel before NN1

    const double pidk = (*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    if(pidk < m_PID_k_cut ) continue;
    if(pidk==0 || pidk==-1000.0) continue;

    const double pidproton = (*ipart)->proto()->info(ProtoParticle::CombDLLp, -1000.0);
    if( pidk - pidproton < m_PIDkp_cut ) continue;

    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " Kaon PIDk="<< pidk <<" Dkp="<<pidk - pidproton<<endmsg;

    const double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    if (cloneDist!=-1) continue;

    const double tsa = (*ipart)->proto()->track()->likelihood();
    if( tsa < m_tsa_cut ) continue;

    if((*ipart)->proto()->track()->ghostProbability() >  m_ghost_cut ) continue;

    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut ) continue;

    const int type =(*ipart)->proto()->track()->type();
    if( type != 3 )   continue;

    ////////////////////////////////
    vtags_sel.push_back(*ipart);         // store presel tagger candidate
    ////////////////////////////////

    ++cands;
  }
  

  double cands_nn_1 = cands;

  typedef std::pair <double, int> myPair;
  typedef std::pair <double, Particle::ConstVector::const_iterator > myPair_ev;
  std::vector<myPair> myMap;
  std::vector<myPair_ev> event_map;

  const int max_tracks = 50;

  std::tr1::array<double, max_tracks> pre_sign_tag;
  std::tr1::array<double, max_tracks> pre_rnet_opp;
  std::tr1::array<double, max_tracks> pre_pidk;

  std::tr1::array<double, max_tracks> pos_sign_tag;
  std::tr1::array<double, max_tracks> pos_rnet_opp;
  std::tr1::array<double, max_tracks> pos_pidk;

  int count = 0;

  for( ipart = vtags_sel.begin(); ipart != vtags_sel.end(); ipart++ ) { // do NN1 sel

    const double Pt = (*ipart)->pt();
    const double P = (*ipart)->p();
    const double lcs = (*ipart)->proto()->track()->chi2PerDoF();
    const double pidNNk = (*ipart)->proto()->info( ProtoParticle::ProbNNk, -1000.0);

    double IP(0), IPerr(0);
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    IP = fabs(IP);
    if(!IPerr) continue;
    const double IPsig = fabs(IP/IPerr);
    const double ippu=(*ipart)->info(LHCb::Particle::FlavourTaggingIPPUs,100000.);

    if(ippu < m_ipPU_cut_kaon)  continue;

    const double eta = (*ipart)->momentum().Eta();
    const double phi = (*ipart)->momentum().Phi();

    const double diff_eta = B_eta - eta;
    const double delta_phi = TaggingHelpers::dphi(B_phi, phi);

    const double ghost_prob = (*ipart)->proto()->track()->ghostProbability();

    std::vector<double> values;
    values.push_back(log(P)          );
    values.push_back(log(Pt)         );
    values.push_back(log(IPsig)      );
    values.push_back(log(IP)         );
    values.push_back(log(lcs)        );
    values.push_back(log(ghost_prob) );
    values.push_back(diff_eta        );
    values.push_back(delta_phi       );
    values.push_back(log(B_Pt)       );
    values.push_back(log(cands_nn_1) );
    values.push_back(no_vtx          );

    m_nn_1 = mymc_reader_reco14->GetMvaValue(values);

    if ( msgLevel(MSG::DEBUG)) {      
      debug()<<" TaggerNNetOSK NN1="<<m_nn_1<<" NNinputs:";
      for(unsigned int i=0; i<values.size(); ++i) debug() << values.at(i)<<" ";      
      debug()<<endmsg;
    }

    if(m_nn_1 < m_NN1_cut_kaon) continue;

    const myPair pair = std::make_pair(m_nn_1, count); // sort tracks by NN1 output, highest first
    myMap.push_back(pair);
    if ( msgLevel(MSG::DEBUG) )
      debug() << " " << pair.first << " " << pair.second << endmsg;

    pre_sign_tag.at(count) = (*ipart)->charge();
    pre_rnet_opp.at(count) = m_nn_1;
    pre_pidk.at(count)     = pidNNk;

    ++count;

    const myPair_ev ev_pair = std::make_pair(m_nn_1, ipart); // find highest nn1 track
    event_map.push_back(ev_pair);

  }
  if ( msgLevel(MSG::DEBUG))   
    debug()<< vtags.size()<<
      " selected x NN1 "<<vtags_sel.size()<<
      " selected x NN2 "<<event_map.size()<<endmsg;

  if ( msgLevel(MSG::DEBUG) )
    debug() << " vtags_sel.size()="<<  vtags_sel.size() <<" myMap.size()"<<myMap.size()<<endmsg;

  count = 0;

  cands = myMap.size();
  double cands_nn_2 = cands;

  if ( msgLevel(MSG::DEBUG) )
    debug() << " after NN cut " << cands << endmsg;

  int    my_os_k_dec = 0;
  double my_os_k_eta = -0.1;

  if(cands == 0) return tkaon;

  else
  {
    std::stable_sort(myMap.begin(), myMap.end(), std::greater<myPair>());
    for(unsigned i =0; i < (unsigned)cands; i++)
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << " <map 1> " << myMap[i].first << " <map 2> " << myMap[i].second << endmsg;
      pos_rnet_opp.at(i) = pre_rnet_opp.at(myMap[i].second);
      pos_sign_tag.at(i) = pre_sign_tag.at(myMap[i].second);
      pos_pidk.at(i)     = pre_pidk.at(myMap[i].second);
    }

    if(cands < max_tracks)
    {
      for(int i=0; i < (max_tracks-cands); ++i)
      {
        pos_rnet_opp.at(cands+i) = 0.;
        pos_sign_tag.at(cands+i) = 0.;
        pos_pidk.at(cands+i)     = 0.;
      }
    }

    std::vector<double> values;
    std::vector<double> values_2;
    values.push_back(pos_sign_tag.at(0)*pos_rnet_opp.at(0));
    values.push_back(pos_sign_tag.at(1)*pos_rnet_opp.at(1));
    values.push_back(pos_sign_tag.at(2)*pos_rnet_opp.at(2));
    values.push_back(pos_sign_tag.at(0)*pos_pidk.at(0)    );
    values.push_back(pos_sign_tag.at(1)*pos_pidk.at(1)    );
    values.push_back(pos_sign_tag.at(2)*pos_pidk.at(2)    );
    values.push_back(B_Pt                                 );
    values.push_back(no_vtx                               );
    values.push_back(cands_nn_2                           );

    values_2.push_back((-1.) * pos_sign_tag.at(0)*pos_rnet_opp.at(0));
    values_2.push_back((-1.) * pos_sign_tag.at(1)*pos_rnet_opp.at(1));
    values_2.push_back((-1.) * pos_sign_tag.at(2)*pos_rnet_opp.at(2));
    values_2.push_back((-1.) * pos_sign_tag.at(0)*pos_pidk.at(0)    );
    values_2.push_back((-1.) * pos_sign_tag.at(1)*pos_pidk.at(1)    );
    values_2.push_back((-1.) * pos_sign_tag.at(2)*pos_pidk.at(2)    );
    values_2.push_back(B_Pt                                         );
    values_2.push_back(no_vtx                                       );
    values_2.push_back(cands_nn_2                                   );

    m_nn_2 = mydata_reader_reco14->GetMvaValue(values);
    
    m_nn_2_flip = mydata_reader_reco14->GetMvaValue(values_2);

    if ( msgLevel(MSG::DEBUG) ) {      
      debug()<<" TaggerNNetOSK NN2="<<m_nn_2<<" & "<<m_nn_2_flip<<" NNinputs:";
      for(unsigned int i=0; i<values_2.size(); ++i) debug() << values_2.at(i)<<" ";      
      debug()<<endmsg;
    }


	// Interpret the conditional probability right:
	// important and significant, if sample split in B or anti-B categories
    my_os_k_eta = (m_nn_2 + (1. - m_nn_2_flip)) / 2.;

    if ( msgLevel(MSG::DEBUG) )
      debug () << " NN 2     " << my_os_k_eta << endmsg;

    myMap.clear();

    // event_map is a vector, so let's sort it descending...
    std::stable_sort( event_map.rbegin(), event_map.rend() );
    ikaon = *event_map[0].second;

    event_map.clear();
  }

  if( ! ikaon ) return tkaon;

  if(my_os_k_eta > (1. - m_ProbMin_kaon))
    my_os_k_dec = 1;

  if(my_os_k_eta < m_ProbMin_kaon)
    my_os_k_dec = -1;

  if(my_os_k_eta > 0.5) // fold first, than calibrate
    my_os_k_eta = 1. - my_os_k_eta;

  // set tag decision before calibration:
  // same decision as used to determine the calibration parameters!
  my_os_k_eta = m_P0_Cal_kaon + m_P1_Cal_kaon * (my_os_k_eta  - m_AverageOmega);


  tkaon.setOmega( my_os_k_eta );
  tkaon.setDecision( my_os_k_dec );
  tkaon.setType( Tagger::OS_nnetKaon);

  for (int i=0; i<cands_nn_2; i++){    
    tkaon.addToTaggerParts(*event_map[i].second);
  }
  
  if ( msgLevel(MSG::DEBUG) )
    debug() << " NNetOSK decision="<<  my_os_k_dec <<" omega="<< my_os_k_eta<<endmsg;

  return tkaon;
}

