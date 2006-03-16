// $Id: DeRichGasRadiator.cpp,v 1.4 2006-03-16 17:36:26 jonrob Exp $
// Include files

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "Kernel/PhysicalConstants.h"


// local
#include "RichDet/DeRichGasRadiator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRichGasRadiator
//
// 2006-03-02 : Antonis Papanestis
//-----------------------------------------------------------------------------

const CLID& CLID_DeRichGasRadiator = 12042;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichGasRadiator::DeRichGasRadiator(  ) 
  : DeRichSingleSolidRadiator()
{
}

//=============================================================================
// Destructor
//=============================================================================
DeRichGasRadiator::~DeRichGasRadiator() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichGasRadiator::classID()
{
  return CLID_DeRichGasRadiator;
}


//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichGasRadiator::initialize ( ) {

  MsgStream msg( msgSvc(), "DeRichGasRadiator" );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  StatusCode sc = DeRichSingleSolidRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  // update refractive index
  IUpdateManagerSvc* ums = updMgrSvc();

  // temperature
  m_temperatureCond = condition( "GasTemperature" );
  if ( m_temperatureCond )
    ums->registerCondition(this,m_temperatureCond.path(),
                           &DeRichGasRadiator::updateProperties );
  else
    msg << MSG::WARNING << "Cannot load Condition GasTemperature" << endmsg;

  // pressure
  m_pressureCond = condition( "GasPressure" );
  if ( m_pressureCond )
    ums->registerCondition(this,m_pressureCond.path(),
                           &DeRichGasRadiator::updateProperties );
  else
    msg << MSG::WARNING << "Cannot load Condition GasPressure" << endmsg;

  sc = ums->update(this);
  if ( sc.isFailure() ) 
  {
    msg << MSG::ERROR << "First UMS update failed" << endreq;
    return sc;
  }

  // Hack - Update interpolators in base class after first update
  sc = initTabPropInterpolators();

  msg << MSG::DEBUG << "Initialisation Complete" << endreq;
  return sc;

}

//=========================================================================
// updateRefIndex
//=========================================================================
StatusCode DeRichGasRadiator::updateProperties ( ) {

  // load parameters
  const double photonEnergyLowLimit     = param<double>("PhotonMinimumEnergy");
  const double photonEnergyHighLimit    = param<double>("PhotonMaximumEnergy");
  const double ckvPhotonEnergyLowLimit  = param<double>("PhotonCkvMinimumEnergy");
  const double ckvPhotonEnergyHighLimit = param<double>("PhotonCkvMaximumEnergy");
  const unsigned int photonEnergyNumBins  = param<int>("PhotonEnergyNumBins");
  const unsigned int ckvPhotonEnergyNumBins = param<int>("CkvPhotonEnergyNumBins");

  if ( (photonEnergyHighLimit < ckvPhotonEnergyHighLimit ) ||
       (ckvPhotonEnergyLowLimit < photonEnergyLowLimit ) )
  {
    MsgStream msg( msgSvc(), myName() );
    msg << MSG::ERROR << "Inadimissible photon energy limits "
        << photonEnergyHighLimit << " " << ckvPhotonEnergyHighLimit
        << ckvPhotonEnergyLowLimit << " " << photonEnergyLowLimit << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double> photonMomentumVect;
  StatusCode sc = prepareMomentumVector( photonMomentumVect, photonEnergyLowLimit,
                                         photonEnergyHighLimit, photonEnergyNumBins );
  if ( !sc ) return sc;

  // calculate the refractive index and update Tabulated property
  StatusCode scRefIndex = calcSellmeirRefIndex( photonMomentumVect, m_refIndexTabProp );
  if ( !scRefIndex ) return scRefIndex;

  std::vector<double> ckvPhotonMomentumVect;
  sc = prepareMomentumVector( ckvPhotonMomentumVect, ckvPhotonEnergyLowLimit,
                              ckvPhotonEnergyHighLimit, ckvPhotonEnergyNumBins );
  if ( !sc ) return sc;

  scRefIndex = calcSellmeirRefIndex( ckvPhotonMomentumVect, m_chkvRefIndexTabProp );
  if ( !scRefIndex ) return scRefIndex;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  calcSellmeirRefIndex
//=========================================================================
StatusCode DeRichGasRadiator::calcSellmeirRefIndex (const std::vector<double>& momVect,
                                                    const TabulatedProperty* tabProp ) {

  // test the tab property pointer
  if ( !tabProp ) {
    MsgStream msg( msgSvc(), myName() );
    msg << MSG::ERROR << "Problem with Tab Property" << endmsg;
    return StatusCode::FAILURE;
  }

  // get temperature and pressure
  const double curPressure = m_pressureCond->param<double>("CurrentPressure");
  const double curTemp = m_temperatureCond->param<double>("CurrentTemperature");

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
  double GasRhoCur( 0.0 );

  if ( Rich::Rich2Gas == radiatorID() )
    GasRhoCur = RhoEffectiveSellDefault*(curPressure/STP_Pressure)*
      (STP_Temperature/curTemp);
  else if ( Rich::Rich1Gas == radiatorID() ) {
    const double RefTemperature = param<double>("C4F10ReferenceTemp");
    GasRhoCur = RhoEffectiveSellDefault*(curPressure/STP_Pressure) *
      ( RefTemperature/curTemp );
  }

  // calculate ref index
  for ( unsigned int ibin = 0; ibin<momVect.size(); ++ibin )
  {
    const double epho = momVect[ibin]/eV;
    const double pfe  = (SellF1/( (SellE1* SellE1) - (epho * epho) ) )+
      (SellF2/( (SellE2*SellE2) -(epho * epho)));
    const double cpfe = SellLorGasFac * (GasRhoCur / GasMolWeight ) * pfe;
    const double curRindex = sqrt((1.0+2*cpfe)/(1.0-cpfe));
    aTable.push_back( TabulatedProperty::Entry(epho*eV,curRindex) );
  }

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::INFO << "Updated the table in Tab property " << tabProp->name()
      << " with " << momVect.size() << " bins" << endmsg;

  return StatusCode::SUCCESS;

}

//=============================================================================
