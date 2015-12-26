// Include files
// local
#include "CLTool.h"
#include "Interpolator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CLTool
//
// 2008-07-02 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CLTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CLTool::CLTool( const std::string& type,
                const std::string& name,
                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_leftRight(""),
    m_overlap(false),
    m_applyLast(false),
    m_nrange(0),
    m_nrangeNmuons(0),
    m_range(0),
    m_rangeNmuons(0),
    m_mombinsCenter(0),
    m_mombinsCenterNmuons(0),
    m_yvals(0),
    m_nvals(0),
    m_minMomentum(0.),
    m_lbinCenter(0.),
    m_lbinCenterNmuons(0.),
    m_signal(0),
    m_bkg(0),
    m_vsignal(0),
    m_vbkg(0),
    m_unifsignal(), //why is there no default constructor for uniformer??
    m_unifbkg(),    //why is there no default constructor for uniformer??
    m_init(StatusCode::FAILURE)
{
  declareInterface<ICLTool>(this);

  //Get momentum bins for signal distributions
  declareProperty("Range", m_range = { {-1.} });
  //Get momentum bins for background distributions
  declareProperty("RangeNmuons", m_rangeNmuons = { {-1.} });
  //Decide wether to integrate to right or left
  declareProperty("LeftRight", m_leftRight);
  //Get distributions for signal
  declareProperty("Signal", m_signal);
  //Get distributions for background
  declareProperty("Bkg", m_bkg);
  //Decide if you apply interpolation between momentum bins
  declareProperty("Overlap",m_overlap=true);
  //Last mom bin for signal: set center
  declareProperty("lbinCenter",m_lbinCenter);
  //Last mom bin for background: set center
  declareProperty("lbinCenterNmuons",m_lbinCenterNmuons=0.);
  //Apply interpolation after center of last mom bin. May induce negative values!
  declareProperty("applyOvlapLast",m_applyLast=false);
}

StatusCode CLTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_init = StatusCode::SUCCESS;
  //check if tool has to be initialized
  if (std::any_of( m_rangeNmuons.begin(), m_rangeNmuons.end(),
                   [](double x) { return x == -1.;} )) {
    m_init.setCode(500);
    // return Error("CLTOOL: NOT INITIALIZED!",m_init);
    return sc;
  }

  // from m_signal and m_range fill the m_vsignal
  m_nrange = m_range.size()-1;
  if (msgLevel(MSG::DEBUG) ) debug() << " range: " << m_range << endmsg;

  //build array with bins center for signal
  for (int i=1;i<=m_nrange;i++) m_mombinsCenter.push_back((m_range[i]+m_range[i-1])/2);
  m_mombinsCenter[m_nrange-1]=m_lbinCenter;

  if (m_signal.size()%m_nrange!=0){
    m_init.setCode(501);
    // return Error( "CLTOOL: INPUT VALUES, WRONG SIZE PER MOM BIN FOR MUONS",m_init );
    return sc;
  }
  if (msgLevel(MSG::DEBUG) ) debug()<<"m_rangeNmuons_init="<<m_rangeNmuons<<endmsg;

  //if range for background was not initialized, make it same as signal's
  bool cond=std::any_of( m_rangeNmuons.begin(), m_rangeNmuons.end(),
                         [](double x) { return x == -1. ; } );
  if (msgLevel(MSG::DEBUG) ) debug()<<"cond="<<cond<<endmsg;

  if (cond) {
    m_rangeNmuons.clear();
    if (msgLevel(MSG::DEBUG) ) debug()<<"m_rangeNmuons.size() after clear is"<<m_rangeNmuons.size()<<endmsg;
    for (int i=0;i<=m_nrange;i++) m_rangeNmuons.push_back(m_range[i]);
    m_lbinCenterNmuons=m_lbinCenter;
  }
  m_nrangeNmuons=m_rangeNmuons.size()-1;

  // check min momentum and initialize
  if (m_range[0]!=m_rangeNmuons[0])
  {
    m_init.setCode(502);
    // return Error( "CLTOOL: MIN MOM NOT THE SAME FOR MUONS AND NOT MUONS!", m_init);
    return sc;
  }

  m_minMomentum=m_range[0];
  if (msgLevel(MSG::DEBUG) ) debug() << " min mom=" << m_minMomentum << endmsg;

  //build array with bins center for background
  for (int i=1;i<=m_nrangeNmuons;i++) m_mombinsCenterNmuons.push_back((m_rangeNmuons[i]+m_rangeNmuons[i-1])/2);
  m_mombinsCenterNmuons[m_nrangeNmuons-1]=m_lbinCenterNmuons;

  if (msgLevel(MSG::DEBUG) ) {
    debug()<<"CLTool:: NMUONSINFO"<<endmsg;
    debug()<<"CLTool:: m_rangeNmuons="<<m_rangeNmuons<<endmsg;
    debug()<<"CLTool:: m_mombinsCenterNmuons="<<m_mombinsCenterNmuons<<endmsg;
  }

  if (m_bkg.size()%m_nrangeNmuons!=0){
    m_init.setCode(503);
    // m_init.setChecked();
    // return m_init;
    // return Error( "CLTOOL: INPUT VALUES, WRONG SIZE PER MOMENTUM BIN FOR NON MUONS", m_init);
    return sc;
  }

  m_nvals=m_signal.size()/m_nrange;
  int nvalsNmuons=m_bkg.size()/m_nrangeNmuons;

  //check number of elements per momentum bin is the same for signal and bkg
  if (m_nvals!=nvalsNmuons){
    m_init.setCode(504);
    // m_init.setChecked();
    // return m_init;
    // return Error( "CLTOOL: DIFFERENT SIZE FOR MUONS AND NON MUONS FOR CL", m_init);
    return sc;
  }

  //find yvals (same for signal and bkg): y points in cl functions
  int ii=0; // @TODO: C++14 -- define ii inside capture expression
  double ii_step = 1./(m_nvals-1); // @TODO: C++14 -- define ii_step inside capture expression;
  std::generate_n( std::back_inserter(m_yvals), m_nvals,
                   [=]() mutable { return ii_step*(ii++); } );
  if (msgLevel(MSG::DEBUG) ) debug() << "CLTool:: recorded "<<m_nrange<<" momentum bins "
         << "with "<< m_yvals.size() << " vals each" << endmsg;

  //build uniformer functions for both signal and bkg
  StatusCode stc1=getClValues(sb::sig);
  StatusCode stc2=getClValues(sb::bkg);

  if  (stc1.isFailure() || stc2.isFailure()) {
    m_init.setCode(505);
    // return Error("CLTOOL: UNIFORMED FUNCTIONS FOR SIGNAL,BKG FAIL!",m_init);
    return sc;
  }

  m_init.setChecked();
  return sc;
}

//build uniformer per each momentum bin
StatusCode CLTool::getClValues(sb sig_bkg)
{
  // initialize depending on signal or bkg
  std::vector< std::vector<double>>& my_v = ( sig_bkg==sb::sig ? m_vsignal : m_vbkg );
  std::vector<double>& my_sbkg = ( sig_bkg==sb::sig ? m_signal : m_bkg );
  std::vector<Uniformer>& my_unif = ( sig_bkg==sb::sig ? m_unifsignal : m_unifbkg );

  int my_nrange = ( sig_bkg==sb::sig ? m_nrange : m_nrangeNmuons );

  static const std::array<std::string,2> labels{"m_vsignal","m_vbkg"};

  //build xinput for uniformers (x values corresponding to cl's) for each momentum bin. Will build one uniformer per mom bin
  my_v.resize(my_nrange);
  my_unif.reserve(my_nrange);
  for (int i=0;i<my_nrange;i++) {
    // if (msgLevel(MSG::DEBUG) ) debug()<<"P range="<<i<<endmsg;
    my_v[i].reserve(m_nvals);
    std::copy_n( std::next(my_sbkg.begin(), i*m_nvals), m_nvals,
                 std::back_inserter( my_v[i] ));
    //debug() << labels[sig_bkg]<< i << my_v[i]<<endmsg;
    my_unif.emplace_back(my_v[i],m_yvals);
  }
  return StatusCode::SUCCESS;
}


// find corresponding momentum bin. May be different for signal and bkg
StatusCode CLTool::findMomRange(const double& mom,int& p_r,sb sig_bkg)
{
  auto& range = (sig_bkg==sb::sig? m_range : m_rangeNmuons );
  // loop over momentum bins edges and return
  auto i = std::upper_bound( range.begin(), range.end(), mom);
  if (i==range.begin()||i==range.end()) {
    p_r=-1;
    if ( msgLevel(MSG::DEBUG)) debug()<<"MOM OUT OF RANGE"<<endmsg;
    return StatusCode::FAILURE;
  }
  p_r = std::distance(range.begin(), i)-1;
  if (msgLevel(MSG::DEBUG) ) debug() << "MOM RANGE IS " << p_r<<endmsg;
  return StatusCode::SUCCESS;
}


//get value from corresponding uniformer (signal or bkg+mombin)
double CLTool::valFromUnif(double value, double mom, int p_r, sb sig_bkg)
{

  double ret_val=0;

  //initialize depending on signal or bkg
  const std::vector<Uniformer>& unifrel = ( sig_bkg==sb::sig ? m_unifsignal : m_unifbkg );
  const std::vector<double>& my_mombinsCenter = ( sig_bkg==sb::sig ? m_mombinsCenter : m_mombinsCenterNmuons );
  int my_nrange = ( sig_bkg==sb::sig ? m_nrange : m_nrangeNmuons );

  double left_val;
  bool single_case=false;

  // if overlap is applied between mom bins, find which ones to interpolate
  if (m_overlap && (my_nrange>1))
  {
    double left_val1,left_val2,rel_val1,rel_val2;
    int ind1,ind2;

    //is mom in first half of bin (not being this first)?
    if (mom<my_mombinsCenter[p_r] &&  p_r>0) {
      ind1=p_r-1;
      ind2=p_r;
    }
    //is mom in first half of first bin
    else if (mom<my_mombinsCenter[p_r]) {
      ind1=p_r;
      ind2=p_r+1;
    }
    //is mom in second half of bin (not being this last)?
    else if (mom>my_mombinsCenter[p_r] && p_r<my_nrange-1) {
      ind1=p_r;
      ind2=p_r+1;
    }
    //second half of last momentum bin
    else {
      single_case=true;
      ind1=p_r-1;
      ind2=p_r;
    }
    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"OVERLAPINFO"<<endmsg;
      debug()<<"p_r="<<p_r<<endmsg;
      debug()<<"binCenter="<<my_mombinsCenter[p_r]<<endmsg;
      // debug()<<"ind1="<<ind1<<",ind2="<<ind2<<endmsg;
    }

    //get y values corresponding to val to interpolate in momentum. If integrating to right, use 1-left_value
    left_val1=unifrel[ind1].getYvalue(value);
    if (m_leftRight=="right") rel_val1 = 1-left_val1;
    else rel_val1=left_val1;

    left_val2=unifrel[ind2].getYvalue(value);
    if (m_leftRight=="right") rel_val2 = 1-left_val2;
    else rel_val2=left_val2;

    // if second half and last momentum bin, decide wether to apply interpolation
    if (msgLevel(MSG::DEBUG) ) debug()<<"CLTool:: single_case="<<single_case<<endmsg;
    if (single_case &&  !m_applyLast) return rel_val2;

    double bcen1=my_mombinsCenter[ind1];
    double bcen2=my_mombinsCenter[ind2];

    if (msgLevel(MSG::DEBUG) ) {
      debug()<<"bcen1="<<bcen1<<endmsg;
      debug()<<"bcen2="<<bcen2<<endmsg;
      debug()<<"cl1="<<rel_val1<<endmsg;
      debug()<<"cl2="<<rel_val2<<endmsg;
    }

    // calculate final value interpolating between mom bins
    ret_val=rel_val1*(mom-bcen2)/(bcen1-bcen2)+rel_val2*(1-(mom-bcen2)/(bcen1-bcen2));
    if (msgLevel(MSG::DEBUG) ) debug()<<"ret_val="<<ret_val<<endmsg;
  } else {
    //simply calculate right or left values
    left_val=unifrel[p_r].getYvalue(value);
    if (m_leftRight=="right") ret_val = 1-left_val;
    else ret_val=left_val;
  }
  return ret_val;
}


StatusCode CLTool::cl(const double value, double& cls, double& clb, double& clr, double mom, int region) {

  StatusCode sc = StatusCode::SUCCESS;
  if (m_init.isFailure())
  {
    sc.setCode(501);
    return Error("CLTool: TOOL NOT INITIALIZED",sc);
  }
  if (value<=0.0)
  {
    sc.setCode(502);
    return Error("CLTool: NOT A VALID VALUE FOR QUANTITY", sc);
  }

  // from range compute i bin for signal
  int p_r;
  StatusCode sc1=findMomRange(mom,p_r,sb::sig);
  if (sc1.isFailure())
  {
    sc.setCode(503);
    if ( msgLevel(MSG::DEBUG)) debug()<<"NOT VALID RANGE OF P SIGNAL"<<endmsg;
    return sc;
  }

  if (msgLevel(MSG::DEBUG) ) debug() << "Region=  "<< region << endmsg;
  // from ibin-range compute CLs
  cls = valFromUnif(value,mom,p_r,sb::sig);
  if (msgLevel(MSG::DEBUG) ) debug()<<"CLS="<<cls<<endmsg;

  // from range compute i bin for bkg
  int p_r_nmuons;
  StatusCode sc2=findMomRange(mom,p_r_nmuons,sb::bkg);
  if (sc2.isFailure())
  {
    sc.setCode(504);
    if ( msgLevel(MSG::DEBUG)) debug()<<"NOT VALID RANGE OF P BKG"<<endmsg;
    return sc;
  }

  // from ibin-range compute CLb
  clb=valFromUnif(value,mom,p_r_nmuons,sb::bkg);
  if (msgLevel(MSG::DEBUG) ) debug()<<"CLB="<<clb<<endmsg;

  //compute ratio
  if (clb!=0 && cls!=0) clr=cls/clb;
  else {
    if (clb==0) {
      if (cls==0) clr=1e-6;
      else {
        return Warning("CLTool: DIVISION BY 0!",sc);
      }
    } else clr=1e-6;
  }
  return sc;
}


CLTool::~CLTool() = default;
