// Include files 
#include "TaggerNEWKaonOppositeTool.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggerNEWKaonOppositeTool
//
// Author: Katharina Kreplin
// 2013-01-23
//--------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggerNEWKaonOppositeTool );

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
  declareProperty( "Kaon_PIDk_cut",     m_PID_k_cut     =  0.75);
  declareProperty( "Kaon_PIDkp_cut",    m_PIDkp_cut     = -3. );
  declareProperty( "Kaon_ghost_cut",    m_ghost_cut     = -999.0 );
  declareProperty( "Kaon_distPhi_cut",  m_distPhi_cut_kaon= 0.005 );

  declareProperty( "Kaon_P0_Cal",        m_P0_Cal_kaon   = 0.4513 );
  declareProperty( "Kaon_P1_Cal",        m_P1_Cal_kaon   = 1.0514 );
  declareProperty( "Kaon_AverageOmega",  m_AverageOmega  = 0.4484 );
  declareProperty( "Kaon_ProbMin",       m_ProbMin_kaon  = 0.47 );
  declareProperty( "Kaon_ProbMax",       m_ProbMax_kaon  = 0.53 );

  declareProperty( "Kaon_ipPU_cut", m_ipPU_cut_kaon      = 7.5 );
  declareProperty( "Kaon_NN1_cut",  m_NN1_cut_kaon       = 0.4 );

  m_nn_1      = 0.;
  m_nn_2      = 0.;

  m_util = 0;
  m_descend = 0;
  
}
TaggerNEWKaonOppositeTool::~TaggerNEWKaonOppositeTool() {}; 

//=====================================================================
StatusCode TaggerNEWKaonOppositeTool::initialize() { 

  warning() << "NEW NN KOS calib ctt: P0_Cal "<<m_P0_Cal_kaon<<", P1_Cal "<<m_P1_Cal_kaon<<endreq;

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

  std::vector<std::string> variable_names;
  variable_names.push_back("log(k_p)");
  variable_names.push_back("log(k_pt)");
  variable_names.push_back("log(k_ipsig)");
  variable_names.push_back("log(k_ip)");
  variable_names.push_back("log(k_lcs)");
  variable_names.push_back("k_diff_eta");
  variable_names.push_back("k_diff_phi");
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



  return StatusCode::SUCCESS;
}

//=====================================================================
Tagger TaggerNEWKaonOppositeTool::tag( const Particle* AXB0, 
                                    const RecVertex* RecVert,
                                    std::vector<const Vertex*>& allVtx, 
                                    Particle::ConstVector& vtags ){
  Tagger tkaon;
  if(!RecVert) return tkaon;

  verbose()<<"--NEW NN Kaon OppositeSide Tagger--"<<endreq;

  double B_Pt     = AXB0->pt();
  double B_eta    = AXB0->momentum().Eta();
  double B_phi    = AXB0->momentum().Phi();
  int    no_vtx   = allVtx.size();

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

  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) { // presel before NN1

    double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    if(pidk < m_PID_k_cut ) continue;
    if(pidk==0 || pidk==-1000.0) continue;

    double pidproton = (*ipart)->proto()->info(ProtoParticle::CombDLLp, -1000.0);
    if( pidk - pidproton < m_PIDkp_cut ) continue;
    verbose() << " Kaon PIDk="<< pidk <<" Dkp="<<pidk - pidproton<<endmsg;

    double cloneDist = (*ipart)->proto()->track()->info(LHCb::Track::CloneDist, -1.);
    if (cloneDist!=-1) continue;

    double tsa = (*ipart)->proto()->track()->likelihood();
    if( tsa < m_ghost_cut ) continue;

    if( m_util->isinTree( *ipart, axdaugh, distphi ) ) continue ;//exclude signal
    if( distphi < m_distPhi_cut_kaon ) continue;

    int type =(*ipart)->proto()->track()->type();
    if( type != 3 )   continue;


    ////////////////////////////////
    vtags_sel.push_back(*ipart);         // store presel tagger candidate
    ////////////////////////////////

    ncand++;
  }

  int cands = m_util->countTracks(vtags_sel);
  assert(ncand == cands);
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


    double Pt = (*ipart)->pt();
    double P = (*ipart)->p();
    const Track* track = (*ipart)->proto()->track();
    double lcs = track->chi2PerDoF();
    double pidk=(*ipart)->proto()->info( ProtoParticle::CombDLLk, -1000.0 );

    double IP, IPerr;
    m_util->calcIP(*ipart, RecVert, IP, IPerr);
	IP = fabs(IP);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);
    double ippu=(*ipart)->info(1,100000.);


	double eta = (*ipart)->momentum().Eta();
	double phi = (*ipart)->momentum().Phi();

	double diff_eta = B_eta - eta;
	double diff_phi = B_phi - phi;

	std::vector<double> values;
	values.push_back(log(P)          );
	values.push_back(log(Pt)         );
	values.push_back(log(IPsig)      );
	values.push_back(log(IP)         );
	values.push_back(log(lcs)        );
	values.push_back(diff_eta        );
	values.push_back(diff_phi        );
	values.push_back(log(B_Pt)       );
	values.push_back(log(cands_nn_1) );
	values.push_back(no_vtx          );

	m_nn_1 = mymc_reader->GetMvaValue(values);

    if(ippu < m_ipPU_cut_kaon)  continue;

    if(m_nn_1 < m_NN1_cut_kaon) continue;

	myPair pair = std::make_pair(m_nn_1, count); // sort tracks by NN1 output, highest first
	myMap.push_back(pair);
    debug() << " " << pair.first << " " << pair.second << endmsg;

	pre_sign_tag.at(count) = (*ipart)->charge();
	pre_rnet_opp.at(count) = m_nn_1;
	pre_pidk.at(count)     = pidk;

	count ++;

	myPair_ev ev_pair = std::make_pair(m_nn_1, ipart); // find highest nn1 track
	event_map.push_back(ev_pair);

  }

  count = 0;

  cands = myMap.size();
  double cands_nn_2 = cands;

  debug() << " after NN cut " << cands << endmsg;

  int    my_os_k_dec = 0;
  double my_os_k_eta = -0.1;

  if(cands == 0) return tkaon;

  else{
	  std::sort(myMap.begin(), myMap.end(), std::greater<myPair>());
	  for(unsigned i =0; i < (unsigned)cands; i++){
		  assert(i < pos_rnet_opp.size());
		  assert(i < pre_rnet_opp.size());
		  assert(i < pos_sign_tag.size());
		  assert(i < pre_sign_tag.size());
		  assert(i < pos_pidk.size());
		  assert(i < pre_pidk.size());
		  debug() << " <map 1> " << myMap[i].first << " <map 2> " << myMap[i].second << endmsg;
		  pos_rnet_opp.at(i) = pre_rnet_opp.at(myMap[i].second);
		  pos_sign_tag.at(i) = pre_sign_tag.at(myMap[i].second);
		  pos_pidk.at(i)     = pre_pidk.at(myMap[i].second);
	  }

	  if(cands < max_tracks){
		  for(int i=0; i < (max_tracks-cands); i++){
			  pos_rnet_opp.at(cands+i) = 0.;
			  pos_sign_tag.at(cands+i) = 0.;
			  pos_pidk.at(cands+i)     = 0.;
		  }
	  }


	  std::vector<double> values;
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

	  m_nn_2 = mydata_reader->GetMvaValue(values);

	  my_os_k_eta = m_nn_2;

	  debug () << " NN 2     " << m_nn_2 << endmsg;

	  myMap.clear();

	  // event_map is a vector, so let's sort it descending...
	  std::sort(event_map.rbegin(), event_map.rend());
	  assert(event_map.front().first >= event_map.back().first);
	  ikaon = *event_map[0].second;

	  event_map.clear();
  }


  if( ! ikaon ) return tkaon;

  my_os_k_eta = m_P0_Cal_kaon + m_P1_Cal_kaon * (my_os_k_eta  - m_AverageOmega);

  if(my_os_k_eta > m_ProbMax_kaon)
      my_os_k_dec = 1;

  if(my_os_k_eta < m_ProbMin_kaon)
      my_os_k_dec = -1;

  if(my_os_k_eta > 0.5)
	  my_os_k_eta = 1. - my_os_k_eta;

  tkaon.setOmega( my_os_k_eta );
  tkaon.setDecision( my_os_k_dec );
  tkaon.setType( Tagger::jetCharge);
  tkaon.addToTaggerParts(ikaon);

  return tkaon;
}

//==========================================================================
StatusCode TaggerNEWKaonOppositeTool::finalize() { return StatusCode::SUCCESS; }

