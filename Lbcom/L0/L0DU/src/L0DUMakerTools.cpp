// $Id: L0DUMakerTools.cpp,v 1.1 2006-02-18 11:29:37 ocallot Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//
//
// ============================================================================

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// local
#include "L0DUMakerTools.h"


/** @file 
 *  Implementation file for class : L0DUMakerTools
 *  
 *  @date 2005-04-20
 *  @author Olivier Deschamps
 */


// ============================================================================
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0DUMakerTools );
// ============================================================================
L0DUMakerTools::L0DUMakerTools 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaudiTool ( type, name , parent )
  // L0DU Data  property
  , m_operator()
  , m_operands()
  // L0DU condition property
  , m_data()
  , m_comparator()
  , m_threshold()
  // L0DU channel property
  , m_conditions()
  , m_dsrate()
{
  // L0DU Elementary Data
  declareProperty( "Operator"                , m_operator );
  declareProperty( "Operands"                , m_operands );
  // L0DU Elementary Condition 
  declareProperty( "Data"                , m_data );
  declareProperty( "Comparator"          , m_comparator );
  declareProperty( "Threshold"           , m_threshold );
  // L0DU Channel
  declareProperty( "Conditions"         , m_conditions);
  declareProperty( "AcceptRate"         , m_dsrate);
  //interfaces 
  declareInterface<L0DUMakerTools> ( this ) ;  
};

// ============================================================================
/// destructor
// ============================================================================
L0DUMakerTools::~L0DUMakerTools () {} ;

// ============================================================================
StatusCode L0DUMakerTools::makeData(std::string dataName,
                                    LHCb::L0DUElementaryData::Map& datas ) 
/* Elementary L0Data Builder
   Input : - Data name
   Ouput : - Updated L0DUElementaryData container
 */
{

  // Check the data already exist or not (if yes overwrite it)
  LHCb::L0DUElementaryData* data;  
  LHCb::L0DUElementaryData::Map::iterator idata = datas.find( dataName );
  if( idata == datas.end() ){
    data = new  LHCb::L0DUElementaryData(); 
    datas[dataName]=data;
  }else{
    warning() << "L0DU Elementary Data  : '" << dataName
              <<"' already exists - Overwrite it ... " << endreq;  
    data = (*idata).second;
  }

  // Init Data
  data -> init ( dataName, m_operator , m_operands ) ;
return StatusCode::SUCCESS;
}


// ============================================================================
StatusCode L0DUMakerTools::makeCondition(std::string conditionName,
                                         LHCb::L0DUElementaryData::Map& dataContainer, 
                                         LHCb::L0DUElementaryCondition::Map& conditions)
/* Elementary Condition  Builder
   Input : - Condition name
           - L0DUElementaryData container
   Ouput : - Updated conditions container
 */
{


  
  // Check the condition already exist or not (if yes overwrite it)
  LHCb::L0DUElementaryCondition::Map::iterator ic = conditions.find( conditionName );
  
  LHCb::L0DUElementaryCondition* condition ;
  if( ic == conditions.end() ){
    condition = new  LHCb::L0DUElementaryCondition(); 
    conditions[conditionName]=condition;
  }else{
    warning() << "L0DU Elementary Condition : '" << conditionName
        <<"' already exists - Overwrite it ... " << endreq;  
    condition = (*ic).second;
  }

  // Find required Data
  LHCb::L0DUElementaryData::Map::iterator idata = dataContainer.find( m_data );
  if( dataContainer.end() == idata ){
    fatal() << " Can not set-up the '" << conditionName
            << "' L0DU Elementary Condition, "
            << " because the required L0DU Data '" << m_data 
            << "' is not defined." << endreq;
    fatal() << " The defined L0DU Data are : " << endreq;
    for (LHCb::L0DUElementaryData::Map::iterator idata = dataContainer.begin() ; 
         idata != dataContainer.end() ;idata++){
      fatal() <<  " -->  "<< (*idata).second->name()  <<  endreq;
    }
    
    return StatusCode::FAILURE;
 }
  // Init condition (name,data,comparator,threshold)
  condition -> init (conditionName, (*idata).second , m_comparator, m_threshold);
  return StatusCode::SUCCESS;
}


// ============================================================================
StatusCode L0DUMakerTools::makeChannel(std::string channelName,
                                       LHCb::L0DUElementaryCondition::Map&  conditionsContainer,
                                       LHCb::L0DUChannel::Map& channels)
/* L0DU Channel Builder
   Input : - Channel name
           - Elementary Conditions Container
   Ouput : - Updated channels container
 */
{

  // Check if the channel already exists : if yes overwrite it
  LHCb::L0DUChannel::Map::iterator ic = channels.find(channelName);
  LHCb::L0DUChannel* channel ;
   if( ic == channels.end() ){
     channel = new  LHCb::L0DUChannel(); 
     channels[channelName]=channel;
   }else{
     warning() << "L0DU Channel : '" << channelName
               <<"' already exists - Overwrite it ... " << endreq;  
     channel = (*ic).second;
     channel -> resetElementaryConditions();    
  }
  // Init channel (name,downscale rate)
  channel  -> init (  channelName , m_dsrate  ) ;
  // add Elementary Condition(s)
 for(std::vector<std::string>::iterator icond = m_conditions.begin() ; 
     icond != m_conditions.end() ; icond++){
    // Find required Elementary Condition
    LHCb::L0DUElementaryCondition::Map::iterator ic = conditionsContainer.find( *icond );
    if( conditionsContainer.end() == ic ){
      fatal() << " Can not set-up the '" <<  channelName
              << "' L0DU Channel "
              << " because the required L0DU Elementary Condition '" << *icond 
              << "' is not defined." << endreq;
      return StatusCode::FAILURE;
    }
    channel -> addElementaryCondition ( (*ic).second ) ;
  }
  return StatusCode::SUCCESS;
};


