
//----------------------------------------------------------------------------
/** @file DeRichGasRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichGasRadiator
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
{ }

//=============================================================================
// Destructor
//=============================================================================
DeRichGasRadiator::~DeRichGasRadiator() { }

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
  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  StatusCode sc = DeRichSingleSolidRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  bool foundGasConditions( false );

  // in HLT mode the normal gas conditions are not available
  bool HltMode( true );

  // setup gas conditions
  if ( hasCondition("GasParameters") && condition("GasParameters") )
  {
    m_gasParametersCond = condition( "GasParameters" );
    if ( msgLevel(MSG::DEBUG,msg) )
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
      if ( msgLevel(MSG::DEBUG,msg) )
        msg <<  MSG::DEBUG << "Did not find non-HLT gas conditions" << endmsg;
    }
    else
    {
      if ( msgLevel(MSG::DEBUG,msg) )
        msg << MSG::DEBUG << "Using conditions <GasTemperature> and <GasPressure>" << endmsg;
      foundGasConditions = true;
      HltMode = false;
    }
  }

  // hlt condition
  if ( hasCondition( "HltGasParameters" ) && condition( "HltGasParameters" ) )
  {
    m_hltGasParametersCond = condition( "HltGasParameters" );
    if ( msgLevel(MSG::DEBUG,msg) )
      msg << MSG::DEBUG << "Found condition <HltGasParameters>" << endmsg;
    foundGasConditions = true;
  }
  else  // use offline conditions for hlt
  {
    m_hltGasParametersCond = m_gasParametersCond;
    if ( msgLevel(MSG::DEBUG,msg) )
      msg << MSG::DEBUG << "Using offline gas condition for HLT" << endmsg;
  }

  if ( !foundGasConditions )
  {
    msg << MSG::ERROR << "Could not find any gas conditions" << endmsg;
    return StatusCode::FAILURE;
  }

  info() << "HltMode = " << HltMode << endmsg;

  // scale factor
  if ( hasCondition( "RefractivityScaleFactor" ) && !HltMode )
  {
    m_scaleFactorCond = condition( "RefractivityScaleFactor" );
    updMgrSvc()->registerCondition( this, m_scaleFactorCond.path(),
                                    &DeRichGasRadiator::updateProperties );
  }

  // composition
  if ( hasCondition( "RadiatorComposition" ) && !HltMode )
  {
    updMgrSvc()->registerCondition( this, condition("RadiatorComposition").path(),
                                    &DeRichGasRadiator::updateProperties );
  }

  // If not in HLT mode, update
  if ( !HltMode )
  {
    sc = updMgrSvc()->update(this);
    if ( sc.isFailure() )
    {
      msg << MSG::ERROR << "First UMS update failed" << endmsg;
      return sc;
    }
  }

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialisation Complete" << endmsg;
 
  // return
  return StatusCode::SUCCESS;
}

//=========================================================================
// updateRefIndex
//=========================================================================
StatusCode DeRichGasRadiator::updateProperties ( )
{

  // load parameters
  const double         photonEnergyLowLimit = param<double>("PhotonMinimumEnergy");
  const double        photonEnergyHighLimit = param<double>("PhotonMaximumEnergy");
  const double      ckvPhotonEnergyLowLimit = param<double>("PhotonCkvMinimumEnergy");
  const double     ckvPhotonEnergyHighLimit = param<double>("PhotonCkvMaximumEnergy");
  const unsigned int    photonEnergyNumBins = param<int>   ("PhotonEnergyNumBins");
  const unsigned int ckvPhotonEnergyNumBins = param<int>   ("CkvPhotonEnergyNumBins");

  if ( m_gasParametersCond )
  {
    const double curPressure = ( m_gasParametersCond->param<double>("Pressure") 
                                 * 0.001*Gaudi::Units::bar ); // convert to bar
    const double curTemp     = m_gasParametersCond->param<double>("Temperature");
    const double scaleFactor = ( !m_scaleFactorCond ? 1.0 :
                                 m_scaleFactorCond->param<double>("CurrentScaleFactor") );
    if ( !m_scaleFactorCond ) info() << "NULL ScaleFactor condition. Using 1.0" << endmsg;
    //if ( msgLevel(MSG::DEBUG) )
    info() << "Refractive index update triggered : Pressure = " << curPressure/Gaudi::Units::bar
           << " bar : Temperature = " << curTemp << " K"
           << " : (n-1) Scale = " << scaleFactor
           << endmsg;
  }

  if ( ( photonEnergyHighLimit   < ckvPhotonEnergyHighLimit ) ||
       ( ckvPhotonEnergyLowLimit < photonEnergyLowLimit     )  )
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

  sc = calcSellmeirRefIndex( ckvPhotonMomentumVect,
                             m_chkvRefIndexTabProp,
                             m_gasParametersCond );
  if ( !sc ) return sc;

  // Update interpolators in base class
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
    curTemp     = gasParamCond->param<double>("Temperature") * Gaudi::Units::kelvin;
  }
  else // use the old conditions
  {
    if ( m_pressureCond )
    { curPressure = m_pressureCond->param<double>("CurrentPressure"); }
    else
    {
      error() << "Old CurrentPressure Condition missing !!" << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_temperatureCond )
    { curTemp     = m_temperatureCond->param<double>("CurrentTemperature"); }
    else
    {
      error() << "Old CurrentTemperature Condition missing !!" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // (n-1) scale factor
  const double scaleFactor = ( !m_scaleFactorCond ? 1.0 :
                               m_scaleFactorCond->param<double>("CurrentScaleFactor") );

  // reset table
  TabulatedProperty* modTabProp = const_cast<TabulatedProperty*>( tabProp );
  TabulatedProperty::Table& aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  // identify the gas radiator and the ref index formula type.
  // for now use the Single term formula for CF4 as default. 
  // This may be switched to classic by changing the following
  // flag in the future, if needed.

  double RefTemperature(293.0);
  unsigned int numOfGases( 1 );
  Condition* compCond( NULL );
  std::vector<std::string> gasNames;
  std::vector<double> gasFractions;

  if ( hasCondition( "RadiatorComposition" ) )
  {
    compCond = condition( "RadiatorComposition" );
    if ( !compCond )
    {
      error() << "Zero pointer to condition RadiatorComposition" << endmsg;
      return StatusCode::FAILURE;
    }

    numOfGases = compCond->param<int>("NumberOfGases");
    gasNames =  compCond->paramVect<std::string>("GasNames");
    gasFractions =  compCond->paramVect<double>("GasFractions");
    if ( numOfGases == 1 )
      gasFractions[0] = 1.0;
  }

  enum GasRadRef { C4F10_Classic, CF4_Classic, CF4_SingleTerm, C3F8_SingleTerm, mixture };

  GasRadRef curRadMedium = C4F10_Classic;  // initialize with one of the options.

  if ( material()->name().find("CF4") != std::string::npos )
  {
    // the following value is for backward compatibility with old versions of DB.
    bool CF4_RefIndex_Use_SingleTerm_Formula= exists("CF4ReferenceTempSellmeirAbjean") ? true : false;
    if ( CF4_RefIndex_Use_SingleTerm_Formula )
    {
      curRadMedium=CF4_SingleTerm ;
    }
    else
    {
      curRadMedium=CF4_Classic;
    }
  }
  else if(  material()->name().find("C3F08") != std::string::npos )
  {
    curRadMedium=C3F8_SingleTerm;
  }
  else if (material()->name().find("C4F10")!= std::string::npos )
  { // this check kept for safety.
    curRadMedium=C4F10_Classic;
    RefTemperature = param<double>("C4F10ReferenceTemp");
  }
  else if (material()->name().find("R1RadiatorGas")!= std::string::npos )
  {
    if ( numOfGases == 1 && gasNames[0] == "C4F10" )
    {
      // same as before
      curRadMedium=C4F10_Classic;
      RefTemperature = param<double>("C4F10ReferenceTemp");
    }
    else
    {
      curRadMedium = mixture;
    }
  }
  else if (material()->name().find("R2RadiatorGas")!= std::string::npos )
  {
    curRadMedium = mixture;
  }
  else
  {  // it is none of the known radiator gases.
    error()<<" Unknown radiator medium for refractive index determination in DeRichGasRadiator:  "
           <<  material()->name()<< endmsg;
    return StatusCode::FAILURE;
  }

  double SellE1(0.0),SellE2(0.0),SellF1(0.0),SellF2(0.0),SellLorGasFac(0.0);
  double RhoEffectiveSellDefault(0.0),GasMolWeight(0.0) ;
  double AParam(0.0),AMultParam(0.0),MomConvWave(0.0), aWaveZero(0.0),EphyZSq(0.0);
  std::vector<double> AParamVect, AMultParamVect, aWaveZeroVect;
  double GasRhoCur( 0.0 );

  // the classic sellmeir formulae here are all at reference temperature of
  // 293K whereas the single pole formulae are at the reference temperature of 273K.
  // the reference temp for classic is read from the db for now.

  if ( (C4F10_Classic == curRadMedium) || (CF4_Classic == curRadMedium)  )
  {

    SellE1 = param<double>("SellE1Param");
    SellE2 = param<double>("SellE2Param");
    SellF1 = param<double>("SellF1Param");
    SellF2 = param<double>("SellF2Param");
    SellLorGasFac = param<double>("SellmeirLorenzFact");
    RhoEffectiveSellDefault = param<double>("RhoEffectiveSellParam");
    GasMolWeight   = param<double>("GasMolWeightParam");
    GasRhoCur = RhoEffectiveSellDefault*(curPressure/Gaudi::Units::STP_Pressure)*
      ( RefTemperature/curTemp );

  }
  else if ( CF4_SingleTerm == curRadMedium )
  {

    AParam = param<double>("CF4SellMeirAFactor");
    AMultParam = param<double>("CF4SellMeirAMultiplicationFactor");
    aWaveZero=  param<double> ("CF4SellMeirLambdaZeroFactor");
    MomConvWave = param<double> ("PhotonMomentumWaveLengthConvFact"  );
    if(aWaveZero > 1.0)  // typical value when exists = 61.9
    {
      EphyZSq =  ( MomConvWave / aWaveZero ) * ( MomConvWave / aWaveZero );
    }
    GasRhoCur = (curPressure/Gaudi::Units::STP_Pressure)*
      (Gaudi::Units::STP_Temperature/curTemp);

  }
  else if ( C3F8_SingleTerm == curRadMedium )
  {

    AParam = param<double>("C3F8SellMeirAFactor");
    AMultParam = param<double>("C3F8SellMeirAMultiplicationFactor");
    aWaveZero= param<double> ("C3F8SellMeirLambdaZeroFactor" );
    MomConvWave = param<double> ("PhotonMomentumWaveLengthConvFact"  );
    if(aWaveZero > 1.0) //typical value when exists=64.4
    {
      EphyZSq =  ( MomConvWave / aWaveZero ) * ( MomConvWave / aWaveZero );
    }
    GasRhoCur = (curPressure/Gaudi::Units::STP_Pressure)*
      (Gaudi::Units::STP_Temperature/curTemp);
  }
  else if ( mixture == curRadMedium )
  {
    // same as above but need to get the papameters from the CondDB
    AParamVect = compCond->paramVect<double>("SellMeirAFactors");
    AMultParamVect = compCond->paramVect<double>("SellMeirAMultiplicationFactors");
    aWaveZeroVect=  compCond->paramVect<double> ("SellMeirLambdaZeroFactors");
    MomConvWave = param<double> ("PhotonMomentumWaveLengthConvFact"  );
    GasRhoCur = (curPressure/Gaudi::Units::STP_Pressure)*
      (Gaudi::Units::STP_Temperature/curTemp);
  }
  else
  {
    error() << "Unknown radiator medium for refractive index determination in DeRichGasRadiator"
            << endmsg;
    return StatusCode::FAILURE;
  }  // end if on ref index options

  // calculate ref index
  double nMinus1( 0.0 );
  for ( unsigned int ibin = 0; ibin<momVect.size(); ++ibin )
  {
    const double epho = momVect[ibin]/Gaudi::Units::eV;

    if ( (C4F10_Classic == curRadMedium) || (CF4_Classic == curRadMedium) )
    {

      const double pfe  = (SellF1/( (SellE1* SellE1) - (epho * epho) ) ) +
        (SellF2/( (SellE2*SellE2) - (epho * epho) ));
      const double cpfe = SellLorGasFac * (GasRhoCur / GasMolWeight ) * pfe;
      nMinus1 = scaleFactor * (std::sqrt((1.0+2*cpfe)/(1.0-cpfe)) - 1.0);

    }
    else if ( (CF4_SingleTerm == curRadMedium) || ( C3F8_SingleTerm == curRadMedium ) )
    {

      nMinus1 =
        scaleFactor * (AParam*AMultParam*MomConvWave*MomConvWave*GasRhoCur)/(EphyZSq-(epho*epho));

    }
    else if ( mixture == curRadMedium )
    {

      nMinus1 = 0.0;
      for (unsigned int gas=0; gas<numOfGases; ++gas)
      {

        if(aWaveZeroVect[gas] > 1.0)  // typical value when exists = 61.9
        {
          EphyZSq =  ( MomConvWave / aWaveZeroVect[gas] ) * ( MomConvWave / aWaveZeroVect[gas] );
        }
        nMinus1 += scaleFactor * gasFractions[gas] *
          (AParamVect[gas]*AMultParamVect[gas]*MomConvWave*MomConvWave*GasRhoCur)/(EphyZSq-(epho*epho));
      }

    }
    else
    {

      nMinus1=0.0;

    }  // end if on ref index options

    const double curRindex = 1.0+nMinus1;

    aTable.push_back( TabulatedProperty::Entry(epho*Gaudi::Units::eV,curRindex));

  } // end loop over bins of photon energy.

  if ( msgLevel(MSG::DEBUG) )
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
  if ( m_hltGasParametersCond != m_gasParametersCond )
  {

    updMgrSvc()->registerCondition(nonConstSelf, m_hltGasParametersCond.path(),
                                   &DeRichGasRadiator::updateHltProperties );
    const StatusCode sc = updMgrSvc()->update(nonConstSelf);
    if ( sc.isFailure() )
    {
      error() << "First UMS update failed for HLT properties" << endmsg;
      return 0;
    }
  }
  else
  {
    nonConstSelf->updateHltProperties();
  }

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
  const unsigned int photonEnergyNumBins = param<int>("PhotonEnergyNumBins");

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
    if ( msgLevel(MSG::DEBUG,msg) )
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
    if ( msgLevel(MSG::DEBUG,msg) )
      msg << MSG::DEBUG << "Cannot load Condition GasPressure" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

