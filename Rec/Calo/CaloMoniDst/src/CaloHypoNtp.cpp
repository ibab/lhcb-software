#include "GaudiKernel/AlgFactory.h"
#include <vector>
#include "Event/CaloHypo.h"
#include "CaloUtils/CaloMomentum.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/ODIN.h" 
#include "Event/RecVertex.h"
#include "Event/L0DUReport.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloHypoNtp.h"
#include "Event/MCParticle.h"


DECLARE_ALGORITHM_FACTORY( CaloHypoNtp );


CaloHypoNtp::CaloHypoNtp( const std::string &name, ISvcLocator *pSvcLocator )
  : GaudiTupleAlg( name, pSvcLocator ){ 
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
  
  using namespace LHCb::CaloHypoLocation;
  m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Photons        ) );
  m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , Electrons      ) );
  m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , MergedPi0s     ) );
  //  m_locs.push_back( LHCb::CaloAlgUtils::PathFromContext( context() , SplitPhotons     ) );
};
CaloHypoNtp::~CaloHypoNtp(){};


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
  m_spdMult = 0;
  std::string l0loc = LHCb::L0DUReportLocation::Default;
  if( exist<LHCb::L0DUReport>( l0loc)){
    LHCb::L0DUReport* l0   = get<LHCb::L0DUReport>( l0loc );
    m_spdMult = (int) l0->dataValue("Spd(Mult)");
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
      //



      if( m_tupling){
        sc=ntp->column("hypothesis"   , hypo->hypothesis()    );
        for( int i = 0 ; i < Last ; ++i){
          sc=ntp->column(Name[i]        , estimator()->data(hypo, (DataType) i ,0.)   );
          counter( Name[i] + " for " + hypothesis ) += estimator()->data(hypo, (DataType) i ,0.);
        }
        // kinematics
        Gaudi::XYZPoint point;
        if( NULL != hypo->position() )
           point=Gaudi::XYZPoint( hypo->position()->x() , hypo->position()->y() , hypo->position()->z() );
        sc=ntp->column("r" , point);
        LHCb::CaloMomentum mt( hypo );
        Gaudi::LorentzVector v( mt.momentum() );
        sc=ntp->column("p" , v);
      

        // Checker Mode
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
        
        
        // odin info
        sc=ntp->column("run"   , run         );
        sc=ntp->column("event" , (double) evt );
        sc=ntp->column("triggertype" , tty );
        // #vertices & tracks
        sc=ntp->column("Nvertices", nVert);
        sc=ntp->column("NTracks", nTrack);
        // #SpdMult
        sc=ntp->column("spdMult", m_spdMult);
        sc=ntp->write();
        ok = true;
      }      
    }
  }
  if(ok)counter("Events in tuple") += 1;
  if( sc.isFailure())Warning("Error with ntupling",StatusCode::SUCCESS).ignore();
  return StatusCode::SUCCESS;
}

StatusCode CaloHypoNtp::finalize() {
  debug() << "==> Finalize" << endmsg;
  return GaudiTupleAlg::finalize();
}



bool CaloHypoNtp::inRange( std::pair<double,double> range, double value){
  return ( value >= range.first) && (value <= range.second);
}

bool CaloHypoNtp::inRange( std::pair<int,int> range, int value){
  return ( value == 0 &&   range.first == 1 ) || (value > 0 && range.second == 1);
}

