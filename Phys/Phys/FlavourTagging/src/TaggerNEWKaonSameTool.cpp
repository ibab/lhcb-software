// Include files 
#include "TaggerNEWKaonSameTool.h"
#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerNEWKaonSameTool
//
// Author: Katharina Kreplin
// 2013-01-23
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerNEWKaonSameTool )

//====================================================================
  TaggerNEWKaonSameTool::TaggerNEWKaonSameTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
  declareInterface<ITagger>(this);

  declareProperty( "CombTech",  m_CombinationTechnique = "NNet" );
  declareProperty( "NeuralNetName", m_NeuralNetName    = "NNetTool_MLP" );

  // preselection  for Reco14
  declareProperty( "Kaon_PIDk_cut",     m_PID_k_cut     =  0.75);
  declareProperty( "Kaon_PIDkp_cut",    m_PIDkp_cut     = -8.5 );
  declareProperty( "Kaon_ghost_cut",    m_ghost_cut     = 10. );     // no cut Reco 12 & Reco14
  declareProperty( "Kaon_distPhi_cut",  m_distPhi_cut_kaon= 0.005 );

  declareProperty( "Kaon_P0_Cal",        m_P0_Cal_kaon   = 0.436 ); // 0.408 Reco12
  declareProperty( "Kaon_P1_Cal",        m_P1_Cal_kaon   = 0.90  );    // 0.86 Reco12
  declareProperty( "Kaon_AverageOmega",  m_AverageOmega  = 0.430 ); // 0.4302 Reco12
  declareProperty( "Kaon_ProbMin",       m_ProbMin_kaon  = 0.5 );
  declareProperty( "Kaon_ProbMax",       m_ProbMax_kaon  = 0.5 );

  //declareProperty( "Kaon_ipPU_cut", m_ipPU_cut_kaon      = 7.5 );
  declareProperty( "Kaon_NN1_cut",  m_NN1_cut_kaon       = 0.65 );  // Reco12 & Reco14

  m_nn_1      = 0.;
  m_nn_2      = 0.;

  m_util = 0;
  m_descend = 0;

}

TaggerNEWKaonSameTool::~TaggerNEWKaonSameTool() {}

//=====================================================================
StatusCode TaggerNEWKaonSameTool::initialize() 
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "NEW NN KSS calib ctt: P0_Cal "<<m_P0_Cal_kaon
            << ", P1_Cal "<<m_P1_Cal_kaon<<endmsg;

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
 
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );

  std::vector<std::string> variable_names;
  variable_names.push_back("log(ntrack)");
  variable_names.push_back("npv");
  variable_names.push_back("log(ptB)");
  variable_names.push_back("log(ptT)");
  variable_names.push_back("log(ptrel)");
  variable_names.push_back("log(pT)");
  variable_names.push_back("log(dEta)");
  variable_names.push_back("log(dphi)");
  variable_names.push_back("log(ip)");
  variable_names.push_back("log(ipSig)");
  variable_names.push_back("log(lcs)");

  mynn1_reader = new NN1ReaderCompileWrapper(variable_names);

  variable_names.clear();

  variable_names.push_back("ntrack");
  variable_names.push_back("npv");
  variable_names.push_back("log(ptB)");
  variable_names.push_back("nn0");
  variable_names.push_back("pid0");
  variable_names.push_back("nn1");
  variable_names.push_back("pid1");
  variable_names.push_back("nn2");
  variable_names.push_back("pid2");

  mynn2_reader = new NN2ReaderCompileWrapper(variable_names);
  variable_names.clear();

  return sc;
}

//=====================================================================
Tagger TaggerNEWKaonSameTool::tag( const Particle* AXB0,
                                   const RecVertex* RecVert,
                                   const int nPV,
                                   Particle::ConstVector& vtags )
{

  Tagger tkaon;
  if(!RecVert) return tkaon;

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<<"--NEW NN Kaon SameSide Tagger--"<<endmsg;

  const double B_Pt     = AXB0->pt();
  const int    no_vtx   = nPV;
  Gaudi::LorentzVector ptotB = AXB0->momentum();

  //fill auxdaugh for distphi
  double distphi;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB0 );
  axdaugh.push_back( AXB0 );
  //select kaon opposite tagger(s)
  Particle const * ikaon=0;
  int ncand = 0;
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

    //const double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    //if (cloneDist!=-1) continue;

    const double ghostProb = (*ipart)->proto()->track()->ghostProbability();
    if( ghostProb > m_ghost_cut ) continue;

    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_kaon ) continue;

    const int type =(*ipart)->proto()->track()->type();
    if( type != 3 )   continue;

    // in future should be changed: use TaggingHelpers:dphi BUT FOR NOW NOT: 
    //need to check the sign and the NNEt is stable
    double diff_phi  = fabs((*ipart)->momentum().Phi() - AXB0->momentum().Phi());
    if(diff_phi>3.1416) diff_phi=6.2832-diff_phi;
    if(diff_phi>=1.5) continue; 

    ////////////////////////////////
    vtags_sel.push_back(*ipart);         // store presel tagger candidate
    ////////////////////////////////

    ++ncand;
  }

  int cands = vtags_sel.size();

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

  for( ipart = vtags_sel.begin(); ipart != vtags_sel.end(); ipart++ ) 
  { // do NN1 sel
  
    Gaudi::LorentzVector ptotT = (*ipart)->momentum();


    const double Pt = (*ipart)->pt();
    const double P = (*ipart)->p();
    const Track* track = (*ipart)->proto()->track();
    const double lcs = track->chi2PerDoF();
    const double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    const double pLrel = (ptotB.Px() * ptotT.Px() + ptotB.Py() * ptotT.Py() + ptotB.Pz() * ptotT.Pz())/ ptotB.P();
    const double pTrel = sqrt(ptotT.P()*ptotT.P() - pLrel*pLrel );

    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
    IP = fabs(IP);
    if(!IPerr) continue;
    const double IPsig = fabs(IP/IPerr);


    const double diff_eta  = fabs(log(tan(ptotB.Theta()/2.)/tan(asin(Pt/P)/2.)));
    double diff_phi  = fabs((*ipart)->momentum().Phi() - ptotB.Phi());
    if(diff_phi>3.1416) diff_phi=6.2832-diff_phi;

    std::vector<double> values;
    values.push_back(log(cands_nn_1) );
    values.push_back(no_vtx          );
    values.push_back(log(B_Pt)       );
    values.push_back(log(Pt)         );
    values.push_back(log(pTrel)      );
    values.push_back(log(P)          );
    values.push_back(log(diff_eta)   );
    values.push_back(log(diff_phi)   );
    values.push_back(log(IP)         );
    values.push_back(log(IPsig)      );
    values.push_back(log(lcs)        );

    m_nn_1 = mynn1_reader->GetMvaValue(values);

    if ( msgLevel(MSG::DEBUG)) {      
      debug()<<" TaggerNNetSSK NN1="<<m_nn_1<<" NNinputs:";
      for(unsigned int i=0; i<values.size(); ++i) debug() << values.at(i)<<" ";      
      debug()<<endmsg;
    }
    
    //if(ippu < m_ipPU_cut_kaon)  continue;

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
      //assert(i < pos_rnet_opp.size());
      //assert(i < pre_rnet_opp.size());
      //assert(i < pos_sign_tag.size());
      //assert(i < pre_sign_tag.size());
      //assert(i < pos_pidk.size());
      //assert(i < pre_pidk.size());
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
    values.push_back(cands_nn_2                           );
    values.push_back(no_vtx                               );
    values.push_back(log(B_Pt)                            );
    values.push_back(pos_sign_tag.at(0)*pos_rnet_opp.at(0));
    values.push_back(pos_sign_tag.at(0)*pos_pidk.at(0)    );
    values.push_back(pos_sign_tag.at(1)*pos_rnet_opp.at(1));
    values.push_back(pos_sign_tag.at(1)*pos_pidk.at(1)    );
    values.push_back(pos_sign_tag.at(2)*pos_rnet_opp.at(2));
    values.push_back(pos_sign_tag.at(2)*pos_pidk.at(2)    );
    //values.push_back(pos_sign_tag.at(3)*pos_rnet_opp.at(3));
    //values.push_back(pos_sign_tag.at(3)*pos_pidk.at(3)    );

    std::vector<double> valuescc;
    valuescc.push_back(cands_nn_2                           );
    valuescc.push_back(no_vtx                               );
    valuescc.push_back(log(B_Pt)                            );
    valuescc.push_back(-1. * pos_sign_tag.at(0)*pos_rnet_opp.at(0));
    valuescc.push_back(-1. * pos_sign_tag.at(0)*pos_pidk.at(0)    );
    valuescc.push_back(-1. * pos_sign_tag.at(1)*pos_rnet_opp.at(1));
    valuescc.push_back(-1. * pos_sign_tag.at(1)*pos_pidk.at(1)    );
    valuescc.push_back(-1. * pos_sign_tag.at(2)*pos_rnet_opp.at(2));
    valuescc.push_back(-1. * pos_sign_tag.at(2)*pos_pidk.at(2)    );
    //valuescc.push_back(-1. * pos_sign_tag.at(3)*pos_rnet_opp.at(3));
    //valuescc.push_back(-1. * pos_sign_tag.at(3)*pos_pidk.at(3)    );

    double nn_2 =  mynn2_reader->GetMvaValue(values);
    double nn_2cc =  mynn2_reader->GetMvaValue(valuescc);

    if ( msgLevel(MSG::DEBUG)) {      
      debug()<<" TaggerNNetSSK NN2="<<nn_2<<" & "<<nn_2cc<<" NNinputs:";
      for(unsigned int i=0; i<valuescc.size(); ++i) debug() << valuescc.at(i)<<" ";      
      debug()<<endmsg;
    }

    m_nn_2 = (nn_2 + (1. - nn_2cc))* 0.5;

    my_os_k_eta = m_nn_2;

    if ( msgLevel(MSG::DEBUG) )
      debug () << " NN 2     " << m_nn_2 << endmsg;

    myMap.clear();

    // event_map is a vector, so let's sort it descending...
    std::stable_sort( event_map.rbegin(), event_map.rend() );
    //assert(event_map.front().first >= event_map.back().first);
    ikaon = *event_map[0].second;

    event_map.clear();
  }

  if( ! ikaon ) return tkaon;

  if(my_os_k_eta >= m_ProbMax_kaon)
    my_os_k_dec = 1;

  if(my_os_k_eta < m_ProbMin_kaon)
    my_os_k_dec = -1;

  if(my_os_k_eta > 0.5)
    my_os_k_eta = 1. - my_os_k_eta;

  my_os_k_eta = m_P0_Cal_kaon + m_P1_Cal_kaon * (my_os_k_eta  - m_AverageOmega);
  
  tkaon.setOmega( my_os_k_eta );
  tkaon.setDecision( my_os_k_dec );
  tkaon.setType( Tagger::SS_nnetKaon);
  //tkaon.addToTaggerParts(ikaon);
  for (int i=0; i<cands_nn_2; i++){    
    tkaon.addToTaggerParts(*event_map[i].second);
  }


  if ( msgLevel(MSG::DEBUG) )
    debug() << " NNetSSK decision="<<  my_os_k_dec <<" omega="<< my_os_k_eta<<endmsg;

  return tkaon;
}

