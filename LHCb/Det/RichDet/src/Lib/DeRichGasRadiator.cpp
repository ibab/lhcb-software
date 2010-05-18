//----------------------------------------------------------------------------
/** @file DeRichGasRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichGasRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichGasRadiator.cpp,v 1.19 2010-01-14 16:39:03 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-03-02
 */
//----------------------------------------------------------------------------

// Include files

// Gaudi
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "RichDet/DeRichGasRadiator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRichGasRadiator
//
// 2006-03-02 : Antonis Papanestis
//-----------------------------------------------------------------------------

const CLID CLID_DeRichGasRadiator = 12042;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichGasRadiator::DeRichGasRadiator(const std::string & name) :
  DeRichSingleSolidRadiator(name),
  m_temperatureCond      (),
  m_pressureCond         (),
  m_gasParametersCond    (),
  m_hltGasParametersCond (),
  m_scaleFactorCond      ()
{}

//=============================================================================
// Destructor
//=============================================================================
DeRichGasRadiator::~DeRichGasRadiator() {}

//=========================================================================
// Retrieve Pointer to class defininition structure
//=========================================================================
const CLID& DeRichGasRadiator::classID() { return CLID_DeRichGasRadiator; }

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichGasRadiator::initialize ( )
{

  MsgStream msg = msgStream( "DeRichGasRadiator" );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  StatusCode sc = DeRichSingleSolidRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  bool foundGasConditions( false );

  // in HLT mode the "normal gas conditions are not available
  bool HltMode( true );

  // setup gas conditions
  if ( hasCondition( "GasParameters" ) && condition("GasParameters") )
  {
    m_gasParametersCond = condition( "GasParameters" );
    msg << MSG::DEBUG << "Using condition <GasParameters>" << endmsg;
    updMgrSvc()->registerCondition( this, m_gasParametersCond.path(),
                                    &DeRichGasRadiator::updateProperties );
    foundGasConditions = true;
    HltMode = false;
  }
  else // old parameters setup
  {
    sc = setupOldGasConditions();
    if ( sc.isFailure() )
    {
      msg <<  MSG::DEBUG << "Did not find non-HLT gas conditions" << endmsg;
    }
    else
    {
      msg << MSG::DEBUG << "Using conditions <GasTemperature> and <GasPressure>" << endmsg;
      foundGasConditions = true;
      HltMode = false;
    }
  }

  // hlt condition
  if ( hasCondition( "HltGasParameters" ) && condition( "HltGasParameters" ) )
  {
    m_hltGasParametersCond = condition( "HltGasParameters" );
    msg << MSG::DEBUG << "Found condition <HltGasParameters>" << endmsg;
    foundGasConditions = true;
  }
  else  // use offline conditions for hlt
  {
    m_hltGasParametersCond = m_gasParametersCond;
    msg << MSG::DEBUG << "Using offline gas condition for HLT" << endmsg;
  }

  if ( !foundGasConditions )
  {
    msg << MSG::ERROR << "Could not find any gas conditions" << endmsg;
    return StatusCode::FAILURE;
  }

  // scale factor
  if ( hasCondition( "RefractivityScaleFactor" ) && !HltMode )
  {
    m_scaleFactorCond = condition( "RefractivityScaleFactor" );
    updMgrSvc()->registerCondition( this, m_scaleFactorCond.path(),
                                    &DeRichGasRadiator::updateProperties );
  }

  if ( !HltMode )
  {
    sc = updMgrSvc()->update(this);
    if ( sc.isFailure() )
    {
      msg << MSG::ERROR << "First UMS update failed" << endmsg;
      return sc;
    }
  }

  msg << MSG::DEBUG << "Initialisation Complete" << endmsg;
  m_firstUpdate = false;

  // return
  return StatusCode::SUCCESS;
}

//=========================================================================
// updateRefIndex
//=========================================================================
StatusCode DeRichGasRadiator::updateProperties ( )
{

  // load parameters
  // get temperature and pressure
  const double photonEnergyLowLimit     = param<double>("PhotonMinimumEnergy");
  const double photonEnergyHighLimit    = param<double>("PhotonMaximumEnergy");
  const double ckvPhotonEnergyLowLimit  = param<double>("PhotonCkvMinimumEnergy");
  const double ckvPhotonEnergyHighLimit = param<double>("PhotonCkvMaximumEnergy");
  const unsigned int photonEnergyNumBins  = param<int>("PhotonEnergyNumBins");
  const unsigned int ckvPhotonEnergyNumBins = param<int>("CkvPhotonEnergyNumBins");

  if ( !m_firstUpdate && m_gasParametersCond )
  {
    const double curPressure = m_gasParametersCond->param<double>("Pressure") * 0.001*Gaudi::Units::bar; //convert to bar
    const double curTemp     = m_gasParametersCond->param<double>("Temperature");
    const double scaleFactor = ( !m_scaleFactorCond ? 1.0 :
                                 m_scaleFactorCond->param<double>("CurrentScaleFactor") );
    info() << "Refractive index update triggered : Pressure = " << curPressure/Gaudi::Units::bar
           << " bar : Temperature = " << curTemp << " K"
           << " : (n-1) Scale = " << scaleFactor
           << endmsg;
  }

  if ( (photonEnergyHighLimit < ckvPhotonEnergyHighLimit ) ||
       (ckvPhotonEnergyLowLimit < photonEnergyLowLimit ) )
  {
    error() << "Inadmissible photon energy limits "
            << photonEnergyHighLimit << " " << ckvPhotonEnergyHighLimit
            << ckvPhotonEnergyLowLimit << " " << photonEnergyLowLimit << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double> photonMomentumVect;
  StatusCode sc = prepareMomentumVector( photonMomentumVect, photonEnergyLowLimit,
                                         photonEnergyHighLimit, photonEnergyNumBins );
  if ( !sc ) return sc;

  // calculate the refractive index and update Tabulated property
  sc = calcSellmeirRefIndex( photonMomentumVect, m_refIndexTabProp, m_gasParametersCond );
  if ( !sc ) return sc;

  std::vector<double> ckvPhotonMomentumVect;
  sc = prepareMomentumVector( ckvPhotonMomentumVect, ckvPhotonEnergyLowLimit,
                              ckvPhotonEnergyHighLimit, ckvPhotonEnergyNumBins );
  if ( !sc ) return sc;

  sc = calcSellmeirRefIndex( ckvPhotonMomentumVect, m_chkvRefIndexTabProp, m_gasParametersCond );
  if ( !sc ) return sc;

  // Hack - Update interpolators in base class after first update
  sc = initTabPropInterpolators();

  return sc;
}

//=========================================================================
//  calcSellmeirRefIndex
//=========================================================================

StatusCode
DeRichGasRadiator::calcSellmeirRefIndex ( const std::vector<double>& momVect,
                                          const TabulatedProperty* tabProp,
                                          const SmartRef<Condition>& gasParamCond ) const
{
  // test the tab property pointer
  if ( !tabProp )
  {
    error() << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  // get temperature and pressure
  double curPressure(0), curTemp(0);
  if ( gasParamCond )
  {
    curPressure = gasParamCond->param<double>("Pressure") * 0.001*Gaudi::Units::bar; //convert to bar
    curTemp = gasParamCond->param<double>("Temperature") * Gaudi::Units::kelvin;
  }
  else // use the old conditions
  {
    curPressure = m_pressureCond->param<double>("CurrentPressure");
    curTemp     = m_temperatureCond->param<double>("CurrentTemperature");
  }

  // (n-1) scale factor
  const double scaleFactor = ( !m_scaleFactorCond ? 1.0 :
                               m_scaleFactorCond->param<double>("CurrentScaleFactor") );

  // reset table
  TabulatedProperty* modTabProp = const_cast<TabulatedProperty*>( tabProp );
  TabulatedProperty::Table& aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  // Get parameters
  const double SellE1 = param<double>("SellE1Param");
  const double SellE2 = param<double>("SellE2Param");
  const double SellF1 = param<double>("SellF1Param");
  const double SellF2 = param<double>("SellF2Param");
  const double SellLorGasFac = param<double>("SellmeirLorenzFact");
  const double RhoEffectiveSellDefault = param<double>("RhoEffectiveSellParam");
  const double GasMolWeight   = param<double>("GasMolWeightParam");
  const bool isC3F8Medium = ( material()->name().find("C3F08") != std::string::npos ) ? true : false;


  double AParam =0.0;
  double AMultParam=0.0;
  double EphyZSq =0.0;
  double MomConvWave=0.0;
  if(isC3F8Medium ) {
    AParam = param<double>("C3F8SellMeirAFactor");
    AMultParam = param<double>("C3F8SellMeirAMultiplicationFactor");
    MomConvWave = param<double> ("PhotonMomentumWaveLengthConvFact"  );
    const double aWaveZero= param<double> ("C3F8SellMeirLambdaZeroFactor" );


    if(aWaveZero != 0.0) {
      EphyZSq =  ( MomConvWave / aWaveZero ) * ( MomConvWave / aWaveZero );
    }
  }

  double GasRhoCur( 0.0 );

  if ( material()->name().find("C4F10") != std::string::npos ) {
    const double RefTemperature = param<double>("C4F10ReferenceTemp");
    GasRhoCur = RhoEffectiveSellDefault*(curPressure/Gaudi::Units::STP_Pressure)*
      ( RefTemperature/curTemp );
  }else if ( isC3F8Medium  ) {
    GasRhoCur = (curPressure/Gaudi::Units::STP_Pressure)*
      (Gaudi::Units::STP_Temperature/curTemp);

  }else {


    GasRhoCur = RhoEffectiveSellDefault*(curPressure/Gaudi::Units::STP_Pressure)*
      (Gaudi::Units::STP_Temperature/curTemp);
  }

  // calculate ref index
  for ( unsigned int ibin = 0; ibin<momVect.size(); ++ibin )
  {
    const double epho = momVect[ibin]/Gaudi::Units::eV;
    double nMinus1=0.0;

    if( isC3F8Medium ) {
      nMinus1 =
        scaleFactor * (AParam*AMultParam*MomConvWave*MomConvWave*GasRhoCur)/(EphyZSq-(epho*epho));
    }else {
      const double pfe  = (SellF1/( (SellE1* SellE1) - (epho * epho) ) )+
        (SellF2/( (SellE2*SellE2) - (epho * epho) ));
      const double cpfe = SellLorGasFac * (GasRhoCur / GasMolWeight ) * pfe;
      nMinus1 = scaleFactor * (std::sqrt((1.0+2*cpfe)/(1.0-cpfe)) - 1.0);
    }


    const double curRindex = 1.0+nMinus1;

    aTable.push_back( TabulatedProperty::Entry(epho*Gaudi::Units::eV,curRindex));
  }

  debug() << "Table in TabulatedProperty " << tabProp->name()
          << " updated with " << momVect.size() << " bins" << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  generateHltRefIndex
//=========================================================================
const Rich::TabulatedProperty1D* DeRichGasRadiator::generateHltRefIndex() const
{
  m_hltRefIndexTabProp = new TabulatedProperty("HltRefIndexTabProperty");

  DeRichGasRadiator* nonConstSelf = const_cast<DeRichGasRadiator*>(this);

  // temperature
  if ( m_hltGasParametersCond !=  m_gasParametersCond )
  {

    updMgrSvc()->registerCondition(nonConstSelf, m_hltGasParametersCond.path(),
                                   &DeRichGasRadiator::updateHltProperties );
    StatusCode sc = updMgrSvc()->update(nonConstSelf);
    if ( sc.isFailure() )
    {
      error() << "First UMS update failed for HLT properties" << endmsg;
      return 0;
    }
  }
  else
    nonConstSelf->updateHltProperties();


  return m_hltRefIndex;
}

//=========================================================================
//  updateHltProperties
//=========================================================================
StatusCode DeRichGasRadiator::updateHltProperties ( )
{

  // load parameters
  const double photonEnergyLowLimit     = param<double>("PhotonMinimumEnergy");
  const double photonEnergyHighLimit    = param<double>("PhotonMaximumEnergy");
  const unsigned int photonEnergyNumBins  = param<int>("PhotonEnergyNumBins");

  if ( photonEnergyHighLimit < photonEnergyLowLimit )
  {
    error() << "Inadmissible photon energy limits "
            << photonEnergyHighLimit << " " << photonEnergyLowLimit << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double> photonMomentumVect;
  StatusCode sc = prepareMomentumVector( photonMomentumVect, photonEnergyLowLimit,
                                         photonEnergyHighLimit, photonEnergyNumBins );
  if ( !sc ) return sc;

  // calculate the refractive index and update Tabulated property
  sc = calcSellmeirRefIndex( photonMomentumVect, m_hltRefIndexTabProp, m_hltGasParametersCond );
  if ( !sc ) return sc;

  if ( !m_hltRefIndex )
  { m_hltRefIndex = new Rich::TabulatedProperty1D( m_hltRefIndexTabProp ); }
  else
  { m_hltRefIndex->initInterpolator( m_hltRefIndexTabProp ); }
  if ( !m_hltRefIndex->valid() )
  {
    error() << "Invalid RINDEX Rich::TabulatedProperty1D for "
            << m_hltRefIndexTabProp->name() << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

//=========================================================================
//  setupOldGasConditions
//=========================================================================
StatusCode DeRichGasRadiator::setupOldGasConditions ( ) {

  MsgStream msg = msgStream( "DeRichGasRadiator" );

  // configure refractive index updates
  // temperature
  if ( hasCondition( "GasTemperature" ) && condition("GasTemperature") )
  {
    m_temperatureCond = condition( "GasTemperature" );
    updMgrSvc()->registerCondition(this, m_temperatureCond.path(),
                                   &DeRichGasRadiator::updateProperties );
  }
  else
  {
    m_temperatureCond = 0;
    msg << MSG::DEBUG << "Cannot load Condition GasTemperature" << endmsg;
    return StatusCode::FAILURE;
  }

  // pressure
  if ( hasCondition( "GasPressure" ) && condition("GasPressure") )
  {
    m_pressureCond = condition( "GasPressure" );
    updMgrSvc()->registerCondition( this, m_pressureCond.path(),
                                    &DeRichGasRadiator::updateProperties );
  }
  else
  {
    m_pressureCond = 0;
    msg << MSG::DEBUG << "Cannot load Condition GasPressure" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

