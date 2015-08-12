// Include files

// local
#include "SubClusterSelectorTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SubClusterSelectorTool
//
// 2014-06-20 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SubClusterSelectorTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SubClusterSelectorTool::SubClusterSelectorTool( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_condition          ("")
  , m_taggerE            ()
  , m_taggerP            ()
  , m_tagE               ()
  , m_tagP               ()
  , m_det                ()
  , m_energyStatus  (LHCb::CaloDigitStatus::UseForEnergy   |  LHCb::CaloDigitStatus::UseForCovariance)
  , m_positionStatus(LHCb::CaloDigitStatus::UseForPosition |  LHCb::CaloDigitStatus::UseForCovariance){
  declareInterface<SubClusterSelectorTool>(this);
  StringArrayProperty eTags( "EnergyTags"     , m_taggerE      ) ;
  StringArrayProperty pTags( "PositionTags"   , m_taggerP      ) ;
  
  // inherit properties from parents
  if( 0 != parent ){ 
    const IProperty* prop = dynamic_cast<const IProperty*> ( parent );
    if( 0 != prop ){
      StatusCode sc;
      sc=prop->getProperty( &eTags      );
      sc=prop->getProperty( &pTags      );
      if( sc.isSuccess() ){
        m_taggerE   = eTags.value();
        m_taggerP   = pTags.value();
      } 
    }
  }
  
  // declare Properties
  declareProperty("EnergyTags"    , m_taggerE   );
  declareProperty("PositionTags"  , m_taggerP   );
  declareProperty("Detector"      , m_det       );
  declareProperty("ConditionName", m_condition );
  
  // define calo-dependent property
  m_det       = LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
  m_condition = "Conditions/Reco/Calo/"+ LHCb::CaloAlgUtils::CaloNameFromAlg( name ) + "ClusterMasks";

  // apply local default setting if not defined by parent
  if(m_taggerE.size() == 0)m_taggerE = std::vector<std::string>(1,"useDB");
  if(m_taggerP.size() == 0)m_taggerP = std::vector<std::string>(1,"useDB");

  // associate known cluster mask to tagger tool
  declareProperty("ClusterTaggers" , m_clusterTaggers);
  m_clusterTaggers[""]            =  "useDB"; // default
  m_clusterTaggers["useDB"]       =  "useDB";
  m_clusterTaggers[ "3x3"]        =  "SubClusterSelector3x3";
  m_clusterTaggers[ "2x2"]        =  "SubClusterSelector2x2";
  m_clusterTaggers[ "SwissCross"] =  "SubClusterSelectorSwissCross";
}
//=============================================================================
// Destructor
//=============================================================================
SubClusterSelectorTool::~SubClusterSelectorTool() {} 

//=============================================================================

StatusCode SubClusterSelectorTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // register to incident service
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;

  // get detector element
  m_detector = getDet<DeCalorimeter> ( m_det   );
  if( NULL == m_detector)return Error("Cannot access DetectorElement at '"+m_det,StatusCode::FAILURE);

  // setup DB accessor tool
  m_dbAccessor = tool<CaloCorrectionBase>("CaloCorrectionBase","DBAccessor",this);
  sc = m_dbAccessor->setConditionParams(m_condition,true);// force access via DB - if not exist will return empty params
  if(sc.isFailure())return Warning("Cannot access DB",StatusCode::FAILURE);
  m_DBtaggerE=std::vector<std::string>(m_detector->numberOfAreas(),"unset");
  m_DBtaggerP=std::vector<std::string>(m_detector->numberOfAreas(),"unset");
  m_sourceE=" (none)";
  m_sourceP=" (none)";

  // always set options parameters first
  sc=getParamsFromOptions();
  if( sc.isFailure())return Error("Cannot setup initial parameters",StatusCode::FAILURE);


  // then possibly update from DB 
  updateParamsFromDB();

  info() << " Energy   mask : " << m_DBtaggerE << m_sourceE << endmsg;
  info() << " Position mask : " << m_DBtaggerP << m_sourceP << endmsg;

  return sc;
}


StatusCode SubClusterSelectorTool::tagEnergy( LHCb::CaloCluster* cluster ){
  // get the tagger
  LHCb::CaloCellID id = cluster->seed();
  ICaloSubClusterTag* tagger = ( id.area() < m_tagE.size() ) ? m_tagE[id.area()] : NULL ;
  if( NULL == tagger )return Warning( "Tagger not found" , StatusCode::FAILURE);
  return tagger->tag(cluster);
}

StatusCode SubClusterSelectorTool::tagPosition( LHCb::CaloCluster* cluster ){
  // get the tagger
  LHCb::CaloCellID id = cluster->seed();
  ICaloSubClusterTag* tagger = ( id.area() < m_tagP.size() ) ? m_tagP[id.area()] : NULL ;
  if( NULL == tagger )return Warning( "Tagger not found" , StatusCode::FAILURE);
  return tagger->tag(cluster);
}

StatusCode SubClusterSelectorTool::tag( LHCb::CaloCluster* cluster ){
  StatusCode sc;
  sc = tagEnergy( cluster);
  sc = tagPosition(cluster );
  return sc;
}


void SubClusterSelectorTool::updateParamsFromDB(){

  using namespace CaloClusterMask;
  unsigned int narea = m_detector->numberOfAreas();
  for( unsigned int area = 0 ; area < narea ; ++area){
    LHCb::CaloCellID id = LHCb::CaloCellID(m_detector->caloName(), area ,0,0);

    if( m_taggerE[area] == "useDB"){
      m_sourceE=" (from DB) ";
      Mask maskE = (Mask) m_dbAccessor->getParameter(CaloCorrection::EnergyMask ,0, id , 0.); // default is 3x3 when no DB
      std::string nameE=maskName[maskE];
      if( nameE != m_DBtaggerE[area] ){  // DB has changed ! update !      
        std::string taggerE   = (m_clusterTaggers.find(nameE) != m_clusterTaggers.end()) ? m_clusterTaggers[nameE]  : "";
        if(taggerE != ""){
          ICaloSubClusterTag* tE = tool<ICaloSubClusterTag>(taggerE,id.areaName()+"EnergyTagger",this);
          tE->setMask(m_energyStatus); 
          m_tagE.push_back( tE  );
          m_DBtaggerE[area] = nameE;
        }else
          Warning("Cannot update energy mask from DB",StatusCode::FAILURE).ignore();
      }
    }    

    if( m_taggerP[area] == "useDB"){
      m_sourceP=" (from DB) ";
      Mask maskP = (Mask) m_dbAccessor->getParameter(CaloCorrection::PositionMask ,0, id , 0.); // default is 3x3 when no DB
      std::string nameP=maskName[maskP];
      if( nameP != m_DBtaggerP[area] ){  // DB has changed ! update !      
        std::string taggerP   = (m_clusterTaggers.find(nameP) != m_clusterTaggers.end()) ? m_clusterTaggers[nameP]  : "";
        if(taggerP != ""){
          ICaloSubClusterTag* tP = tool<ICaloSubClusterTag>(taggerP,id.areaName()+"PositionTagger",this);
          tP->setMask(m_positionStatus); 
          m_tagP.push_back( tP  );
          m_DBtaggerP[area] = nameP;
        }else
          Warning("Cannot update position mask from DB",StatusCode::FAILURE).ignore();
      }
    }
  }
}

  

  
StatusCode SubClusterSelectorTool::getParamsFromOptions(){
  
  unsigned int narea = m_detector->numberOfAreas();
  LHCb::CaloCellID fake = LHCb::CaloCellID(m_detector->caloName(), 0 ,0,0);

  // extend tagger per area when needed
  if( m_taggerE.size() == 1)m_taggerE = std::vector<std::string>(narea , m_taggerE[0]);
  if( m_taggerP.size() == 1)m_taggerP = std::vector<std::string>(narea , m_taggerP[0]);
  if( m_taggerE.size() != m_detector->numberOfAreas() || m_taggerP.size() != m_detector->numberOfAreas())
    return Error("You must define the tagger for each calo area");


  using namespace CaloClusterMask;
  // == Define Energy tagger per area
  m_DBtaggerE=std::vector<std::string>(narea,"unset");
  m_DBtaggerP=std::vector<std::string>(narea,"unset");
  for( unsigned int area = 0 ; area < narea ; ++area){
    LHCb::CaloCellID id = LHCb::CaloCellID(m_detector->caloName(), area ,0,0); // fake cell
    std::string areaName = id.areaName();

    std::string nameE = m_taggerE[area] ;
    if( nameE != "useDB" ){
      m_sourceE=" (from options) ";
      std::string taggerE   = (m_clusterTaggers.find(nameE) != m_clusterTaggers.end()) ? m_clusterTaggers[nameE]  : "";
      if(taggerE == "")
        return Error("Cannot find a  '"+nameE+"' tagger - You must select or define a known tagging method",StatusCode::FAILURE);
      ICaloSubClusterTag* tE = tool<ICaloSubClusterTag>(taggerE,areaName+"EnergyTagger",this);
      tE->setMask(m_energyStatus); 
      m_tagE.push_back( tE  );
      m_DBtaggerE[area]=nameE;
    }

    std::string nameP = m_taggerP[area] ;
    if( nameP != "useDB" ){
      m_sourceP=" (from options) ";
      std::string taggerP   = (m_clusterTaggers.find(nameP) != m_clusterTaggers.end()) ? m_clusterTaggers[nameP]  : ""; 
      if(taggerP == "")
        return Error("Cannot find a  '"+nameP+"' tagger - You must select or define a known tagging method",StatusCode::FAILURE);
      ICaloSubClusterTag* tP= tool<ICaloSubClusterTag>(taggerP,areaName+"PositionTagger",this);
      tP->setMask(m_positionStatus);    
      m_tagP.push_back( tP  );    
    }    
  }

  return StatusCode::SUCCESS;
}


StatusCode SubClusterSelectorTool::finalize() {
  return GaudiTool::finalize();  // must be called after all other actions
}
