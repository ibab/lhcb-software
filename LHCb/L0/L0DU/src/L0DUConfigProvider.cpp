#include "boost/assign/list_of.hpp"
// from Gaudi
#include "GaudiKernel/StateMachine.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "L0DUConfigProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUConfigProvider
//
//  Provide a **SINGLE** L0DU configuration according to 
//  the Trigger Configuration Key
//  Configuration described in options file
//
//
// 2007-10-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUConfigProvider )

namespace {
  static const std::vector<std::string> s_dataFlags = 
    (boost::assign::list_of(std::string("name")),"data","operator");
  static const std::vector<std::string> s_condFlags = 
    (boost::assign::list_of(std::string("name")),"data","comparator","threshold","index","reported");
  static const std::vector<std::string> s_chanFlags = 
    (boost::assign::list_of(std::string("name")),"condition","rate","enable","disable","mask","index");
  static const std::vector<std::string> s_trigFlags = 
    (boost::assign::list_of(std::string("name")),"channel","index","type");
  // define the allowed operator and comparators
  static const std::vector<std::string> s_comparators=  (boost::assign::list_of( std::string(">") ),"<","==","!=");
  // pair(operator,dimension)
  static const std::vector<std::pair<std::string, unsigned int> > 
  s_operators = boost::assign::list_of(std::make_pair("Id",1))
    (std::make_pair("+", 2))
    (std::make_pair("-", 2))
    (std::make_pair("&", 2))
    (std::make_pair("^", 2));
  // index of the predefined triggers
  static const std::map<std::string,int> s_tIndices = boost::assign::map_list_of(std::string("L0Ecal"), 0)
                                                                                (std::string("L0Hcal"), 1)
                                                                                (std::string("L0Muon"), 2)
                                                                                (std::string("Other"),  3);
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUConfigProvider::L0DUConfigProvider( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_cData(0)
  , m_pData(0)
  , m_template(false)
  , m_uptodate(false)
  , m_reOrder(false)
  , m_reported(0)
{
  declareInterface<IL0DUConfigProvider>(this);
  
  declareProperty( "constData"               , m_constData  )->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "Data"                    , m_data       )->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "Conditions"              , m_conditions )->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "Channels"                , m_channels   )->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "Triggers"                , m_triggers   )->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "MuonFOIx"                , m_FOIx)->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "MuonFOIy"                , m_FOIy)->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  // for options defined configuration
  declareProperty( "Description"             , m_def     = "NO DESCRIPTION")
    ->declareUpdateHandler(&L0DUConfigProvider::handler,this);  
  declareProperty( "Name"                    , m_recipe  = "")->declareUpdateHandler(&L0DUConfigProvider::handler,this);  
  m_sepMap["["] = "]";
  declareProperty( "Separators"              , m_sepMap      )->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty( "FullDescription"         , m_detail  = false)->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  // TCK from name
  int idx = name.find_last_of(".")+1;
  std::string nam = name.substr(idx,std::string::npos);
  if ( (nam == LHCb::L0DUTemplateConfig::Name) | (nam == "L0DUConfig")) {
      m_template = true;  
  } else { 
      size_t index = nam.rfind("0x");
      nam = (index != std::string::npos ) ? nam.substr( index ) : "0x0000";
  }


  declareProperty( "TCK"                     , m_tck  = m_template ? format("0x%04X" , LHCb::L0DUTemplateConfig::TCKValue )
                   : nam )->declareUpdateHandler(&L0DUConfigProvider::handler,this);


 // expert usage
  declareProperty("ForceConditionOrdering" , m_forceOrder = false)->declareUpdateHandler(&L0DUConfigProvider::handler,this);
  declareProperty("HardwareCheckDetail"    , m_check = false)->declareUpdateHandler(&L0DUConfigProvider::handler,this);

  if( context().find("Check") != std::string::npos ){
    m_detail = true;
    m_check = true;
  }
}
//============================================================================= 
// Destructor 
//=============================================================================
L0DUConfigProvider::~L0DUConfigProvider() {}  


//============
// finalize 
//============
StatusCode L0DUConfigProvider::finalize(){

  info() <<  "--------------- TCK = " << format("0x%04X" , m_tckopts) << "------------------"<<endmsg;
  configChecker();
  reset();  
  return GaudiTool::finalize();
}

//============================================================================= 
// Initialize is the main method
//=============================================================================
StatusCode L0DUConfigProvider::initialize(){
  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize L0DUConfigProvider" << endmsg;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  //get condDB tool
  m_condDB = tool<IL0CondDBProvider>("L0CondDBProvider");

  return update();
}

void L0DUConfigProvider::handler(Property& ) {
    if (!m_template && FSMState() >= Gaudi::StateMachine::INITIALIZED ) {
        // on-the-fly update of properties only allowed for template!
        error() << "only template L0DUConfig can be updated after  Initialize has been called" << endmsg;
        throw GaudiException("L0DUConfig: update of properties only allowd for template","",StatusCode::FAILURE);
    }
    m_uptodate=false;
}

void L0DUConfigProvider::reset() {
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "reset L0DUConfigProvider" << endmsg;
    debug() << "Deleting " <<  m_triggersMap.size() << " L0DUTrigger* " << endmsg;
  }
  for(LHCb::L0DUTrigger::Map::iterator id=m_triggersMap.begin();id!=m_triggersMap.end();id++){
   delete id->second;
  }
  m_triggersMap.clear();
  
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Deleting " <<  m_dataMap.size() << " L0DUElementaryData* " << endmsg;
  for(LHCb::L0DUElementaryData::Map::iterator id=m_dataMap.begin();id!=m_dataMap.end();id++){
    delete id->second;
  }  
  m_dataMap.clear();
  m_pData = 0;
  m_cData = 0;


  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Deleting " <<  m_conditionsMap.size() << " L0DUElementaryConditions* " << endmsg;
  for(LHCb::L0DUElementaryCondition::Map::iterator id=m_conditionsMap.begin();id!=m_conditionsMap.end();id++){
    delete id->second;
  }
  m_conditionsMap.clear();
  
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Deleting " <<  m_channelsMap.size() << " L0DUElementaryChannels* " << endmsg;
  for(LHCb::L0DUChannel::Map::iterator id=m_channelsMap.begin();id!=m_channelsMap.end();id++){
   delete id->second;
  }
  m_channelsMap.clear();


  //delete m_config;
  for(std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.begin();it!=m_configs.end();++it){
    LHCb::L0DUConfigs* configs = it->second;
    if( configs == 0 )continue;
    configs->release();
    delete configs;
  }
  m_config = 0;
  m_configs.clear();
}

StatusCode L0DUConfigProvider::update() {
  int index = m_tck.rfind("0x") + 2 ;
  std::string tck = m_tck.substr( index, m_tck.length() );
  std::istringstream is( tck.c_str() );
  is >> std::hex >> m_tckopts  ;

  // init
  m_condOrder.clear();
  m_condMax.clear();
  int num = L0DUBase::NumberOf::Data + L0DUBase::NumberOf::Compounds; // incl. BCID
  m_condOrder.reserve( num );
  for(int i=0;i<num;++i){
    std::vector<LHCb::L0DUElementaryCondition*> temp;
    m_condOrder.push_back(temp);
    m_condMax.push_back( 0 );
  }
  


  info() <<  "--------------- TCK = " << format("0x%04X" , m_tckopts) << "------------------"<<endmsg;

  // load predefined elementary data
  predefinedData(); // must be 1st
  constantData();   // must be 2nd

  //=====================================
  if(m_def == "")m_def = "No Description";
  if(m_template)m_def += " (L0DUConfig.Template)";
  else if(m_channels.size()   == 0  || m_conditions.size() == 0) {
    Warning("Configuration (TCK = " + format("0x%04X" , m_tckopts )  + ") is empty",StatusCode::SUCCESS).ignore();
  }

  StatusCode sc = createTriggers();  // the main method - crate triggers->channels->conditions(->data) chain

  if(sc.isFailure()){
    fatal() << " configuring L0DU (TCK = " << format("0x%04X" , m_tckopts )  << ") failed" << endmsg;
    return StatusCode::FAILURE; 
  }

  std::string slot = ( "" == rootInTES() ) ? "T0" : rootInTES() ;
  if( slot == "") slot = "T0";

  // The main method
  createConfig(slot);

  configChecker();

  m_uptodate = true;
  return StatusCode::SUCCESS;
}


//-------------------------------------------
void L0DUConfigProvider::createConfig(std::string slot){

  // create L0DU configuration 
  m_config = new LHCb::L0DUConfig(m_tckopts);

  m_config->setDefinition( m_def);

  if(m_recipe == ""){
    std::stringstream recipe("");
    recipe << "Undefined recipe name for TCK=" <<  format("0x%04X" , m_tckopts ) ;    
    m_config->setRecipe( recipe.str() );
  }else{
    m_config->setRecipe( m_recipe );
  }  
  //=====================================
  m_config->setData( m_dataMap );
  m_config->setConditions( m_conditionsMap );
  m_config->setChannels( m_channelsMap );
  if(m_triggersMap.size() !=0)m_config->setTriggers( m_triggersMap);
  std::map<std::string,LHCb::L0DUConfigs*>::iterator it = m_configs.find( slot );
  if(it == m_configs.end()){
    m_configs[slot] = new LHCb::L0DUConfigs();
  }
  if( !m_FOIx.empty() )m_config->setMuonFOIx( m_FOIx );
  if( !m_FOIy.empty() )m_config->setMuonFOIy( m_FOIy );

  m_configs[slot]->insert( m_config );
  //=====================================
  printConfig(*m_config,slot);
}



void L0DUConfigProvider::printConfig(LHCb::L0DUConfig config,std::string slot){
  if( slot == "") slot = "T0";
  info() << "**** L0DU Config loading : L0TCK = " << format("0x%04X" , config.tck()) << " for slot " 
         << slot << " ==> OK " << endmsg;
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "              - " << config.data().size()<< " data with "<<endmsg;
    debug() << "                    - " << m_pData << " predefined data "<<endmsg;
    debug() << "                    - " << m_cData << " constant   data "<<endmsg;
    debug() << "                    - " << config.data().size()-m_cData-m_pData << " new data "<<endmsg;
    debug() << "              - " << config.conditions().size() << " Elementary conditions " << endmsg;
    debug() << "              - " << config.channels().size()   << " Channels " << endmsg;
    debug() << " " << endmsg;
  }
  info() <<  "Short description :: " << config.definition()  << endmsg;
  if(m_detail)info() << "Full description  :: " << config.description() << endmsg;
}



//-----------------
// Predefined data 
//-----------------
//=============================================================================
void L0DUConfigProvider::predefinedData( ) {
  using namespace L0DUBase;  
  
  int max = NumberOf::Data;
  for(int i = 0 ; i < max ; ++i){
    std::string name = PredefinedData::Name[i];
    LHCb::L0DUElementaryData* data = new LHCb::L0DUElementaryData(i,LHCb::L0DUElementaryData::Predefined,name,"Id",name);
    m_dataMap[name]= data;
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Predefined Data : " << data->description() << endmsg;
    m_pData++;
  }
}

//=============================================================================

//---------------
// Constant data 
//---------------

void L0DUConfigProvider::constantData(){
  for(std::map<std::string,int>::const_iterator idata = m_constData.begin();idata!=m_constData.end();++idata){
    LHCb::L0DUElementaryData* data = 
      new LHCb::L0DUElementaryData(m_pData+m_cData,LHCb::L0DUElementaryData::Constant,idata->first,"Id",idata->first);
    data->setOperand( idata->second , 1. );
    m_dataMap[idata->first]=data ;
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Constant Data : " << data->summary() << endmsg;
    m_cData++;
  }  
}


//===============================================================
std::vector<std::string> L0DUConfigProvider::Parse(std::string flag, std::vector<std::string> config ){

  std::pair <std::string,std::string> separators = *(m_sepMap.begin());
  if(m_sepMap.size() != 1)
    warning() << "A single pair of separators must be defined - will use the first : " 
              << separators.first << "data" << separators.second << endmsg;

  std::vector<std::string> values;
  for(std::vector<std::string>::iterator iconfig= config.begin(); iconfig != config.end(); iconfig++){
    std::string val;
    // produce case-insensitive result
    std::string conf(*iconfig);
    std::string uConf(conf);
    std::string uFlag(flag);
    std::transform( conf.begin() , conf.end() , uConf.begin () , ::toupper ) ;
    std::transform( flag.begin() , flag.end() , uFlag.begin () , ::toupper ) ;
    int index = uConf.find( uFlag );
    if(index != -1 ){
      if( msgLevel(MSG::VERBOSE) ) 
        verbose() << "Flag '" << flag << "' found in the data string '" << *iconfig << "'" << endmsg;

      // loop over separators
      int id = iconfig->find(separators.first);
      if( id < 0){
        error() << "Unable to parse the tag " << *iconfig 
                << " due to a missing separator (" <<separators.first<<"..."<<separators.second<<")"<< endmsg;
        values.clear();
        return values;
      }
      while(id >= 0){
        int from = iconfig->find(separators.first,id); 
        int to   = iconfig->find(separators.second,from+1);
        if(from != -1 && to != -1){
          val = iconfig->substr(from+1, to-from-1);
          if( msgLevel(MSG::VERBOSE) ) 
            verbose() << "parsed value = '" << val << "'" <<endmsg;  
          values.push_back( val );
          id = iconfig->find(separators.first,to+1);
        }
        else{
          id = -1;    
          error() << "Unable to parse the tag " << *iconfig 
                  << " due to a missing separator (" <<separators.first<<"..."<<separators.second<<")"<< endmsg;
          values.clear();
          return values;
        }
        
      }
    }
  }
  return values;
}

  



//===============================================================
StatusCode L0DUConfigProvider::createData(){

  int id = m_dataMap.size();
  for(ConfigIterator iconfig = m_data.begin(); iconfig != m_data.end() ; ++iconfig){
  std::vector<std::string> values;

    // check all tags exist
    for(std::vector<std::string>::iterator itag = iconfig->begin() ; itag != iconfig->end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::const_iterator iflag = s_dataFlags.begin();iflag!=s_dataFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( itag->begin() , itag->end() , uTag.begin () , ::toupper ) ;
        std::transform( iflag->begin() , iflag->end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new DATA defined via options (num =  " 
                << iconfig-m_data.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new data description are : " << s_dataFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }



    // The Data name 
    //--------------
    values = Parse("name", *iconfig);

    if(values.size() != 1){
      error() << "The DATA definied via option (num = " << iconfig-m_data.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXXX.Data +={ {''name=[DataName]'', "
             <<" ''operator=[+/-/&/^]'', ''data=[OperandName1]'', ''data=[OperandName2]'' , ...}};" << endmsg;

      return StatusCode::FAILURE;  
    }
    std::string dataName = *(values.begin()); // The NAME

    // Check the data already exist or not 
    LHCb::L0DUElementaryData::Map::iterator it = m_dataMap.find( dataName );
    if( it != m_dataMap.end() ){
      warning() << "One L0DUElementaryData  with name : '" << dataName 
                <<"' already exists - Please check your settings" << endmsg;  
      return StatusCode::FAILURE;
    }


    //  The operator
    // -------------
    values = Parse("operator", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique operator for the new data : " 
              << dataName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string op =  *(values.begin()); // The OPERATOR

    bool ok = false;
    unsigned int dim = 0;
    for(std::vector<std::pair<std::string,unsigned int> >::const_iterator iop = s_operators.begin();iop!=s_operators.end();++iop){
      if( op == iop->first){ok=true;dim=iop->second;break;}
    }
    if(!ok){
      fatal() << "requested operator "<< op <<" is not allowed " << endmsg;
      info() << "allowed operators are : " << endmsg;
      for(std::vector<std::pair<std::string, unsigned int> >::const_iterator  it2 = s_operators.begin();
          it2!=s_operators.end();it2++){
        info() << "--> " << it2->first << endmsg;
      } 
      return StatusCode::FAILURE;
    }


    // the operands
    //--------------
    std::vector<std::string> operands = Parse("data", *iconfig);
    if(dim != operands.size() ){
      fatal() << "Number of operands " << values.size() 
              << "does not match the dimension of the operator " << op << "(dim = " << dim << ")"<<endmsg;
      return StatusCode::FAILURE;
    }

    for(std::vector<std::string>::iterator iop = operands.begin() ; iop != operands.end() ; iop++){
      LHCb::L0DUElementaryData::Map::iterator icheck = m_dataMap.find( *iop );
      if( icheck == m_dataMap.end() ){
        error() << "new compound Data is based on an unknown Data '" << *iop  
                << "' ... check  your settings " << endmsg;
        return StatusCode::FAILURE;
      }
    }

    
    // create Data
    LHCb::L0DUElementaryData* data = 
      new LHCb::L0DUElementaryData(id, LHCb::L0DUElementaryData::Compound, dataName, op , operands ) ;
    m_dataMap[dataName]=data;
    id++;
    
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Created Data : " << data->description() << endmsg;

  }
  return StatusCode::SUCCESS;
}





//===============================================================
StatusCode L0DUConfigProvider::createConditions(){

  StatusCode sc = createData();
  if(sc.isFailure())return sc;

  
  int id = m_conditionsMap.size();
  for(ConfigIterator iconfig = m_conditions.begin(); iconfig != m_conditions.end() ; ++iconfig){
    std::vector<std::string> values;
  
    // check all tags exist
    for(std::vector<std::string>::iterator itag = iconfig->begin() ; itag != iconfig->end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::const_iterator iflag = s_condFlags.begin();iflag!=s_condFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( itag->begin() , itag->end() , uTag.begin () , ::toupper ) ;
        std::transform( iflag->begin() , iflag->end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new CONDITION defined via options (num =  " 
                << iconfig-m_conditions.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new CONDITION description are : " << s_condFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }
    
    // The Condition name 
    //--------------------
    values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The CONDITION defined via option (num = " << iconfig-m_conditions.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXX.Conditions +={ {''name=[ConditionName]'', "
             <<" ''data=[dataName]'', ''comparator=[>/</==/!=]'', ''threshold=[value]''} };" << endmsg;

      return StatusCode::FAILURE;  
    }

    std::string conditionName = *(values.begin()); // The NAME 

  // Check the condition already exist or not (if yes overwrite it)
    LHCb::L0DUElementaryCondition::Map::iterator ic = m_conditionsMap.find( conditionName );
    if( ic != m_conditionsMap.end() ){
      warning() << "One L0DUElementaryCondition with name : '" << conditionName
                <<"' already exists  - Please check your settings" << endmsg;
       return StatusCode::FAILURE;
    }


    // The Data
    //------------------
    values = Parse("data", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique dataName for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string data =  *(values.begin()); // The dataName


    // Special case : create RAM(BCID) data on-the-fly
    if( data.rfind("RAM") != std::string::npos && data.rfind("(BCID)") != std::string::npos ){
      int idData = m_dataMap.size();
      LHCb::L0DUElementaryData* ramData = 
        new LHCb::L0DUElementaryData(idData, LHCb::L0DUElementaryData::RAMBcid, data, "Id" , data ) ;
      m_dataMap[data]=ramData;
      // TEMP
      int ind = data.rfind("(BCID)");
      std::string vsn = data.substr(0,ind);
      if ( msgLevel(MSG::DEBUG) ) 
        debug() <<"RAM(BCID) L0DU DATA for RAM vsn = " << vsn << " HAS BEEN DEFINED" <<endmsg;
      //check the RAM version exists
      const std::vector<int> ram = m_condDB->RAMBCID( vsn );
      if( ram.size() == 0){
        fatal() << "RAM(BCID) versions '" << vsn << "' is not registered" << endmsg;
        return StatusCode::FAILURE;
      }
    }



    
    // Check data consistency
    LHCb::L0DUElementaryData::Map::iterator idata = m_dataMap.find( data );
    if( m_dataMap.end() == idata ){
      fatal() << " Can not set-up the '" << conditionName
              << "' L0DU Elementary Condition, "
              << " because the required L0DU Data '" << data 
              << "' is not defined." << endmsg;
      info() << " The predefined L0DU Data are : " << endmsg;
      for (LHCb::L0DUElementaryData::Map::iterator idata = m_dataMap.begin() ; 
           idata != m_dataMap.end() ;idata++){
        info() <<  " -->  "<< idata->second->name()  <<  endmsg; 
      }
      return StatusCode::FAILURE;
    }

    // the Comparator
    //---------------
    values = Parse("comparator", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique comparator for the CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    
    std::string comp =  *(values.begin()); // The COMPARATOR

    bool ok = false;
    for(std::vector<std::string>::const_iterator icomp = s_comparators.begin();icomp!=s_comparators.end();++icomp){
      if( comp == *icomp){ok=true;break;}
    }
    if(!ok){
      fatal() << "requested comparator "<< comp <<" is not allowed " << endmsg;
      info() << "Allowes comparators are : " << s_comparators << endmsg;
      return StatusCode::FAILURE;
    }
   
    // the Threshold
    //---------------
    values = Parse("threshold", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique threshold for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string cut =  *(values.begin()); // The THRESHOLD
    std::stringstream str("");
    str << cut;
    unsigned long threshold;
    str >> threshold ;


    // report  (facultatif)  
    //---------- 
    std::vector<std::string> reports = Parse("reported", *iconfig);
    bool reported = true;
    if( reports.size() > 1 ){
      error() << "Should be an unique report for the new CONDITION : " 
              << conditionName << " (found "<< reports.size() << ")" << endmsg;
      return StatusCode::FAILURE;
    }else if(reports.size() == 1 ){
      std::string report = *(reports.begin());
      std::string uReport(report);
      std::transform( report.begin() , report.end() , uReport.begin () , ::toupper ) ;
      if( uReport == "FALSE" ){
        reported = false;
        info() << "The condition '" << conditionName << "' is NOT to be reported in L0DUReport " << endmsg;
      }else if( uReport != "TRUE"){
        error() << "the CONDITION '" << conditionName << "' report should be True or False" << endmsg;
        return StatusCode::FAILURE;
      }
    }
    if( reported && msgLevel(MSG::DEBUG) )
      debug() << "The condition '" << conditionName << "' is to be reported in L0DUReport " << endmsg;


    // the index (facultatif)  
    //---------- 
    int index = id;
    values = Parse("index", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUElementaryCondition::Map::iterator ii = m_conditionsMap.begin(); ii!=m_conditionsMap.end();ii++){
      if(index == ii->second->index() ){
        error() << "The bit index " << index << " is already assigned to the Condition " << ii->second->name() << endmsg;
        return StatusCode::FAILURE;
      }
    }     

    

    // create condition (name,data,comparator,threshold)
    LHCb::L0DUElementaryCondition* condition = 
      new LHCb::L0DUElementaryCondition(index , conditionName, idata->second , comp , threshold);

    condition->setReported(reported);
    m_conditionsMap[conditionName]=condition;
    id++;

    if( !conditionCheck( condition ) )return Error( "Condition '" + conditionName +"' check failed",StatusCode::FAILURE);

    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Created Condition : " << condition->description() << endmsg;

  }
  //
  if( !conditionOrdering() )return StatusCode::FAILURE;
 
  return StatusCode::SUCCESS;
  
}


//===============================================================
StatusCode L0DUConfigProvider::createChannels(){

  StatusCode sc = createConditions();
  if(sc.isFailure())return sc;
  
  int id = m_channelsMap.size();
  for(ConfigIterator iconfig = m_channels.begin(); iconfig != m_channels.end() ; ++iconfig){
    std::vector<std::string> values;  

    // check all tags exist
    for(std::vector<std::string>::iterator itag = iconfig->begin() ; itag != iconfig->end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::const_iterator iflag = s_chanFlags.begin();iflag!=s_chanFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( itag->begin() , itag->end() , uTag.begin () , ::toupper ) ;
        std::transform( iflag->begin() , iflag->end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new CHANNEL defined via options (num =  " 
                << iconfig-m_channels.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new CHANNEL description are : " << s_chanFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }    

    // The Channel name 
    //------------------
    values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The CHANNEL defined via option (num = " << iconfig-m_channels.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXX.Channels +={ {''name=[ChannelName]'', "
             <<" ''rate=[value]'', ''condition=[conditionName1],[conditionName2], ...'', ''disable=[FALSE]'' } };" << endmsg;

      return StatusCode::FAILURE;  
    }

    std::string channelName = *(values.begin()); // The NAME

    // Check if the channel already exists 
    LHCb::L0DUChannel::Map::iterator ic = m_channelsMap.find(channelName);
    if( ic != m_channelsMap.end() ){
      warning() << "One L0DUChannel with name = : '" << channelName <<"' already exists " 
                << " - Please check your settings" << endmsg;
      return StatusCode::FAILURE;
    }


    // The rate
    //----------
    values = Parse("rate", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique rate for the new CHANNEL : " 
              << channelName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }

    std::string srate =  *(values.begin()); // The rate
    std::stringstream str("");
    str << srate;
    double rate;
    str>> rate;

    if(rate<0. || rate > 100.){
      error() << "The channel accept rate " << rate << " (%) rate should be between 0 and 100" << endmsg;
      return StatusCode::FAILURE;
    }

    rate *= ((double) LHCb::L0DUCounter::Scale) / 100.;
    unsigned int irate = (unsigned int) rate;


    // decision mask (facultatif : default == 0x1 == Physics)
    //-----------------------
    std::vector<std::string> enables  = Parse("enable"  , *iconfig);
    std::vector<std::string> disables = Parse("disable" , *iconfig);
    std::vector<std::string> masks    = Parse("mask"    , *iconfig);

    int type = LHCb::L0DUDecision::Physics; // default
    if( enables.size()+disables.size() + masks.size() > 1 ){
      error() << "Is the channel " << channelName << " enabled or disabled ?  Please check your settings." << endmsg;
      return StatusCode::FAILURE;
    }else if( enables.size() == 1){
      Warning("L0DUChannel flag 'Enable' is deprecated - please move to 'Mask' instead",StatusCode::SUCCESS).ignore();
      std::string item(*(enables.begin()));
      std::string uItem(item); 
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ; 
      if( uItem == "FALSE" ){ 
        type = LHCb::L0DUDecision::Disable; 
      }else{ 
        error() << "Decision type for channel '" << channelName << "' is badly defined -  Please check your setting" << endmsg;
        return StatusCode::FAILURE;         
      }          
    }else if( disables.size() == 1){
      Warning("L0DUChannel flag 'Disable' is deprecated - please move to 'Mask' instead",StatusCode::SUCCESS).ignore();
      std::string item(*(disables.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( uItem == "TRUE" ){
        type = LHCb::L0DUDecision::Disable;
      }else{
        error() << "Decision type for channel '" << channelName << "' is badly defined -  Please check your setting" << endmsg;
        return StatusCode::FAILURE;        
      }
    }else if( masks.size() == 1){
      std::string item(*(masks.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( uItem == "1" || uItem == "0X1" || uItem == "001" || uItem == "Physics" ){
        type = LHCb::L0DUDecision::Physics;
      }else if( uItem == "2" || uItem == "0X2" || uItem == "010" || uItem == "Beam1" ){
        type = LHCb::L0DUDecision::Beam1;
      }else if( uItem == "4" || uItem == "0X4" || uItem == "100" || uItem == "Beam2" ){
        type = LHCb::L0DUDecision::Beam2;
      } else if( uItem == "011" || uItem == "3" || uItem == "0x3" ){
        type = 3;
      } else if( uItem == "101" || uItem == "5" || uItem == "0x5" ){
        type = 5;
      } else if( uItem == "110" || uItem == "6" || uItem == "0x6" ){
        type = 6;
      } else if( uItem == "111" || uItem == "7" || uItem == "0x7" ){
        type = 7;
      } else if( uItem == "000" || uItem == "0" || uItem == "0x0" ){
        type = 0;
      }else {
        error() << "Decision type for channel '" << channelName << "' is badly defined -  Please check your setting" << endmsg;
        return StatusCode::FAILURE;        
      }
    }else{
      // no indication => enable the channel for Physics (default)
      type = LHCb::L0DUDecision::Physics; // default
    }

    // the index (facultatif) 
    // ---------
    int index = id;
    values = Parse("index", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new CHANNEL : " 
              << channelName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUChannel::Map::iterator ii = m_channelsMap.begin(); ii!=m_channelsMap.end();ii++){
      if(index == (ii->second)->index() ){
        error() << "The bit index " << index << " is already assigned to the Channel " << (ii->second)->name() << endmsg;
        return StatusCode::FAILURE;
      }
    }     


    // create channel
    LHCb::L0DUChannel* channel = new LHCb::L0DUChannel(index,  channelName , irate , type ) ;

    // The conditions
    // --------------
    values = Parse("condition", *iconfig);    

    if(values.size() == 0 ){
      error() << "The channel " << channelName << " has no ElementaryCondition" << endmsg;
      return StatusCode::FAILURE;
    }


    // add Elementary Condition(s)
    for(std::vector<std::string>::iterator icond = values.begin() ;icond != values.end() ; icond++){
      // Find required Elementary Condition
      LHCb::L0DUElementaryCondition::Map::iterator ic = m_conditionsMap.find( *icond );
      LHCb::L0DUChannel::Map::iterator icc = m_channelsMap.find( *icond );
      if( m_conditionsMap.end() != ic && m_channelsMap.end() != icc){
        error() << "An ElementaryCondition and a Channel have the same name - please check your setting " << endmsg;
        return StatusCode::FAILURE;
      }
      if( m_conditionsMap.end() == ic ){
        // check if the name  is a previously defined channel
        if( m_channelsMap.end() != icc && *icond != channelName ){
          // YES it is - add all the conditions from this channel
          LHCb::L0DUElementaryCondition::Map condFromChanMap = (icc->second)->elementaryConditions();
          for(LHCb::L0DUElementaryCondition::Map::iterator iccc = condFromChanMap.begin();
              iccc!=condFromChanMap.end();iccc++){
            channel->addElementaryCondition ( iccc->second ) ;
          }          
        } else{        
          fatal() << " Can not set-up the '" <<  channelName
                  << "' L0DU Channel "
                  << " because the required '" << *icond 
                  << "' is neither a  defined ElementaryCondition nor a defined Channel." << endmsg;
          return StatusCode::FAILURE;
        }
      } else {
        channel->addElementaryCondition ( ic->second ) ;
      } 
    }
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Created Channel : " << channel->description() << endmsg;
    
    
    m_channelsMap[channelName]=channel;
    id++;
  }
  return StatusCode::SUCCESS;
}



//===============================================================
StatusCode L0DUConfigProvider::createTriggers(){

  // crate channels -> conditions (-> compound data)
  StatusCode sc = createChannels();
  if(sc.isFailure())return sc;


  if(m_channels.size()   == 0  || m_conditions.size() == 0)return StatusCode::SUCCESS;

  // pre-defined triggers
  predefinedTriggers();
  
  // Additional user-defined triggers
  int id = m_triggersMap.size();  
  for(ConfigIterator iconfig = m_triggers.begin(); iconfig != m_triggers.end() ; ++iconfig){
    
    // check all tags exist
    for(std::vector<std::string>::iterator itag = iconfig->begin() ; itag != iconfig->end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::const_iterator iflag = s_trigFlags.begin();iflag!=s_trigFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( itag->begin() , itag->end() , uTag.begin () , ::toupper ) ;
        std::transform( iflag->begin() , iflag->end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new TRIGGER set defined via options (num =  " 
                << iconfig-m_triggers.begin()  << ")" << endmsg;
        info()  << "Allowed flags for new TRIGGER description are : " << s_trigFlags << endmsg;
        return StatusCode::FAILURE;
      } 
    }



    // The Trigger name 
    //------------------
    std::vector<std::string> values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The SubTrigger defined via option (num = " << iconfig-m_channels.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endmsg;
      info() << "The syntax is ToolSvc.L0DUConfig.TCK_0xXXXX.Triggers +={ {''name=[TriggerName]'', "
             <<" ''channels=[channelName1],[channelName2], ...'' } };" << endmsg;

      return StatusCode::FAILURE;  
    }

    
    std::string triggerName = *(values.begin()); // The NAME


    // Check if the triggers set already exists 
    LHCb::L0DUTrigger::Map::iterator ic = m_triggersMap.find(triggerName);
    if( ic != m_triggersMap.end() ){
      warning() << "A L0DU SubTrigger with name  '" << triggerName <<"' already exists " 
                << " (NB : 'L0Ecal', 'L0Hcal', 'L0Muon' and 'Other' SubTrigger names are predefined)" 
                << " - Please check your settings" << endmsg;
      return StatusCode::FAILURE;
    }


    // the index (facultatif) 
    // ---------
    int index = id;
    values = Parse("index", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new SubTrigger: " 
              << triggerName << " (found "<< values.size() << ")" << endmsg;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUTrigger::Map::iterator ii = m_triggersMap.begin(); ii!=m_triggersMap.end();ii++){
      if(index == (ii->second)->index() ){
        error() << "The bit index " << index << " is already assigned to the SubTrigger " << ii->second->name() << endmsg;
        return StatusCode::FAILURE;
      }
    }


    // The decision type (facultatif)
    // -----------------
    std::vector<std::string> types    = Parse("type"    , *iconfig);
    int mask = LHCb::L0DUDecision::Physics;    
    if(types.size() > 0){
      std::string item(*(types.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( item == "PHYSICS" )mask = LHCb::L0DUDecision::Physics;
      else if( item == "BEAM1" )mask = LHCb::L0DUDecision::Beam1;
      else if( item == "BEAM2" )mask = LHCb::L0DUDecision::Beam2;
      else{    
        error() << "Trigger type '" << item << "' is not valid (must be PHYSICS, BEAM1 or BEAM2)" << endmsg;
        return StatusCode::FAILURE;  
      }
    }

    // create trigger
    LHCb::L0DUTrigger* trigger = new LHCb::L0DUTrigger(index,  triggerName , mask) ;


     // The channels 
    // --------------
    std::vector<std::string> channels = Parse("channel", *iconfig);

    if(channels.size() == 0 ){
      error() << "The SubTrigger " << triggerName << " has no Channel" << endmsg;
      return StatusCode::FAILURE;
    }


    



    // check all requested channels exists
    for(std::vector<std::string>::iterator ichan = channels.begin() ;ichan != channels.end() ; ichan++){
      LHCb::L0DUChannel::Map::iterator ic  = m_channelsMap.find( *ichan );
      LHCb::L0DUTrigger::Map::iterator icc = m_triggersMap.find( *ichan );
      if( m_triggersMap.end() != icc && m_channelsMap.end() != ic){
        error() << "A Channel  and a SubTrigger have the same name - please check your setting " << endmsg;
        return StatusCode::FAILURE;
      }
      if( m_channelsMap.end() == ic ){
        if( m_triggersMap.end() != icc && *ichan != triggerName ){
          LHCb::L0DUChannel::Map chanFromTrigMap = icc->second->channels();
          for(LHCb::L0DUChannel::Map::iterator iccc = chanFromTrigMap.begin(); iccc!=chanFromTrigMap.end();iccc++){
            trigger->addChannel( iccc->second );
          }
        } else {
          fatal() << " Can not set-up the '" <<  triggerName
                  << "' L0DU SubTrigger "
                  << " because the required '" << *ichan 
                  << "' is neither a  defined Channel nor a defined SubTrigger." << endmsg;
          return StatusCode::FAILURE;
        }
      } else {
        trigger->addChannel ( ic->second );
      }
    }

    // check all channels->decisionType match the trigger->decisionType 
    const LHCb::L0DUChannel::Map& chans = trigger->channels();
    for( LHCb::L0DUChannel::Map::const_iterator ic = chans.begin() ; ic != chans.end() ; ++ic){
      LHCb::L0DUChannel* chan = ic->second;
      if( ( chan->decisionType() & trigger->decisionType() ) == 0 ){
        warning() << "The channel '" << LHCb::L0DUDecision::Name[chan->decisionType() ] << "|" << chan->name() 
                  << "' associated to  subTrigger '" 
                  << LHCb::L0DUDecision::Name[trigger->decisionType()] << "|" << trigger->name()
                  << "' will be ignored in the subtrigger decision (decisionType mismatch)"
                  << endmsg;        
      }
    }
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "Created Trigger  : " << trigger->description() << endmsg; 
    m_triggersMap[triggerName] = trigger;

    id++;
  }   // end loop over subtrigger 

  return StatusCode::SUCCESS;
}


//===============================================================
void L0DUConfigProvider::predefinedTriggers(){

  // create predefined triggers (decisionType = physics)
  for(std::map<std::string,int>::const_iterator imap = s_tIndices.begin() ; s_tIndices.end() != imap ; imap++){
    LHCb::L0DUTrigger* trigger = new LHCb::L0DUTrigger(imap->second,  imap->first ) ;
    m_triggersMap[ imap->first ] = trigger;
  }    

  // Associate one channel to one (or several) trigger(s) according to elementary data (physics decisionType only)
  for(  LHCb::L0DUChannel::Map::iterator ic  = m_channelsMap.begin() ; ic != m_channelsMap.end() ; ic++){
    LHCb::L0DUChannel* channel = ic->second;
    if( (channel->decisionType() & LHCb::L0DUDecision::Physics) ==0 )continue;
    // loop over conditions
    std::vector<std::string> dataList;
    for( LHCb::L0DUElementaryCondition::Map::const_iterator ie = channel->elementaryConditions().begin(); 
         ie != channel->elementaryConditions().end() ; ie++){
      const LHCb::L0DUElementaryCondition* condition = ie->second;
      const LHCb::L0DUElementaryData* data = condition->data();
      if( !getDataList( data->name() , dataList ) )
        Warning("Cannot associate the data name '" + data->name() +"' to any (list of) L0DUElementaryData").ignore();
    }
    std::vector<std::string> name = triggerNameFromData( dataList );
    for( std::vector<std::string>::iterator it = name.begin() ; it != name.end() ; it++){
      LHCb::L0DUTrigger::Map::iterator imap = m_triggersMap.find( *it );
      if( imap == m_triggersMap.end()){
        error() << " Unknow trigger name '" << *it << "'" << endmsg;
        continue;
      }
      imap->second->addChannel( channel );
    }
  } 
} 

bool L0DUConfigProvider::getDataList(const std::string dataName, std::vector<std::string>& dataList){
  LHCb::L0DUElementaryData::Map::iterator it = m_dataMap.find( dataName );
  if( it == m_dataMap.end() )return false;
  LHCb::L0DUElementaryData* data = it->second;
  bool ok = true;
  if( LHCb::L0DUElementaryData::Predefined == data->type() ){
    dataList.push_back( data->name() );
    return ok;
  }else if(  LHCb::L0DUElementaryData::Compound == data->type() ){
    for(std::vector<std::string>::const_iterator op = data->operandsName().begin() ; op != data->operandsName().end() ; op++ ){
      ok = ok && getDataList( *op , dataList );
    }    
  }
  return ok;
}

std::vector<std::string> L0DUConfigProvider::triggerNameFromData( std::vector<std::string> dataList ){

  std::vector<std::string> name;

  // The actual trigger->data association definition

  // 1st pass for the main trigger : L0Muon, L0Ecal, L0Hcal
  bool hasTrigger = false;
  for(std::vector<std::string>::iterator id = dataList.begin() ; id != dataList.end() ; id++){
    std::string dataName = id->substr(0,4);
    if( dataName == "Muon" || dataName == "DiMu" ){ 
      name.push_back( "L0Muon" ) ;
      hasTrigger = true;
    } else if( dataName == "Elec" || dataName == "Phot" || dataName == "Loca" || dataName == "Glob" ){
      name.push_back( "L0Ecal" );
      hasTrigger = true;
    } else if( dataName == "Hadr" ){
      name.push_back( "L0Hcal" );
      hasTrigger = true;
    }
  }
  if( !hasTrigger)name.push_back( "Other" );
  return name;
}


bool L0DUConfigProvider::conditionCheck(LHCb::L0DUElementaryCondition* condition){
  using namespace L0DUBase;

  if (NULL == condition)return false;
  const LHCb::L0DUElementaryData* data = condition->data();
  if (NULL == data)return false;

  // check the data consistency
  std::vector<std::string> dataList;
  if( !getDataList( data->name() , dataList ) ){
    warning()<< "Cannot associate the data name '" << data->name() <<"' to any (list of) L0DUElementaryData" << endmsg;
    return false;
  }

  int compoundID = CompoundData::None;
  int dataID = -1;
  for(std::vector<std::string>::iterator il = dataList.begin() ; il != dataList.end() ; ++il){
    LHCb::L0DUElementaryData::Map::iterator it = m_dataMap.find( *il );
    if( it == m_dataMap.end() ){
      warning() << "data '" << *il <<"' associated to condition '"<< condition->name() << "' not found" << endmsg;
      return false;
    }
    
    LHCb::L0DUElementaryData* elData = it->second;
    if( elData->type() != LHCb::L0DUElementaryData::Predefined )continue;
    dataID = elData->id();    

    if( dataID <0 || dataID >= (int)NumberOf::Data){
      warning() << "data ID not correct : " << dataID << endmsg;
      return false; 
    }    
    int type = L0DUBase::PredefinedData::CompoundType[ dataID ]; 
    if( compoundID == L0DUBase::CompoundData::None)compoundID = type; 
    if( type != compoundID )error() << "Compound data does not support inconsistent data" << endmsg; 
  } 


  //
  unsigned int max   =  0;
  int order = -1;
  if( LHCb::L0DUElementaryData::RAMBcid  == data->type() ){
    order = L0DUBase::RAMBCID::ConditionOrder;
    max   = L0DUBase::RAMBCID::MaxNumber;
  }else if( LHCb::L0DUElementaryData::Compound == data->type() && compoundID != CompoundData::None){
    order = CompoundData::ConditionOrder[ compoundID ];
    max   = CompoundData::MaxNumber[compoundID];
  }else if( LHCb::L0DUElementaryData::Predefined == data->type() && dataID != -1 ){
    order = PredefinedData::ConditionOrder[ dataID ];
    max   = PredefinedData::MaxNumber[ dataID ];
  }


  if( order <= 0 ){
    warning() << "Cannot associate a condition order to '" << condition->name() << "'" << endmsg;
    return false;
  }
  //  if( condition->threshold() == 0 && condition->comparator() == ">" ){ // special case for empty register
  //  std::vector<LHCb::L0DUElementaryCondition*>::iterator it = m_condOrder[order].begin();
  //  m_condOrder[order].insert(it, condition);
  //}else{
  m_condOrder[ order ].push_back( condition );
  //  }
  m_condMax[ order ]= max;
  return true;
}


bool L0DUConfigProvider::conditionOrdering(){
  unsigned int iorder = 0;
  m_reOrder = false;
  for( std::vector<std::vector<LHCb::L0DUElementaryCondition*> >::iterator 
         it = m_condOrder.begin() ; m_condOrder.end() != it; ++it){
    std::vector<LHCb::L0DUElementaryCondition*> conds = *it;
    for(std::vector<LHCb::L0DUElementaryCondition*>::iterator itt = conds.begin(); itt != conds.end() ; ++itt){
      LHCb::L0DUElementaryCondition* condition = *itt;

      if ( msgLevel(MSG::DEBUG) ) 
        debug() << "Configuration "<< format("0x%04X" , m_tckopts )
                << " Condition '" << condition->name() 
                << "'  index = " << condition->id()
                << "   order = " << iorder<< endmsg;

      if( condition->id() != iorder ){
        m_reOrder = true;
        if(!m_forceOrder)condition->setId( iorder );
      }
      
      iorder++;
    }
  }  

  if(m_reOrder){
    if( m_forceOrder)
      warning() << "The conditions ordering (that does not match the hardware) is FORCED" << endmsg;
    else
      info() << "The conditions have been re-ordered to match the hardware constraints" << endmsg;
  }

  if( iorder != m_conditionsMap.size() ){
    error() << "Failed to re-order all conditions " << iorder << "/" << m_conditionsMap.size() << endmsg;
    return false;
  }


  // set the reportBit
  unsigned int reportBit = 0;
  for(unsigned int ib = 0 ; ib < m_conditionsMap.size(); ++ib){
    for(LHCb::L0DUElementaryCondition::Map::iterator i=m_conditionsMap.begin();i!=m_conditionsMap.end();i++){
      LHCb::L0DUElementaryCondition* iCond = i->second;
      if( iCond->id() != ib )continue;
      if( iCond->reported() ){
        iCond->setReportBit( reportBit );
        reportBit++;
      }else{
        iCond->setReportBit( 0 );
      }
    }
  }
  m_reported = reportBit;

  // check all indices & all reportBits are unique
  for(LHCb::L0DUElementaryCondition::Map::iterator i=m_conditionsMap.begin();i!=m_conditionsMap.end();i++){
    LHCb::L0DUElementaryCondition* iCond = i->second;
    for(LHCb::L0DUElementaryCondition::Map::iterator j=m_conditionsMap.begin();j!=m_conditionsMap.end();j++){
      LHCb::L0DUElementaryCondition* jCond = j->second;
      if( iCond == jCond )continue;
      if( iCond->id() == jCond->id() ){
        error() << "The condition index MUST be unique : " << iCond->name() << "/" <<jCond->name() <<endmsg;
        return false;
      }
      if( !iCond->reported() || !jCond->reported() )continue;
      if( iCond->reportBit() == jCond->reportBit() ){
        error() << "The condition reportBit MUST be unique : " << iCond->name() << "/" <<jCond->name() <<endmsg;
        return false;
      }
    }
  }
  
  return true;
}



bool L0DUConfigProvider::configChecker(){

  if(m_conditionsMap.size() == 0 && m_channelsMap.size() == 0)return true;
  
  using namespace L0DUBase;
  // check number of channels
  double chRate = (double) m_channelsMap.size() / (double) NumberOf::Channels;
  bool ok  = true;

  if(m_check)info() << "ConfigChecker : channels usage    = "  
                    <<  m_channelsMap.size() << " / " << NumberOf::Channels  
                    << " = [" << format("%3.1f", 100.*chRate) << "% ] " << endmsg;
  if( chRate > 1. ){
    warning() << "L0DU ConfigChecker : the number of channels exceeds the hardware capabilities " << m_channelsMap.size() 
              << " / " << NumberOf::Channels << endmsg;
    ok = false;
  }
  

  // check number of conditions
  double cdRate = (double) m_conditionsMap.size() / (double) NumberOf::Conditions;
  if(m_check)info() << "ConfigChecker : conditions usage  = "  
                    <<  m_conditionsMap.size() << " / " << NumberOf::Conditions  
                    << " = [" << format("%3.1f", 100.*cdRate) << "% ] " << endmsg;
  if( cdRate > 1. ){
    warning()  << "L0DU ConfigChecker : the number of conditions exceeds the hardware capabilities " << m_conditionsMap.size() 
               << " / " << NumberOf::Conditions << endmsg;
    ok = false;
  }
  if( m_conditionsMap.size() > NumberOf::ConditionsInBank ){
    warning() << "L0DU ConfigChecker : only " << NumberOf::ConditionsInBank << " condition-bits among the " 
              << m_conditionsMap.size() << " conditions are stored in the L0DU bank from hardware" << endmsg;
  }
  if(m_check)info() <<"ConfigChecker : reported conditions : " << m_reported << " / " << m_conditionsMap.size() 
                    << " / max = " << NumberOf::ConditionsInBank << endmsg;
  
  

  // check number of conditions / type
  unsigned int k = 0;
  double maxRate = -1.;
  for( std::vector<std::vector<LHCb::L0DUElementaryCondition*> >::iterator it = m_condOrder.begin();m_condOrder.end()!=it;++it){
    std::vector<LHCb::L0DUElementaryCondition*> conds = *it;
    double ctRate = 0;
    if( conds.size() != 0)ctRate = (m_condMax[k] > 0) ? (double) conds.size() / (double) m_condMax[k] : 999.;
    if(ctRate > maxRate)maxRate = ctRate;


    if(m_check && conds.size() != 0){
      std::string name = "??";
      if( k == L0DUBase::RAMBCID::ConditionOrder){
        name = L0DUBase::RAMBCID::Name;
      }else {
        for(unsigned int i=0;i<L0DUBase::NumberOf::Data;++i){
          if( k == L0DUBase::PredefinedData::ConditionOrder[ i ])name=L0DUBase::PredefinedData::Name[i];
        }      
        for(unsigned int i=0;i<L0DUBase::NumberOf::Compounds;++i){
          if( k == L0DUBase::CompoundData::ConditionOrder[ i ])name=L0DUBase::CompoundData::Name[i];
        }
      }
      info() << "ConfigChecker : condition usage  [type = " << name << "] :"
             <<  conds.size()  << " / " << m_condMax[k]  
             << " = [" << format("%3.1f", 100.*ctRate) << "% ] " << endmsg;
    }
    

    if( ctRate > 1.){
      warning() << "ConfigChecker : number of conditions of type " << k 
                << " exceeds the hardware capabilities " << conds.size()<< " / " << m_condMax[k] << endmsg;
      ok = false;
    }
    k++; 
  } 



  //
  if(m_check && m_reOrder ){
    info() << "Hardware-like condition  " ;
    if( ! m_forceOrder ) info() << "ordering has been restored : "<< endmsg;
    else info() << "ordering has NOT been applied : " << endmsg;
    int kk = 0;
    for( std::vector<std::vector<LHCb::L0DUElementaryCondition*> >::iterator it = m_condOrder.begin();m_condOrder.end()!=it;++it){
      std::vector<LHCb::L0DUElementaryCondition*> conds = *it;
      for(std::vector<LHCb::L0DUElementaryCondition*>::iterator itt = conds.begin();itt!=conds.end();++itt){
        info() << " condition = '" << (*itt)->name() << "' index = " << (*itt)->id()  
               << " |   hardware  " <<kk<< " |  reported ? "<< (*itt)->reported() << " (reportBit  "<< (*itt)->reportBit() << ")"
               << endmsg;
        kk++;
      }
    } 
  }


  std::string order = "OK" ;


  if( m_reOrder){
    order = "SWAP";
    if(m_forceOrder) {
      ok = false;
      order +=" (forced)";
    }else{  
      order += " (restored)" ;
    }
  }
  if( ok )info() << "The configuration "<< format("0x%04X" , m_tckopts ) <<" matches the hardware limitations " << endmsg;
  else warning() << "The configuration "<< format("0x%04X" , m_tckopts ) << " DOES NOT match the hardware limitations " << endmsg;



  info() << "- Usage : Channels ["  << format("%3.1f", 100.*chRate) << "% ]  |  "
         << "Conditions [" << format("%3.1f", 100.*cdRate) << "% ]; "
         << "max/type [" << format("%3.1f", 100.*maxRate) << "% ]; "
         << "order : " << order << " ; "
         << "reported  : " << m_reported<<"/" << m_conditionsMap.size()
         << endmsg; 
  return ok; 
}
