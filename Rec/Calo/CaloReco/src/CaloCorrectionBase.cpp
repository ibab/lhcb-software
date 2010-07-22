// $Id: CaloCorrectionBase.cpp,v 1.2 2010-05-27 07:36:46 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// local
#include "CaloCorrectionBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloCorrectionBase
//
// 2010-05-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloCorrectionBase );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloCorrectionBase::CaloCorrectionBase( const std::string& type   , 
                                  const std::string& name   ,
                                  const IInterface*  parent ) 
  : GaudiTool ( type, name , parent )
  , m_conditionName()
  , m_useCondDB( true )
  , m_cond    ( NULL )
{
  declareInterface<CaloCorrectionBase>(this);

  declareProperty ( "UseCondDB"    , m_useCondDB   = true);
  declareProperty ( "ConditionName", m_conditionName = "none" ) ;
  declareProperty ( "Parameters"   , m_optParams);
  declareProperty ( "Corrections"  , m_corrections); // expect usage
  m_corrections.push_back("All");
}
//=============================================================================
// Destructor
//=============================================================================
CaloCorrectionBase::~CaloCorrectionBase() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloCorrectionBase::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;



  // get parameters
  if ( !existDet<DataObject>( m_conditionName)  ){
    debug() << "Initialize :  Condition '" << m_conditionName
              << "' not found -- apply options parameters !" << endmsg; 
    m_useCondDB = false;
  }

  for( std::vector<std::string>::iterator it = m_corrections.begin() ; m_corrections.end() != it ; ++it){
    debug() << "Accepted corrections :  '" << *it <<"'" << endmsg;
  }
  sc = m_useCondDB ? setDBParams() : setOptParams();
  return sc;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloCorrectionBase::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  for( std::vector<std::string>::iterator it = m_corrections.begin() ; m_corrections.end() != it ; ++it){
    info() << "Accepted corrections :  '" << *it <<"'" << endmsg;
  }

  if( m_cond == NULL )
    info() << " Applied corrections configured via options for : " << endmsg;
  else
    info() << " Applied corrections configured via condDB  ('" << m_conditionName << "') for : " << endmsg;
  for(  std::map<std::string, std::vector<double> >::iterator it = m_params.begin() ; m_params.end() != it ; ++it ){    
    std::string type = (*it).first;
    std::vector<double> vec = (*it).second;
    if( !vec.empty() ){
      int func = (int) vec[0];
      int dim  = (int) vec[1];
      info() << " o  '" << type <<"'  correction as a '" << CaloCorrection::funcName[ func ] 
             << "' function of " << dim << " parameters" << endmsg;
    }else
      warning() << " o '" << type << "' correction HAS NOT BEEN APPLIED  (badly configured)" << endmsg;
  }
  

  return GaudiTool::finalize();  // must be called after all other actions
}

//=============================================================================©©ﬁ
StatusCode CaloCorrectionBase::setDBParams(){
  debug() << "Get params from CondDB condition = " << m_conditionName << endmsg;
  registerCondition(m_conditionName, m_cond, &CaloCorrectionBase::updParams);
  return runUpdate();  
}
// ============================================================================
StatusCode CaloCorrectionBase::setOptParams(){
  debug() << "Get params from options - no condition '" << m_conditionName << "'" << endmsg;
  if( m_optParams.empty() )return Warning("No parameters - no correction to be applied",StatusCode::SUCCESS);

  for(std::map<std::string, std::vector<double> >::iterator p = m_optParams.begin() ; m_optParams.end() != p ; ++p){
    std::string name = (*p).first;
    std::vector<double> vec = (*p).second;
    if( accept( name ) )m_params[name] = vec;
  }
  
  checkParams();
  return StatusCode::SUCCESS;
}
// ============================================================================
StatusCode CaloCorrectionBase::updParams(){
  debug() << "updParams() called" << endmsg;
  if ( !m_cond ) return Error("Condition points to NULL", StatusCode::FAILURE);

  // toDo


  std::vector<std::string> paramNames = m_cond->paramNames();
  
  for(std::vector<std::string>::iterator n = paramNames.begin(); paramNames.end() != n ; ++n){
    std::string paramName = *n;
    if( m_cond -> exists( paramName ) ){
      std::vector<double> params = m_cond->paramAsDoubleVect( paramName ); 
      if( accept( paramName ) )m_params[ paramName ] = params;
    }
  }
  
  checkParams(); 
  return StatusCode::SUCCESS;
}


CaloCorrection::Parameters CaloCorrectionBase::getParams(CaloCorrection::Type type, const LHCb::CaloCellID id) const{

  // define empty Parameters
  std::vector<double> vdef;
  CaloCorrection::Parameters pdef = make_pair( CaloCorrection::Unknown, vdef );
  std::string name = CaloCorrection::typeName[ type ];
  std::map<std::string, std::vector<double> >::const_iterator it = m_params.find( name );
  if( m_params.end() == it )return pdef;

  // get parameters
  std::vector<double> pars = (*it).second;

  // consistency of pars checked elsewhere - straight parsing here
  int func = (int) pars[0];
  int dim  = (int) pars[1];
  std::vector<double> v;
  int pos = 2 + id.area();
  int narea = 3;
  for( int i = 0 ; i< dim ; ++ i){
    v.push_back( pars[ pos ] );
    pos += narea;
  }
  return std::make_pair( (CaloCorrection::Function) func , v); 
}


double CaloCorrectionBase::getCorrection(CaloCorrection::Type type,  const LHCb::CaloCellID id , double var, double def) const{

  CaloCorrection::Parameters pars = getParams( type , id );

  std::string name =  CaloCorrection::typeName[ type ];
  if ( msgLevel( MSG::DEBUG) ) 
    debug() << "Correction type " << name << " to be applied on cluster (seed = " << id << ") is a '" 
            << CaloCorrection::funcName[ pars.first ] << "' function with params = " << pars.second << endmsg;


  // compute correction 
  double cor = def; 
  if( pars.first == CaloCorrection::Unknown ||  pars.second.empty() ) return cor; 

  // polynomial correction 
  if (pars.first == CaloCorrection::Polynomial || pars.first == CaloCorrection::InversPolynomial){
    std::vector<double> temp = pars.second;
    double v = 1.;
    cor = 0.;
    for( std::vector<double>::iterator i = temp.begin() ; i != temp.end() ; ++ i){
      cor += (*i) * v;
      v *= var;
    }
    if( pars.first == CaloCorrection::InversPolynomial) cor = ( cor == 0 ) ? def : 1./cor;
  }    
  counter(name + " correction processing") += cor;
  return cor;
}


void CaloCorrectionBase::checkParams(){

  for(  std::map<std::string, std::vector<double> >::iterator it = m_params.begin() ; m_params.end() != it ; ++it ){
    std::string type = (*it).first;
    // is the type registered
    bool ok = false;
    for( unsigned int i = 0 ; i != CaloCorrection::lastType ; ++i ){
      if( type == CaloCorrection::typeName[i]){
        ok = true;
        break;
      }      
    }
    if( !ok ) warning() << " o Type " << type << " is not registered" << endmsg;
    std::vector<double> vec = (*it).second;
    int func = CaloCorrection::Unknown;
    int dim  = 0;
    if( vec.size() < 3 ) ok = false;
    else{
      func = (int) vec[0];
      dim  = (int) vec[1];
      if( func >= CaloCorrection::Unknown )ok = false;
      if( 3*dim+2 != (int) vec.size())ok=false;    
      if( dim <= 0 ) ok = false;
    }
    if( !ok ){
      warning() << " o Parameters for correction '"<< type << "' are badly defined : [ " << vec << " ]"<< endmsg;
      m_params[ type ].clear();
    }else{
      debug() << " o Will apply correction '" << type <<"' as a '" << CaloCorrection::funcName[ func ] 
             << "' function of " << dim << " parameters" << endmsg;
    }
  }
}

