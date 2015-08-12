// Include files 

#include "Event/RawEvent.h"
// local
#include "CaloNtpBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloNtpBase
//
// 2010-10-12 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloNtpBase )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloNtpBase::CaloNtpBase( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_extrapolator(NULL)
  , m_calo(NULL)
  , m_caloElectron(NULL)
  , m_toSpd(NULL)
  , m_toPrs(NULL)
  , m_odin(NULL)
{

  declareProperty("ExtrapolatorType" , m_extrapolatorType = "TrackRungeKuttaExtrapolator" ) ;
  declareProperty("InputContainer" , m_input );
  declareProperty("Tuple"     , m_tuple = true);
  declareProperty("Histo"     , m_histo = true);
  declareProperty("Profile"    , m_profil = true);

  // hypo cuts
  declareProperty("EFilter"   , m_e = std::make_pair(0.,99999999.));
  declareProperty("EtFilter"  , m_et = std::make_pair(150.,999999.));
  declareProperty("PrsFilter" , m_prs  = std::make_pair(-1.,1024.));
  declareProperty("SpdFilter" , m_spd  = std::make_pair(-1.,9999.));
        
  // global cuts
  declareProperty("SpdMult"  , m_nSpd=std::make_pair(-1.,999999.));
  declareProperty("nTracks"  , m_nTrk=std::make_pair(-1.,999999.));
  declareProperty("nVertices", m_nVtx=std::make_pair(-1.,999999.));
  declareProperty("inAreaAcc", m_inArea = false);

  declareProperty( "VertexLoc", m_vertLoc = "");
  declareProperty( "UsePV3D", m_usePV3D = false);
  declareProperty( "TrackTypes", m_tracks);
  m_tracks.push_back(LHCb::Track::Long);
  m_tracks.push_back(LHCb::Track::Downstream);
}
//=============================================================================
// Destructor
//=============================================================================
CaloNtpBase::~CaloNtpBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloNtpBase::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTupleAlg

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_calo = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal);

  // set vertex location
  if(m_vertLoc == "")
    m_vertLoc=(m_usePV3D) ? LHCb::RecVertexLocation::Velo3D :  LHCb::RecVertexLocation::Primary;
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this); 
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  m_toSpd = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Spd" , this );
  m_toSpd->setCalos( "Ecal" ,"Spd");
  m_toPrs = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Prs" , this );
  m_toPrs->setCalos( "Ecal" ,"Prs");
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);
//      
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this); 
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorType,"Extrapolator",this );
  m_toSpd = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Spd" , this );
  m_toSpd->setCalos( "Ecal" ,"Spd");
  m_toPrs = tool<ICaloHypo2Calo> ( "CaloHypo2Calo", "CaloHypo2Prs" , this );
  m_toPrs->setCalos( "Ecal" ,"Prs");
  m_odin = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);
//
  if( !m_tracks.empty() )info() << "Will only look at track type(s) = " << m_tracks << endmsg;
  else 
    info() << "Will look at any track type" << endmsg;

  return StatusCode::SUCCESS;
}


unsigned int CaloNtpBase::nVertices(){
  int nVert = 0;

  if( !m_usePV3D){
    if( exist<LHCb::RecVertices>(m_vertLoc) ){
      LHCb::RecVertices* verts= get<LHCb::RecVertices>(m_vertLoc);
      if( NULL != verts){
        nVert = verts->size();
        counter("#PV="+ Gaudi::Utils::toString(nVert) + " ["+ m_vertLoc+"]")+=1;
        return nVert;
      }
    }
  }
  // try PV3D if explicitely requested or if RecVertices not found
  if(!m_usePV3D)m_vertLoc = LHCb::RecVertexLocation::Velo3D;
  if( exist<LHCb::VertexBases>( m_vertLoc ) ){
    LHCb::VertexBases* verts= get<LHCb::VertexBases>( m_vertLoc );
    if( NULL != verts)nVert = verts->size();
  }
  counter("#PV="+ Gaudi::Utils::toString(nVert) + " ["+ m_vertLoc+"]")+=1;
  return nVert;
}


bool CaloNtpBase::inArea(const LHCb::CaloHypo* hypo) {
    Gaudi::XYZPoint hypoR( hypo->position()->x(),hypo->position()->y(),hypo->position()->z());// hypo position
    const CellParam* cell = calo()->Cell_( hypoR ) ;
    if ( !cell ){
	counter("no cell related to position") += 1;
	info() << "No valid cell related to position " << hypoR << endmsg;
	return false;
    }
    if ( cell->neighbors().size() == 8 ) { return true ; }    
    return false;
}

bool CaloNtpBase::hypoProcessing(const LHCb::CaloHypo* hypo) {
    if ( NULL == hypo ) return false;
     LHCb::CaloMomentum momentum( hypo );
    const double e = momentum.e();
    const double et= momentum.pt();
    if( !inRange(m_et , et ))return false;
    if( !inRange(m_e  , e  ))return false;
    double ePrs = m_toPrs->energy ( *hypo , "Prs"  );
    int iSpd = m_toSpd->multiplicity ( *hypo , "Spd");    
    if( !inRange(m_prs, ePrs))return false;
    if( !inRange(m_spd, (double) iSpd))return false;
    if( m_inArea && !inArea( hypo) )return false;

    return true;
}

bool CaloNtpBase::eventProcessing() {
  // get input data
  if( !exist<LHCb::ProtoParticles> ( m_input ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "no protoP container found at " << m_input << endmsg;
    return false ;
  };

  // GET ODIN INFO
  m_run = 0;
  m_evt = 0;
  m_tty = 0;

 if( exist<LHCb::RawEvent>("/pRec/RawEvent") ||  exist<LHCb::RawEvent>("/DAQ/RawEvent") ){
   m_odin->getTime(); 
   if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
     LHCb::ODIN* odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
     m_run = odin->runNumber();
     m_evt =  odin->eventNumber();
     m_tty =  odin->triggerType();    
   }
 }
 
  // apply global cuts

  if( !inRange(m_nSpd , (double) nSpd() ))return false;
  if( !inRange(m_nTrk , (double) nTracks() ))return false;
  if( !inRange(m_nVtx , (double) nVertices() ))return false;

  return true;
}



bool CaloNtpBase::acceptTrack(const LHCb::Track* track){
  if( NULL == track)return false;
    for(std::vector<int>::iterator itr = m_tracks.begin();m_tracks.end() != itr;++itr){
      if( (int) track->type() == *itr)return true;
    }
return m_tracks.empty() ? true : false;
}
//=============================================================================
