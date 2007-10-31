// $Id: L0DUConfigProvider.cpp,v 1.1 2007-10-31 15:04:45 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"

// local
#include "L0DUConfigProvider.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0DUConfigProvider
//
//  Provide L0DU configuration according to Trigger Configuration Key
//  Configuration described in option file
//
//
// 2007-10-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUConfigProvider );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DUConfigProvider::L0DUConfigProvider( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_data(),
    m_conditions(),
    m_channels(),
    m_configs(),
    m_cData(0),
    m_pData(0){
  declareInterface<IL0DUConfigProvider>(this);
  
  declareProperty( "constData"               , m_constData);
  declareProperty( "Data"                    , m_data     );
  declareProperty( "Conditions"              , m_conditions );
  declareProperty( "Channels"                , m_channels );
  declareProperty( "PrintOut"                , m_print=true);
  // for options defined configuration
  declareProperty( "TCK"                     , m_tckopts = 0xFFFF);  
  declareProperty( "Description"             , m_def     = "NO DESCRIPTION");  
  declareProperty( "Separators"              , m_sepMap);

  //
  m_sepMap["["] = "]";
  m_dataFlags.push_back("name");
  m_dataFlags.push_back("data");
  m_dataFlags.push_back("operator");

  m_condFlags.push_back("name");
  m_condFlags.push_back("data");
  m_condFlags.push_back("comparator");
  m_condFlags.push_back("threshold");
  m_condFlags.push_back("index");
  
  
  m_chanFlags.push_back("name");
  m_chanFlags.push_back("condition");
  m_chanFlags.push_back("rate");
  m_chanFlags.push_back("enable");
  m_chanFlags.push_back("disable");
  m_chanFlags.push_back("index");


  // define the allowed operators, comparators and units
  m_comparators.push_back(">");
  m_comparators.push_back("<");
  m_comparators.push_back("==");
  m_comparators.push_back("!=");
  //
  m_operators.push_back(std::make_pair("Id",1));  // pair(operator,dimension)
  m_operators.push_back(std::make_pair("+",2));
  m_operators.push_back(std::make_pair("-",2));
  m_operators.push_back(std::make_pair("&",2));
  m_operators.push_back(std::make_pair("^",2));

}
//============================================================================= 
// Destructor 
//=============================================================================
L0DUConfigProvider::~L0DUConfigProvider() {}  


//============
// Initialize 
//============
StatusCode L0DUConfigProvider::finalize(){

  debug() << "release L0DUConfigProvider" << endreq;
  
  
  
  debug() << "Deleting " <<  m_dataMap.size() << " L0DUElementaryData* " << endreq;
  for(LHCb::L0DUElementaryData::Map::iterator id=m_dataMap.begin();id!=m_dataMap.end();id++){
    delete (*id).second;
  }  
  debug() << "Deleting " <<  m_conditionsMap.size() << " L0DUElementaryConditions* " << endreq;
  for(LHCb::L0DUElementaryCondition::Map::iterator id=m_conditionsMap.begin();id!=m_conditionsMap.end();id++){
    delete (*id).second;
  }
  
  debug() << "Deleting " <<  m_channelsMap.size() << " L0DUElementaryChannels* " << endreq;
  for(LHCb::L0DUChannel::Map::iterator id=m_channelsMap.begin();id!=m_channelsMap.end();id++){
   delete (*id).second;
  }
  
  m_configs.release();

  
  return GaudiTool::finalize();
}

//============================================================================= 
// Initialize is the main method
//=============================================================================
StatusCode L0DUConfigProvider::initialize(){
  debug() << "Initialize L0DUConfigProvider" << endreq;
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure())return sc;

  //
  
  m_separators = *(m_sepMap.begin());
  if(m_sepMap.size() != 1)
    warning() << "Only one pair of separators must be defined - will use the first : " 
              << m_separators.first << "data" << m_separators.second << endreq;

  // clear L0DUConfig container

  //
  // load predefined elementary data
  hardcodedData();
  constantData();



  //=====================================
  // definition of the configuration from condDB/xml-file
  //@TODO

  // getConfigViaXML()


  //=====================================
  // from options


  if(m_channels.size() == 0 )return StatusCode::FAILURE;  // NO configuration
   
  // create L0DU configuration 
  m_config = new LHCb::L0DUConfig(m_tckopts);
  sc = createChannels();
  if(sc.isFailure()){
    fatal() << " configuring L0DU failed" << endreq;
    return StatusCode::FAILURE; 
  }
  m_config->setDefinition( m_def);

  //=====================================
  m_config->setData( m_dataMap );
  m_config->setConditions( m_conditionsMap );
  m_config->setChannels( m_channelsMap );
  m_configs.insert(m_config);
  //=====================================
  printConfig(*m_config);
  return sc;
}  







void L0DUConfigProvider::printConfig(LHCb::L0DUConfig config){
  if(!m_print)return;
  info() << "**** L0DU Configs loading : " <<endreq;
  info() << "              - " << config.data().size()<< " data with "<<endreq;
  info() << "                    - " << m_pData << " predefined data "<<endreq;
  info() << "                    - " << m_cData << " constant   data "<<endreq;
  info() << "                    - " << config.data().size()-m_cData-m_pData << " new data "<<endreq;
  info() << "              - " << config.conditions().size() << " Elementary conditions " << endreq;
  info() << "              - " << config.channels().size()   << " Channels " << endreq;
  info() << " " << endreq;
  info() << "==> " << config.description() << endreq;
}



//-----------------
// Predefined data 
//-----------------
//=============================================================================
void L0DUConfigProvider::hardcodedData( ) {
  
  // 1- content fields (Et/Pt/Multiplicity) ...
  predefinedData("Electron(Et)"       );
  predefinedData("Photon(Et)"         );    
  predefinedData("Hadron(Et)"         );  
  predefinedData("LocalPi0(Et)"       );
  predefinedData("GlobalPi0(Et)"      );
  predefinedData("Sum(Et)"            );
  predefinedData("Spd(Mult)"          );
  predefinedData("Muon1(Pt)"          );
  predefinedData("Muon2(Pt)"          );
  predefinedData("Muon3(Pt)"          );
  predefinedData("DiMuon(Pt)"         );
  predefinedData("PUHits(Mult)"       );
  predefinedData("PUPeak1(Cont)"      );
  predefinedData("PUPeak2(Cont)"      );
  // 2- addresses Fields
  predefinedData("Electron(Add)"      );
  predefinedData("Photon(Add)"        );
  predefinedData("Hadron(Add)"        );
  predefinedData("LocalPi0(Add)"      );
  predefinedData("GlobalPi0(Add)"     );
  predefinedData("Muon1(Add)"         );
  predefinedData("Muon2(Add)"         );
  predefinedData("Muon3(Add)"         );
  predefinedData("PUPeak1(Add)"       );
  predefinedData("PUPeak2(Add)"       );
  // 3 - additional elementary data field (muon sign ... )
  predefinedData("Muon1(Sgn)"         );
  predefinedData("Muon2(Sgn)"         );
  predefinedData("Muon3(Sgn)"         );  
}

//=============================================================================
void L0DUConfigProvider::predefinedData(const std::string& name){
  LHCb::L0DUElementaryData* data = 
    new LHCb::L0DUElementaryData(m_pData,LHCb::L0DUElementaryData::Predefined,name,"Id",name);
  m_dataMap[name]=data ;
  debug() << "Predefined Data : " << data->description() << endreq;
  m_pData++;
}

//---------------
// Constant data 
//---------------

void L0DUConfigProvider::constantData(){
  for(std::map<std::string,int>::const_iterator idata = m_constData.begin();idata!=m_constData.end();++idata){
    LHCb::L0DUElementaryData* data = 
      new LHCb::L0DUElementaryData(m_pData+m_cData,LHCb::L0DUElementaryData::Constant,(*idata).first,"Id",(*idata).first);
    data->setOperand( (*idata).second , 1. );
    m_dataMap[(*idata).first]=data ;
    debug() << "Constant Data : " << data->summary() << endreq;
    m_cData++;
  }  
}


//===============================================================
std::vector<std::string> L0DUConfigProvider::Parse(std::string flag, std::vector<std::string> config ){

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
      verbose() << "Flag '" << flag << "' found in the data string '" << *iconfig << "'" << endreq;

      // loop over separators
      int id = (*iconfig).find(m_separators.first);
      if( id < 0){
        error() << "Unable to parse the tag " << *iconfig 
                << " due to a missing separator (" <<m_separators.first<<"..."<<m_separators.second<<")"<< endreq;
        values.clear();
        return values;
      }
      while(id >= 0){
        int from = (*iconfig).find(m_separators.first,id); 
        int to   = (*iconfig).find(m_separators.second,from+1);
        if(from != -1 && to != -1){
          val = (*iconfig).substr(from+1, to-from-1);
          verbose() << "parsed value = '" << val << "'" <<endreq;  
          values.push_back( val );
          id = (*iconfig).find(m_separators.first,to+1);
        }
        else{
          id = -1;    
          error() << "Unable to parse the tag " << *iconfig 
                  << " due to a missing separator (" <<m_separators.first<<"..."<<m_separators.second<<")"<< endreq;
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
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_dataFlags.begin();iflag!=m_dataFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new DATA defined via options (num =  " 
                << iconfig-m_data.begin()  << ")" << endreq;
        info()  << "Allowed flags for new data description are : " << m_dataFlags << endreq;
        return StatusCode::FAILURE;
      } 
    }



    // The Data name 
    //--------------
    values = Parse("name", *iconfig);

    if(values.size() != 1){
      error() << "The DATA definied via option (num = " << iconfig-m_data.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endreq;
      info() << "The syntax is ToolSvc.L0DUConfig.Data +={ {''name=[DataName]'', "
             <<" ''operator=[+/-/&/^]'', ''data=[OperandName1]'', ''data=[OperandName2]'' , ...}};" << endreq;

      return StatusCode::FAILURE;  
    }
    std::string dataName = *(values.begin()); // The NAME

    // Check the data already exist or not 
    LHCb::L0DUElementaryData::Map::iterator it = m_dataMap.find( dataName );
    if( it != m_dataMap.end() ){
      warning() << "One L0DUElementaryData  with name : '" << dataName 
                <<"' already exists - Please check your settings" << endreq;  
      return StatusCode::FAILURE;
    }


    //  The operator
    // -------------
    values = Parse("operator", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique operator for the new data : " 
              << dataName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }

    std::string op =  *(values.begin()); // The OPERATOR

    bool ok = false;
    unsigned int dim = 0;
    for(std::vector<std::pair<std::string,unsigned int> >::iterator iop = m_operators.begin();iop!=m_operators.end();++iop){
      if( op == (*iop).first){ok=true;dim=(*iop).second;break;}
    }
    if(!ok){
      fatal() << "requested operator "<< op <<" is not allowed " << endreq;
      info() << "allowed operators are : " << endreq;
      for(std::vector<std::pair<std::string, unsigned int> >::iterator  it = m_operators.begin();it!=m_operators.end();it++){
        info() << "--> " << (*it).first << endreq;
        return StatusCode::FAILURE;
      } 
    }


    // the operands
    //--------------
    std::vector<std::string> operands = Parse("data", *iconfig);
    if(dim != operands.size() ){
      fatal() << "Number of operands " << values.size() 
              << "does not match the dimension of the operator " << op << "(dim = " << dim << ")"<<endreq;
      return StatusCode::FAILURE;
    }

    for(std::vector<std::string>::iterator iop = operands.begin() ; iop != operands.end() ; iop++){
      LHCb::L0DUElementaryData::Map::iterator icheck = m_dataMap.find( *iop );
      if( icheck == m_dataMap.end() ){
        error() << "new compound Data is based on an unknown Data '" << *iop  
                << "' ... check  your settings " << endreq;
        return StatusCode::FAILURE;
      }
    }

    
    // create Data
    LHCb::L0DUElementaryData* data = 
      new LHCb::L0DUElementaryData(id, LHCb::L0DUElementaryData::Compound, dataName, op , operands ) ;
    m_dataMap[dataName]=data;
    id++;
    
    debug() << "Created Data : " << data->description() << endreq;

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
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_condFlags.begin();iflag!=m_condFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new CONDITION defined via options (num =  " 
                << iconfig-m_conditions.begin()  << ")" << endreq;
        info()  << "Allowed flags for new CONDITION description are : " << m_condFlags << endreq;
        return StatusCode::FAILURE;
      } 
    }
    
    // The Condition name 
    //--------------------
    values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The CONDITION defined via option (num = " << iconfig-m_conditions.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endreq;
      info() << "The syntax is ToolSvc.L0DUConfig.Conditions +={ {''name=[DataName]'', "
             <<" ''data=[dataName]'', ''comparator=[>/</=/!=]'', ''threshold=[value]''} };" << endreq;

      return StatusCode::FAILURE;  
    }

    std::string conditionName = *(values.begin()); // The NAME 

  // Check the condition already exist or not (if yes overwrite it)
    LHCb::L0DUElementaryCondition::Map::iterator ic = m_conditionsMap.find( conditionName );
    if( ic != m_conditionsMap.end() ){
      warning() << "One L0DUElementaryCondition with name : '" << conditionName
                <<"' already exists  - Please check your settings" << endreq;
       return StatusCode::FAILURE;
    }


    // The Data
    //------------------
    values = Parse("data", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique dataName for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }

    std::string data =  *(values.begin()); // The dataName


    LHCb::L0DUElementaryData::Map::iterator idata = m_dataMap.find( data );
    if( m_dataMap.end() == idata ){
      fatal() << " Can not set-up the '" << conditionName
              << "' L0DU Elementary Condition, "
              << " because the required L0DU Data '" << data 
              << "' is not defined." << endreq;
      info() << " The previously defined L0DU Data are : " << endreq;
      for (LHCb::L0DUElementaryData::Map::iterator idata = m_dataMap.begin() ; 
           idata != m_dataMap.end() ;idata++){
        info() <<  " -->  "<< (*idata).second->name()  <<  endreq;
      }    
      return StatusCode::FAILURE;
    }

    // the Comparator
    //---------------
    values = Parse("comparator", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique comparator for the CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }
    
    std::string comp =  *(values.begin()); // The COMPARATOR

    bool ok = false;
    for(std::vector<std::string>::iterator icomp = m_comparators.begin();icomp!=m_comparators.end();++icomp){
      if( comp == *icomp){ok=true;break;}
    }
    if(!ok){
      fatal() << "requested comparator "<< comp <<" is not allowed " << endreq;
      info() << "Allowes comparators are : " << m_comparators << endreq;
      return StatusCode::FAILURE;
    }
   
    // the Threshold
    //---------------
    values = Parse("threshold", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique threshold for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }

    std::string cut =  *(values.begin()); // The THRESHOLD
    std::stringstream str("");
    str << cut;
    unsigned long threshold;
    str >> threshold ;


    // the index (facultatif)
    //---------- 
    int index = id;
    values = Parse("bit", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new CONDITION : " 
              << conditionName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUElementaryCondition::Map::iterator ii = m_conditionsMap.begin(); ii!=m_conditionsMap.end();ii++){
      if(index == ((*ii).second)->index() ){
        error() << "The bit index " << index << " is already assigned to the Condition " << ((*ii).second)->name() << endreq;
        return StatusCode::FAILURE;
      }
    }     

    

    // create condition (name,data,comparator,threshold)
    LHCb::L0DUElementaryCondition* condition = 
      new LHCb::L0DUElementaryCondition(index , conditionName, (*idata).second , comp , threshold);
    m_conditionsMap[conditionName]=condition;
    id++;

    debug() << "Created Condition : " << condition->description() << endreq;

  }
 
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
    for(std::vector<std::string>::iterator itag = (*iconfig).begin() ; itag != (*iconfig).end() ; itag++){
      bool ok = false;
      for(std::vector<std::string>::iterator iflag = m_chanFlags.begin();iflag!=m_chanFlags.end();iflag++){
        std::string uTag(*itag);
        std::string uFlag(*iflag);
        std::transform( (*itag).begin() , (*itag).end() , uTag.begin () , ::toupper ) ;
        std::transform( (*iflag).begin() , (*iflag).end() , uFlag.begin () , ::toupper ) ;
        int index = (uTag).find( uFlag );
        if( index > -1)ok=true;
      }      
      if( !ok ){ 
        error() << "Description tag : '" << *itag << "' is unknown for the new CHANNEL defined via options (num =  " 
                << iconfig-m_channels.begin()  << ")" << endreq;
        info()  << "Allowed flags for new CHANNEL description are : " << m_chanFlags << endreq;
        return StatusCode::FAILURE;
      } 
    }

    // The Channel name 
    //------------------
    values = Parse("name", *iconfig);
    if(values.size() != 1){
      error() << "The CHANNEL defined via option (num = " << iconfig-m_channels.begin() 
              << ") should have an unique name (found " << values.size() << ")" << endreq;
      info() << "The syntax is ToolSvc.L0DUConfig.Channel +={ {''name=[DataName]'', "
             <<" ''rate=[value]'', ''condition=[conditionName1],[conditionName2], ...'', ''disable=[FALSE]'' } };" << endreq;

      return StatusCode::FAILURE;  
    }

    std::string channelName = *(values.begin()); // The NAME

    // Check if the channel already exists 
    LHCb::L0DUChannel::Map::iterator ic = m_channelsMap.find(channelName);
    if( ic != m_channelsMap.end() ){
      warning() << "One L0DUChannel with name = : '" << channelName <<"' already exists " 
                << " - Please check your settings" << endreq;
      return StatusCode::FAILURE;
    }


    // The rate
    //----------
    values = Parse("rate", *iconfig);
    if(values.size() != 1){
      error() << "Should be an unique rate for the new CHANNEL : " 
              << channelName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }

    std::string srate =  *(values.begin()); // The rate
    std::stringstream str("");
    str << srate;
    double rate;
    str>> rate;

    if(rate<0. || rate > 100.){
      error() << "The channel accept rate " << rate << " (%) rate should be between 0 and 100" << endreq;
      return StatusCode::FAILURE;
    }

    rate *= ((double) LHCb::L0DUCounter::Scale) / 100.;
    unsigned int irate = (unsigned int) rate;


    // Enable/disable channel (facultatif : default == enable)
    //-----------------------
    std::vector<std::string> enables = Parse("enable", *iconfig);
    std::vector<std::string> disables = Parse("disable", *iconfig);

    bool enable = true;
    if( enables.size()+disables.size() > 1 ){
      error() << "Is the channel " << channelName << " enabled or disabled ?  Please check your settings." << endreq;
      return StatusCode::FAILURE;
    }else if( enables.size() == 1){
      std::string item(*(enables.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( uItem == "FALSE" ){
        enable = false;
      }else{
        error() << "Is the channel " << channelName << " enabled or disabled ?  Please check your settings." << endreq;
        return StatusCode::FAILURE;        
      }          
    }else if( disables.size() == 1){
      std::string item(*(disables.begin()));
      std::string uItem(item);
      std::transform( item.begin() , item.end() , uItem.begin () , ::toupper ) ;
      if( uItem == "TRUE" ){
        enable = false;
      }else{
        error() << "Is the channel " << channelName << " enabled or disabled ?  Please check your settings." << endreq;
        return StatusCode::FAILURE;        
      }
    }else{
      // no indication => enable the channel by default
      enable = true;
    }

    // the index (facultatif) 
    // ---------
    int index = id;
    values = Parse("bit", *iconfig);
    if(values.size() > 0){
      std::string id =  *(values.begin()); // The INDEX
      std::stringstream str("");
      str << id;
      str >> index;
    }
    else if(values.size() > 1){
      error() << "Should be an unique index for the new CHANNEL : " 
              << channelName << " (found "<< values.size() << ")" << endreq;
      return StatusCode::FAILURE;  
    }
    // check the index is not already used
    for(LHCb::L0DUChannel::Map::iterator ii = m_channelsMap.begin(); ii!=m_channelsMap.end();ii++){
      if(index == ((*ii).second)->index() ){
        error() << "The bit index " << index << " is already assigned to the Channel " << ((*ii).second)->name() << endreq;
        return StatusCode::FAILURE;
      }
    }     


    // create channel
    LHCb::L0DUChannel* channel = new LHCb::L0DUChannel(index,  channelName , irate , enable ) ;


    // The conditions
    // --------------
    values = Parse("condition", *iconfig);    

    // add Elementary Condition(s)
    for(std::vector<std::string>::iterator icond = values.begin() ;icond != values.end() ; icond++){
      // Find required Elementary Condition
      LHCb::L0DUElementaryCondition::Map::iterator ic = m_conditionsMap.find( *icond );
      LHCb::L0DUChannel::Map::iterator icc = m_channelsMap.find( *icond );
      if( m_conditionsMap.end() != ic && m_channelsMap.end() != icc){
        error() << "An ElementaryCondition and a Channel have the same name - please check your setting " << endreq;
        return StatusCode::FAILURE;
      }
      if( m_conditionsMap.end() == ic ){
        // check if the name  is a previously defined channel
        if( m_channelsMap.end() != icc && *icond != channelName ){
          // YES it is - add all the conditions from this channel
          LHCb::L0DUElementaryCondition::Map m_condFromChanMap = ((*icc).second)->elementaryConditions();
          for(LHCb::L0DUElementaryCondition::Map::iterator iccc = m_condFromChanMap.begin();
              iccc!=m_condFromChanMap.end();iccc++){
            channel->addElementaryCondition ( (*iccc).second ) ;
          }          
        } else{        
          fatal() << " Can not set-up the '" <<  channelName
                  << "' L0DU Channel "
                  << " because the required '" << *icond 
                  << "' is neither a  defined ElementaryCondition nor a defined Channel." << endreq;
          return StatusCode::FAILURE;
        }
      } else {
        channel->addElementaryCondition ( (*ic).second ) ;
      }
    }
    
    debug() << "Created Channel : " << channel->description() << endreq;
    
    
    m_channelsMap[channelName]=channel;
    id++;
  }
  return StatusCode::SUCCESS;
};


