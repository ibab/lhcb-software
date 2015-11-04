#include <algorithm>
#include <limits>
#include <fstream>
#include <cmath>

#include <stdlib.h>

#include "LoKiNNBTag.h"

// LoKi
#include "LoKi/ILoKiSvc.h"
#include "LoKi/PhysExtract.h"

DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,NNBTag)


LoKi::NNBTag::NNBTag(const std::string& type, const std::string& name, const IInterface* parent)
: GaudiTool(type, name, parent)
  , m_distCalc(0)
  , m_jet(0)
  , m_bTrackExpertForOdd(0)
  , m_bTrackExpertForEven(0)
  , m_bJetExpertForOdd(0)
  , m_bJetExpertForEven(0)
{
  declareInterface<IJetTagTool>(this);

  declareProperty("TrackNetMinIP",m_trackNetMinIP=0.08
                  ,"NeuroBayes b-track network min. IP precut in [mm], should be consistent with training.");

  declareProperty("TrackNetMaxIP",m_trackNetMaxIP=3.0
                  ,"NeuroBayes b-track network max. IP precut in [mm], should be consistent with training.");

  declareProperty("TrackNetMinIPSig",m_trackNetMinIPSig=0.5
                  ,"NeuroBayes b-track network min. IP significance precut, should be consistent with training.");

  declareProperty("TrackNetPtCut",m_trackNetMinPt=500.0
                  ,"NeuroBayes b-track network p_T precut in [MeV], should be consistent with training.");

  declareProperty("RefitTracksAndPVs",m_refitTracksAndPVs=false
                  ,"Set this to 'true' if you want to refit tracks and primary vertices.");

  declareProperty("NBExpertiseDirectory", m_nbExpertiseDirectory=""
                  ,"Custom NB expert directory. Defaults to $JETNNBTAGPARAMROOT/data");

  declareProperty("NBExpertiseVersion", m_nbExpertiseVersion="7TeV_v1"
                  ,"NB expert version subdirectory.");
  return;
}


LoKi::NNBTag::~NNBTag()
{
#ifdef __GNUC__
  delete m_bTrackExpertForOdd;
  delete m_bTrackExpertForEven;
  delete m_bJetExpertForOdd;
  delete m_bJetExpertForEven;
#endif
  return;
}


// ============================================================================
// Standard Initialization
StatusCode LoKi::NNBTag::initialize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Initialising algorithm" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())
    return sc;

  svc<LoKi::ILoKiSvc>("LoKiSvc", true);

  m_pvTool      = tool<IPVOfflineTool >("PVOfflineTool");
  m_distCalc    = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  m_ipJetTagger = tool<IJetTagTool>("LoKi::IPTag");

#ifdef __GNUC__
  // check expertise path and create NeuroBayes experts.

  std::string expertiseDirectory;

  if ( m_nbExpertiseDirectory.empty() ) { //use expertises from JetNNBTagParam
    const char* jetNNBTagParamRoot = getenv("JETTAGGINGROOT");

    if ( 0 == jetNNBTagParamRoot ) {
      error() << "$JETTAGGINGROOT is not set! Aborting." << endmsg;
      return StatusCode::FAILURE;
    }

    expertiseDirectory = std::string(jetNNBTagParamRoot) + "/data/nn_configs";

  } else { //use custom expertise directory
    expertiseDirectory = m_nbExpertiseDirectory;
  }

  //  assemble full file path for all expertises
  std::vector< std::string > expertisePaths;
  expertisePaths.push_back(expertiseDirectory + "/" + m_nbExpertiseVersion + "/btrack_expert_for_odd.nb");
  expertisePaths.push_back(expertiseDirectory + "/" + m_nbExpertiseVersion + "/btrack_expert_for_even.nb");
  expertisePaths.push_back(expertiseDirectory + "/" + m_nbExpertiseVersion + "/bjet_expert_for_odd.nb");
  expertisePaths.push_back(expertiseDirectory + "/" + m_nbExpertiseVersion + "/bjet_expert_for_even.nb");

  // check whether files are readable
  for ( unsigned int i=0; i<expertisePaths.size(); ++i ) {

    std::ifstream fileTest(expertisePaths[i].c_str());

    if ( !fileTest.is_open() ) {
      error() << "Could not open expertise file " << expertisePaths[i] << "! Aborting." << endmsg;
      return StatusCode::FAILURE;
    } else {
      fileTest.close();
    }
  }

  // create track and jet experts
  m_bTrackExpertForOdd  = new Expert(expertisePaths[0].c_str());
  m_bTrackExpertForEven = new Expert(expertisePaths[1].c_str());
  m_bJetExpertForOdd    = new Expert(expertisePaths[2].c_str());
  m_bJetExpertForEven   = new Expert(expertisePaths[3].c_str());

#else
  error() << "NeuroBayes is not supported on this platform! Aborting." << endmsg;
  sc = StatusCode::FAILURE;
#endif


  return sc;
}


// ============================================================================
// Standard Finalization
StatusCode LoKi::NNBTag::finalize()
{
  if(msgLevel(MSG::VERBOSE))
    verbose() << "Finalising algorithm" << endmsg;

  return GaudiTool::finalize();
}


// ===========================================================================
// Calculate Jet Properties
bool LoKi::NNBTag::calculateJetProperty
(const LHCb::Particle *jet,   std::map <std::string,double> &jetProps)
{
  StatusCode sc = StatusCode::SUCCESS;


  // ODIN event number, relevant for odd/even NB training
  LHCb::ODIN* odin_bank = getOdinBank();
  if ( 0 != odin_bank ) {
    // we don't care about the number, only odd or even classification.
    m_oddEvent = 1 == odin_bank->eventNumber()%2;
  } else {
    // we can't reliably use NeuroBayes without knowing which training to choose
    error() << "ODIN bank is not available, can't choose NeuroBayes expertise." << endmsg;
    return StatusCode::FAILURE;
  }

  clearState();
  m_jet = jet;

  getPrimaries();
  extractTracks();
  calculateTrackVariables();
  calculateJetVariables();

  float nb_jet_net = applyJetNet();



  jetProps["j_max_trk_netout"] = m_nb_jet_vars[0];
  jetProps["j_sum_trk_netout"] = m_sum_trk_netout;
  jetProps["j_pt"] = m_nb_jet_vars[1];
  jetProps["j_max_trk_pt"] = m_nb_jet_vars[2];
  jetProps["j_max_trk_ipsig"] = m_nb_jet_vars[3];
  jetProps["j_avg_trk_ip"] = m_nb_jet_vars[4];
  jetProps["j_trk_ktsum"] = m_nb_jet_vars[5];
  jetProps["j_ip_tag_sum"] = m_nb_jet_vars[6];
  jetProps["j_ip_tag_2nd"] = m_nb_jet_vars[7];
  jetProps["j_ip_tag_3rd"] = m_nb_jet_vars[8];
  jetProps["j_nb_net"] = nb_jet_net;


  //IT HAS TO BEHTE LAST ONE: GLOBAL DECISTION
  jetProps["Tag"] = nb_jet_net ;

  jetProps["extraInfo"] = 5300;


  if (jetProps["j_nb_net"]>0)
    return true;
  else
    return false;
}

// ===========================================================================
// safely retrieve ODIN bank
LHCb::ODIN* LoKi::NNBTag::getOdinBank()
{
  LHCb::ODIN* odin = 0;
  if (!exist<LHCb::ODIN>(LHCb::ODINLocation::Default)) {
    if (UNLIKELY(msgLevel(MSG::DEBUG))) {
      debug() << "No ODIN bank found. Can't retrieve event numbers. (Relevant for odd/even NB training only)"
              << endmsg;
    }
  } else {
    odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
  }
  return odin;
}

// ===========================================================================
// clear all data
void LoKi::NNBTag::clearState()
{
  for ( unsigned int i=0; i<m_n_nb_track_vars; ++i ) m_nb_track_vars[i] = -999.0f;
  for ( unsigned int i=0; i<m_n_nb_jet_vars; ++i )   m_nb_jet_vars  [i] = -999.0f;
  m_ntrk_bprob = 0U;
  m_PVs = LHCb::RecVertex::Range();
  m_pvTracks.clear();
  m_refittedPVs.clear();
  m_refittedPVsPtrs.clear();
  m_jet_trk_parts.clear();
  m_jet_all_parts.clear();
  m_jet_tracks.clear();
  m_trk_ip.clear();
  m_trk_ipsig.clear();
  m_trk_bprob.clear();

  return;
}

// ===========================================================================
// get all primary vertices, refit if requested
void LoKi::NNBTag::getPrimaries()
{

  if ( exist<LHCb::RecVertex::Range>(LHCb::RecVertexLocation::Primary)) {
    m_PVs = get<LHCb::RecVertex::Range>(LHCb::RecVertexLocation::Primary);
  } else {
    warning() << "No primary vertices found at " << LHCb::RecVertexLocation::Primary << "." << endmsg;
    return;
  }

  if ( m_refitTracksAndPVs ) {
    // get all tracks from all vertices and refit them
    std::vector<const LHCb::RecVertex*>::const_iterator ipv = m_PVs.begin();
    for ( ; m_PVs.end()!=ipv; ++ipv) {
      SmartRefVector<LHCb::Track>::const_iterator itrk = (*ipv)->tracks().begin();
      for ( ; itrk != (*ipv)->tracks().end(); ++itrk ) {
        LHCb::Track* track = const_cast<LHCb::Track*>(itrk->target());
        if ( 0 == track ) {
          error() << "Invalid track in PV, skipping." << endmsg;
          continue;
        }
        m_pvTracks.push_back(track);
      }
    }

    // reconstruct all PVs and set range accordingly
    m_pvTool->reconstructMultiPVFromTracks(m_pvTracks,m_refittedPVs);
    std::vector<LHCb::RecVertex>::const_iterator irfpv = m_refittedPVs.begin();
    for ( ; irfpv !=  m_refittedPVs.end(); ++irfpv ) {
      m_refittedPVsPtrs.push_back(&(*irfpv));
    }
    m_PVs = LHCb::RecVertex::Range(m_refittedPVsPtrs.begin(),m_refittedPVsPtrs.end());
  }

  return;
}

// ===========================================================================
// extract all charged particles & tracks from jet, refit if requested.
void LoKi::NNBTag::extractTracks()
{
  std::copy(m_jet->daughters().begin(),m_jet->daughters().end(), std::back_inserter(m_jet_all_parts));

  // sort particles in this jet by decreasing p_T
  std::sort(m_jet_all_parts.begin(),m_jet_all_parts.end(),ParticlePtrComparePt());

  // find all constituents with tracks
  for ( std::vector < const LHCb::Particle* >::const_iterator ci = m_jet_all_parts.begin();
        ci != m_jet_all_parts.end();
        ++ci ) {

    // only consider basic particles for tracks
    if ( ! (*ci)->isBasicParticle() ) continue;

    // get track, if any
    std::vector<LHCb::Track*> tracks;
    LoKi::Extract::getTracks((*ci),std::back_inserter(tracks));

    if  ( !tracks.empty() ) {
      m_jet_tracks.push_back(tracks.front());
      m_jet_trk_parts.push_back(*ci);
    }
  }

  return;
}

// ===========================================================================
// calculate IP and IP significance
void LoKi::NNBTag::calculateIP(const LHCb::Particle* p,
                               const Gaudi::XYZPoint& v,
                               double& ip,
                               double& ipsig)
{

  double ipC=0.0, ipChi2=0.0;

  StatusCode sc = m_distCalc->distance(p, v, ipC, ipChi2);
  ipC = std::fabs(ipC);

  if(sc && ipChi2>0.0 ) {
    ip = ipC;
    ipsig = std::sqrt(ipChi2);
  }

  return;
}

// ===========================================================================
// calculate per track variables e.g., ip and NN b-track probability
void LoKi::NNBTag::calculateTrackVariables()
{
#ifdef __GNUC__

  std::vector<const LHCb::Particle*>::const_iterator ipart = m_jet_trk_parts.begin();
  for ( std::vector<LHCb::Track*>::const_iterator itrk = m_jet_tracks.begin(); 
        itrk != m_jet_tracks.end(); ++itrk, ++ipart)
  {
    const LHCb::Particle* part = *ipart;
    LHCb::Track*         track = *itrk;

    // get the best PV associated with this particle
    // and calculate IP. There might be no PV associated
    // and the IP calculation might fail.
    double ip     = -999.0;
    double ip_sig = -999.0;
    double ip_err = -999.0;

    if ( ! m_PVs.empty() )
    {
      std::vector<const LHCb::Track*> excludeTracks;
      excludeTracks.push_back(track);

      double ip_tmp     =  std::numeric_limits<double>::max();
      double ip_sig_tmp =  std::numeric_limits<double>::max();

      for ( std::vector<const LHCb::RecVertex*>::const_iterator ipv = m_PVs.begin(); 
            m_PVs.end()!=ipv; ++ipv) 
      {
        const LHCb::RecVertex* biasedPV = (*ipv);
        LHCb::RecVertex unbiasedPV;
        m_pvTool->removeTracksAndRecalculatePV (biasedPV,excludeTracks,unbiasedPV);

        calculateIP(part, unbiasedPV.position(), ip_tmp, ip_sig_tmp);
        if ( std::fabs(ip_tmp) < std::fabs(ip) &&
             !std::isnan(ip_tmp) && !std::isnan(ip_sig_tmp) &&
             std::isfinite(ip_tmp)  && std::isfinite(ip_sig_tmp) )
        {
          ip = ip_tmp;
          ip_sig = ip_sig_tmp;
          ip_err = ip_tmp/ip_sig_tmp;
        }
      }
    }

    m_trk_ip.push_back(ip);
    m_trk_ipsig.push_back(ip_sig);

    // calculate and set track variables for NeuroBayes b-track expert
    double pt,p;
    if ( m_refitTracksAndPVs ) { // after refit use tracks as they are more accurate...
      pt = track->pt();
      p  = track->p ();
    } else { // ...otherwise use the particle properties as they are better
      pt = part->pt();
      p  = part->p ();
    }
    double theta = std::asin(pt/p);
    double eta = -std::log(std::tan(0.5*theta));

    m_nb_track_vars[0] = static_cast<float>(ip);
    m_nb_track_vars[1] = static_cast<float>(ip_err);
    m_nb_track_vars[2] = static_cast<float>(ip_sig);
    m_nb_track_vars[3] = static_cast<float>(pt);
    m_nb_track_vars[4] = static_cast<float>(eta);

    double nb_track_bprob = -999.0;
    if (   ip > m_trackNetMinIP
           && ip < m_trackNetMaxIP
           && ip_sig > m_trackNetMinIPSig
           && pt > m_trackNetMinPt) {
      ++m_ntrk_bprob;
      nb_track_bprob = applyTrackNet();
    }
    m_trk_bprob.push_back(nb_track_bprob);

  }
#endif

  return;
}

// ===========================================================================
// calculate per jet variables
void LoKi::NNBTag::calculateJetVariables()
{

  // only consider jets with charged constituents
  if ( m_jet_tracks.size() < 2 ) return;

  double max_trk_b_prob = -999.0;
  double max_trk_ip     = -999.0;
  double max_trk_ip_sig = -999.0;
  double avg_trk_ip     = 0.0;
  double avg_trk_ip_sig = 0.0;
  double trk_kt_sum     = 0.0;
  m_sum_trk_netout = 0.0;

  unsigned int n_with_ip = 0;

  for ( unsigned int itrk=0; itrk<m_jet_tracks.size(); ++itrk ) {
    if(m_trk_bprob[itrk]>-10.0)
      m_sum_trk_netout +=  m_trk_bprob[itrk];
    if ( m_trk_bprob[itrk] > max_trk_b_prob )  max_trk_b_prob = m_trk_bprob[itrk];

    float ip     = m_trk_ip   [itrk];
    float ip_sig = m_trk_ipsig[itrk];
    if ( ip > 0 ) {
      if ( ip > max_trk_ip )          max_trk_ip     = ip;
      if ( ip_sig > max_trk_ip_sig )  max_trk_ip_sig = ip_sig;

      avg_trk_ip     +=  m_trk_ip   [itrk];
      avg_trk_ip_sig +=  m_trk_ipsig[itrk];
      ++n_with_ip;
    }

    Gaudi::LorentzVector ptrk = m_jet_trk_parts[itrk]->momentum();
    Gaudi::LorentzVector pjet = m_jet->momentum();
    Gaudi::XYZVector vtrk(ptrk.x()/ptrk.r(),ptrk.y()/ptrk.r(),ptrk.z()/ptrk.r());
    Gaudi::XYZVector vjet(pjet.x()/pjet.r(),pjet.y()/pjet.r(),pjet.z()/pjet.r());
    // ensure dot-product within acos bounds
    double dot   = vtrk.Dot(vjet);
    if (dot > 1) dot = 1;
    else if (dot < -1) dot = -1;
    double angle = std::acos(dot);
    trk_kt_sum += ptrk.P()*std::sin(angle);
  }
  if ( 0 <  n_with_ip ) {
    avg_trk_ip     /= n_with_ip;
    avg_trk_ip_sig /= n_with_ip;
  } else {
    avg_trk_ip = -999.0f;
    avg_trk_ip_sig = -999.0f;
  }

  bool ip_tag_sum_valid = false;
  float ip_tag_3rd = -999.0f;
  float ip_tag_2nd = -999.0f;
  float ip_tag_1st = -999.0f;
  float ip_tag_sum = -999.0f;

  m_jetWeights.clear();
  if ( m_ipJetTagger->calculateJetProperty(m_jet,m_jetWeights) && !m_jetWeights.empty() ) {
    if ( std::abs(m_jetWeights["ThirdTrack"]) < 250.0 ) {
      ip_tag_3rd =  static_cast<float>(m_jetWeights["ThirdTrack"]);
    }
    ip_tag_sum_valid = ip_tag_sum_valid || (ip_tag_3rd != -999.0f);

    if ( std::abs(m_jetWeights["SecondTrack"]) < 250.0 ) {
      ip_tag_2nd =  static_cast<float>(m_jetWeights["SecondTrack"]);
    }
    ip_tag_sum_valid = ip_tag_sum_valid || (ip_tag_2nd != -999.0f);

    if ( std::abs(m_jetWeights["FirstTrack"]) < 250.0 ) {
      ip_tag_1st =  static_cast<float>(m_jetWeights["FirstTrack"]);
    }
    ip_tag_sum_valid = ip_tag_sum_valid || (ip_tag_1st != -999.0f);

    // we only accept valid and reasonable values and
    // move everything else to the -999 delta function
    if ( ip_tag_sum_valid && std::abs(m_jetWeights["SumSignValue"]) < 250.0 ) {
      ip_tag_sum = static_cast<float>(m_jetWeights["SumSignValue"]);
    }
  }

  unsigned int var=0;

  // maximum b-track probability
  m_nb_jet_vars[var++] = static_cast<float>(max_trk_b_prob);

  // jet pt
  m_nb_jet_vars[var++] = static_cast<float>(m_jet->pt());

  // highest track pt
  m_nb_jet_vars[var++] = static_cast<float>(m_jet_tracks.front()->pt());

  // maximum track ip sig
  m_nb_jet_vars[var++] = static_cast<float>(max_trk_ip_sig);

  // average track ip
  m_nb_jet_vars[var++] = static_cast<float>(avg_trk_ip);

  // kt sum of all tracks with respect to jet
  m_nb_jet_vars[var++] = static_cast<float>(trk_kt_sum);

  // signed ip tag sum
  m_nb_jet_vars[var++] = static_cast<float>(ip_tag_sum);

  // signed ip tag 2nd
  m_nb_jet_vars[var++] = static_cast<float>(ip_tag_2nd);

  // signed ip tag 3rd
  m_nb_jet_vars[var++] = static_cast<float>(ip_tag_3rd);

  return;
}

// ===========================================================================
// apply b-track network
float LoKi::NNBTag::applyTrackNet() {

  float netOut = -999.0f;
  if ( m_oddEvent ) { netOut = m_bTrackExpertForOdd->nb_expert (m_nb_track_vars); }
  else              { netOut = m_bTrackExpertForEven->nb_expert(m_nb_track_vars); }

  return netOut;
}

// ===========================================================================
// apply b-jet network
float LoKi::NNBTag::applyJetNet() {

  float netOut = -999.0f;
  if ( m_oddEvent ) { netOut = m_bJetExpertForOdd->nb_expert (m_nb_jet_vars); }
  else              { netOut = m_bJetExpertForEven->nb_expert(m_nb_jet_vars); }

  return netOut;
}
