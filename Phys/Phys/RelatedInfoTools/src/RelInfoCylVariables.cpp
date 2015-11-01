// Include files
// local

#include "RelInfoCylVariables.h"
#include "Kernel/RelatedInfoNamed.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/HitPattern.h>
#include "Event/Particle.h"
#include "math.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoCylVariables
//
// 2014-06-30 : Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoCylVariables )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoCylVariables::RelInfoCylVariables( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent):
GaudiTool ( type, name , parent )
{

  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "Variables",
                   m_variables,
                   "List of variables to store (store all if empty)");

  declareProperty( "useVelo",m_useVelo=true,"Use VELO tracks");
  declareProperty( "trackQuality",m_trackQuality=true,"Appy quality cuts on tracks");
  declareProperty( "samePV",m_samePV=true,"Use only tracks associated to the same PV as the signal");
  declareProperty( "cylSize",m_cyl_size=15,"Cylinder size in units of DOCAchi2");
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoCylVariables::~RelInfoCylVariables() {}


//====================================================================
// Initialize DVAlg, etc
//====================================================================
StatusCode RelInfoCylVariables::initialize() {

  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;

  //initialize the tool keys
  m_keys.clear();

  for ( const auto& var : m_variables )
  {
    short int key = RelatedInfoNamed::indexByName( var );
    if (key != RelatedInfoNamed::UNKNOWN) {
      m_keys.push_back( key );
      debug() << "Adding variable " << var << ", key = " << key << endmsg;
    } else {
      Warning("Unknown variable " + var + ", skipping");
    }
  }

  debug()<< "Initialization correct" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// invariant mass for track1 and track2 assuming masses m1 and m2
//=============================================================================
double RelInfoCylVariables::invariantMass(const LHCb::Track* tr1, const LHCb::Track* tr2,
                                          const double m1, const double m2){
  double E1 = sqrt(pow(tr1->p(),2)+pow(m1,2));
  double E2 = sqrt(pow(tr2->p(),2)+pow(m2,2));
  Gaudi::LorentzVector v1,v2;
  v1.SetPxPyPzE(tr1->momentum().x(),tr1->momentum().y(),tr1->momentum().z(),E1);
  v2.SetPxPyPzE(tr2->momentum().x(),tr2->momentum().y(),tr2->momentum().z(),E2);
  Gaudi::LorentzVector vsum = v1+v2;
  return vsum.M();
}


//=============================================================================
// get the PV with the smallest chi2 of all
//=============================================================================
void RelInfoCylVariables::getBestPV(const LHCb::Track* track, LHCb::VertexBase* & mypv, double & myipchi2){

  if(!exist<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary)){
    debug()<<"No PVs found in the event"<<endmsg;
    return;
  }

  double ip,ipchi2;
  LHCb::RecVertex::Container* verts = NULL;
  std::vector<PairDoubleVertex> myvector;

  // Get DVAlgo
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) return;

  verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  // create a list of pairs ipchi2/vertex
  for ( LHCb::RecVertex::Container::const_iterator iv = verts->begin(); iv != verts->end(); iv++) {
    StatusCode sc = dva->distanceCalculator()->distance(track,(*iv),ip,ipchi2);
    if (sc.isFailure()) {
      debug()<<"Failure obtaining IPchi2"<<endmsg;
      continue;
    }
    PairDoubleVertex mypair(ipchi2,(*iv));
    myvector.push_back(mypair);
  }
  if (myvector.empty()){
    debug()<<"No IPchi2 obtained, return"<<endmsg;
    return;
  }

  // sort them from smaller to largest
  std::sort(myvector.begin(), myvector.end());
  // now get the bestPV and ipchi2
  mypv = myvector.at(0).second;
  myipchi2 = myvector.at(0).first;
  return;
}



//=============================================================================
// find tracks inside the cylinder
// add info about IPchi2, pT and Bmass for those
//=============================================================================
void RelInfoCylVariables::tracksInsideCyl(const LHCb::Track*& mytrack, const int mypvkey, MapTrackList& out_list)
{

  // Get DVAlgo
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) return;

  double doca,docachi2,ipchi2;
  LHCb::Tracks* all_tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
  // loop in all the tracks
  debug()<<"Looping in all event tracks"<<endmsg;
  for( LHCb::Tracks::const_iterator it = all_tracks->begin(); it != all_tracks->end(); ++it){
    LHCb::Track* track = (*it);

    // this is our track
    if (mytrack->key()==track->key()) continue;

    // get only VELO or Long tracks
    if (!(track->type()==LHCb::Track::Long || track->type()==LHCb::Track::Velo)){
      debug()<<"Track is not long or velo"<<endmsg;
      continue;
    }


    if (track->type()==LHCb::Track::Velo && !m_useVelo) {
      debug()<<"Track is VELO"<<endmsg;
      continue;
    }


    // apply quality tracks
    // chi2
    const double chi2 = track->chi2PerDoF();
    if ( m_trackQuality && (chi2 > m_max_chi2 || chi2< m_min_chi2 || track->nDoF()<=0 ) ){
      debug()<<"Track does not pass the chi2 cut"<<endmsg;
      continue;
    }

    // cloneDist (for long tracks)
    const double cloneDist = track->info(LHCb::Track::CloneDist,9e99);
    if ( m_trackQuality &&
         (track->type()==LHCb::Track::Long) && ( track->checkFlag(LHCb::Track::Clone) ||
                                                 cloneDist < m_min_clone || cloneDist > m_max_clone ) ) {
      debug()<<"Track does not pass the clone cut"<<endmsg;
      continue;
    }


    // velo holes (for velo tracks)
    if ( m_trackQuality && (track->type()==LHCb::Track::Velo) )
    {
      LHCb::HitPattern hitpattern(track->lhcbIDs()) ;
      int numVeloHoles = hitpattern.numVeloHoles() ;
      if( numVeloHoles > m_max_nveloholes ) {
        debug()<<"Velo track does not pass the VELO holes cut"<<endmsg;
        continue;
      }

    }

    // is from the same PV?
    ipchi2=-1.;
    LHCb::VertexBase* pv=NULL;
    getBestPV(track,pv,ipchi2);
    int bpvkey;
    if (pv) bpvkey  = pv->key();
    else bpvkey  = -1;
    if (m_samePV && bpvkey!=mypvkey) {
      debug()<<"The track comes from a different PV"<<endmsg;
      continue;
    }


    // cut in docachi2
    StatusCode sc = dva->distanceCalculator()->distance(mytrack,track,doca,docachi2);
    if (sc.isFailure()) {
      debug()<<"Could not find the docachi2 between both tracks"<<endmsg;
      continue;
    }

    if (docachi2 > m_cyl_size) {
      debug()<<"Track is outside the cylinder"<<endmsg;
      continue;
    }

    MapStringDouble myvect;
    // add pt
    if (track->type()==LHCb::Track::Velo) myvect["pt"]=-1.;
    else myvect["pt"]=track->pt();

    // add IPchi2
    myvect["ipchi2"]=ipchi2;

    // add the invariant mass in the pipi hypothesis
    if (track->type()==LHCb::Track::Velo) myvect["bmass"]=-1.;
    else myvect["bmass"]=invariantMass(mytrack,track,m_mPi,m_mPi);

    // add DOCAchi2
    myvect["docachi2"]=docachi2;


    out_list[track]=myvect;
  }

}

//=============================================================================
// Function to obtain max/min/av inside the cylinder
// vartype to know if the track we want to extract is the maximum (pT, IPchi2)
// or the minimum (DOCAchi2)
//=============================================================================
void RelInfoCylVariables::varSorted(const MapTrackList mycyl, const std::string vartype,
                                    LHCb::Track* & out_track,
                                    double & out_min, double & out_max, double & out_av)
{

  // some initialize
  out_min = 1e7;
  out_max = -1.;
  out_av = 0.;

  if (mycyl.empty()) {
    debug()<<"No tracks in the cylinder, nothing to do"<<endmsg;
    return;
  }

  MapStringDouble vals;
  double val;
  double c1 = 0.;

  // start the loop
  for( MapTrackList::const_iterator it = mycyl.begin(); it != mycyl.end(); ++it){
    vals = it->second;
    val = vals[vartype];

    if (val<0) continue; // this can only happen for failures or VELO tracks and pT, so ignore
    // max vals
    if (val>out_max){
      out_max = val;
      if (vartype=="ipchi2"||vartype=="pt") out_track = it->first;
    }
    // min vals
    if (val<out_min){
      out_min = val;
      if (vartype=="docachi2") out_track = it->first;
    }
    // average vals
    out_av+=val;
    c1+=1.;
  }
  if (c1>0){
    out_av/=c1;
    debug()<< "The selected track is " << out_track->key()<<endmsg;
  }

  debug()<< "Loop in cylinder done for var" << vartype << endmsg;
  debug()<< "Number of tracks was" << mycyl.size() << "..." <<endmsg;
  debug()<< "... out of which " << c1 << "had a valid value"  <<endmsg;
  debug()<< "Min value is "<<out_min<<endmsg;
  debug()<< "Max value is "<<out_max<<endmsg;
  debug()<< "Average value is "<<out_av<<endmsg;
}

//=============================================================================
// Function to obtain the track yielding a mass closest to the B
// and the value of this mass
//=============================================================================
void RelInfoCylVariables::varSortedB(const MapTrackList mycyl, LHCb::Track* & out_track, double & out_val)
{

  // some initialize
  out_val = -1.;

  if (mycyl.empty()) {
    debug()<<"No tracks in the cylinder, nothing to do"<<endmsg;
    return;
  }


  MapStringDouble vals;
  double val, bdiff, bdiff0;
  int c1=0;
  bdiff0 = 1e7;

  // start the loop
  for( MapTrackList::const_iterator it = mycyl.begin(); it != mycyl.end(); ++it)
  {

    vals = it->second;
    val = vals["bmass"]; // this is the index for Bmass

    if (val<0) continue; // this can only happen for failures or VELO tracks and pT, so ignore
    c1+=1;
    // get absolute value of the difference to 5000
    bdiff = val-m_mB;
    if ( bdiff < 0 ) bdiff = -bdiff;

    if ( bdiff < bdiff0) {
      out_val = val;
      bdiff0 = bdiff;
      out_track = it->first;
    }
  }
  debug()<< "Loop in cylinder done for B mass" << endmsg;
  debug()<< "Number of tracks was" << mycyl.size() << "..." <<endmsg;
  debug()<< "... out of which " << c1 << "had a valid value"  <<endmsg;
  debug()<< "Min value is "<<out_val<<endmsg;
  if (c1>0) debug()<< "The selected track is " << out_track->key()<<endmsg;
}


//=============================================================================
// add combination to the particle list
//=============================================================================
void RelInfoCylVariables::addToParList(const LHCb::Track* tr1,const LHCb::Track* tr2,
                                       const double m1, const double m2, const bool isKs,
                                       std::vector< std::pair<double,double> > & myl)
{
  double mydiff;
  double mass = invariantMass(tr1,tr2,m1,m2);
  // separate the KS and D0 cases
  if (isKs) mydiff = mass-m_mKs;
  else mydiff = mass - m_mD0;
  if (mydiff<0) mydiff = -mydiff;
  std::pair<double,double> mypair(mydiff,mass);
  debug()<<"Adding "<<mypair<<" to pair list"<<endmsg;
  myl.push_back(mypair);
}


//=============================================================================
// get the best D0 or Ks mass in the cylinder
//=============================================================================
void RelInfoCylVariables::getBestD0Ks(const MapTrackList & mycyl)
{
  m_properties["bestD0"]=-1.;
  m_properties["bestKS"]=-1.;

  if (mycyl.empty()) {
    debug()<<"The cylinder list is empty, nothing to do"<<endmsg;
    return;
  }



  std::vector< std::pair<double,double> > mylKs;
  std::vector< std::pair<double,double> > mylD0;

  for (MapTrackList::const_iterator it1 = mycyl.begin();it1 != mycyl.end(); ++it1){
    LHCb::Track* tr1 = it1->first;
    if (tr1->type()==LHCb::Track::Velo) continue;
    for (MapTrackList::const_iterator it2 = mycyl.begin();it2 != mycyl.end(); ++it2){
      LHCb::Track* tr2 = it2->first;
      if (tr2->type()==LHCb::Track::Velo) continue;
      if (tr1==tr2) continue;
      // these four are for D0
      addToParList(tr1,tr2,m_mPi,m_mPi,false,mylD0);
      addToParList(tr1,tr2,m_mPi,m_mK,false,mylD0);
      addToParList(tr1,tr2,m_mK,m_mPi,false,mylD0);
      addToParList(tr1,tr2,m_mK,m_mK,false,mylD0);
      // and this for Ks
      addToParList(tr1,tr2,m_mPi,m_mPi,true,mylKs);
      debug()<<"best D0/Ks mass: combination "<< tr1->pt() <<"/"<< tr2->pt() <<"done"<<endmsg;
    }
  }
  if (mylKs.empty())
  {
    debug()<< "There were not useful tracks, nothing to do"<<endmsg;
    return;
  }

  std::sort(mylD0.begin(), mylD0.end());
  m_properties["bestD0"] = mylD0.at(0).second;

  std::sort(mylKs.begin(), mylKs.end());
  m_properties["bestKS"] = mylKs.at(0).second;

}




//=============================================================================
// Empty properties list
//=============================================================================
void RelInfoCylVariables::emptyProperties(void)
{
  m_properties["bestD0"]= -1;
  m_properties["bestKS"]= -1;
  m_properties["tracksNCyl"]= -1;

  // track with best PT
  m_properties["minPT"]= -1;
  m_properties["maxPT"]= -1;
  m_properties["avPT"]= -1;
  m_properties["maxPTtrIPchi2"]= -1;
  m_properties["maxPTtrDOCAChi2"]= -1;
  m_properties["maxPTtrBmass"]= -1;

  // track with best IPchi2
  m_properties["minIPchi2"]= -1;
  m_properties["maxIPchi2"]= -1;
  m_properties["avIPchi2"]= -1;
  m_properties["maxIPchi2trPT"]= -1;
  m_properties["maxIPchi2trDOCAChi2"]= -1;
  m_properties["maxIPchi2trBmass"]= -1;

  // track with best DOCAchi2
  m_properties["minDOCAchi2"]= -1;
  m_properties["maxDOCAchi2"]= -1;
  m_properties["avDOCAchi2"]= -1;
  m_properties["minDOCAtrPT"]= -1;
  m_properties["minDOCAtrIPchi2"]= -1;
  m_properties["minDOCAtrBmass"]= -1;

  // track with best B mass
  m_properties["bestBmass"]= -1;
  m_properties["bestBmasstrPT"]= -1;
  m_properties["bestBmasstrIPchi2"]= -1;
  m_properties["bestBmasstrDOCAChi2"]= -1;
}

//=============================================================================
// Fill properties for a var
//=============================================================================
MapStringDouble RelInfoCylVariables::fillVar(MapTrackList mycyl,std::string vartype)
{
  MapStringDouble out_vals;
  double out_min, out_max, out_av;
  LHCb::Track* mytrack=NULL;
  if (vartype=="bmass"){
    varSortedB(mycyl,mytrack,out_min);
    out_vals["min"]=out_min;
  }
  else {
    varSorted(mycyl,vartype,mytrack,out_min,out_max,out_av);
    out_vals["min"]=out_min;
    out_vals["max"]=out_max;
    out_vals["av"]=out_av;
  }

  if (mytrack) {
    out_vals["track_pt"]=mycyl[mytrack]["pt"];
    out_vals["track_ipchi2"]=mycyl[mytrack]["ipchi2"];
    out_vals["track_docachi2"]=mycyl[mytrack]["docachi2"];
    out_vals["track_bmass"]=mycyl[mytrack]["bmass"];
  }
  else {
    out_vals["track_pt"]=-1.;
    out_vals["track_ipchi2"]=-1.;
    out_vals["track_docachi2"]=-1.;
    out_vals["track_bmass"]=-1.;
  }
  return out_vals;
}



//=============================================================================
// Fill all properties
//=============================================================================
void RelInfoCylVariables::fillProperties(MapTrackList mycyl)
{

  // fill the numbers for the important variables
  MapStringDouble vals_pt = fillVar(mycyl,"pt");
  MapStringDouble vals_ipchi2 = fillVar(mycyl,"ipchi2");
  MapStringDouble vals_docachi2 = fillVar(mycyl,"docachi2");
  MapStringDouble vals_bmass = fillVar(mycyl,"bmass");

  // fill the best D0/Ks info
  getBestD0Ks(mycyl);
  m_properties["tracksNCyl"]=mycyl.size();

  // track with best PT
  m_properties["minPT"]=vals_pt["min"];
  m_properties["maxPT"]=vals_pt["max"];
  m_properties["avPT"]=vals_pt["av"];
  m_properties["maxPTtrIPchi2"]=vals_pt["track_ipchi2"];
  m_properties["maxPTtrDOCAChi2"]=vals_pt["track_docachi2"];
  m_properties["maxPTtrBmass"]=vals_pt["track_bmass"];

  // track with best IPchi2
  m_properties["minIPchi2"]=vals_ipchi2["min"];
  m_properties["maxIPchi2"]=vals_ipchi2["max"];
  m_properties["avIPchi2"]=vals_ipchi2["av"];
  m_properties["maxIPchi2trPT"]=vals_ipchi2["track_pt"];
  m_properties["maxIPchi2trDOCAChi2"]=vals_ipchi2["track_docachi2"];
  m_properties["maxIPchi2trBmass"]=vals_ipchi2["track_bmass"];

  // track with best DOCAchi2
  m_properties["minDOCAchi2"]=vals_docachi2["min"];
  m_properties["maxDOCAchi2"]=vals_docachi2["max"];
  m_properties["avDOCAchi2"]=vals_docachi2["av"];
  m_properties["minDOCAtrPT"]=vals_docachi2["track_pt"];
  m_properties["minDOCAtrIPchi2"]=vals_docachi2["track_ipchi2"];
  m_properties["minDOCAtrBmass"]=vals_docachi2["track_bmass"];

  // track with best B mass
  m_properties["bestBmass"]=vals_bmass["min"];
  m_properties["bestBmasstrPT"]=vals_bmass["track_pt"];
  m_properties["bestBmasstrIPchi2"]=vals_bmass["track_ipchi2"];
  m_properties["bestBmasstrDOCAChi2"]=vals_bmass["track_docachi2"];

  debug() << "Dictionary created for cylinder " << endmsg;
  debug() << "with value " << m_properties << endmsg;
}

//=============================================================================
// Fill Cone Info structure
//=============================================================================
StatusCode RelInfoCylVariables::calculateRelatedInfo( const LHCb::Particle *top,
                                                      const LHCb::Particle *part )
{

  debug() << "The top particle is " << top->key() << endmsg;
  debug() << "And the particle is " << part->key() << endmsg;

  emptyProperties();
  fillMap();
  debug()<< "Filled empty dicts" << endmsg;

  // only fill infos for top particle!
  if (part!=top) return StatusCode::SUCCESS;

  const LHCb::ProtoParticle* myproto = top->proto();
  if (!myproto) {
    debug()<<"Particle has no proto..."<<endmsg;
    return StatusCode::SUCCESS;;
  }

  const LHCb::Track* mytrack = myproto->track();
  if (!mytrack) {
    debug()<<"Proto has no track..."<<endmsg;
    return StatusCode::SUCCESS;
  }


  // get bestPV of the top particle
  double bestipchi2=-1.;
  LHCb::VertexBase* pv=NULL;
  getBestPV(mytrack,pv,bestipchi2);
  int mypvkey;
  if (pv) mypvkey  = pv->key();
  else mypvkey  = -1;
  debug() << "The top particle is associated to PV " << mypvkey << endmsg;

  MapTrackList mycyl;
  tracksInsideCyl(mytrack, mypvkey, mycyl);
  if (mycyl.empty()){
    debug()<<"Cylinder is empty, nothing to do"<<endmsg;
    return StatusCode::SUCCESS;
  }
  fillProperties(mycyl);
  fillMap();
  return StatusCode::SUCCESS;
}

//=============================================================================
// Fill Output m_map
//=============================================================================
void  RelInfoCylVariables::fillMap(){

  m_map.clear();
  for ( const auto key : m_keys ){

    float value = 0;
    switch (key) {

    case RelatedInfoNamed::CYLBESTD0    : value = m_properties["bestD0"]; break;
    case RelatedInfoNamed::CYLBESTKS    : value = m_properties["bestKS"]; break;
    case RelatedInfoNamed::CYLTRACKSNCYL    : value = m_properties["tracksNCyl"]; break;
    case RelatedInfoNamed::CYLMINPT    : value = m_properties["minPT"]; break;
    case RelatedInfoNamed::CYLMAXPT    : value = m_properties["maxPT"]; break;
    case RelatedInfoNamed::CYLAVPT    : value = m_properties["avPT"]; break;
    case RelatedInfoNamed::CYLMAXPTTRIPCHI2    : value = m_properties["maxPTtrIPchi2"]; break;
    case RelatedInfoNamed::CYLMAXPTTRDOCACHI2    : value = m_properties["maxPTtrDOCAChi2"]; break;
    case RelatedInfoNamed::CYLMAXPTTRBMASS    : value = m_properties["maxPTtrBmass"]; break;
    case RelatedInfoNamed::CYLMINIPCHI2    : value = m_properties["minIPchi2"]; break;
    case RelatedInfoNamed::CYLMAXIPCHI2    : value = m_properties["maxIPchi2"]; break;
    case RelatedInfoNamed::CYLAVIPCHI2    : value = m_properties["avIPchi2"]; break;
    case RelatedInfoNamed::CYLMAXIPCHI2TRPT    : value = m_properties["maxIPchi2trPT"]; break;
    case RelatedInfoNamed::CYLMAXIPCHI2TRDOCACHI2    : value = m_properties["maxIPchi2trDOCAChi2"]; break;
    case RelatedInfoNamed::CYLMAXIPCHI2TRBMASS    : value = m_properties["maxIPchi2trBmass"]; break;
    case RelatedInfoNamed::CYLMINDOCACHI2    : value = m_properties["minDOCAchi2"]; break;
    case RelatedInfoNamed::CYLMAXDOCACHI2    : value = m_properties["maxDOCAchi2"]; break;
    case RelatedInfoNamed::CYLAVDOCACHI2    : value = m_properties["avDOCAchi2"]; break;
    case RelatedInfoNamed::CYLMINDOCATRPT    : value = m_properties["minDOCAtrPT"]; break;
    case RelatedInfoNamed::CYLMINDOCATRIPCHI2    : value = m_properties["minDOCAtrIPchi2"]; break;
    case RelatedInfoNamed::CYLMINDOCATRBMASS    : value = m_properties["minDOCAtrBmass"]; break;
    case RelatedInfoNamed::CYLBESTBMASS    : value = m_properties["bestBmass"]; break;
    case RelatedInfoNamed::CYLBESTBMASSTRPT    : value = m_properties["bestBmasstrPT"]; break;
    case RelatedInfoNamed::CYLBESTBMASSTRIPCHI2    : value = m_properties["bestBmasstrIPchi2"]; break;
    case RelatedInfoNamed::CYLBESTBMASSTRDOCACHI2    : value = m_properties["bestBmasstrDOCAChi2"]; break;

    default: value = 0.; break;
    }

    debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endmsg;
    m_map.insert( std::make_pair(key,value) );
  }

}


LHCb::RelatedInfoMap* RelInfoCylVariables::getInfo(void){
  return &m_map;
}


