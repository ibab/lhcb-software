// Include files 

#include "GaudiKernel/ToolFactory.h" 
#include "Event/ProtoParticle.h"
#include "CaloCorrectionBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloCorrectionBase
//
// 2010-05-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloCorrectionBase )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloCorrectionBase::CaloCorrectionBase( const std::string& type   , 
                                  const std::string& name   ,
                                  const IInterface*  parent ) 
  : GaudiTool ( type, name , parent )
  , m_conditionName()
  , m_corrections()
  , m_hypos  () 
  , m_hypos_ () 
  , m_area   ()
  , m_calo       (DeCalorimeterLocation::Ecal )
  , m_spd        (DeCalorimeterLocation::Spd  )
  , m_prs        (DeCalorimeterLocation::Prs  )
  , m_detData    (DeCalorimeterLocation::Ecal )
  , m_det(0)
  , m_origin     (Gaudi::XYZPoint())
  , m_pileup(0)
  , m_caloElectron(0)
  , m_useCondDB( true )
  , m_cond    ( NULL )
{

  declareInterface<CaloCorrectionBase>(this);

  declareProperty ( "UseCondDB"    , m_useCondDB   = true);
  declareProperty ( "ConditionName", m_conditionName = "none" ) ;
  declareProperty ( "Parameters"   , m_optParams);
  declareProperty ( "Corrections"  , m_corrections); // expect usage
  declareProperty ( "Hypotheses"   , m_hypos_   ) ;
  declareProperty ( "ClusterMatchLocation"   , m_cmLoc );

  m_cmLoc= LHCb::CaloAlgUtils::CaloIdLocation("ClusterMatch", context());
  m_corrections.push_back("All");

  /// acceptable hypotheses 
  m_hypos_.push_back ( (int) LHCb::CaloHypo::Photon               ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::PhotonFromMergedPi0  ) ;
  m_hypos_.push_back ( (int) LHCb::CaloHypo::EmCharged ) ;
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

  // check the setting

  // transform vector of accepted hypos
  m_hypos.clear () ;
  for( Hypotheses_::const_iterator ci = m_hypos_.begin() ; m_hypos_.end() != ci ; ++ci ){
    const int hypo = *ci ;
    if( hypo <= (int) LHCb::CaloHypo::Undefined || 
        hypo >= (int) LHCb::CaloHypo::Other      ) 
    { return Error("Invalid/Unknown  Calorimeter hypothesis object!" ) ; }
    m_hypos.push_back( (LHCb::CaloHypo::Hypothesis) hypo );
  }
  
  // locate and set and configure the Detector 
  m_det = getDet<DeCalorimeter>( m_detData ) ;
  if( 0 == m_det ) { return StatusCode::FAILURE ; }
  m_calo.setCalo( m_detData);
  //
  if( m_hypos.empty() )return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; 
  
  // debug printout of all allowed hypos 
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << " List of allowed hypotheses : " << endmsg;
    for( Hypotheses::const_iterator it = m_hypos.begin() ; m_hypos.end() != it ; ++it ){ 
      debug ()  <<  " -->" << *it  << endmsg ; 
    }

    for( std::vector<std::string>::iterator it = m_corrections.begin() ; m_corrections.end() != it ; ++it){
      debug() << "Accepted corrections :  '" << *it <<"'" << endmsg;
    }
  }

  // get external tools
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this);
  m_pileup = tool<ICaloDigitFilterTool>("CaloDigitFilterTool","FilterTool");

  return setConditionParams(m_conditionName);
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloCorrectionBase::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if( m_corrections.size() > 1 || *(m_corrections.begin()) != "All" ){
    for( std::vector<std::string>::iterator it = m_corrections.begin() ; m_corrections.end() != it ; ++it){
      info() << "Accepted corrections :  '" << *it <<"'" << endmsg;
    }
  }
  if( m_corrections.size() == 0)warning() << "All corrections have been disabled for " <<  name() << endmsg;

  if( m_cond == NULL )
    info() << " Applied corrections configured via options for  " << name() <<endmsg;
  else
    info() << " Applied corrections configured via condDB  ('" << m_conditionName << "') for " << name() << endmsg;
  for(  std::map<std::string, std::vector<double> >::iterator it = m_params.begin() ; m_params.end() != it ; ++it ){    
    std::string type = (*it).first;
    std::vector<double> vec = (*it).second;
    if( !vec.empty() ){
      int func = (int) vec[0];
      int dim  = (int) vec[1];
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << " o  '" << type <<"'  correction as a '" << CaloCorrection::funcName[ func ] 
                << "' function of " << dim << " parameters" << endmsg;
    }else
      warning() << " o '" << type << "' correction HAS NOT BEEN APPLIED  (badly configured)" << endmsg;
  }
  

  return GaudiTool::finalize();  // must be called after all other actions
}

//=============================================================================©©ﬁ
StatusCode CaloCorrectionBase::setDBParams(){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Get params from CondDB condition = " << m_conditionName << endmsg;
  m_params.clear();
  registerCondition(m_conditionName, m_cond, &CaloCorrectionBase::updParams);
  return runUpdate();  
}
// ============================================================================
StatusCode CaloCorrectionBase::setOptParams(){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Get params from options - no condition '" << m_conditionName << "'" << endmsg;
  if( m_optParams.empty() )return Warning("No parameters - no correction to be applied",StatusCode::SUCCESS);
  m_params.clear();
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
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
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
  if( pars.size() < 2 )return pdef;
  
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
  std::vector<double> temp = pars.second;

  // polynomial functions
  if (pars.first == CaloCorrection::Polynomial || 
      pars.first == CaloCorrection::InversPolynomial || 
      pars.first == CaloCorrection::ExpPolynomial ||
      pars.first == CaloCorrection::ReciprocalPolynomial ){
    double v = 1.;
    cor = 0.;
    for( std::vector<double>::iterator i = temp.begin() ; i != temp.end() ; ++ i){
      cor += (*i) * v;
      if( pars.first == CaloCorrection::ReciprocalPolynomial )
        v = (var == 0) ? 0. : v/var ;
      else
        v *= var;
    }
    if( pars.first == CaloCorrection::InversPolynomial) cor = ( cor == 0 ) ? def : 1./cor;
    if( pars.first == CaloCorrection::ExpPolynomial) cor = ( cor == 0 ) ? def : exp(cor);
  }

  // sigmoid function
  if( pars.first == CaloCorrection::Sigmoid ){
    if( temp.size() == 4){
      double a = temp[0];
      double b = temp[1];
      double c = temp[2];
      double d = temp[3];
      cor = a + b*tanh(c*(var+d));
    }
    else{
      Warning("The power sigmoid function must have 4 parameters").ignore();
    }
  }

  // Sshape function
  if( pars.first == CaloCorrection::Sshape ){
    if( temp.size() == 1){
      double b = temp[0];
      double delta = 0.5;
      if( b > 0 ) {
        double arg = var/delta * cosh( delta/b );
        cor = b * log (arg + sqrt( arg*arg + 1. ));
      }
    }
    else{
      Warning("The Sshape function must have 1 parameter").ignore();
    }  
  }

  // Shower profile function
  if( pars.first == CaloCorrection::ShowerProfile ){
    if( temp.size() == 10){
      if( var > 0.5 ) {
        cor = temp[0] * exp( -temp[1]*var);
        cor += temp[2] * exp( -temp[3]*var);
        cor += temp[4] * exp( -temp[5]*var);
      }else{
        cor  = 2.;
        cor -= temp[6] * exp( -temp[7]*var);
        cor -= temp[8] * exp( -temp[9]*var);
      }
    }else{
      Warning("The ShowerProfile function must have 10 parameters").ignore();
    }  
  }

  
  counter(name + " correction processing (" + id.areaName() + ")") += cor;
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
    if( !ok ){
      warning() << " o Type " << type << " is not registered" << endmsg;
      m_params[ type ].clear();
      continue;
    }
    
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
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug() << " o Will apply correction '" << type <<"' as a '" << CaloCorrection::funcName[ func ] 
                << "' function of " << dim << " parameters" << endmsg;
    }
  }
}



double CaloCorrectionBase::incidence(const LHCb::CaloHypo* hypo, bool straight)const {

  const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo(hypo,true) ;

  double incidence = 0;
  if(  LHCb::CaloHypo::EmCharged == hypo->hypothesis() && !straight ){
    // for electron hypothesis : get the matching track
    if (exist<LHCb::Calo2Track::IClusTrTable> (m_cmLoc)) {
      LHCb::Calo2Track::IClusTrTable* ctable = get<LHCb::Calo2Track::IClusTrTable> (m_cmLoc);
      const LHCb::Calo2Track::IClusTrTable::Range range = ctable -> relations(cluster);
      if ( !range.empty() ){
        const LHCb::Track* ctrack = range.front();
        // temporary protoParticle
        LHCb::ProtoParticle* proto = new LHCb::ProtoParticle();
        proto->setTrack( ctrack );
        proto->addToCalo( hypo );
        if( m_caloElectron->set(proto))incidence = m_caloElectron->caloState().momentum().Theta();
        delete proto;
      }
    } 
  }else{
    // for neutrals :
    LHCb::CaloMomentum cMomentum = LHCb::CaloMomentum( hypo );
    incidence = cMomentum.momentum().Theta();    
  }
  return incidence;
}
