// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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
  , m_taggerE            ( 1 , "useDB") 
  , m_taggerP            ( 1 , "useDB") 
  , m_tagE               ()
  , m_tagP               ()
  , m_det(DeCalorimeterLocation::Ecal){
  declareInterface<SubClusterSelectorTool>(this);
  StringArrayProperty eTags( "EnergyTags"     , m_taggerE      ) ;
  StringArrayProperty pTags( "PositionTags"   , m_taggerP      ) ;
  StringProperty      det  ( "Detector"       , m_det          ) ;  
  StringProperty      cond ( "TagCondition"   , m_condition    ) ;  
  
  // inherit properties from parents
  if( 0 != parent ){ const IProperty* prop = dynamic_cast<const IProperty*> ( parent );
    if( 0 != prop ){
      StatusCode sc;
      sc=prop->getProperty( &cond       );
      sc=prop->getProperty( &eTags      );
      sc=prop->getProperty( &pTags      );
      sc=prop->getProperty( &det        );
      if(sc.isFailure())warning()<<"Unable to get properties"<<endmsg;
      else{
        m_taggerE   = eTags.value();
        m_taggerP   = pTags.value();
        m_det       = det.value();
        m_condition = cond.value();
      } 
    }
  }

  // declare Properties
  declareProperty("EnergyTags"    , m_taggerE   );
  declareProperty("PositionTags"  , m_taggerP   );
  declareProperty("Detector"      , m_det       );
  declareProperty("TagCondition"  , m_condition );

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

  // DB accessor tool
  m_dbAccessor = tool<CaloCorrectionBase>("CaloCorrectionBase","DBAccessor",this);

  // get detector element
  m_detector = getDet<DeCalorimeter> ( m_det   );

  // set
  return set(m_detector , m_taggerE , m_taggerP );
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



StatusCode SubClusterSelectorTool::set(DeCalorimeter* detector, 
                                        std::vector<std::string> tagE, std::vector<std::string> tagP){
  // first check if parameters exists in reconstruction conditions
  unsigned int narea = detector->numberOfAreas();
  LHCb::CaloCellID fake = LHCb::CaloCellID(m_detector->caloName(), 0 ,0,0);
  if( m_condition == "" )m_condition = "Conditions/Reco/Calo/"+fake.caloName() + "ClusterMasks"; // default condition path
  StatusCode sc = m_dbAccessor->setConditionParams(m_condition,true);// force access via DB - if not exist will return empty params
  if(sc.isFailure())return Warning("Cannot access DB",StatusCode::FAILURE);
  // extend tagger per area when needed
  if( tagE.size() == 1)tagE = std::vector<std::string>(narea , tagE[0]);
  if( tagP.size() == 1)tagP = std::vector<std::string>(narea , tagP[0]);
  if( tagE.size() != detector->numberOfAreas() || tagP.size() != detector->numberOfAreas())
    return Error("You must define the tagger for each calo area");

  using namespace LHCb::CaloDigitStatus; 
  LHCb::CaloDigitStatus::Status forEnergy   = UseForEnergy   | UseForCovariance;
  LHCb::CaloDigitStatus::Status forPosition = UseForPosition | UseForCovariance;

  using namespace CaloClusterMask;
  bool fromDB = false;
  // == Define Energy tagger per area
  for( unsigned int area = 0 ; area < narea ; ++area){
    std::string nameE = tagE[area] ;
    std::string nameP = tagP[area] ;
    std::string  flagE = "From User";
    std::string  flagP = "From User";

    // fake cell
    LHCb::CaloCellID id = LHCb::CaloCellID(m_detector->caloName(), area ,0,0);
    if( nameE == "useDB" ){
      fromDB = true;
      CaloClusterMask::Mask maskE = (CaloClusterMask::Mask) m_dbAccessor->getParameter(CaloCorrection::EnergyMask ,0, id , 0.); // default is 3x3 when no DB
      nameE=maskName[maskE];
      tagE[area] = nameE;
    }
    if( nameP == "useDB" ){
      fromDB = true;
      CaloClusterMask::Mask maskP = (CaloClusterMask::Mask) m_dbAccessor->getParameter(CaloCorrection::PositionMask,0, id , 0.); // default is 3x3 when no DB
      nameP=maskName[maskP];
      tagP[area] = nameP;
    }
    std::string taggerE   = (m_clusterTaggers.find(nameE) != m_clusterTaggers.end()) ? m_clusterTaggers[nameE]  : "";
    if(taggerE == "")return Error("Cannot find a  '"+nameE+"' tagger - You must select or define a known tagging method",
                                  StatusCode::FAILURE);
    std::string taggerP   = (m_clusterTaggers.find(nameP) != m_clusterTaggers.end()) ? m_clusterTaggers[nameP]  : ""; 
    if(taggerP == "")return Error("Cannot find a  '"+nameP+"' tagger - You must select or define a known tagging method",
                                  StatusCode::FAILURE);
    std::string areaName = id.areaName();
    ICaloSubClusterTag* tE = tool<ICaloSubClusterTag>(taggerE,areaName+"EnergyTagger",this);
    tE->setMask(forEnergy); 
    m_tagE.push_back( tE  );

    ICaloSubClusterTag* tP= tool<ICaloSubClusterTag>(taggerP,areaName+"PositionTagger",this);
    tP->setMask(forPosition);    
    m_tagP.push_back( tP  );    

    // printout
    if ( UNLIKELY(msgLevel(MSG::DEBUG)))debug()   << areaName << " Energy tagger   : " << nameE << " (" << tagE << ")"  << endmsg;
    if ( UNLIKELY(msgLevel(MSG::DEBUG)))debug()   << areaName << " Position tagger : " << nameP << " (" << tagP << ")"  << endmsg;
  }
  info() << "Tagger -  Energy : " << tagE << " - Position :  " << tagP;
  if(fromDB) info() << " (from DB)" ;
  info() << endmsg;
  
  return StatusCode::SUCCESS;
}


StatusCode SubClusterSelectorTool::finalize() {
  return GaudiTool::finalize();  // must be called after all other actions
}
