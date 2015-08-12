// Include files 

#include  "CaloUtils/CaloMomentum.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
// local
#include "CaloProtoElectronMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloProtoElectronMonitor
//
// 2009-12-11 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloProtoElectronMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloProtoElectronMonitor::CaloProtoElectronMonitor( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : CaloMoniAlg ( name , pSvcLocator )
  , m_extrapolator(NULL)
  , m_caloElectron(NULL)
{

  declareProperty("HistoEoPMin" , m_eOpMin = 0.); 
  declareProperty("HistoEoPMax" , m_eOpMax = 3.); 
  declareProperty("HistoEoPBin" , m_eOpBin = 100); 
  declareProperty("PrsCut"      , m_prsCut = 50.* Gaudi::Units::MeV);  
  declareProperty("ElectronPairing", m_pairing = false );
  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty( "TrackTypes", m_tracks);
  m_tracks.push_back(LHCb::Track::Long);
  m_tracks.push_back(LHCb::Track::Downstream);
  

  setInputData( LHCb::ProtoParticleLocation::Charged ); 

  m_massFilterMax = 100;
  m_multMax = 100;
  m_massMin = 0;
  m_massMax = 5000;
  m_massBin = 500;  

}
//=============================================================================
// Destructor
//=============================================================================
CaloProtoElectronMonitor::~CaloProtoElectronMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloProtoElectronMonitor::initialize() {
  StatusCode sc = CaloMoniAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  if( !m_tracks.empty() )info() << "Will only look at track type(s) = " << m_tracks << endmsg;
  else 
    info() << "Will look at any track type" << endmsg;

  // get tools
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this); 
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );

  // histograms 
  hBook1(  "1", "# of CaloElectron protoP   " + inputData(),  m_multMin   ,    m_multMax   , m_multBin  );
  hBook1(  "2", "CaloElectron protoP Energy   " + inputData(),  m_energyMin ,    m_energyMax , m_energyBin );
  hBook1(  "3", "CaloElectron protoP Pt       " + inputData(),  m_etMin     ,    m_etMax     , m_etBin);
  hBook2(  "4", "CaloElectron protoP barycenter position x vs y   " 
           + inputData(),  m_xMin, m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
  hBook2(  "5", "Energy-weighted CaloElectron protoP  barycenter position x vs y " 
           + inputData(),m_xMin,m_xMax, m_xBin, m_yMin, m_yMax, m_yBin);
  hBook1(  "6", "e/p  " + inputData(),  m_eOpMin ,    m_eOpMax , m_eOpBin );
  hBook1(  "7", "Eprs  " + inputData(),  0. , 300. , m_energyBin );
  if( m_pairing ){    
    hBook1(  "8", "m(track pair)  " + inputData()  ,  m_massMin   , m_massMax  ,  m_massBin );
    hBook1(  "9", "m(clust pair)  " + inputData()  ,  m_massMin   , m_massMax  ,  m_massBin );
    hBook1(  "10", "e/p for M(ee) < " + Gaudi::Utils::toString(m_massFilterMax)
             + inputData(),  m_eOpMin ,    m_eOpMax , m_eOpBin );
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloProtoElectronMonitor::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  if ( !produceHistos() ) return StatusCode::SUCCESS;
  // get input data
  if( !exist<LHCb::ProtoParticles> ( inputData() ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "no protoP container found at " << inputData() << endmsg;
    return StatusCode::SUCCESS ;
  };
  
  LHCb::ProtoParticles* protos = get<LHCb::ProtoParticles> ( inputData() );
  
  initCounters();
  for( LHCb::ProtoParticles::const_iterator p = protos->begin(); protos->end () != p ; ++p ){ 
    const LHCb::ProtoParticle* proto = *p;

    if( !m_caloElectron->set(proto))continue;;
    LHCb::CaloHypo* hypo = m_caloElectron->electron();

    // track type selection
    bool accept = m_tracks.empty() ? true : false;
    for(std::vector<int>::iterator itr = m_tracks.begin();m_tracks.end() != itr;++itr){
      if( (int)proto->track()->type() == *itr)accept=true;
    }
    if( !accept )continue;
    //

    if ( NULL == hypo ) continue;
    LHCb::CaloMomentum momentum( hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    if(e    < m_eFilter)continue;
    if(et   < m_etFilter)continue;
    double ePrs = proto->info(LHCb::ProtoParticle::CaloPrsE, 0.);
    if( m_prsCut > 0 &&  ePrs < m_prsCut )continue;
    LHCb::CaloCellID id = LHCb::CaloCellID();
    if ( hypo->clusters().size() > 0 ){
      SmartRef<LHCb::CaloCluster> cluster= *(hypo->clusters().begin());
      id = (*cluster).seed();      
    }
    double eOp = m_caloElectron->eOverP();

    count(id);
    hFill1(id, "2", e  );
    hFill1(id, "3", et );
    const LHCb::CaloHypo::Position *position = hypo->position();
    if( NULL != position){
      hFill2(id, "4", position->x(),position->y() );
      hFill2(id, "5", position->x(),position->y() , e);
    }
    hFill1(id, "6", eOp );
    hFill1(id, "7", ePrs );


    // perform electron pairing
    if( !m_pairing)continue;
    for( LHCb::ProtoParticles::const_iterator pp = p+1 ; protos->end () != pp ; ++pp ){

      const LHCb::ProtoParticle* proto2 = *pp;
      if( !m_caloElectron->set(proto2))continue;;
      LHCb::CaloHypo* hypo2 = m_caloElectron->electron();
      if ( NULL == hypo2 ) continue;
      if( hypo == hypo2 )continue;
      LHCb::CaloMomentum momentum2( hypo2 );

      // filtering proto2
      const double e2 = momentum2.e();
      const double et2 = momentum2.pt();
      if(e2   < m_eFilter)continue;
      if(et2   < m_etFilter)continue;
      double ePrs2 = proto2->info(LHCb::ProtoParticle::CaloPrsE, 0.);
      if( m_prsCut > 0 &&  ePrs2 < m_prsCut )continue;



      LHCb::CaloMomentum momentumSum( hypo );
      momentumSum.addCaloPosition( hypo2 );
      double caloM = momentumSum.mass();      
      const LHCb::Track*  t1 = proto->track();
      const LHCb::Track*  t2 = proto2->track();


      bool accept2 = m_tracks.empty() ? true : false;
      for(std::vector<int>::iterator itr = m_tracks.begin();m_tracks.end() != itr;++itr){
        if( (int)t2->type() == *itr)accept2=true;
      }
      if( !accept2 )continue;


      if( NULL == t1 || NULL == t2)continue;
      if( -1 != t1->charge()*t2->charge())continue;
      LHCb::State st1 = t1->firstState();
      LHCb::State st2 = t2->firstState();

      if( NULL == extrapolator() ){
        Warning("No extrapolator defined").ignore();
        continue;
      }
      StatusCode sc = extrapolator()->propagate(st1, 0.);
      if(sc.isFailure())Warning("Propagation 1 failed").ignore();
      sc = extrapolator()->propagate(st2, 0.);
      if(sc.isFailure())Warning("Propagation 2 failed").ignore();
      Gaudi::XYZVector p1 = st1.momentum();
      Gaudi::XYZVector p2 = st2.momentum();
      double m2  = p1.R()*p2.R();
      m2 -= p1.X()*p2.X();
      m2 -= p1.Y()*p2.Y();
      m2 -= p1.Z()*p2.Z();
      m2 *= 2;
      double m = (m2>0) ? sqrt(m2) : 0;
      hFill1(id, "8", m );
      hFill1(id, "9", caloM );
      if( m2 < m_massFilterMax ){
        hFill1(id, "10", eOp );
        double eOp2 = m_caloElectron->eOverP();
        hFill1(id, "10", eOp2 );
      }
    } 
  }
  fillCounters("1");
  return StatusCode::SUCCESS;
}
//=============================================================================
