#include <vector>
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloMomentum.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/ODIN.h" 
#include "Event/RecVertex.h"
#include "Event/L0DUReport.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloHypoNtp.h"
#include "Event/MCParticle.h"


DECLARE_ALGORITHM_FACTORY( CaloHypoNtp )


CaloHypoNtp::CaloHypoNtp( const std::string &name, ISvcLocator *pSvcLocator )
  : GaudiTupleAlg( name, pSvcLocator )
  , m_estimator(NULL)
  , m_odin(NULL)
  , m_2MC(NULL)
{ 
  declareProperty( "RangePt"      , m_et      = std::make_pair(100. , 15000.) );
  declareProperty( "RangeE"       , m_e       = std::make_pair(0.   , 5000000) );
  declareProperty( "RangeSpdM"    , m_spdM    = std::make_pair(0.   , 5000000.) );
  declareProperty( "RangePrsE"    , m_prsE     = std::make_pair(0.   , 9999. ) );
  declareProperty( "Tupling"  , m_tupling = true);
  declareProperty( "VertexLoc", m_vrtLoc =  LHCb::RecVertexLocation::Primary);
  declareProperty( "TrackLoc" , m_trkLoc = LHCb::TrackLocation::Default);
  declareProperty( "Locations", m_locs);
  declareProperty( "CheckerMode", m_checker=false);
  declareProperty("Extrapolation", m_extrapol = true);  
  declareProperty("AddSeed"      , m_seed = false);  
  declareProperty("AddNeighbors" , m_neig = false);  
  declareProperty("MCID" , m_mcID=-99999999);
  declareProperty("Hypos", m_hypos);
  declareProperty("Printout",m_print=false);
  declareProperty("Statistics",m_stat=true);
  
  m_hypos.push_back("Electrons");
  m_hypos.push_back("Photons");
  m_hypos.push_back("MergedPi0s");
}

CaloHypoNtp::~CaloHypoNtp(){}


StatusCode CaloHypoNtp::initialize(){
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc; // error already printedby GaudiAlgorithm
  // get tools
  m_2MC       = tool<ICalo2MCTool>   ( "Calo2MCTool","Calo2MCTool",this );
  m_odin      = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);
  m_estimator = tool<ICaloHypoEstimator> ( "CaloHypoEstimator", "CaloHypoEstimator" , this );
  std::string seed = m_seed ? "true" : "false";
  std::string line = m_extrapol ? "true" : "false";
  std::string neig = m_neig ? "true" : "false";
  m_estimator->hypo2Calo()->_setProperty("Seed", seed).ignore();
  m_estimator->hypo2Calo()->_setProperty("PhotonLine", line).ignore();
  m_estimator->hypo2Calo()->_setProperty("AddNeighbors", neig).ignore();

  //
  using namespace LHCb::CaloHypoLocation;
  for(std::vector<std::string>::iterator h = m_hypos.begin(); h<m_hypos.end(); ++h){
    if(*h == "Photons")m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Photons        ) );
    else if(*h == "Electrons")m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Electrons      ) );
    else if(*h == "MergedPi0s")m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , MergedPi0s     ) );
    else if(*h == "SplitPhotons")m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , SplitPhotons     ) );
  }
  

  return StatusCode::SUCCESS;
}


// ============================================================================
// standard execution method
// ============================================================================
StatusCode CaloHypoNtp::execute(){

  typedef LHCb::CaloHypo::Container    Hypos;
  typedef LHCb::CaloHypo               Hypo;
  using namespace CaloDataType;



  // declare tuple
  Tuple ntp = NULL;
  if( m_tupling)ntp = nTuple(500, "HypoNtp" ,CLID_ColumnWiseTuple);

  
  // SpdMult
  int spdMult = 0;
  std::string l0loc = LHCb::L0DUReportLocation::Default;
  if( exist<LHCb::L0DUReport>( l0loc)){
    LHCb::L0DUReport* l0   = get<LHCb::L0DUReport>( l0loc );
    spdMult = (int) l0->dataValue("Spd(Mult)");
  }

  // vertices&tracks
  int nVert = 0;
  if( exist<LHCb::RecVertices>(m_vrtLoc) ){
    LHCb::RecVertices* verts= get<LHCb::RecVertices>(m_vrtLoc);
    if( NULL != verts)nVert = verts->size();
  }
  int nTrack = 0;
  if( exist<LHCb::Tracks>(m_trkLoc) ){
    LHCb::Tracks* tracks= get<LHCb::Tracks>(m_trkLoc);
    if( NULL != tracks)nTrack = tracks->size();
  }

  // Odin
  int run = 0;
  ulonglong evt = 0;
  int tty = 0;
  m_odin->getTime();
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
    if( NULL != odin ){
      run = odin->runNumber();
      evt =  odin->eventNumber();
      tty =  odin->triggerType();
    } else
      counter("NULL ODIN")+=1;
  }

  // loop over hypo containers
  bool ok = false;
  StatusCode sc;
  for( std::vector<std::string>::iterator iloc = m_locs.begin() ; m_locs.end() != iloc ; ++iloc){
    if( !exist<Hypos>(*iloc) ){
      Warning("Hypo container not found at " + *iloc).ignore();
      continue;
    }
    Hypos* hypos = get<Hypos>( *iloc );
    if ( 0 == hypos ) continue;


    for( Hypos::iterator h = hypos->begin(); hypos->end() != h; ++h ){
      Hypo* hypo = *h;
      if ( NULL == hypo ) continue;
      std::ostringstream type("");
      type << hypo->hypothesis();
      std::string hypothesis = type.str();


      // filtering hypo
      if( !inRange( m_et ,  estimator()->data(hypo, HypoEt ,0.) ) )continue;
      if( !inRange( m_e  ,  estimator()->data(hypo, HypoE  ,0.) ) )continue;
      if( !inRange( m_spdM ,  estimator()->data(hypo, HypoSpdM ,0.) ) )continue;
      if( !inRange( m_prsE ,  estimator()->data(hypo, HypoPrsE ,0.) ) )continue;
      
      // MC-associated filtering
      if( m_checker ){
        const LHCb::MCParticle* mcp = calo2MC()->from(hypo)->bestMC();
        if( NULL == mcp )continue;
        if( m_mcID >= 0 && (int) mcp->particleID().abspid()  != m_mcID)continue;        
      }      

      // PrintOut
      if ( m_print ){
        info() << "+++ Run/Evt " << run << "/" << evt << endmsg;
        info() << " === hypothesis " << hypo->hypothesis() << "(" << *iloc << ")" << endmsg;
        if( m_checker )calo2MC()->from(hypo)->descriptor();
      }

      // DataTypes statistics
      for( int i = 0 ; i < Last ; ++i){
        double val = estimator()->data(hypo, (DataType) i ,0.);
        if(m_stat)counter( Name[i] + " for " + hypothesis ) += val;
        if(m_tupling)sc=ntp->column(Name[i]        , val);
        if(m_print)info() << "   --> " <<  Name[i] << " : " <<   val << endmsg;
      }

      // Tupling
      if( m_tupling){        

        // hypothesis
        sc=ntp->column("hypothesis"   , hypo->hypothesis()    );
        // kinematics
        const LHCb::CaloCluster* cluster = estimator()->toCluster(CaloClusterType::SplitOrMain);
        Gaudi::XYZPoint cPoint;
        if( NULL != cluster)
          cPoint = Gaudi::XYZPoint( cluster->position().x() , cluster->position().y() , cluster->position().z() );
        sc=ntp->column("ClusterR" , cPoint);
        Gaudi::XYZPoint point;
        if( NULL != hypo->position() )
           point=Gaudi::XYZPoint( hypo->position()->x() , hypo->position()->y() , hypo->position()->z() );
        sc=ntp->column("HypoR" , point);
        LHCb::CaloMomentum mt( hypo );
        Gaudi::LorentzVector v( mt.momentum() );
        sc=ntp->column("HypoP" , v);

        // matched tracks
        for( int match = 0 ; match < CaloMatchType::Last ; ++match){
          Gaudi::LorentzVector p;
          const LHCb::Track* track = estimator()->toTrack( (CaloMatchType::MatchType) match);
          if( NULL != track)p=Gaudi::LorentzVector(track->momentum().x(),track->momentum().y(),track->momentum().z (),track->p());
          sc=ntp->column( CaloMatchType::Name[match] + "TrackP" , p);
        }        
        
        // odin info
        sc=ntp->column("Run"   , run         );
        sc=ntp->column("Event" , (double) evt );
        sc=ntp->column("Triggertype" , tty );
        
        // #vertices & tracks
        sc=ntp->column("Nvertices", nVert);
        sc=ntp->column("NTracks", nTrack);
        
        // #SpdMult
        sc=ntp->column("spdMult", spdMult);

        // Checker Mode (MC info)
        if( m_checker ){
          int id = -999;
          double weight = -999;
          double quality = -999;
          const LHCb::MCParticle* mcp = calo2MC()->from(hypo)->bestMC();
          if( NULL != mcp ){
            id     = mcp->particleID().pid();
            weight = calo2MC()->from(hypo)->weight( mcp );
            quality= calo2MC()->from(hypo)->quality( mcp );
          }
          sc=ntp->column( "MCid" , id );
          sc=ntp->column( "MCw" , weight );
          sc=ntp->column( "MCq" , quality );
        }
        sc=ntp->write();
        ok = true;
      }      
    }
  }
  if(ok)counter("Events in tuple") += 1;
  if( sc.isFailure())Warning("Error with ntupling",StatusCode::SUCCESS).ignore();
  return StatusCode::SUCCESS;
}

bool CaloHypoNtp::inRange( std::pair<double,double> range, double value){
  return ( value >= range.first) && (value <= range.second);
}

bool CaloHypoNtp::inRange( std::pair<int,int> range, int value){
  return ( value == 0 &&   range.first == 1 ) || (value > 0 && range.second == 1);
}

