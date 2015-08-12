// ============================================================================
// Include files
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Event/CaloCluster.h"
#include "ClusterCovarianceMatrixTool.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class ClusterCovarianceMatrixTool
 *
 *  @date 02/11/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  modified 02/07/2014 by O. Deschamps
 */
// ============================================================================

DECLARE_TOOL_FACTORY( ClusterCovarianceMatrixTool )

// ============================================================================
/** Standard constructor
 *  @param type tool type (useless)
 *  @param name tool name
 *  @param parent pointer to parent object (service, algorithm or tool)  
 */
// ============================================================================
ClusterCovarianceMatrixTool::ClusterCovarianceMatrixTool
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GaudiTool( type , name , parent )
  , m_estimator (      ) 
  , m_parameters ( )
  , m_detData    ( )
  , m_det(0)
  , m_useDB(true)
  , m_source(){
  // interface!
  declareInterface<ICaloClusterTool> (this);

  // properties :
  declareProperty( "Parameters"       , m_parameters   );
  declareProperty( "UseDBParameters"  , m_useDB=true   );
  declareProperty( "ConditionName"    , m_conditionName);
  declareProperty( "Detector"         , m_detData      );

  // set default configuration as a function of detector
  using namespace CaloCovariance;
  m_detData    = LHCb::CaloAlgUtils::DeCaloLocation( name ) ;
  std::string caloName =  LHCb::CaloAlgUtils::CaloNameFromAlg( name );
  m_conditionName = "Conditions/Reco/Calo/"+caloName+"Covariance";


  // get parameters from parent property when defined
  ParameterProperty p("CovarianceParameters", m_parameters);
  if( 0 != parent ){ const IProperty* prop = dynamic_cast<const IProperty*> ( parent );
    if( 0 != prop ){
      if( prop->getProperty( &p ).isSuccess() &&  p.value().size() != 0 ){
        m_parameters = p.value();
        m_useDB = false ;  // parent settings win !
      }
    }
  }

  // apply local parameters if not defined in parent algorithm
  if(caloName == "Ecal"){
    if(m_parameters.find(ParameterName[Stochastic])     ==m_parameters.end())m_parameters[ParameterName[Stochastic]]     .push_back( 0.10);
    if(m_parameters.find(ParameterName[GainError])      ==m_parameters.end())m_parameters[ParameterName[GainError]]      .push_back( 0.01 );
    if(m_parameters.find(ParameterName[IncoherentNoise])==m_parameters.end())m_parameters[ParameterName[IncoherentNoise]].push_back( 1.20 );
    if(m_parameters.find(ParameterName[CoherentNoise])  ==m_parameters.end())m_parameters[ParameterName[CoherentNoise]]  .push_back( 0.30 );
    if(m_parameters.find(ParameterName[ConstantE])      ==m_parameters.end())m_parameters[ParameterName[ConstantE]]      .push_back( 0. );
    if(m_parameters.find(ParameterName[ConstantX])      ==m_parameters.end())m_parameters[ParameterName[ConstantX]]      .push_back( 0. );
    if(m_parameters.find(ParameterName[ConstantY])      ==m_parameters.end())m_parameters[ParameterName[ConstantY]]      .push_back( 0. );
  }else if( caloName == "Hcal"){
    if(m_parameters.find(ParameterName[Stochastic])     ==m_parameters.end())m_parameters[ParameterName[Stochastic]]     .push_back( 0.70 );
    if(m_parameters.find(ParameterName[GainError])      ==m_parameters.end())m_parameters[ParameterName[GainError]]      .push_back( 0.10 );
    if(m_parameters.find(ParameterName[IncoherentNoise])==m_parameters.end())m_parameters[ParameterName[IncoherentNoise]].push_back( 1.20 );
    if(m_parameters.find(ParameterName[CoherentNoise])  ==m_parameters.end())m_parameters[ParameterName[CoherentNoise]]  .push_back( 0.30 );
    if(m_parameters.find(ParameterName[ConstantE])      ==m_parameters.end())m_parameters[ParameterName[ConstantE]]      .push_back( 0. );
    if(m_parameters.find(ParameterName[ConstantX])      ==m_parameters.end())m_parameters[ParameterName[ConstantX]]      .push_back( 0. );
    if(m_parameters.find(ParameterName[ConstantY])      ==m_parameters.end())m_parameters[ParameterName[ConstantY]]      .push_back( 0. );
  }
}

ClusterCovarianceMatrixTool::~ClusterCovarianceMatrixTool() {}

//==========
StatusCode ClusterCovarianceMatrixTool::getParamsFromOptions(){
  m_source.clear();
  unsigned int nareas = m_det->numberOfAreas();  
  for(CaloCovariance::ParameterMap::const_iterator imap = m_parameters.begin() ; m_parameters.end() != imap ; ++imap){
    const std::vector<double>& pars = imap->second;
    if( pars.size()    == 1)m_parameters[imap->first] = std::vector<double>( nareas , pars[0]   );
    if( pars.size() != nareas )return Error("Parameters must be set for each calo area",StatusCode::FAILURE);
  }
  // check all expected parameters are defined
  using namespace CaloCovariance;
  for(unsigned int index = 0 ; index < CaloCovariance::Last ; ++index){
    if( m_parameters.find(ParameterName[index]) == m_parameters.end() )
      return Error("No default value for parameter '"+ParameterName[index]+"'", StatusCode::FAILURE);
    m_source[index]="from options";
  }
  return StatusCode::SUCCESS;
}

//------
StatusCode ClusterCovarianceMatrixTool::getParamsFromDB(){

  unsigned int nareas = m_det->numberOfAreas();
  // overwrite m_parameters using DB value
  if( !m_useDB )return StatusCode::SUCCESS;
  m_source.clear();
  using namespace CaloCovariance; 
  ParameterMap parameters;
  for( unsigned int area = 0 ; area < nareas ; ++area){  // loop over calo area
    const LHCb::CaloCellID id(m_det->caloName(),area,0,0); // fake cell
    const std::vector<double>& params = m_dbAccessor->getParamVector(CaloCorrection::ClusterCovariance,id);
    if( params.size() > CaloCovariance::Last )
      Warning("Parameters vector exceeds the number of known parameters - only "
              +Gaudi::Utils::toString(Last)+" parameters will be applied",StatusCode::SUCCESS).ignore();
    for(unsigned int index = 0 ; index < CaloCovariance::Last ; ++index){
      if( index < params.size() ){
        parameters[ParameterName[index]].push_back( params[index] );
        m_source[index]="from Covariance DB";
      }else{
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug() << "Parameter '"<<ParameterName[index] << "' not found in DB - use default options value"<<endmsg;
        if( m_parameters.find(ParameterName[index]) == m_parameters.end() )
          return Error("No default value for parameter '"+ParameterName[ index ]+"'", StatusCode::FAILURE);
        parameters[ParameterName[index]].push_back( m_parameters[ParameterName[index]][area] );
        m_source[index]="from options";
      }      
    }
  }
  m_parameters=parameters;
  if( m_parameters.size() == 0)return StatusCode::FAILURE; // no parameters set
  return StatusCode::SUCCESS;
}

//-------
void ClusterCovarianceMatrixTool::setEstimatorParams(bool init){  

  // update DB parameters
  if( !init && !m_useDB)return; // estimator setting via options :  at initialization only
  if( !init && !m_dbAccessor-> hasConditionChanged())return; // estimator setting via DB : no need to update - condition has not changed
  if( m_useDB && getParamsFromDB().isFailure() ){
    Error("Failed updating the covariance parameters from DB",StatusCode::FAILURE).ignore(); // update DB parameters
    return; // failed to update parameters from DB
  }
  using namespace CaloCovariance;
  m_estimator.setStochastic      ( m_parameters[ParameterName[Stochastic]]      ) ;
  m_estimator.setGainError       ( m_parameters[ParameterName[GainError]]       ) ;
  m_estimator.setIncoherentNoise ( m_parameters[ParameterName[IncoherentNoise]] ) ;
  m_estimator.setCoherentNoise   ( m_parameters[ParameterName[CoherentNoise]]   ) ;
  m_estimator.setConstantE       ( m_parameters[ParameterName[ConstantE]]       ) ;
  m_estimator.setConstantX       ( m_parameters[ParameterName[ConstantX]]       ) ;
  m_estimator.setConstantY       ( m_parameters[ParameterName[ConstantY]]       ) ;  
  counter("Parameter update") += 1;
  //info() << "ESTIMATOR HAS BEEN UPDATED (" << m_conditionName << ") : init = " << init <<  endmsg;
}

//---------
StatusCode ClusterCovarianceMatrixTool::initialize (){
  StatusCode sc = GaudiTool::initialize ();
  if( sc.isFailure() ){ return Error("Could not initialize the base class!") ; }


  // register to incident service
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;

  // get detector
  m_det = getDet<DeCalorimeter>( m_detData ) ;

  // set DB accessor
  m_dbAccessor = tool<CaloCorrectionBase>("CaloCorrectionBase","DBAccessor",this);
  if( m_useDB && (m_conditionName == "" || m_dbAccessor->setConditionParams(m_conditionName,true).isFailure()) )
    return Error("Cannot access DB",StatusCode::FAILURE);

  // always set default parameters from options (will be updated by DB if requested)
  sc= getParamsFromOptions();

  // check the parameters consistency
  for(CaloCovariance::ParameterMap::const_iterator imap = m_parameters.begin() ; m_parameters.end() != imap ; ++imap){
    std::string name = imap->first;
    bool ok = false;
    for(unsigned int index = 0 ; index < CaloCovariance::Last ; ++index){
      if( CaloCovariance::ParameterName[index] == name ){ ok = true; break;}
    }
    if( !ok )return Error("Parameter type '"+name+"' is unknown",StatusCode::FAILURE);
  }

  // configure estimator (possibly from DB if requested)
  m_estimator.setDetector( m_det ) ;
  setEstimatorParams(true); // force initialization 
  info()      << " Has initialized with parameters: "              << endmsg 
              << " \t 'Detector'         = '" << m_detData << "'"  << endmsg 
    //            << " \t Estimator is          " << m_estimator       << endmsg;
              << " \t ==  Parameters for covariance estimation ==" << endmsg;
  using namespace CaloCovariance;
  for(unsigned int index = 0 ; index < CaloCovariance::Last ; ++index){
    info() << CaloCovariance::ParameterName[index] << " \t : " 
           << m_parameters[ParameterName[index]] 
           << " " << ParameterUnit[index]
           << "\t : "<< m_source[index]<<""<< endmsg;
  }
  return sc;
}
// ============================================================================
StatusCode ClusterCovarianceMatrixTool::finalize   (){  
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
  return GaudiTool::finalize ();
}

// ============================================================================
StatusCode 
ClusterCovarianceMatrixTool::operator() ( LHCb::CaloCluster* cluster ) const{
  /// check the argument 
  if( 0 == cluster                )return Error( "CaloCluster*   points to NULL!") ; 
  if( 0 == m_estimator.detector() )return Error( "DeCalorimeter* points to NULL!") ; 
  /// apply the estimator 
  return m_estimator( cluster );
}
// ============================================================================

StatusCode ClusterCovarianceMatrixTool::process ( LHCb::CaloCluster* cluster ) const { 
  return (*this)( cluster ); 
}
// ============================================================================

