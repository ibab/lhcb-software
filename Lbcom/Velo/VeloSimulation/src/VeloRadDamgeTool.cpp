// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from MCEvent
#include "Event/MCHit.h"

// VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "VeloRadDamgeTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloRadDamageTool
//
// 2011-03-31 : David Hutchcroft
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( VeloRadDamageTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloRadDamageTool::VeloRadDamageTool(const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : GaudiTool ( type, name , parent ),
    m_veloDet(0),
    m_radConditionMap()
{
  declareInterface<IRadDamageTool>(this);
}

// Destructor
VeloRadDamageTool::~VeloRadDamageTool(){
  // delete the response splines (if any)
  std::map<unsigned int,RadParam>::iterator iMap = m_radParamMap.begin();
  for( ; iMap !=  m_radParamMap.end() ; ++iMap ){
    if(iMap->second.responseSpline != 0){
      delete iMap->second.responseSpline;
    }
  }
}

StatusCode VeloRadDamageTool::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_veloDet = getDet<DeVelo>( DeVeloLocation::Default );

  if( !existDet<DataObject>(detSvc(),"Conditions/HardwareProperties/Velo/RadiationDamage") ){
    Warning("VELO radiation damage parameters not in conditions DB",
            StatusCode::SUCCESS).ignore();
    std::vector<DeVeloSensor*>::const_iterator iSens = m_veloDet->sensorsBegin();
    for( ; iSens != m_veloDet->sensorsEnd(); ++iSens ){
      RadParam param;
      param.useConst = true;
      // fully set param so copy and finish this sensor
      m_radParamMap[(*iSens)->sensorNumber()] = param;
      continue; // next sensor
    }
    return StatusCode::SUCCESS;
  }

  std::vector<DeVeloSensor*>::const_iterator iSens = m_veloDet->sensorsBegin();
  for( ; iSens != m_veloDet->sensorsEnd() ; ++iSens ){

    std::string conditionName =
      format("Conditions/HardwareProperties/Velo/RadiationDamage/Sensor%03i-RadiationParam",(*iSens)->sensorNumber());

    if( !existDet<Condition>(detSvc(),conditionName) ){
      return Warning("Condition directory exists but missing condition " +
                     conditionName,StatusCode::FAILURE);
    }
    m_radConditionMap[(*iSens)->sensorNumber()] = get<Condition>(detSvc(),conditionName);
    registerCondition(conditionName,
                      &VeloRadDamageTool::i_loadCondtion);
    if(msgLevel( MSG::VERBOSE ))
      verbose() << "Registered " << conditionName
                << " into m_radConditionMap[" << (*iSens)->sensorNumber()
                << "] = " << m_radConditionMap[(*iSens)->sensorNumber()]
                << endmsg;
  }
  sc = i_loadCondtion();
  sc = runUpdate();
  if(!sc) return sc;

  return StatusCode::SUCCESS;
}

StatusCode VeloRadDamageTool::i_loadCondtion(){

  std::vector<DeVeloSensor*>::const_iterator iSens = m_veloDet->sensorsBegin();
  for( ; iSens != m_veloDet->sensorsEnd(); ++iSens ){

    unsigned int sNum = (*iSens)->sensorNumber();
    
    // param is the current parameter (created if not existing already)
    RadParam &param = m_radParamMap[sNum];

    // delete the old spline (if it exists)
    if( param.responseSpline != 0 ) delete param.responseSpline;
    
    param.useConst = m_radConditionMap[sNum]->param<int>("UseConst");
    if( param.useConst ) {
      param.constantFrac =
        m_radConditionMap[sNum]->param<double>("ConstFrac");
      // fully set param so copy and finish this sensor
      m_radParamMap[sNum] = param;
      continue; // next sensor
    }
    // so need spline
    std::vector<double> radii =
      m_radConditionMap[sNum]->param<std::vector<double> >("Radii");
    std::vector<double> chargeFrac =
      m_radConditionMap[sNum]->param<std::vector<double> >("ChargeFrac");

    if(radii.size() == 0 ||
       ( radii.size() != chargeFrac.size() ) )
      return Error(format("radii and chargeFrac not consistent for sensor %i",sNum),
                   StatusCode::FAILURE);

    // outside range make a constant at the extremal value
    param.rMin = radii.front();
    param.rMax = radii.back();
    param.responseMin = chargeFrac.front();
    param.responseMax = chargeFrac.back();

    param.responseSpline =
      new GaudiMath::SimpleSpline( radii, chargeFrac,
                                   GaudiMath::Interpolation::Linear );

    // if debug/verbose dump parameters to the screen
    if(msgLevel( MSG::DEBUG )){
      debug() << "For sensor " << sNum << endmsg;
      if( param.useConst ){
        debug() << "Constant charge fraction " << param.constantFrac << endmsg;
      }else{
        debug() << "Spline parameters: pointer" << param.responseSpline << endmsg;
        debug() << "radius      ChargeFraction" <<endmsg;
        for ( unsigned int i = 0 ; i < radii.size() ; ++i ){
          debug() << format("%5.2f   %5.2f",
                            radii[i],chargeFrac[i])
                  << endmsg;
        }
      }
      if(msgLevel( MSG::VERBOSE )) dumpResponse(*iSens);
    }
  }

  return StatusCode::SUCCESS;
}

void VeloRadDamageTool::dumpResponse(const DeVeloSensor *sens){
    verbose() << "response in 1mm steps from 0 to 55mm from function "
              <<endmsg;
    verbose() << "radius chargeFrac" <<endmsg;
    LHCb::MCHit hit;
    hit.setDisplacement(Gaudi::XYZVector(0.,0.,0.)); // explicitly set
    hit.setSensDetID(sens->sensorNumber()); // set the sensor
    for ( double r = 0 ; r < 55.5 ; r+=1.0 ){
      Gaudi::XYZPoint local(r,0.,0.); // want r in local frame so ...
      hit.setEntry(sens->localToGlobal(local));
      verbose() << format("%5.2f   %5.2f",r, chargeFrac( hit ) )
                << endmsg;
    }
}

double VeloRadDamageTool::chargeFrac(const LHCb::MCHit & hit) const {
  // "find" the map element as operator[] is non-const
  RadParam const &param = m_radParamMap.find(hit.sensDetID())->second;
  if(param.useConst) return param.constantFrac; // in case spline not required
  const DeVeloSensor* sens=m_veloDet->sensor(hit.sensDetID());
  double localR = (sens->globalToLocal(hit.midPoint())).Rho();
  if( localR < param.rMin ) return param.responseMin; // less than spline range
  if( localR > param.rMax ) return param.responseMax; // more than spline range
  
  return param.responseSpline->eval(localR); // actually use the spline
}
