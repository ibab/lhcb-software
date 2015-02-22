// Include files
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/Particle.h"
// kernel

#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include <TROOT.h>
#include <TObject.h>
#include "TH1D.h"
#include <iostream>
//#include "TMVA/Reader.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Math/Boost.h"

#include "RelInfoBs2MuMuIsolations.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoBs2MuMuIsolations
//
// 2014-08-18 : Fatima Soomro
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoBs2MuMuIsolations )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoBs2MuMuIsolations::RelInfoBs2MuMuIsolations( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent)
  : GaudiTool( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "tracktype", m_tracktype = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");

  declareProperty("angle"     , m_angle  = 0.27     ); //
  declareProperty("fc"        , m_fc  = 0.60     ); //
  declareProperty("doca_iso"  , m_doca_iso  = 0.13     ); //
  declareProperty("ips"       , m_ips  = 3.0     ); //
  declareProperty("svdis"     , m_svdis  = -0.15     ); //
  declareProperty("svdis_h"   , m_svdis_h  = 30.     ); //
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); //
  declareProperty("pvdis_h"   , m_pvdis_h  = 40.    ); //
  declareProperty("clone_cut"   , m_clone_cut  = -9999. ); //
  declareProperty("ghost_cut"   , m_ghost_cut  = 0.3 ); //
  declareProperty("trchi2_cut"   , m_trchi2_cut  = 3.0 ); //
  declareProperty("makeTrackCuts", m_makeTrackCuts = false);
  declareProperty("IsoTwoBody", m_IsoTwoBody = false);
  declareProperty("ParticlePath",   m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles");
  declareProperty ( "TrackContainer",   m_TracksPath = LHCb::TrackLocation::Default); // default is "Rec/Track/Best "
  declareProperty("Variables", m_variables, "List of variables to store (store all if empty)");
  declareProperty("PVInputLocation",m_PVInputLocation = LHCb::RecVertexLocation::Primary);

  m_keys.clear();

}


//=============================================================================
// Destructor
//=============================================================================
RelInfoBs2MuMuIsolations::~RelInfoBs2MuMuIsolations() {}


//=============================================================================
// Initialization
//=============================================================================
StatusCode RelInfoBs2MuMuIsolations::initialize() {


  Error("This tool is deprecated, please switch to the new one RelInfoBs2MuMuBIsolations and RelInfoBs2MuMuTrackIsolations");

  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;

  //get from DV algorithm
  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);

  m_dist  = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  if( !m_dist ){
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }

  m_combiner  = m_dva->particleCombiner();
  //m_combiner = tool<IParticleCombiner>("LoKi::", this)

  debug()<<" ==> Initialize"<<endmsg;

  //configure keys
  m_keys.push_back(RelatedInfoNamed::BSMUMUCDFISO);
  m_keys.push_back(RelatedInfoNamed::BSMUMUOTHERBMAG);
  m_keys.push_back(RelatedInfoNamed::BSMUMUOTHERBANGLE);
  m_keys.push_back(RelatedInfoNamed::BSMUMUOTHERBBOOSTMAG);
  m_keys.push_back(RelatedInfoNamed::BSMUMUOTHERBBOOSTANGLE);
  m_keys.push_back(RelatedInfoNamed::BSMUMUOTHERBTRACKS);
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKPLUSISO);
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKMINUSISO);
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKPLUSISOTWO);
  m_keys.push_back(RelatedInfoNamed::BSMUMUTRACKMINUSISOTWO);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYQMINUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYQPLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYMASSISOMINUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYMASSISOPLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYCHI2ISOMINUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYCHI2ISOPLUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYISO5MINUS);
  m_keys.push_back(RelatedInfoNamed::ISOTWOBODYISO5PLUS);

  return sc;
}

//rel infor methods
LHCb::RelatedInfoMap* RelInfoBs2MuMuIsolations::getInfo(void) {
  return &m_map;
}

std::string RelInfoBs2MuMuIsolations::infoPath(void){ // check with Alex
  std::stringstream ss;
  ss << std::string("Particle2VertexIsolationRelations");
  return ss.str();
}


//=============================================================================
// Fill the related info with track variables
//=============================================================================

StatusCode RelInfoBs2MuMuIsolations::calculateRelatedInfo(const LHCb::Particle* part, const LHCb::Particle *top){

  Error("This tool is deprecated, please switch to the new one RelInfoBs2MuMuBIsolations and RelInfoBs2MuMuTrackIsolations");

  if ( msgLevel(MSG::DEBUG) ){
    if(part != top) debug() << " part != top ... must be a B to Jpsi X decay ?"<<endmsg;
    else debug() << " part == top ... must be a B -> mu mu  decay "<<endmsg;
  }

  m_otherB_mag=0.;
  m_otherB_angle=-1.;
  m_otherB_boost_mag=0.;
  m_otherB_boost_angle=-1.;
  otherBtracks= 0;
  m_CDFIso = -1;
  m_count_mup_Giampi = -1;
  m_count_mum_Giampi = -1;
  m_count_mup_iso2 = -1;
  m_count_mum_iso2 = -1;
  m_massisoplus = -1;
  m_massisominus= -1;
  m_chargeplus  = -100;
  m_chargeminus = -100;
  m_chi2isoplus = -1;
  m_chi2isominus= -1;
  m_iso5plus    = -1;
  m_iso5minus   = -1;

  // -- The vector m_decayParticles contains all the particles that belong to the given decay
  // -- according to the decay descriptor.

  // -- Clear the vector with the particles in the specific decay Probably not needed here
  m_decayParticles.clear();

  if ( part->isBasicParticle() ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Do not run this tool on basic particles... skipping" << endmsg;
    return StatusCode::SUCCESS ;
  }

  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_TracksPath);
  if ( tracks->empty() )
  {
    if ( msgLevel(MSG::WARNING) ) Warning( std::string("Could not retrieve tracks at location: ")+m_TracksPath+ " .... Skipping" );
    return StatusCode::FAILURE;
  }

  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
    m_vertices = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  }

  /// check that it is a 2 body decay
  const LHCb::Particle::ConstVector& daughterVec = part->daughtersVector();
  int idx = 0;
  for (LHCb::Particle::ConstVector::const_iterator ipart=daughterVec.begin();ipart!=daughterVec.end();++ipart)
  {
    if ( NULL==(*ipart)->proto() ) continue;
    idx++;
  }
  if(idx != 2 )  {
    return StatusCode::SUCCESS;
  }

  StatusCode testcode ;
  // Call functions to do the work and assign values

  if ( msgLevel(MSG::DEBUG) ) debug() << "Going to call CDFIsolation computation" << endmsg ;
  testcode = CDFIsolation(part, daughterVec.at(0), daughterVec.at(1));
  if ( msgLevel(MSG::DEBUG) ) debug() << "Computed CDFIsolation: "<< m_CDFIso <<" with statuscode "<<testcode <<endmsg;
  if(!testcode) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Going to call OtherB computation" << endmsg ;
  testcode = OtherB(part, daughterVec.at(0), daughterVec.at(1));
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Computed quantities "<< m_otherB_mag<<" "<< m_otherB_angle<< " "<<m_otherB_boost_mag<<" "<<m_otherB_boost_angle <<" "<< otherBtracks<<" with statuscode "<< testcode <<endmsg;
  if(!testcode) return StatusCode::FAILURE;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Going to call TrackIsolations computation" << endmsg ;
  testcode = TrackIsolations(part); //, daughterVec.at(0), daughterVec.at(1));
  if ( msgLevel(MSG::DEBUG) ) debug() << "Computed TrackIsolations: "<< m_count_mup_Giampi<<" and "<< m_count_mum_Giampi <<" with statuscode "<<testcode <<endmsg;
  if(!testcode) return StatusCode::FAILURE;

  if(m_IsoTwoBody){
    if ( msgLevel(MSG::DEBUG) ) debug() << "Going to call IsolationTwoBody to compute IsolationTwoBody variables " << endmsg ;
    testcode = IsolationTwoBodyVariables(part);
    if ( msgLevel(MSG::DEBUG) ) debug() << "Computed IsolationTwoBody variables "<< m_count_mup_iso2<<" and "<< m_count_mum_iso2 <<" with statuscode "<<testcode <<endmsg;
    if(!testcode) return StatusCode::FAILURE;
  }

  m_map.clear();
  std::vector<short int>::const_iterator ikey;
  for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {

    float value = 0;

    switch (*ikey) {
    case RelatedInfoNamed::BSMUMUCDFISO           : value = m_CDFIso; break;
    case RelatedInfoNamed::BSMUMUOTHERBMAG        : value = m_otherB_mag; break;
    case RelatedInfoNamed::BSMUMUOTHERBANGLE      : value = m_otherB_angle; break;
    case RelatedInfoNamed::BSMUMUOTHERBBOOSTMAG   : value = m_otherB_boost_mag; break;
    case RelatedInfoNamed::BSMUMUOTHERBBOOSTANGLE : value = m_otherB_boost_angle; break;
    case RelatedInfoNamed::BSMUMUOTHERBTRACKS     : value = otherBtracks; break;
    case RelatedInfoNamed::BSMUMUTRACKPLUSISO         : value = m_count_mup_Giampi; break;
    case RelatedInfoNamed::BSMUMUTRACKMINUSISO        : value = m_count_mum_Giampi; break;
    case RelatedInfoNamed::BSMUMUTRACKPLUSISOTWO      : value = m_count_mup_iso2; break;
    case RelatedInfoNamed::BSMUMUTRACKMINUSISOTWO     : value = m_count_mum_iso2; break;

    case RelatedInfoNamed::ISOTWOBODYMASSISOPLUS  : value = m_massisoplus ; break;
    case RelatedInfoNamed::ISOTWOBODYMASSISOMINUS : value = m_massisominus; break;
    case RelatedInfoNamed::ISOTWOBODYCHI2ISOPLUS  : value = m_chi2isoplus ; break;
    case RelatedInfoNamed::ISOTWOBODYCHI2ISOMINUS : value = m_chi2isominus ; break;
    case RelatedInfoNamed::ISOTWOBODYISO5PLUS     : value = m_iso5plus    ; break;
    case RelatedInfoNamed::ISOTWOBODYISO5MINUS    : value = m_iso5minus    ; break;
    case RelatedInfoNamed::ISOTWOBODYQPLUS        : value = m_chargeplus    ; break;
    case RelatedInfoNamed::ISOTWOBODYQMINUS       : value = m_chargeminus    ; break;

    }
    if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;

    m_map.insert( std::make_pair( *ikey, value) );
  } //iter over keys

  return StatusCode(testcode);

}

//=============================================================================
// Other B quantities
// From /afs/cern.ch/user/s/serranoj/public/Bs2MuMu/Bs2MuMuAnalysis.cpp
//=============================================================================

StatusCode RelInfoBs2MuMuIsolations::OtherB(const LHCb::Particle *part,
                                            const LHCb::Particle *vdau1,
                                            const LHCb::Particle *vdau2){

  const LHCb::VertexBase* goodPV = m_dva->bestVertex(part);

  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho();

  LHCb::Particles OTHERB_parts;

  // in cppm code m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles";
  LHCb::Particles* m_allparts = get<LHCb::Particles>(m_ParticlePath);
  for ( LHCb::Particles::const_iterator ipp = m_allparts->begin(); ipp != m_allparts->end() ; ++ipp) {
    Gaudi::XYZVector p_track = ((*ipp)->proto()->track()->momentum());
    double pttrack = p_track.rho();
    double ptrackmag = p_track.R();
    double imp = 0.;
    double ips = -1.;
    m_dist->distance ( (*ipp), goodPV, imp, ips );
    ips = TMath::Sqrt(ips);

    if (ips>4.0 && ips<40.0 && pttrack>200.0 && pttrack<2000.0
        && ptrackmag<30000.0 && ratio(pttrack, ptmu1) > 0.0001 && ratio(pttrack,ptmu2) > 0.0001)
    {
      LHCb::Particle* track = *(ipp);
      OTHERB_parts.add(track) ;
      otherBtracks++;
    }
  }// ipp

  if ( otherBtracks>0 )
  {

    Gaudi::XYZVector ptproj(0.,0.,0.);
    LHCb::Particles::const_iterator ipp;
    for ( ipp = (&OTHERB_parts)->begin(); ipp != (&OTHERB_parts)->end() ; ++ipp) {
      Gaudi::XYZVector ptrack = ((*ipp)->proto()->track()->momentum());
      ptproj += ptrack;
    }

    m_otherB_mag = ptproj.R();
    m_otherB_angle=arcosine( ptproj, part->slopes());

    const LHCb::Particle *myDau = NULL;
    if (vdau1->charge()>0) myDau = vdau1;
    else if (vdau2->charge()>0) myDau = vdau2;
    if(vdau1->charge() == vdau2->charge() ) myDau = vdau2;


    Gaudi::XYZVector ptproj2(0.,0.,0.);
    const Gaudi::LorentzVector p2(part->momentum());
    ROOT::Math::Boost boostToB( p2.BoostToCM() );
    const Gaudi::LorentzVector p3(myDau->momentum());
    const Gaudi::LorentzVector boostedMu = boostToB( p3 );
    const Gaudi::XYZVector pmuplus = Gaudi::XYZVector( boostedMu ) ;
    for ( ipp = (&OTHERB_parts)->begin(); ipp != (&OTHERB_parts)->end() ; ++ipp) {
      const Gaudi::LorentzVector p1((*ipp)->momentum());
      const Gaudi::LorentzVector boostedTrk = boostToB( p1 );
      const Gaudi::XYZVector ptrack = Gaudi::XYZVector( boostedTrk ) ;
      ptproj2 += ptrack;
    }

    m_otherB_boost_mag= ptproj2.R();
    m_otherB_boost_angle =  arcosine(pmuplus,ptproj2);

  } // if ntracks>0

  else {
    m_otherB_mag=-1.;
    m_otherB_angle= -1;
    m_otherB_boost_mag=-1.;
    m_otherB_boost_angle= -1;
  }

  return StatusCode::SUCCESS;

}

//-------------------------------------------------------------------------
// isolation variables
//-------------------------------------------------------------------------
StatusCode RelInfoBs2MuMuIsolations::TrackIsolations(const LHCb::Particle *part){

  if ( msgLevel(MSG::DEBUG) ) debug() <<" Entering  TrackIsolations "<<endmsg;

  int isotype = 5;
  if ( msgLevel(MSG::DEBUG) ) debug()<<" calling iso with isotype  "<<isotype<<endreq;

  //calling Giampi's tool. setting isotype=5, the default of Bs2MuMu 2013 analysis
  std::vector<int> iso5 = getIso( part, isotype, false);
  std::vector<int> iso2_new = getIso( part, 66132, true);

  const LHCb::Particle::ConstVector parts = part->daughtersVector();

  if (parts.at(0)->charge() < 0) {
    m_count_mum_Giampi = static_cast<float>( iso5[0] );
    m_count_mup_Giampi = static_cast<float>( iso5[1] );
    m_count_mum_iso2 = static_cast<float>( iso2_new[0] );
    m_count_mup_iso2 = static_cast<float>( iso2_new[1] );
  }
  else{
    m_count_mum_Giampi = static_cast<float>( iso5[1] );
    m_count_mup_Giampi = static_cast<float>( iso5[0] );
    m_count_mum_iso2 = static_cast<float>( iso2_new[1] );
    m_count_mup_iso2 = static_cast<float>( iso2_new[0] );
  }

  return StatusCode::SUCCESS;

}//TrackIsolations(part)



//=============================================================================
// method to calculate Giampi's isolation
//=============================================================================
std::vector<int>  RelInfoBs2MuMuIsolations::getIso(const LHCb::Particle* B, int isotype, bool cutOnTracks){

  std::vector<int> iso(2, -9999);
  const LHCb::VertexBase *PV = m_dva->bestVertex(B);
  /// Fatima, this will be the BPV of the jpsi in some cases!
  const LHCb::VertexBase *SV = B->endVertex();
  const LHCb::Particle::ConstVector parts = B->daughtersVector();
  if(parts.size() !=2)   return iso;

  LHCb::Particles* allparts = get<LHCb::Particles>(m_ParticlePath);
  if (!allparts) {
    Error( " Failed to get particles container "+m_ParticlePath);
    return iso;
  }

  StatusCode sc = StatusCode::SUCCESS;
  Gaudi::XYZPoint PosPV = PV->position();
  Gaudi::XYZPoint PosSV = SV->position();
  int i = 0;
  ROOT::Math::SVector<int, 2> iso0;
  iso0[0]=0;
  iso0[1]=0;
  ROOT::Math::SVector<int, 2> iso5;
  iso5[0]=0;
  iso5[1]=0;
  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;

  //Loop over input particles, get their simple kinematics
  LHCb::Particle::ConstVector::const_iterator ip_part;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++) {

    const LHCb::ProtoParticle * proto =  (*ip_part)->proto();
    const LHCb::Track* track = proto->track();
    o_mu(0,i) = track->position().x();
    o_mu(1,i) = track->position().y();
    o_mu(2,i) = track->position().z();
    p_mu(0,i) = track->momentum().x();
    p_mu(1,i) = track->momentum().y();
    p_mu(2,i) = track->momentum().z();
    ptmu[i] = sqrt(pow(p_mu(0,i),2)+pow(p_mu(1,i),2));
    i++;
  }

  if (i>2) { Error("more than 2 daughters of the B" );
    return iso;
  }


  for ( int i2 = 0; i2 < 2; i2++) {
    bool hltgood = false;
    double fc = 0.;
    Gaudi::XYZPoint omu(o_mu(0,i2),o_mu(1,i2),o_mu(2,i2));
    Gaudi::XYZVector pmu(p_mu(0,i2),p_mu(1,i2),p_mu(2,i2));

    //Loop over all particles
    LHCb::Particles::const_iterator ip;
    int j=0;  // for counting ntracks
    for ( ip = allparts->begin(); ip != allparts->end() ; ++ip) {

      const LHCb::ProtoParticle * proto =  (*ip)->proto();
      const LHCb::Track* track = proto->track();
      const LHCb::Particle*  cand = (*ip);
      Gaudi::XYZPoint o(track->position());
      Gaudi::XYZVector p(track->momentum());
      double pt = p.Rho();

      if(cutOnTracks && isotype ==5 && (track->ghostProbability()>0.3 || track->chi2PerDoF()> 3)) continue;

      if (track->type()!=m_tracktype)   continue;
      j++;

      // skip if other particle is in input list
      if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {  continue; }

      IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc);

      if (track->type()== m_tracktype) {
        iso0[i2] += hltgood;
        debug()<<"isisold "<< i2 << iso0[i2]<< endreq;
      }

      // find doca and angle between input and other tracks
      Gaudi::XYZPoint vtx(0.,0.,0.);
      double doca(-1.);
      double angle(-1.);
      InCone(omu,pmu, o, p, vtx, doca, angle);

      // find impact parameters, distances from secondary and primary vertex
      double imp = 0.;
      double impchi2 = 0.;
      double ips,pvdis,svdis;
      ips=100000.;
      LHCb::RecVertex::Container::const_iterator iv;
      LHCb::RecVertex::Container* verts = NULL;

      if(exist<LHCb::RecVertex::Container>(m_PVInputLocation))
      {
        verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
      }
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
        m_dist->distance(&(*cand),(*iv),imp,impchi2);
        if (impchi2<ips) ips = impchi2;
      }
      ips=sqrt(ips);

      pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();

      // non-isolating criteria #5
      if (isotype==5 && angle <m_angle && fc<m_fc && doca<m_doca_iso && ips>m_ips &&
          svdis>m_svdis && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h ){
        iso5[i2] += 1;
      }// if

      double iso_angle  =0.;
      double iso_fc = 0.;
      double iso_doca = 0.;
      double iso_ips = 0.;
      double iso_svdis=0.; double iso_svdis_h = 0.;
      double iso_pvdis=0.; double iso_pvdis_h = 0.;
      float  iso_track_clonedist = track->info(LHCb::Track::CloneDist, 10000);
      float  iso_track_ghost = 0.;
      float  iso_track_trchi2 =0.;

      if(isotype == 66132){
        iso_angle=0.27;
        iso_fc=0.60;
        iso_doca=0.13;
        iso_ips=3.0;
        iso_svdis=-0.15;
        iso_svdis_h=330.;
        iso_pvdis=0.5;
        iso_pvdis_h=340.;
        iso_track_clonedist = 99999.;
        iso_track_ghost = 10000000;
        iso_track_trchi2 = 5.;
      }

      float mytrack_clone = track->info(LHCb::Track::CloneDist, 100);
      float mytrack_gp =  track->ghostProbability();
      float mytrack_trchi2 = track->chi2PerDoF();
      if (isotype >100
          && angle < iso_angle
          && fc < iso_fc
          && doca < iso_doca
          && ips > iso_ips
          && svdis > iso_svdis
          && svdis < iso_svdis_h
          && pvdis > iso_pvdis
          && pvdis < iso_pvdis_h
          && mytrack_clone <  iso_track_clonedist
          && mytrack_gp < iso_track_ghost
          && mytrack_trchi2 < iso_track_trchi2
          ) {
        iso5[i2] += 1;
      }//  if

      if ( msgLevel(MSG::DEBUG) )
        debug() << " isotype was "<<isotype<<"   "<<
          angle <<" - "<< iso_angle<<" "<<
          fc <<" - "<< iso_fc<< " \n "<<
          doca <<" -  "<< iso_doca<<" \n "<<
          ips <<" - "<< iso_ips<<" \n"<<
          svdis <<" - "<< iso_svdis<<" \n"<<
          svdis <<" - "<< iso_svdis_h<<"\n "<<
          pvdis <<" - "<< iso_pvdis<<" \n"<<
          pvdis <<" - "<< iso_pvdis_h<<" \n"<<
          mytrack_clone<<" - "<<iso_track_clonedist<<" \n"<<
          mytrack_gp <<" - "<< iso_track_ghost <<" \n "<<
          mytrack_trchi2 <<" - "<<iso_track_trchi2<< endmsg;

    }// matches allparts
  } //i2

  if (isotype==0) {
    iso[0] = iso0[0] ;
    iso[1] = iso0[1] ;
  }
  else {
    iso[0] = iso5[0] ;
    iso[1] = iso5[1] ;
  }

  return iso;

} // getIso method


//=============================================================================
// IsHLTGood method,used by isolation calculation
//=============================================================================

void  RelInfoBs2MuMuIsolations::IsHltGood(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                          Gaudi::XYZPoint o_mu,Gaudi::XYZVector
                                          p_mu, Gaudi::XYZPoint PV, bool& hltgood,
                                          double& fc) {

  Gaudi::XYZVector rv;
  Gaudi::XYZPoint vtx;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);

  closest_point(o,p,o_mu,p_mu,close,close_mu,vtx,fail);

  if (fail) {
    fc = -1.;
    hltgood = -1;
  }
  else {
    double pete = p.Rho();
    rv = vtx - PV;
    double DOCA_b = (close-close_mu).R();
    double ZZ = rv.z();
    fc = pointer(rv,p,p_mu);
    //hltgood=( (DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.) );
    if       ((DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.)) hltgood = 1;
  }
}


//=============================================================================
// Other functions needed by isolation
//=============================================================================

double RelInfoBs2MuMuIsolations::pointer (Gaudi::XYZVector vertex,
                                          Gaudi::XYZVector p, Gaudi::XYZVector
                                          p_mu)  {
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double temp = arcosine(vertex,ptot);
  double  num=ptot.R()*sin(temp);
  double  den=(num+pt);
  double fc = num/den;
  return fc;
}

//=============================================================================

double RelInfoBs2MuMuIsolations::getphi(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){
  double dphi = vdau1->momentum().Phi() - vdau2->momentum().Phi();
  return dphi;
}

//=============================================================================
double RelInfoBs2MuMuIsolations::gettheta(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){

  double dtheta = vdau1->momentum().Eta() -  vdau2->momentum().Eta();
  return dtheta;
}

//=============================================================================
double RelInfoBs2MuMuIsolations::ratio( double p1, double p2){
  return fabs(p1 -p2)*(1./fabs(p1+p2));
}

//=============================================================================
double RelInfoBs2MuMuIsolations::IsClose(const LHCb::Particle* p1,const LHCb::Particle* p2) {

  double deta = gettheta(p1,p2);
  double dphi = getphi(p1,p2);
  return sqrt(deta*deta+dphi*dphi);
}

//=============================================================================
void RelInfoBs2MuMuIsolations::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                             Gaudi::XYZPoint o_mu,Gaudi::XYZVector p_mu,
                                             Gaudi::XYZPoint& close1,
                                             Gaudi::XYZPoint& close2,
                                             Gaudi::XYZPoint& vertex, bool& fail) {

  Gaudi::XYZVector v0(o - o_mu);
  Gaudi::XYZVector v1(p.unit());
  Gaudi::XYZVector v2(p_mu.unit());
  Gaudi::XYZPoint temp1(0.,0.,0.);
  Gaudi::XYZPoint temp2(0.,0.,0.);
  fail = false;
  double  d02 = v0.Dot(v2);
  double  d21 = v2.Dot(v1);
  double  d01 = v0.Dot(v1);
  double  d22 = v2.Dot(v2);
  double  d11 = v1.Dot(v1);
  double  denom = d11 * d22 - d21 * d21;
  if (fabs(denom) <= 0.) {
    close1 = temp1;
    close2 = temp2;
    fail = true;
  }
  else {
    double numer = d02 * d21 - d01 * d22;
    double mu1 = numer / denom;
    double mu2 = (d02 + d21 * mu1) / d22;
    close1 = o+v1*mu1;
    close2 = o_mu+v2*mu2;
  }
  vertex = (close1+(close2-close1)*0.5);
}


double RelInfoBs2MuMuIsolations::arcosine(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {

  double num    = (p1.Cross(p2)).R();
  double den    = p1.R()*p2.R();
  double seno   = num/den;
  double coseno = p1.Dot(p2)/den;
  double alpha  = asin(fabs(seno));
  if (coseno < 0 ) {
    alpha = ROOT::Math::Pi() - alpha;
  }
  return alpha;
}

//=============================================================================
void RelInfoBs2MuMuIsolations::InCone(Gaudi::XYZPoint o1,
                                      Gaudi::XYZVector p1,Gaudi::XYZPoint o2,
                                      Gaudi::XYZVector p2,
                                      Gaudi::XYZPoint& vtx, double&
                                      doca, double& angle){
  Gaudi::XYZPoint rv;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);
  closest_point(o1,p1,o2,p2,close,close_mu,vtx, fail);
  if (fail) {
    doca =-1.;
    angle=-1.;
  }
  else {
    doca = (close-close_mu).R();
    angle = arcosine(p1,p2);
  }
}


//=============================================================================
// CDF isolation variable for the B
//=============================================================================
StatusCode RelInfoBs2MuMuIsolations::CDFIsolation(const LHCb::Particle* B,
                                                  const LHCb::Particle* vdau1,
                                                  const LHCb::Particle* vdau2){

  bool test = true;
  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug()<<"Inside CDFisloation, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::Particle::Range  parts = get<LHCb::Particle::Range>(m_ParticlePath);
  if (!parts) {
    Error( " Failed to get particles container " );
    return StatusCode::SUCCESS;
  }

  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho();
  double pt_bs = B->momentum().rho();

  double iso_giampi = 0.0;
  double iso_giampi_tc = 0.0;

  for(LHCb::Particle::Range::const_iterator ipp=parts.begin();ipp!=parts.end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {

        Gaudi::XYZVector ptrack =  ((*ipp)->proto()->track()->momentum());
        double pttrack = ptrack.rho();

        double del1 = (TMath::Abs (ptmu1 - pttrack))/(TMath::Abs (ptmu1 + pttrack));
        double del2 = (TMath::Abs (ptmu2 - pttrack))/(TMath::Abs (ptmu2 + pttrack));
        if ( del1 > 0.0001 && del2 > 0.0001 ){

          double deta      =  B->momentum().Eta() - (*ipp)->momentum().Eta() ;
          double delta_phi =  B->momentum().Phi() - (*ipp)->momentum().Phi();
          delta_phi = TMath::Abs(delta_phi);

          if(!m_IsoTwoBody)
            if (delta_phi > TMath::Pi() )  delta_phi = 2*TMath::Pi() - delta_phi;

          double rad_cone = TMath::Sqrt(  (delta_phi*delta_phi + deta*deta) );
          if (  rad_cone <=1.0)
          {
            iso_giampi += pttrack;

            if(atrack->ghostProbability()<0.3 && atrack->chi2PerDoF()< 3)
            {
              iso_giampi_tc += pttrack;
            }
          }

        }// del1/2
      } // atrack
    } //proto
  } // ipp

  iso_giampi = pt_bs/(iso_giampi+pt_bs);  // stored by TupleToolMuonVariables as "_yetanother_CDF_iso"
  m_CDFIso = iso_giampi;

  //Version with track cuts
  iso_giampi_tc = pt_bs/(iso_giampi_tc+pt_bs);  // stored by TupleToolMuonVariables as "_yetanother_CDF_iso_tc"
  if(m_makeTrackCuts) m_CDFIso = iso_giampi_tc;

  debug()<<"isotwobody was "<<m_IsoTwoBody<<" and maketrackcuts was "<<m_makeTrackCuts<<" so I computed "<<iso_giampi<<" and "<<iso_giampi_tc<<" and set "<<m_CDFIso<<endmsg;

  return test;

}


//=============================================================================
// Compute IsolationTwoBody variables
//=============================================================================
StatusCode RelInfoBs2MuMuIsolations::IsolationTwoBodyVariables(const LHCb::Particle *P){

  const LHCb::Particle::ConstVector & parts = P->daughtersVector();
  if ( msgLevel(MSG::DEBUG) ) debug() <<"daughters vector "<< endreq;
  float massiso[2];
  float chi2iso[2];

  const LHCb::VertexBase *PV = m_dva->bestVertex(P);
  const LHCb::VertexBase *SV = P->endVertex();

  LHCb::Particle::Range  m_allparts = get<LHCb::Particle::Range>(m_ParticlePath);

  const Gaudi::XYZPoint& PosPV = PV->position();
  const Gaudi::XYZPoint& PosSV = SV->position();

  int i = 0;
  ROOT::Math::SVector<int, 2> iso5;
  iso5[0]=0;
  iso5[1]=0;
  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;
  int storeCharge[2];

  LHCb::Particle::ConstVector iso_parts_0;
  LHCb::Particle::ConstVector iso_parts_1;

  // thats where you decide which daughter is at [0] and [1] of the arrays
  LHCb::Particle::Range::const_iterator ip_part;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++)
  {
    const LHCb::Particle* part = *(ip_part);

    if (i==0) iso_parts_0.push_back(part) ;
    if (i==1) iso_parts_1.push_back(part) ;
    const LHCb::ProtoParticle * proto =  (*ip_part)->proto();
    const LHCb::Track* track = proto->track();
    storeCharge[i] = part->charge();

    o_mu(0,i) = track->position().x();
    o_mu(1,i) = track->position().y();
    o_mu(2,i) = track->position().z();
    p_mu(0,i) = track->momentum().x();
    p_mu(1,i) = track->momentum().y();
    p_mu(2,i) = track->momentum().z();
    ptmu[i]   = std::sqrt(std::pow(p_mu(0,i),2)+std::pow(p_mu(1,i),2));
    ++i;

  }// ip_part
  int j=0;

  float doca2[2];
  doca2[0]=10000.;
  doca2[1]=10000.;
  float bestdoca[2];
  bestdoca[0]=10000.;
  bestdoca[1]=10000.;
  const LHCb::Particle* bestpart_0(0) ;
  const LHCb::Particle* bestpart_1(0) ;

  LHCb::Particle::Range::const_iterator ip;
  for ( ip = m_allparts.begin(); ip != m_allparts.end() ; ++ip) {

    if ( msgLevel(MSG::DEBUG) ) debug() <<" looping over all parts "<< endreq;
    j++;
    const LHCb::ProtoParticle * proto =  (*ip)->proto();
    const LHCb::Track* track = proto->track();
    const LHCb::Particle*  cand = (*ip);
    Gaudi::XYZPoint o(track->position());
    Gaudi::XYZVector p(track->momentum());
    bool isInList = 0;
    double pt = p.Rho();

    // skip if other particle is in input list
    if ( msgLevel(MSG::DEBUG) ) debug() <<" check isInList "<< endreq;
    if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {
      isInList = 1;
      if ( msgLevel(MSG::DEBUG) ) debug() <<"isInList is found to be "<<isInList<< endreq;
    }

    if (!isInList) {
      if ( msgLevel(MSG::DEBUG) ) debug() <<"part inNOTinlist"<< endreq;

      int i2=0;

      //Loop over input particles (2)
      for ( i2 = 0; i2 < 2; i2++) {
        bool hltgood = false;
        double fc = 0.;
        Gaudi::XYZPoint omu(o_mu(0,i2),o_mu(1,i2),o_mu(2,i2));
        Gaudi::XYZVector pmu(p_mu(0,i2),p_mu(1,i2),p_mu(2,i2));
        // find roadmap isolation (only long tracks)
        IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc);

        // find doca and angle between input and other tracks
        Gaudi::XYZPoint vtx(0.,0.,0.);
        double doca(-1.);
        double angle(-1.);
        InCone(omu,pmu, o, p, vtx, doca, angle);
        // find impact parameters, distances from secondary and primary vertex
        doca2[i2]=doca;
        if (doca2[i2]<bestdoca[i2]) {
          if (i2==0) bestpart_0 = cand;
          if (i2==1) bestpart_1 = cand;
          bestdoca[i2] = doca2[i2];
        }
        double imp = 0.;
        double impchi2 = 0.;
        double ips,pvdis,svdis;
        ips=100000.;
        LHCb::RecVertex::Container::const_iterator iv;
        LHCb::RecVertex::Container* verts = NULL;
        if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
          verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
        }
        for ( iv = verts->begin(); iv != verts->end(); iv++) {
          m_dist->distance(&(*cand),(*iv),imp,impchi2);
          if (impchi2<ips) ips = impchi2;
        }
        ips=std::sqrt(ips);

        pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
        svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();

        float clone = proto->track()->info(LHCb::Track::CloneDist,100000);
        float ghost = proto->track()->ghostProbability();
        double trchi2 = proto->track()->chi2()/proto->track()->nDoF();
        // non-isolating criteria #5
        double m_ghost = 10000000;
        double m_trchi2 = 10000000;
        if (angle <m_angle && fc<m_fc && doca<m_doca_iso && ips>m_ips && svdis>m_svdis
            && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h
            && clone>m_clone_cut && ghost<m_ghost && track->type()==m_tracktype
            && trchi2<m_trchi2 && hltgood){
          if (i2==0) iso_parts_0.push_back(cand) ;
          if (i2==1) iso_parts_1.push_back(cand) ;
          iso5[i2] += 1;
          if ( msgLevel(MSG::DEBUG) ) debug()<<"isiso "<< i2 << iso5[i2]<< endreq;
        }

      }// i<2
    } // (!isInList)
  }// allparts

  massiso[0]=0.;
  chi2iso[0]=-100.;
  massiso[1]=0.;
  chi2iso[1]=-100.;

  LHCb::Vertex   vertex0   ;
  LHCb::Particle mother0  ;
  LHCb::Vertex   vertex1   ;
  LHCb::Particle mother1  ;

  if (iso_parts_0.size()>1) {

    StatusCode sc0 = StatusCode::SUCCESS;
    sc0 = m_combiner->combine(iso_parts_0,mother0,vertex0 ) ;
    if(sc0==StatusCode::SUCCESS) {
      massiso[0] = mother0.measuredMass();
      chi2iso[0] = mother0.endVertex()->chi2();
      chi2iso[0] = chi2iso[0]/mother0.endVertex()->nDoF();
    }
  }

  else {
    iso_parts_0.push_back(bestpart_0);
    if ( msgLevel(MSG::DEBUG) ) debug()<<"trying to get to this stage "<<endreq;
    StatusCode sc0a = StatusCode::SUCCESS;
    sc0a = m_combiner->combine(iso_parts_0,mother0,vertex0 ) ;
    if(sc0a==StatusCode::SUCCESS) {
      chi2iso[0] = mother0.endVertex()->chi2();
      chi2iso[0] = chi2iso[0]/mother0.endVertex()->nDoF();
    }
  }

  if (iso_parts_1.size()>1) {
    StatusCode sc1 = StatusCode::SUCCESS;
    sc1 = m_combiner->combine(iso_parts_1,mother1,vertex1 ) ;
    if(sc1==StatusCode::SUCCESS) {
      massiso[1] = mother1.measuredMass();
      chi2iso[1] = mother1.endVertex()->chi2();
      chi2iso[1] = chi2iso[1]/mother1.endVertex()->nDoF();
    }
  }

  else {
    iso_parts_1.push_back(bestpart_1);
    StatusCode sc1a = StatusCode::SUCCESS;
    sc1a = m_combiner->combine(iso_parts_1,mother1,vertex1 ) ;
    if(sc1a==StatusCode::SUCCESS) {
      chi2iso[1] = mother1.endVertex()->chi2();
      chi2iso[1] = chi2iso[1]/mother1.endVertex()->nDoF();
    }
  }

  if ( msgLevel(MSG::DEBUG) ) debug()<<"NTRACKS "<<j<<endreq;

  if (msgLevel(MSG::DEBUG) )
  {
    debug()<<"massiso " << massiso[0]<<"  "<<massiso[1]<<" chi2iso " << chi2iso[0]<<"  "<<chi2iso[1]<<endmsg;
    debug()<<" Usual iso5 " << iso5[0]<<"  "<< iso5[1]<<endmsg;
  }

  m_massisoplus =  massiso[0];
  m_massisominus=  massiso[1];

  m_chargeplus  = storeCharge[0];
  m_chargeminus = storeCharge[1];

  m_chi2isoplus = chi2iso[0];
  m_chi2isominus= chi2iso[1];

  m_iso5plus    = iso5[0];
  m_iso5minus   = iso5[1];

  return StatusCode::SUCCESS;

}

