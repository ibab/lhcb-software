// Include files

// local
#include "GetArrival.h"
#include "Interpolator.h"


//-----------------------------------------------------------------------------
// Implementation file for class : GetArrival
//
// 2008-07-02 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GetArrival )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetArrival::GetArrival( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IGetArrival>(this);

  declareProperty("removeSmallest", m_removeSmallest=false,
                  "remove smallest amongst all probabilities for 4 stations");

  declareProperty("useFunct", m_useFunct=false,
                  "use fitted functions instead of histograms");

  declareProperty("moms", m_moms = { {-1.} }, "moms for all 4 stations");

  declareProperty("probs", m_probs,
                  "probs corresponding to moms in each station");

  declareProperty("alpha", m_alpha = { { -1.} }, "parameter alpha in fitted curves");

  declareProperty("beta", m_beta = { { -1. } }, "parameter beta in fitted curves");

  declareProperty("eff", m_eff=.99, "eff of muon chambers");

  declareProperty("MinHits",m_minhits=2,"number of min hits for cls arr");

}

// from m_probs and m_moms fill the m_vprobs
StatusCode GetArrival::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_init = StatusCode::SUCCESS;

  auto un_init = [](double x) { return x == -1. ; };
  if (m_useFunct) {
    if (std::any_of(m_beta.begin(), m_beta.end(),un_init)) {
      m_init.setCode(401);
      return sc;
      // return Error( "BETAS NOT INITIALIZED", m_init);
    }
    if (std::any_of(m_alpha.begin(), m_alpha.end(),un_init)) {
      m_init.setCode(401);
      return sc;
      // return Error( "ALPHAS NOT INITIALIZED", m_init);
    }
  } else{
    //check if tool has been initialized
    if (std::any_of(m_moms.begin(), m_moms.end(), un_init)) {
      //TODO: sure?
      m_init.setCode(401);
      if (msgLevel(MSG::DEBUG) ) debug() << "ARRIVAL TOOL NOT INITIALIZED"<<endmsg;
      // m_init.setChecked();
      return sc;
    }

    //number of points per each station
    unsigned int npoints = m_moms.size();
    if (msgLevel(MSG::DEBUG) ) debug() << " npoints: " << npoints << endmsg;
    if ((m_probs.size()%npoints)!=0){
      m_init.setCode(402);
      return sc;
      // return Error( "INPUT VALUES WRONG SIZE PER STATION", m_init);
    }

    //build vector for each station. For station 0, initialization to 0
    m_vprobs.resize(5);
    for(int i=0;i<5;i++) {
      if (i==0) {
        std::vector<double> vd(npoints);
        m_functprobs.emplace_back(m_moms,vd);
        continue;
      }
      if (msgLevel(MSG::DEBUG) ) debug()<<"ST="<<i<<endmsg;
      std::copy_n( std::next(std::begin(m_probs),(i-1)*npoints), npoints,
                   std::back_inserter(m_vprobs[i]));

      //from vector build uniformer to interpolate between different moms.
      if (msgLevel(MSG::DEBUG) ) debug() << " m_vprobs "<< i << m_vprobs[i]<<endmsg;
      m_functprobs.emplace_back(m_moms,m_vprobs[i]);
    }
  }

  std::array<std::initializer_list<int>,16> types_st{ { {1,1,1,1},
                                                        {1,1,1,0},
                                                        {1,1,0,1},
                                                        {1,0,1,1},
                                                        {0,1,1,1},
                                                        {1,1,0,0},
                                                        {0,0,1,1},
                                                        {0,1,1,0},
                                                        {1,0,0,1},
                                                        {1,0,1,0},
                                                        {0,1,0,1},
                                                        {1,0,0,0},
                                                        {0,1,0,0},
                                                        {0,0,1,0},
                                                        {0,0,0,1},
                                                        {0,0,0,0} } };
  for (const auto& type_st : types_st ) {
    if (countArray(type_st,4,1)>=m_minhits) m_types_st.push_back(type_st);
  }

  if (msgLevel(MSG::DEBUG) ) debug()<<"m_types_st="<<m_types_st<<endmsg;

  m_init.ignore();
  return sc;

}

//find prob for one single station according to mom
double GetArrival::findProbStation(const int st, const double mom) {
  double val;
  if (m_useFunct) {
    //calculate function value for station st
    auto val1=std::pow(m_alpha[st]*(mom-m_beta[st]), st);
    val = ( mom>m_beta[st] ? val1/(1+val1) : .01 );
  } else {
    //calculate value from histogram
    val=m_functprobs[st].getYvalue(mom);
  }
  return 0.99*val;
}


//find arrival probs for all 4 stations
double GetArrival::findProbAllStations(std::vector<int> sts, const double mom)
{
  sort(sts.begin(),sts.end());

  //find prob per each station
  std::vector<double> all_probs; all_probs.reserve(5);
  for (int st=1;st<5;++st) {
    double prob_st=findProbStation(st,mom);
    all_probs.push_back( binary_search(sts.begin(),sts.end(),st) ? prob_st
                                                                 : 1-prob_st);

    if (msgLevel(MSG::DEBUG) ) debug()<<"ST="<<st<<",prob="<<all_probs.back()<<endmsg;
  }

  //sort all probs according to value
  sort(all_probs.begin(),all_probs.end());
  if (msgLevel(MSG::DEBUG) ) debug()<<"all_probs sorted="<<all_probs<<endmsg;

  //if removeSmallest, remove smallest
  if (m_removeSmallest) all_probs.erase(all_probs.begin());

  if (msgLevel(MSG::DEBUG) ) debug()<<"after removal (if removeSmallest)"<<all_probs<<endmsg;

  //calculate prod for all stored values
  double prob = std::accumulate( all_probs.begin(),all_probs.end(),1.,std::multiplies<double>{});

  if (msgLevel(MSG::DEBUG) ) debug()<<"all_prob="<<prob<<endmsg;
  return prob!=0 ? prob : 1e-6;
}


bool GetArrival::stInStations(const int myst,const std::vector<int>& stations)
{
  return std::any_of( stations.begin(), stations.end(),
                      [&](int s) { return s==myst; } );
}

//get array with stations from track's lhcbIDs
StatusCode GetArrival::getStationsFromTrack(const LHCb::Track& mutrack, std::vector<int>& sts, const bool opt)
{
  std::vector<int> sts_init;
  for (const auto& id : mutrack.lhcbIDs()) {
    if (!id.isMuon()) continue;
    auto st=id.muonID().station();
    if (!stInStations(st,sts_init)) sts_init.push_back(st);
  }

  if (msgLevel(MSG::DEBUG)) {
      for (const auto& st : sts_init) {
          debug()<<"added station "<<st<<endmsg;
      }
  }

  if (opt){
    sts.reserve(sts.size()+5);
    for (int ist=1;ist<5;ist++){
      sts.push_back( stInStations(ist,sts_init) ? 1 : 0 );
    }
  } else sts=sts_init;

  return !sts.empty() ? StatusCode::SUCCESS
                      : StatusCode::FAILURE;
}


//get array prob directly from track
StatusCode GetArrival::getArrivalFromTrack(const LHCb::Track& mutrack,double& parr)
{
  parr = 0.;
  StatusCode sc = StatusCode::SUCCESS;
  if (m_init.isFailure()){
    return m_init;
  }

  if (mutrack.lhcbIDs().empty()) {
    sc.setCode(410);
    return Error("NO LHCbIDs ON TRACK. IMPOSSIBLE TO CALCULATE QUALITY",sc);
  }

  std::vector<int> type_st;
  sc = getStationsFromTrack(mutrack,type_st);
  if (sc.isFailure())
  {
    sc.setCode(411);
    return Error("COULD NOT RETRIEVE STS FROM LHCbIDs",sc);
  }

  parr= probTypeSt(mutrack.p(),type_st);
  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"sts= "<<type_st<<endmsg;
    debug()<<"prob= "<<parr<<endmsg;
  }
  return sc;
}

double GetArrival::probTypeSt(const double p, const std::vector<int>& type_st) {

  // returns the probability of getting a given type_st [1,1,1,1] (all stations for example)

  double p4 = probTypeStStation(p,type_st,4);
  double p3 = 0.;
  double p2 = 0.;
  double p1 = 0.;
  double p0 = 0.;

  if (!type_st[3]){
    p3 = probTypeStStation(p,type_st,3);
    if (!type_st[2]){
      p2 = probTypeStStation(p,type_st,2);
      if (!type_st[1]){
        p1 = probTypeStStation(p,type_st,1);
        if (!type_st[0]) p0 = probTypeStStation(p,type_st,0);
      }
    }
  }

  double pp = p4 + p3 + p2 + p1 + p0;

  if (msgLevel(MSG::DEBUG) ) debug()<< "@probTypeSt: "<<type_st<<","<<p4<<","<<p3<<","<<p2<<","<<p1<<","<<p0<<","<<pp<<endmsg;
  return pp;
}



int GetArrival::countArray(const std::vector<int>& type_st, const int up_to, const int el){
  auto first = type_st.begin();
  return std::count( first, first+up_to, el );
}



double GetArrival::probTypeStStation(const double p, const std::vector<int>& type_st,const int station){

  //returns the probability of a given type_st, assuming that a track reach that station (3 or 4)
  auto f = [&](int i) { return findProbStation(i,p); };

  double PP; int m; int nholes;
  switch (station) {
    case 4 : PP =    f(4);                             m=4; nholes = countArray(type_st,4,0);break;
    case 3 : PP = (1-f(4))*   f(3);                    m=3; nholes = countArray(type_st,3,0);break;
    case 2 : PP = (1-f(4))*(1-f(3))*   f(2);           m=2; nholes = countArray(type_st,2,0);break;
    case 1 : PP = (1-f(4))*(1-f(3))*(1-f(2))*   f(1);  m=1; nholes = countArray(type_st,1,0);break;
    default: PP = (1-f(4))*(1-f(3))*(1-f(2))*(1-f(1)); m=0; nholes = 0; break;
  }

  double val = PP*std::pow(m_eff,m-nholes)*std::pow((1-m_eff),nholes);

  if (msgLevel(MSG::DEBUG) ) debug()
    <<"@probTypeStStation: "<<p<<","<<type_st<<","<<station
    <<" \np4, p3, p2, p1, pp ="<<f(4)<<","<<f(3)<<","<<f(2)<<","<<f(1)<<","<<PP
    <<" \neff, (1-eff) "<<","<<std::pow(m_eff,m-nholes)<<","<<std::pow((1-m_eff),nholes)
    <<" \nval "<<val<<endmsg;

  return val;
}


StatusCode GetArrival::clArrival(const LHCb::Track& muTrack, double& clarr){
  StatusCode sc = StatusCode::SUCCESS;

  if (m_init.isFailure()) {
    clarr=0.;
    return m_init;
  }

  if (muTrack.lhcbIDs().empty()) {
    sc.setCode(410);
    return Error("NO LHCbIDs ON TRACK. IMPOSSIBLE TO CALCULATE CL",sc);
  }


  std::vector<int> type_st;
  sc = getStationsFromTrack(muTrack,type_st);
  if (sc.isFailure()) {
    clarr=0.;
    sc.setCode(411);
    return Error("COULD NOT RETRIEVE STS FROM LHCbIDs",sc);
  }
  return clArrival(muTrack.p(), type_st, clarr);
}


StatusCode GetArrival::clArrival(const double p,const std::vector<int>& type_st, double& clarr)
{
  StatusCode sc = StatusCode::SUCCESS;

  if (countArray(type_st,4,1)<m_minhits) {
    sc.setCode(412);
    if (msgLevel(MSG::DEBUG) ) debug()<<"number of hits less than min"<<endmsg;
    return sc;
  }

  //return CL of a muon having as this type_st or a worse type_st of this
  std::vector<double> vals; vals.reserve(m_types_st.size());
  std::transform( m_types_st.begin(), m_types_st.end(),
                  std::back_inserter(vals),
                  [&](const std::vector<int>& i) { return probTypeSt(p,i); } );
  double tot  = std::accumulate( vals.begin(), vals.end(), 0. );
  double sval = probTypeSt(p,type_st);
  double stot = std::accumulate( vals.begin(), vals.end(), 0.,
                                 [&](double s, double v) {
        return (std::abs(v)<sval+1.e-5) ? s+v : s ;
  });
  if (tot > ((double) 0.)) clarr = stot/tot;
  else {
    if (msgLevel(MSG::DEBUG) ) debug()<<"tot=0, division by 0"<<endmsg;
    return sc;
  }

  if (msgLevel(MSG::DEBUG)) {
    for (unsigned int i=0; i<m_types_st.size();i++){
      debug() << " probTypeSt " << m_types_st[i]<<","<<vals[i]<<endmsg;
    }
    debug() << " currenttype_st "<<type_st<<","<<sval
            << " tot,stot,cls "<<tot<<","<<stot<<","<<clarr<<endmsg;
  }

  return sc;
}

GetArrival::~GetArrival() = default;
