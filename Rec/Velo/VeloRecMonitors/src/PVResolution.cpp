/**
 *  Algorithm to generate ntuples for measuring PV resolutions.
 *  The input track container is split in various ways to produce pairs of vertices.
 *  Use the various flags to steer the ntuple production according to the track splittings.
 */

// Include files

// from std
#include <string>
#include <vector>
#include <iterator>

// from Gaudi
#include "GaudiKernel/ToolHandle.h"

// from TrackEvent
#include "Event/Track.h"
#include "Event/MCVertex.h"
#include "Event/RecVertex.h"
#include "Event/ODIN.h"
#include "Event/L0DUReport.h"
#include "TrackInterfaces/IPVOfflineTool.h"

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

namespace Al {
  class Equations;
  class Residuals ;
}

/** @class PVResolution
 *
 *
 *  @author Marco Gersabeck
 *  @date   2010-05-04
 */

struct ElementHistos
{
  IHistogram1D* resh1 ;
  IHistogram1D* resfineh1 ;
  IHistogram1D* respullh1 ;
} ;

class PVResolution : public GaudiTupleAlg
{

public:
  /// Standard constructor
  PVResolution( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PVResolution( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

private:
  void fill_ntuplePV(std::vector<LHCb::RecVertex> outvec, std::string name);       ///< Fill vertex information
  
  void fill_ntuplePVMC(std::vector<LHCb::RecVertex> outvec, 
                       std::vector<LHCb::MCVertex* > mcp, 
                       std::string name);       ///< Fill vertex positions with MC truth comparison
  
  void fill_ntupleDiffPV(std::vector<LHCb::RecVertex> outvec1, 
                         std::vector<LHCb::RecVertex> outvec2, 
                         std::string name);       ///< Fill vertex position differences


private:
  std::string m_trackLocation ;
  std::string m_MCVertexLocation ;

  //  double m_maxDistance ;
  //  double m_maxFineDistance ;

  double m_numSigma ;
  double m_hitSigma ;
  double m_maxdchisq;
  unsigned long m_maxiteration;

  int m_runodin;
  unsigned long long int m_eventodin;
  int m_bunchid;
  int m_bxtype; 
  double m_odinTime;
  unsigned long long m_odinTime0;
  unsigned long long m_odinEvtTime;
  bool m_firstOdin;
  bool m_MC;
  bool m_fillIndividualSplits;
  bool m_fillAll;
  bool m_fillLR;
  bool m_fillUD;
  bool m_fillFB;
  bool m_fillRand;
  IRndmGenSvc* m_rsvc;
  Rndm::Numbers m_rand;
  
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PVResolution )
IPVOfflineTool* m_pvtool;

struct TrackTypePredicate
{
  int m_type ;
  TrackTypePredicate(int atype) : m_type(atype) {} 
  bool operator()(const LHCb::Track* track) const { return track->type()==m_type ; }
} ;

struct TrackFlagPredicate
{
  LHCb::Track::Flags m_flag ;
  bool m_positive ;
  TrackFlagPredicate(LHCb::Track::Flags flag, bool positive=true) : m_flag(flag), m_positive(positive) {} 
  bool operator()(const LHCb::Track* track) const { return m_positive ? track->checkFlag(m_flag) : !track->checkFlag(m_flag) ; }
} ;

struct TrackVeloSidePredicate
{
  int m_sign ;
  TrackVeloSidePredicate(int asign) : m_sign(asign) {}
  bool operator()(LHCb::Track* track) const { 
    return track->firstState().tx()*m_sign*(track->checkFlag(LHCb::Track::Backward)? -1 : 1 )>0 ; }
} ;

struct TrackUpDownPredicate
{
  int m_sign ;
  TrackUpDownPredicate(int asign) : m_sign(asign) {}
  bool operator()(const LHCb::Track* track) const { 
    return track->firstState().ty()*m_sign*(track->checkFlag(LHCb::Track::Backward)? -1 : 1 )>0 ; }
} ;

template< class TrackContainer, class Predicate >
std::vector<const LHCb::Track*> myselect(TrackContainer& tracks, Predicate selector){
  std::vector<const LHCb::Track*> rc ;
  for( typename TrackContainer::const_iterator itrack = tracks.begin() ;
       itrack != tracks.end(); ++itrack) 
    if( selector(*itrack) ) rc.push_back(*itrack) ;
  return rc ;
}

std::vector<const LHCb::Track*> myconvert( const SmartRefVector<LHCb::Track> & tracks )
{
  std::vector<const LHCb::Track*> rc(tracks.size()) ;
  std::copy(tracks.begin(),tracks.end(),rc.begin()) ;
  return rc ;
}

/**
 * Split track container randomly in two containers of euqal size (or different by 1 track)
 */
void splitRandomTracks( std::vector<LHCb::Track*> input, 
                        std::vector<const LHCb::Track*> *outputA, 
                        std::vector<const LHCb::Track*> *outputB, 
                        Rndm::Numbers rand ) {
  unsigned int min_size = (unsigned int) floor( input.size() / 2. );
  unsigned int max_size = (unsigned int) input.size() - min_size;
  std::vector<LHCb::Track*>::iterator it_in = input.begin();
  for( ; it_in != input.end(); it_in++ ) {
    if ( ( outputA->size() < max_size ) && ( outputB->size() < max_size ) ) {
      if ( rand() < 0.5 ) outputA->push_back( *it_in );
      else outputB->push_back( *it_in ); 
    }
    else if ( outputA->size() < max_size ) outputA->push_back( *it_in );
    else outputB->push_back( *it_in );
  }
}

PVResolution::PVResolution( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiTupleAlg( name , pSvcLocator ),
    //m_maxDistance(50.),
    //m_maxFineDistance(10.),
    m_rsvc(0)
{
  declareProperty("TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty("VertexLocation", m_MCVertexLocation = LHCb::MCVertexLocation::Default );
  declareProperty("NumSigma", m_numSigma = 3 ) ;
  declareProperty("HitSigma", m_hitSigma = 1.4 ) ;
  declareProperty("MaxChiSq",m_maxdchisq=0.01);
  declareProperty("MaxIteration",m_maxiteration=10) ;
  declareProperty("WithMC",m_MC=false) ;
  declareProperty("FillIndividualSplits",m_fillIndividualSplits=false) ;
  declareProperty("FillAll",m_fillAll=true) ;
  declareProperty("FillLR",m_fillLR=true) ;
  declareProperty("FillUD",m_fillUD=false) ;
  declareProperty("FillFB",m_fillFB=false) ;
  declareProperty("FillRand",m_fillRand=true) ;
}

PVResolution::~PVResolution() {}

//========================================================================
// Initialization
//========================================================================

StatusCode PVResolution::initialize() {
  if ( !GaudiTupleAlg::initialize().isSuccess() ) return StatusCode::FAILURE ;
    
  m_pvtool =tool<IPVOfflineTool>("PVOfflineTool", this);
  m_firstOdin = true;
  m_rsvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  m_rand.initialize( m_rsvc , Rndm::Flat(0.,1.0) ).ignore();

  return StatusCode::SUCCESS;
}
//========================================================================
// Finalize
//========================================================================

StatusCode PVResolution::finalize() 
{
  return  GaudiTupleAlg::finalize();  // must be called after all other actions
}


//========================================================================
// Main execution
//========================================================================


StatusCode PVResolution::execute() 
{
  LHCb::Tracks* tracks = get<LHCb::Tracks>(m_trackLocation);
  LHCb::MCVertices* mcvertices = NULL; 
  if ( m_MC ) { 
    mcvertices = get<LHCb::MCVertices>( m_MCVertexLocation );
    if ( !mcvertices ) m_MC = false;
  }

  std::vector< LHCb::MCVertex* > mcprimaries;
  if ( m_MC ) {
    LHCb::MCVertices::iterator itMCV = mcvertices->begin();
    for ( ; itMCV != mcvertices->end(); itMCV++ ) {
      if ( (*itMCV)->isPrimary() ) mcprimaries.push_back( *itMCV );
    }
  }

  LHCb::ODIN* odin = getIfExists<LHCb::ODIN> ( LHCb::ODINLocation::Default );
  if( NULL != odin ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Run "    << odin->runNumber()
              << ", Event " << odin->eventNumber() << endmsg;
    m_runodin=odin->runNumber();
    m_eventodin= odin->eventNumber();
    m_bunchid= odin->bunchId();
    m_bxtype= odin->bunchCrossingType();
    m_odinEvtTime = (long unsigned int) (odin->eventTime().ns() / 1e9);
    m_odinEvtTime -= 1259622000; // time in seconds since 01-Dec-09 00:00 CET
    if ( m_firstOdin ) {
      m_firstOdin = false;
      m_odinTime = 0.;
      m_odinTime0 = odin->gpsTime();
    }
    else {
      unsigned long itime = (unsigned long) (odin->gpsTime() - m_odinTime0);
      m_odinTime = 1e-6 * itime;
    }

    
  } else {
    Warning("No ODIN Header").ignore();
    m_runodin=0;
    m_eventodin=0;
    m_bunchid= 0;
    m_odinTime= 0.;
    m_bxtype=-1; 
  }

  std::vector<const LHCb::Track*> trackvectorConst;
  for (LHCb::Tracks::const_iterator iTrack 
         = tracks->begin(); iTrack != tracks->end(); ++iTrack ){
    if ( (*iTrack)->hasVelo() ) trackvectorConst.push_back(*iTrack);
  }
  std::vector<LHCb::Track*> trackvector;
  for (LHCb::Tracks::const_iterator iTrack 
         = tracks->begin(); iTrack != tracks->end(); ++iTrack ){
    if ( (*iTrack)->hasVelo() ) trackvector.push_back(*iTrack);
  }

  TrackFlagPredicate isBackward( LHCb::Track::Backward ) ;
  TrackFlagPredicate isForward( LHCb::Track::Backward,false ) ;

  std::vector<const LHCb::Track*> lefttracks  = myselect(trackvector,TrackVeloSidePredicate(+1));
  std::vector<const LHCb::Track*> righttracks = myselect(trackvector,TrackVeloSidePredicate(-1));
  std::vector<const LHCb::Track*> uptracks    = myselect(trackvector,TrackUpDownPredicate(+1));
  std::vector<const LHCb::Track*> downtracks  = myselect(trackvector,TrackUpDownPredicate(-1));
  std::vector<const LHCb::Track*> forwtracks  = myselect(trackvector,isForward);
  std::vector<const LHCb::Track*> backwtracks = myselect(trackvector,isBackward);
  std::vector<const LHCb::Track*> randAtracks;
  std::vector<const LHCb::Track*> randBtracks;
  splitRandomTracks( trackvector, &randAtracks, &randBtracks, m_rand );

  std::vector<LHCb::RecVertex> outvtxvec;
  std::vector<LHCb::RecVertex> leftoutvtxvec;
  std::vector<LHCb::RecVertex> rightoutvtxvec;
  std::vector<LHCb::RecVertex> upoutvtxvec;
  std::vector<LHCb::RecVertex> downoutvtxvec;
  std::vector<LHCb::RecVertex> forwoutvtxvec;
  std::vector<LHCb::RecVertex> backwoutvtxvec;
  std::vector<LHCb::RecVertex> a_outvtxvec;
  std::vector<LHCb::RecVertex> b_outvtxvec;
    
  StatusCode scfit;
  if (trackvector.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(trackvectorConst, outvtxvec);
    if ( m_fillAll ) {
      if ( !m_MC ) fill_ntuplePV(outvtxvec,"AllPV");
      else fill_ntuplePVMC(outvtxvec,mcprimaries,"AllPV");
    }
  }

  if (righttracks.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(righttracks, rightoutvtxvec);
    if ( m_fillIndividualSplits ) fill_ntuplePV(rightoutvtxvec,"RightPV");
  }
  if (lefttracks.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(lefttracks, leftoutvtxvec);
    if ( m_fillIndividualSplits ) fill_ntuplePV(leftoutvtxvec,"LeftPV");
  }
  if ( (righttracks.size()>2) && (lefttracks.size()>2) ) {
    if ( m_fillLR ) fill_ntupleDiffPV(leftoutvtxvec,rightoutvtxvec,"LeftRightPV");
  }
  if (uptracks.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(uptracks, upoutvtxvec);
    if ( m_fillIndividualSplits ) fill_ntuplePV(upoutvtxvec,"UpPV");
  }
  if (downtracks.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(downtracks, downoutvtxvec);
    if ( m_fillIndividualSplits ) fill_ntuplePV(downoutvtxvec,"DownPV");
  }
  if ( (downtracks.size()>2) && (uptracks.size()>2) ) {
    if ( m_fillUD ) fill_ntupleDiffPV(upoutvtxvec,downoutvtxvec,"UpDownPV");
  }
  if (forwtracks.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(forwtracks, forwoutvtxvec);
    if ( m_fillIndividualSplits ) fill_ntuplePV(forwoutvtxvec,"ForwPV");
  }
  if (backwtracks.size()>2){
    scfit = m_pvtool->reconstructMultiPVFromTracks(backwtracks, backwoutvtxvec);
    if ( m_fillIndividualSplits ) fill_ntuplePV(backwoutvtxvec,"BackwPV");
  }
  if ( (backwtracks.size()>2) && (forwtracks.size()>2) ) {
    if ( m_fillFB ) fill_ntupleDiffPV(forwoutvtxvec,backwoutvtxvec,"ForwBackwPV");
  }
  if ( (randAtracks.size()>2) && (randBtracks.size()>2) ) {
    scfit = m_pvtool->reconstructMultiPVFromTracks(randAtracks, a_outvtxvec);
    scfit = m_pvtool->reconstructMultiPVFromTracks(randBtracks, b_outvtxvec);
    if ( m_fillRand ) fill_ntupleDiffPV(a_outvtxvec,b_outvtxvec,"RandPV");
  }
 
  return StatusCode::SUCCESS ;

}

/////////////////////////////////////////////////////

void PVResolution::fill_ntuplePV(std::vector<LHCb::RecVertex> outvec, std::string name)
{

  Tuple tuple=nTuple(name, "PV info");

  for( std::vector<LHCb::RecVertex>::const_iterator ipv = outvec.begin() ;
       ipv != outvec.end(); ++ipv ) {
    LHCb::L0DUReport* report = getIfExists<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
    if( NULL != report ) {
      if ( ! tuple->column( "L0Decision", report->decision() ) ) return;
      if (msgLevel(MSG::DEBUG)) debug() << "L0 decision:  " << report->decision() << endmsg;
      LHCb::L0DUChannel::Map channels = report->configuration()->channels();
      for(LHCb::L0DUChannel::Map::const_iterator it = channels.begin();
          it!=channels.end();it++){
        if ( ! tuple->column( "L0_"+(*it).first ,
                              report->channelDecision( ((*it).second)->id() )))
          return;
      }
    } else {
      Warning("Can't get LHCb::L0DUReportLocation::Default (" +
              LHCb::L0DUReportLocation::Default + ")" ).ignore();
      if ( ! tuple->column( "L0Decision", -1 ) ) return;
    }
    //LHCb::RecVertex* pv = *ipv ;
    std::vector<const LHCb::Track*> pvtracks = myconvert(ipv->tracks()) ;
    TrackFlagPredicate isBackward( LHCb::Track::Backward ) ;
    TrackFlagPredicate isForward( LHCb::Track::Backward,false ) ;
    std::vector<const LHCb::Track*> forwtracks  = myselect(pvtracks,isForward);
    std::vector<const LHCb::Track*> backwtracks = myselect(pvtracks,isBackward);
    const Gaudi::SymMatrix3x3 covMat = ipv->covMatrix();
    std::vector<const LHCb::Track*>::iterator it = pvtracks.begin();
    std::vector< double > chi2PerDoF; 
    std::vector< int > nVeloIDs;
    std::vector< double > momentum;
    std::vector< double > transverse_momentum;
    std::vector< double > eta;
    std::vector< double > phi;
    std::vector< LHCb::LHCbID >::const_iterator lbidit;
    int nLong = 0;
    for( ; it != pvtracks.end(); it++ ) {
      chi2PerDoF.push_back( (*it)->chi2PerDoF() );
      lbidit = (*it)->lhcbIDs().begin();
      int veloIDcount = 0;
      for( ; lbidit != (*it)->lhcbIDs().end(); lbidit++ ) {
        if ( lbidit->isVelo() ) veloIDcount++;
      }
      nVeloIDs.push_back( veloIDcount );
      eta.push_back( (*it)->pseudoRapidity() );
      phi.push_back( (*it)->phi() );
      if ( (*it)->checkType( LHCb::Track::Long ) ) {
        nLong++;
        momentum.push_back( (*it)->p() );
        transverse_momentum.push_back( (*it)->pt() );
      }
    }
    tuple->farray( "trChi2PerDoF", chi2PerDoF, "nTrChi2PerDoF", 100 );
    tuple->farray( "trNVeloIDs", nVeloIDs, "nTrNVeloIDs", 100 );
    tuple->farray( "trP", momentum, "nTrP", 100 );
    tuple->farray( "trPT", transverse_momentum, "nTrPT", 100 );
    tuple->farray( "trEta", eta, "nTrEta", 100 );
    tuple->farray( "trPhi", phi, "nTrPhi", 100 );
    tuple->column( "run",m_runodin);
    tuple->column( "evt",(unsigned long int) m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "bxtype",m_bxtype);
    tuple->column( "time",(unsigned long int) m_odinTime);
    tuple->column( "gpsTime",(unsigned long int) m_odinEvtTime);
    tuple->column( "trNum",pvtracks.size() );
    tuple->column( "trNumLong",nLong );
    tuple->column( "trNumForw",forwtracks.size() );
    tuple->column( "trNumBackw",backwtracks.size() );
    tuple->column( "chi2",ipv->chi2() );
    tuple->column( "ndof",ipv->nDoF() );
    tuple->column( "pvx", ipv->position().x());
    tuple->column( "pvy", ipv->position().y());
    tuple->column( "pvz", ipv->position().z());
    tuple->column( "pvxe", sqrt(covMat.At(0,0)));
    tuple->column( "pvye", sqrt(covMat.At(1,1)));
    tuple->column( "pvze", sqrt(covMat.At(2,2)));
    tuple->column( "cov01", covMat.At(0,1));
    tuple->column( "cov02", covMat.At(0,2));
    tuple->column( "cov12", covMat.At(1,2));
    tuple->write();
  }
}	  

void PVResolution::fill_ntuplePVMC(std::vector<LHCb::RecVertex> outvec, std::vector< LHCb::MCVertex* > mcp, std::string name)
{

  Tuple tuple=nTuple(name, "PV info");

  for( std::vector<LHCb::RecVertex>::const_iterator ipv = outvec.begin() ;
       ipv != outvec.end(); ++ipv ) {
    //LHCb::RecVertex* pv = *ipv ;
    std::vector<const LHCb::Track*> pvtracks = myconvert(ipv->tracks()) ;
    TrackFlagPredicate isBackward( LHCb::Track::Backward ) ;
    TrackFlagPredicate isForward( LHCb::Track::Backward,false ) ;
    std::vector<const LHCb::Track*> forwtracks  = myselect(pvtracks,isForward);
    std::vector<const LHCb::Track*> backwtracks = myselect(pvtracks,isBackward);
    const Gaudi::SymMatrix3x3 covMat = ipv->covMatrix();
    std::vector<const LHCb::Track*>::iterator it = pvtracks.begin();
    std::vector< double > chi2PerDoF; 
    std::vector< int > nVeloIDs;
    std::vector< double > momentum;
    std::vector< double > transverse_momentum;
    std::vector< double > eta;
    std::vector< double > phi;
    std::vector< LHCb::LHCbID >::const_iterator lbidit;
    int nLong = 0;
    for( ; it != pvtracks.end(); it++ ) {
      chi2PerDoF.push_back( (*it)->chi2PerDoF() );
      lbidit = (*it)->lhcbIDs().begin();
      int veloIDcount = 0;
      for( ; lbidit != (*it)->lhcbIDs().end(); lbidit++ ) {
        if ( lbidit->isVelo() ) veloIDcount++;
      }
      nVeloIDs.push_back( veloIDcount );
      eta.push_back( (*it)->pseudoRapidity() );
      phi.push_back( (*it)->phi() );
      if ( (*it)->checkType( LHCb::Track::Long ) ) {
        nLong++;
        momentum.push_back( (*it)->p() );
        transverse_momentum.push_back( (*it)->pt() );
      }
    }
    std::vector< LHCb::MCVertex* >::iterator itmcp = mcp.begin();
    double maxdist = 9999.;
    LHCb::MCVertex* bestPV = NULL;
    for( ; itmcp != mcp.end(); itmcp++ ) {
      double dist = (ipv->position()-(*itmcp)->position()).R();
      if ( dist < maxdist ) {
        maxdist = dist;
        bestPV = *itmcp;
      }
    }
    tuple->farray( "trChi2PerDoF", chi2PerDoF, "nTrChi2PerDoF", 100 );
    tuple->farray( "trNVeloIDs", nVeloIDs, "nTrNVeloIDs", 100 );
    tuple->farray( "trP", momentum, "nTrP", 100 );
    tuple->farray( "trPT", transverse_momentum, "nTrPT", 100 );
    tuple->farray( "trEta", eta, "nTrEta", 100 );
    tuple->farray( "trPhi", phi, "nTrPhi", 100 );
    tuple->column( "run",m_runodin);
    tuple->column( "evt",(unsigned long int) m_eventodin);
    tuple->column( "bunchid",m_bunchid);
    tuple->column( "bxtype",m_bxtype);
    tuple->column( "time",m_odinTime);
    tuple->column( "gpsTime",(unsigned long int) m_odinEvtTime);
    tuple->column( "trNum",pvtracks.size() );
    tuple->column( "trNumLong",nLong );
    tuple->column( "trNumForw",forwtracks.size() );
    tuple->column( "trNumBackw",backwtracks.size() );
    tuple->column( "chi2",ipv->chi2() );
    tuple->column( "ndof",ipv->nDoF() );
    tuple->column( "pvx", ipv->position().x());
    tuple->column( "pvy", ipv->position().y());
    tuple->column( "pvz", ipv->position().z());
    tuple->column( "nMCPrimaries", mcp.size());
    tuple->column( "pvxMC", bestPV->position().x());
    tuple->column( "pvyMC", bestPV->position().y());
    tuple->column( "pvzMC", bestPV->position().z());
    tuple->column( "diff_pvx", ipv->position().x()-bestPV->position().x());
    tuple->column( "diff_pvy", ipv->position().y()-bestPV->position().y());
    tuple->column( "diff_pvz", ipv->position().z()-bestPV->position().z());
    tuple->column( "pvxe", sqrt(covMat.At(0,0)));
    tuple->column( "pvye", sqrt(covMat.At(1,1)));
    tuple->column( "pvze", sqrt(covMat.At(2,2)));
    tuple->column( "cov01", covMat.At(0,1));
    tuple->column( "cov02", covMat.At(0,2));
    tuple->column( "cov12", covMat.At(1,2));
    tuple->write();
  }
}	  

void PVResolution::fill_ntupleDiffPV(std::vector<LHCb::RecVertex> outvec1, std::vector<LHCb::RecVertex> outvec2, std::string name)
{

  Tuple tuple=nTuple(name, "PV diff info");

  tuple->column( "run",m_runodin);
  tuple->column( "evt",(unsigned long int) m_eventodin);
  tuple->column( "bunchid",m_bunchid);
  tuple->column( "bxtype",m_bxtype);
  tuple->column( "time",m_odinTime);
  tuple->column( "gpsTime",(unsigned long int) m_odinEvtTime);
  tuple->column( "nPV_1",outvec1.size());
  tuple->column( "nPV_2",outvec2.size());
  std::vector<LHCb::RecVertex>::const_iterator ipv1;
  std::vector<const LHCb::Track*> pvtracks1;
  std::vector<LHCb::RecVertex>::const_iterator ipv2;
  std::vector<const LHCb::Track*> pvtracks2;
  TrackFlagPredicate isBackward( LHCb::Track::Backward ) ;
  TrackFlagPredicate isForward( LHCb::Track::Backward,false ) ;
  if ( 1 == outvec1.size() ) {
    ipv1 = outvec1.begin() ;
    pvtracks1 = myconvert(ipv1->tracks()) ;
    std::vector<const LHCb::Track*> forwtracks1  = myselect(pvtracks1,isForward);
    std::vector<const LHCb::Track*> backwtracks1 = myselect(pvtracks1,isBackward);
    std::vector<const LHCb::Track*>::iterator it = pvtracks1.begin();
    const Gaudi::SymMatrix3x3 covMat1 = ipv1->covMatrix();
    std::vector< double > chi2PerDoF; 
    std::vector< int > nVeloIDs;
    std::vector< double > momentum;
    std::vector< double > transverse_momentum;
    std::vector< double > eta;
    std::vector< double > phi;
    std::vector< LHCb::LHCbID >::const_iterator lbidit;
    int nLong = 0;
    for( ; it != pvtracks1.end(); it++ ) {
      chi2PerDoF.push_back( (*it)->chi2PerDoF() );
      lbidit = (*it)->lhcbIDs().begin();
      int veloIDcount = 0;
      for( ; lbidit != (*it)->lhcbIDs().end(); lbidit++ ) {
        if ( lbidit->isVelo() ) veloIDcount++;
      }
      nVeloIDs.push_back( veloIDcount );
      eta.push_back( (*it)->pseudoRapidity() );
      phi.push_back( (*it)->phi() );
      if ( (*it)->checkType( LHCb::Track::Long ) ) {
        nLong++;
        momentum.push_back( (*it)->p() );
        transverse_momentum.push_back( (*it)->pt() );
      }
    }
    tuple->farray( "trChi2PerDoF_1", chi2PerDoF, "nTrChi2PerDoF_1", 100 );
    tuple->farray( "trNVeloIDs_1", nVeloIDs, "nTrNVeloIDs_1", 100 );
    tuple->farray( "trP_1", momentum, "nTrP_1", 100 );
    tuple->farray( "trPT_1", transverse_momentum, "nTrPT_1", 100 );
    tuple->farray( "trEta_1", eta, "nTrEta_1", 100 );
    tuple->farray( "trPhi_1", phi, "nTrPhi_1", 100 );
    tuple->column( "trNum_1",pvtracks1.size() );
    tuple->column( "trNumLong_1",nLong );
    tuple->column( "trNumForw_1",forwtracks1.size() );
    tuple->column( "trNumBackw_1",backwtracks1.size() );
    tuple->column( "chi2_1",ipv1->chi2() );
    tuple->column( "ndof_1",ipv1->nDoF() );
    tuple->column( "pvx_1", ipv1->position().x());
    tuple->column( "pvy_1", ipv1->position().y());
    tuple->column( "pvz_1", ipv1->position().z());
    tuple->column( "pvxe_1", sqrt(covMat1.At(0,0)));
    tuple->column( "pvye_1", sqrt(covMat1.At(1,1)));
    tuple->column( "pvze_1", sqrt(covMat1.At(2,2)));
    tuple->column( "cov01_1", covMat1.At(0,1));
    tuple->column( "cov02_1", covMat1.At(0,2));
    tuple->column( "cov12_1", covMat1.At(1,2));
  }
  else {
    std::vector< double > chi2PerDoF; 
    std::vector< int > nVeloIDs;
    std::vector< double > momentum;
    std::vector< double > transverse_momentum;
    std::vector< double > eta;
    std::vector< double > phi;
    tuple->farray( "trChi2PerDoF_1", chi2PerDoF, "nTrChi2PerDoF_1", 100 );
    tuple->farray( "trNVeloIDs_1", nVeloIDs, "nTrNVeloIDs_1", 100 );
    tuple->farray( "trP_1", momentum, "nTrP_1", 100 );
    tuple->farray( "trPT_1", transverse_momentum, "nTrPT_1", 100 );
    tuple->farray( "trEta_1", eta, "nTrEta_1", 100 );
    tuple->farray( "trPhi_1", phi, "nTrPhi_1", 100 );
    tuple->column( "trNum_1",0 );
    tuple->column( "trNumLong_1",0);
    tuple->column( "trNumForw_1",0 );
    tuple->column( "trNumBackw_1",0 );
    tuple->column( "chi2_1",0.);
    tuple->column( "ndof_1",0);
    tuple->column( "pvx_1", 0.);
    tuple->column( "pvy_1", 0.);
    tuple->column( "pvz_1", 0.);
    tuple->column( "pvxe_1", 0.);
    tuple->column( "pvye_1", 0.);
    tuple->column( "pvze_1", 0.);
    tuple->column( "cov01_1", 0.);
    tuple->column( "cov02_1", 0.);
    tuple->column( "cov12_1", 0.);
  }
  if ( 1 == outvec2.size() ) {
    ipv2 = outvec2.begin() ;
    pvtracks2 = myconvert(ipv2->tracks()) ;
    std::vector<const LHCb::Track*> forwtracks2  = myselect(pvtracks2,isForward);
    std::vector<const LHCb::Track*> backwtracks2 = myselect(pvtracks2,isBackward);
    std::vector<const LHCb::Track*>::iterator it = pvtracks2.begin();
    const Gaudi::SymMatrix3x3 covMat2 = ipv2->covMatrix();
    std::vector< double > chi2PerDoF2; 
    std::vector< int > nVeloIDs2;
    std::vector< double > momentum2;
    std::vector< double > transverse_momentum2;
    std::vector< double > eta2;
    std::vector< double > phi2;
    int nLong = 0;
    std::vector< LHCb::LHCbID >::const_iterator lbidit;
    for( ; it != pvtracks2.end(); it++ ) {
      chi2PerDoF2.push_back( (*it)->chi2PerDoF() );
      lbidit = (*it)->lhcbIDs().begin();
      int veloIDcount = 0;
      for( ; lbidit != (*it)->lhcbIDs().end(); lbidit++ ) {
        if ( lbidit->isVelo() ) veloIDcount++;
      }
      nVeloIDs2.push_back( veloIDcount );
      eta2.push_back( (*it)->pseudoRapidity() );
      phi2.push_back( (*it)->phi() );
      if ( (*it)->checkType( LHCb::Track::Long ) ) {
        nLong++;
        momentum2.push_back( (*it)->p() );
        transverse_momentum2.push_back( (*it)->pt() );
      }
    }
    tuple->farray( "trChi2PerDoF_2", chi2PerDoF2, "nTrChi2PerDoF_2", 100 );
    tuple->farray( "trNVeloIDs_2", nVeloIDs2, "nTrNVeloIDs_2", 100 );
    tuple->farray( "trP_2", momentum2, "nTrP_2", 100 );
    tuple->farray( "trPT_2", transverse_momentum2, "nTrPT_2", 100 );
    tuple->farray( "trEta_2", eta2, "nTrEta_2", 100 );
    tuple->farray( "trPhi_2", phi2, "nTrPhi_2", 100 );
    tuple->column( "trNum_2",pvtracks1.size() );
    tuple->column( "trNumLong_2",nLong );
    tuple->column( "trNumForw_2",forwtracks2.size() );
    tuple->column( "trNumBackw_2",backwtracks2.size() );
    tuple->column( "chi2_2",ipv2->chi2() );
    tuple->column( "ndof_2",ipv2->nDoF() );
    tuple->column( "pvx_2", ipv2->position().x());
    tuple->column( "pvy_2", ipv2->position().y());
    tuple->column( "pvz_2", ipv2->position().z());
    tuple->column( "pvxe_2", sqrt(covMat2.At(0,0)));
    tuple->column( "pvye_2", sqrt(covMat2.At(1,1)));
    tuple->column( "pvze_2", sqrt(covMat2.At(2,2)));
    tuple->column( "cov01_2", covMat2.At(0,1));
    tuple->column( "cov02_2", covMat2.At(0,2));
    tuple->column( "cov12_2", covMat2.At(1,2));
  }
  else {
    std::vector< double > chi2PerDoF2;
    std::vector< int > nVeloIDs2;
    std::vector< double > momentum2;
    std::vector< double > transverse_momentum2;
    std::vector< double > eta2;
    std::vector< double > phi2;
    tuple->farray( "trChi2PerDoF_2", chi2PerDoF2, "nTrChi2PerDoF_2", 100 );
    tuple->farray( "trNVeloIDs_2", nVeloIDs2, "nTrNVeloIDs_2", 100 );
    tuple->farray( "trP_2", momentum2, "nTrP_2", 100 );
    tuple->farray( "trPT_2", transverse_momentum2, "nTrPT_2", 100 );
    tuple->farray( "trEta_2", eta2, "nTrEta_2", 100 );
    tuple->farray( "trPhi_2", phi2, "nTrPhi_2", 100 );
    tuple->column( "trNum_2",0);
    tuple->column( "trNumLong_2",0);
    tuple->column( "trNumForw_2",0 );
    tuple->column( "trNumBackw_2",0 );
    tuple->column( "chi2_2",0.);
    tuple->column( "ndof_2",0);
    tuple->column( "pvx_2", 0.);
    tuple->column( "pvy_2", 0.);
    tuple->column( "pvz_2", 0.);
    tuple->column( "pvxe_2", 0.);
    tuple->column( "pvye_2", 0.);
    tuple->column( "pvze_2", 0.);
    tuple->column( "cov01_2", 0.);
    tuple->column( "cov02_2", 0.);
    tuple->column( "cov12_2", 0.);
  }
  if ( ( 1 == outvec1.size() ) && ( 1 == outvec2.size() ) ) {
    tuple->column( "diff_pvx", ipv1->position().x()-ipv2->position().x());
    tuple->column( "diff_pvy", ipv1->position().y()-ipv2->position().y());
    tuple->column( "diff_pvz", ipv1->position().z()-ipv2->position().z());
  }
  else {
    tuple->column( "diff_pvx", 0.);
    tuple->column( "diff_pvy", 0.);
    tuple->column( "diff_pvz", 0.);
  }
  tuple->write();
}	  
