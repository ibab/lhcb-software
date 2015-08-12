// Include files 

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
  , m_origin     ()
  , m_pileup(0)
  , m_correctCovariance( false )
  , m_caloElectron(0)
  , m_cond    ( nullptr )
  , m_tables  ( nullptr )
  , m_useCondDB( true )
  , m_update  (false )
{

  declareInterface<CaloCorrectionBase>(this);

  declareProperty ( "UseCondDB"    , m_useCondDB   = true);
  declareProperty ( "ConditionName", m_conditionName = "none" ) ;
  declareProperty ( "Parameters"   , m_optParams);
  declareProperty ( "Corrections"  , m_corrections); // expert usage
  declareProperty ( "Hypotheses"   , m_hypos_   ) ;
  declareProperty ( "ClusterMatchLocation"   , m_cmLoc );
  declareProperty ( "CorrectCovariance"      , m_correctCovariance = true);

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

  if ( UNLIKELY (msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  // register to incident service
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::EndEvent ) ;

  // check the setting

  // transform vector of accepted hypos
  m_hypos.clear () ;
  for( const auto& hypo : m_hypos_ ) {
    if( hypo <= (int) LHCb::CaloHypo::Undefined || 
        hypo >= (int) LHCb::CaloHypo::Other      ) 
    { return Error("Invalid/Unknown  Calorimeter hypothesis object!" ) ; }
    m_hypos.push_back( LHCb::CaloHypo::Hypothesis( hypo ));
  }
  
  // locate and set and configure the Detector 
  m_det = getDet<DeCalorimeter>( m_detData ) ;
  if( !m_det ) { return StatusCode::FAILURE ; }
  m_calo.setCalo( m_detData);
  //
  if( m_hypos.empty() )return Error("Empty vector of allowed Calorimeter Hypotheses!" ) ; 
  
  // debug printout of all allowed hypos 
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << " List of allowed hypotheses : " << endmsg;
    for( const auto& h : m_hypos ) {
      debug ()  <<  " -->" << h  << endmsg ; 
    }
    for( const auto& c : m_corrections ) {
      debug() << "Accepted corrections :  '" << c <<"'" << endmsg;
    }
  }

  // get external tools
  m_caloElectron = tool<ICaloElectron>("CaloElectron", this);
  m_pileup = tool<ICaloDigitFilterTool>("CaloDigitFilterTool","FilterTool");
  m_tables = tool<ICaloRelationsGetter>("CaloRelationsGetter","CaloRelationsGetter",this);
  return setConditionParams(m_conditionName);
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloCorrectionBase::finalize() {

  if ( UNLIKELY(msgLevel(MSG::DEBUG)) ) debug() << "==> Finalize" << endmsg;

  if( m_corrections.size() > 1 || *(m_corrections.begin()) != "All" ){
    for( const auto& c :  m_corrections ) {
      info() << "Accepted corrections :  '" << c <<"'" << endmsg;
    }
  }
  if( m_corrections.empty())warning() << "All corrections have been disabled for " <<  name() << endmsg;
  
    if( m_cond == nullptr )
      warning() << " Applied corrections configured via options for  " << name() <<endmsg;
    else if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << " Applied corrections configured via condDB  ('" << m_conditionName << "') for " 
                                                       << name() << endmsg;
    


  for(  const auto& param : m_params ) {
    const std::string& type = param.first;
    const std::vector<double>& vec = param.second;
    if( !vec.empty() ){
      int func = vec[0];
      int dim  = vec[1];
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
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "Get params from options " << endmsg;
  if( m_optParams.empty() && m_conditionName != "none"  ){
    info()<< "No default options parameters defined"<<endmsg;
    return StatusCode::SUCCESS;
  }
  m_params.clear();
  for(const auto&  p : m_optParams ) {
    const std::string& name = p.first;
    if ( accept( name ) ) m_params[name] = p.second;
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


  for( const auto& paramName : m_cond->paramNames() ) {
    if( m_cond -> exists( paramName ) ){
      const std::vector<double>&  params = m_cond->paramAsDoubleVect( paramName ); 
      if( accept( paramName ) ) m_params[ paramName ] = params;
    }
  }
  m_update = true;
  checkParams(); 
  return StatusCode::SUCCESS;
}


CaloCorrection::Parameters CaloCorrectionBase::getParams(CaloCorrection::Type type, const LHCb::CaloCellID id) const{

  const std::string& name = CaloCorrection::typeName[ type ];
  auto it = m_params.find( name );
  if( m_params.end() == it )return {  CaloCorrection::Unknown, std::vector<double>{} };

  // get parameters
  const std::vector<double>& pars = it->second;
  if( pars.size() < 2 )return {  CaloCorrection::Unknown, std::vector<double>{} };
  
  // consistency of pars checked elsewhere - straight parsing here
  int func = pars[0];
  int dim  = pars[1];
  int narea = ( func != CaloCorrection::GlobalParamList ) ? 3         : 1;
  int shift = ( func != CaloCorrection::GlobalParamList ) ? id.area() : 0;
  int pos = 2 + shift;  

  std::vector<double> v; v.reserve(dim);
  for( int i = 0 ; i< dim ; ++i ){
    v.push_back( pars[ pos ] );
    pos += narea;
  }
  return { (CaloCorrection::Function) func , std::move(v) }; 
}


double CaloCorrectionBase::getCorrection(CaloCorrection::Type type,  const LHCb::CaloCellID id , double var, double def) const{

  CaloCorrection::Parameters pars = getParams( type , id );

  const auto& name =  CaloCorrection::typeName[ type ];
  if ( UNLIKELY(msgLevel( MSG::DEBUG) ) )
    debug() << "Correction type " << name << " to be applied on cluster (seed = " << id << ") is a '" 
            << CaloCorrection::funcName[ pars.first ] << "' function with params = " << pars.second << endmsg;


  // compute correction 
  if( pars.first == CaloCorrection::Unknown ||  pars.second.empty() ) return def; 

  // list accessor - not correction :
  if( pars.first == CaloCorrection::ParamList || pars.first == CaloCorrection::GlobalParamList){
    warning() << " Param accessor is a fake function - no correction to be applied - return default value" << endmsg;
    return def;
  }

  double cor = def; 
  // polynomial correction 
  const std::vector<double>& temp = pars.second;

  // polynomial functions
  if (pars.first == CaloCorrection::Polynomial || 
      pars.first == CaloCorrection::InversPolynomial || 
      pars.first == CaloCorrection::ExpPolynomial ||
      pars.first == CaloCorrection::ReciprocalPolynomial ){
    double v = 1.;
    cor = 0.;
    for( auto  i = temp.begin() ; i != temp.end() ; ++ i){
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
  if( pars.first == CaloCorrection::Sshape || pars.first == CaloCorrection::SshapeMod ){
    if( temp.size() == 1){
      double b = temp[0];
      double delta = 0.5;
      if( b > 0 ) {
        double arg = var/delta ;
        if( pars.first == CaloCorrection::SshapeMod) arg *= sinh(delta/b);
        else if(pars.first==CaloCorrection::Sshape)  arg *= cosh(delta/b);        
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
  
  // Sinusoidal function
  if( pars.first == CaloCorrection::Sinusoidal ){
    if( temp.size() == 1){
      double A = temp[0];
      double pi=acos(-1.);
      cor = A*sin(2*pi*var);
    }
    else{
      Warning("The Sinusoidal function must have 1 parameter").ignore();
    }
  }
  
  kounter(type, id.areaName()) += cor;
  return cor;
}
      


void CaloCorrectionBase::checkParams(){

  for(  const auto& param :  m_params ) {
    const std::string& type = param.first;
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
    
    const std::vector<double>& vec = param.second;
    int func = CaloCorrection::Unknown;
    int dim  = 0;
    if( vec.size() < 3 ) ok = false;
    else{
      func = vec[0];
      dim  = vec[1];
      if( func >= CaloCorrection::Unknown )ok = false;
      int narea = ( func != CaloCorrection::GlobalParamList ) ? 3         : 1;
      if( narea*dim+2 != (int) vec.size())ok=false;    
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
      LHCb::Calo2Track::IClusTrTable* ctable = m_tables->getClusTrTable( m_cmLoc );
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

double CaloCorrectionBase::getCorrectionDerivative(CaloCorrection::Type type,  const LHCb::CaloCellID id , double var, double def) const{

  CaloCorrection::Parameters pars = getParams( type , id );

  std::string name =  CaloCorrection::typeName[ type ];
  if ( msgLevel( MSG::DEBUG) ) 
    debug() << "Derivative for Correction type " << name << " to be calculated for cluster (seed = " << id << ") is a '" 
            << CaloCorrection::funcName[ pars.first ] << "' function with params = " << pars.second << endmsg;

  // compute correction 
  double cor = def; 
  if( pars.first == CaloCorrection::Unknown ||  pars.second.empty() ) return cor; 

  // polynomial correction 
  std::vector<double> temp = pars.second;


  // polynomial functions
  double ds = 0.;
  if (pars.first == CaloCorrection::Polynomial ){
    ds  = 0.;
    double v = 1.;
    int cnt = 0;
    std::vector<double>::iterator i = temp.begin();
    for( ++ i, cnt ++ ; i != temp.end() ; ++ i, cnt ++){
      ds += (*i) * cnt * v;
      v  *= var;
    }
  }

  if (pars.first == CaloCorrection::InversPolynomial ){
    double v = 1.;
    cor = 0.;
    for( std::vector<double>::iterator i = temp.begin() ; i != temp.end() ; ++ i){
      cor += (*i) * v;
      v *= var;
    }
    cor = ( cor == 0 ) ? def : 1./cor;

    v       = 1.;
    ds      = 0.;
    int cnt = 0;
    std::vector<double>::iterator i = temp.begin();
    for( ++ i, cnt ++ ; i != temp.end() ; ++ i, cnt ++){
      ds  += (*i) * cnt * v;
      v *= var;
    }
 
    ds *= -cor*cor;
  }


  if (pars.first == CaloCorrection::ExpPolynomial ){
    double v = 1.;
    cor = 0.;
    for( std::vector<double>::iterator i = temp.begin() ; i != temp.end() ; ++ i){
      cor += (*i) * v;
      v *= var;
    }
    cor = ( cor == 0 ) ? def : exp(cor);
 
    ds = 0.;
    v  = 1.;
    int cnt = 0;
    std::vector<double>::iterator i = temp.begin();
    for( ++ i, cnt ++ ; i != temp.end() ; ++ i, cnt ++){
      ds  += (*i) * cnt * v;
      v *= var;
    }
 
    ds *= cor;
  }

  if (pars.first == CaloCorrection::ReciprocalPolynomial ){
    ds = 0.;
    if ( var != 0 ){ 
      double v = 1./var/var;
      int cnt = 0;
      std::vector<double>::iterator i = temp.begin();
      for( ++ i, cnt ++ ; i != temp.end() ; ++ i, cnt ++){
        ds  -= (*i) * cnt * v;
        v /= var;
      }
    }
  }

  // sigmoid function
  if( pars.first == CaloCorrection::Sigmoid ){
    ds = 0.;
    if( temp.size() == 4){
      // double a = temp[0];
      double b = temp[1];
      double c = temp[2];
      double d = temp[3];
      ds  = b*c*(1.-pow(tanh(c*(var+d)),2));
    }
    else{
      Warning("The power sigmoid function must have 4 parameters").ignore();
    }
  }


  // Sshape function
  if( pars.first == CaloCorrection::Sshape || pars.first == CaloCorrection::SshapeMod ){
    ds = 0.;
    if( temp.size() == 1){
      double b = temp[0];
      double delta = 0.5;
      if( b > 0 ) {
        double csh = 1.;
        if( pars.first == CaloCorrection::SshapeMod) csh = sinh(delta/b);
        else if(pars.first==CaloCorrection::Sshape)  csh = cosh(delta/b);        
        double arg = var/delta * csh;
        ds = b / delta * csh / sqrt( arg*arg + 1. );
      }
    }
    else{
      Warning("The Sshape function must have 1 parameter").ignore();
    }  
  }

  // Shower profile function
  if( pars.first == CaloCorrection::ShowerProfile ){
    ds = 0.;
    if( temp.size() == 10){
      if( var > 0.5 ) {
        ds = -temp[0]*temp[1]*exp( -temp[1]*var);
        ds+= -temp[2]*temp[3]*exp( -temp[3]*var);
        ds+= -temp[4]*temp[5]*exp( -temp[5]*var);
      }else{
        ds =  temp[6]*temp[7]*exp( -temp[7]*var);
        ds+=  temp[8]*temp[9]*exp( -temp[9]*var);
      }
    }else{
      Warning("The ShowerProfile function must have 10 parameters").ignore();
    }  
  }

  // Sinusoidal function
  if( pars.first == CaloCorrection::Sinusoidal ){
    ds = 0.;
    if( temp.size() == 1){
      double A = temp[0];
      double twopi=2.*M_PI;
      ds = A*twopi*cos(twopi*var);
    }
    else{
      Warning("The Sinusoidal function must have 1 parameter").ignore();
    }
  }
 
  
  //counter(name + " derivative processing (" + id.areaName() + ")") += cor;
  return ds;
}
 
