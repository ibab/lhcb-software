
//----------------------------------------------------------------------------
/** @file DeRichAerogelRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichAerogelRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichAerogelRadiator.cpp,v 1.10 2008-08-18 18:30:39 jonrob Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-03-02
 */
//----------------------------------------------------------------------------

// Include files

#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "RichDet/DeRich.h"

// local
#include "RichDet/DeRichAerogelRadiator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRichAerogelRadiator
//
// 2006-03-02 : Antonis Papanestis
//-----------------------------------------------------------------------------

const CLID CLID_DeRichAerogelRadiator = 12043;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichAerogelRadiator::DeRichAerogelRadiator() : DeRichSingleSolidRadiator() {}

//=============================================================================
// Destructor
//=============================================================================
DeRichAerogelRadiator::~DeRichAerogelRadiator() {}

//=========================================================================
// Retrieve Pointer to class defininition structure
//=========================================================================
const CLID& DeRichAerogelRadiator::classID() { return CLID_DeRichAerogelRadiator; }

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichAerogelRadiator::initialize ( )
{

  MsgStream msg( msgSvc(), "DeRichAerogelRadiator" );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  StatusCode sc = DeRichSingleSolidRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  // extract tile number from detector element name
  const std::string::size_type pos = name().find(':');
  if ( std::string::npos == pos )
  {
    msg << MSG::FATAL << "An Aerogel tile without a number!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_tileNumber = atoi( name().substr(pos+1).c_str() );

  m_photMomWaveConv = 1243.125*Gaudi::Units::nanometer*Gaudi::Units::eV;

  SmartDataPtr<DetectorElement> deRich1( dataSvc(), DeRichLocations::Rich1 );
  m_deRich1 = deRich1;

  // configure refractive index updates

  // aerogel parameters from cond DB
  m_AerogelCond = condition( "AerogelParameters" );
  if ( m_AerogelCond ) {
    updMgrSvc()->registerCondition( this,
                                    m_AerogelCond.path(),
                                    &DeRichAerogelRadiator::updateProperties ); }
  else
  { msg << MSG::WARNING << "Cannot load Condition AerogelParameters" << endmsg; }


  sc = updMgrSvc()->update(this);
  if ( sc.isFailure() )
  {
    msg << MSG::ERROR << "First UMS update failed" << endreq;
    return sc;
  }

  msg << MSG::DEBUG << "Initialisation Complete" << endreq;
  m_firstUpdate = false;

  // return
  return sc;
}

//=========================================================================
// updateRefIndex
//=========================================================================
StatusCode DeRichAerogelRadiator::updateProperties ( )
{
  MsgStream msg( msgSvc(), myName() );
  if ( !m_firstUpdate )
    msg << MSG::INFO << "Refractive index update triggered" << endreq;

  // load various parameters
  const double photonEnergyLowLimit     = m_deRich1->param<double>("PhotonMinimumEnergyAerogel");
  const double photonEnergyHighLimit    = m_deRich1->param<double>("PhotonMaximumEnergyAerogel");
  const double ckvPhotonEnergyLowLimit  = m_deRich1->param<double>("PhotonCkvMinimumEnergyAerogel");
  const double ckvPhotonEnergyHighLimit = m_deRich1->param<double>("PhotonCkvMaximumEnergyAerogel");
  const unsigned int photonEnergyNumBins  = m_deRich1->param<int>("PhotonEnergyNumBins");
  const unsigned int ckvPhotonEnergyNumBins = m_deRich1->param<int>("CkvPhotonEnergyNumBins");

  if ( photonEnergyHighLimit    <  ckvPhotonEnergyHighLimit ||
       ckvPhotonEnergyLowLimit  <  photonEnergyLowLimit  ) {
    msg << MSG::ERROR << "Inadmissible photon energy limits "
        << photonEnergyLowLimit << " " << photonEnergyHighLimit
        << "  " << ckvPhotonEnergyLowLimit << " "
        << ckvPhotonEnergyHighLimit << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double> photonMomentumVect;
  StatusCode sc = prepareMomentumVector( photonMomentumVect, photonEnergyLowLimit,
                                         photonEnergyHighLimit, photonEnergyNumBins );
  if ( !sc ) return sc;

  // calculate the refractive index and update Tabulated property
  sc = calcSellmeirRefIndex( photonMomentumVect, m_refIndexTabProp );
  if ( !sc ) return sc;

  // calculate Rayleigh scattering and update Tabulated property
  sc = calcRayleigh( photonMomentumVect, m_rayleighTabProp );
  if ( !sc ) return sc;

  // calculate Absorption and update Tabulated property
  sc = calcAbsorption( photonMomentumVect, m_absorptionTabProp );
  if ( !sc ) return sc;

  std::vector<double> ckvPhotonMomentumVect;
  sc = prepareMomentumVector( ckvPhotonMomentumVect, ckvPhotonEnergyLowLimit,
                              ckvPhotonEnergyHighLimit, ckvPhotonEnergyNumBins );
  if ( !sc ) return sc;

  // calculate the refractive index CKVRNDX and update Tabulated property
  sc = calcSellmeirRefIndex( ckvPhotonMomentumVect, m_chkvRefIndexTabProp );
  if ( !sc ) return sc;

  // Hack - Update interpolators in base class after first update
  sc = initTabPropInterpolators();

  return sc;
}

//=========================================================================
//  calcSellmeirRefIndex
//=========================================================================
StatusCode DeRichAerogelRadiator::
calcSellmeirRefIndex (const std::vector<double>& momVect,
                      const TabulatedProperty* tabProp )
{
  MsgStream msg( msgSvc(), myName() );

  // test the tab property pointer
  if ( !tabProp )
  {
    msg << MSG::ERROR << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  // reset table
  TabulatedProperty* modTabProp = const_cast<TabulatedProperty*>( tabProp );
  TabulatedProperty::Table& aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  const double fixedLambdaValue = m_deRich1->param<double>("FixedLambdaValueForAerogel");

  const double SellE1 = m_AerogelCond->param<double>("CurrentAerogel_SellE1");
  const double SellE2 = m_AerogelCond->param<double>("CurrentAerogel_SellE2");
  const double SellF1 = m_AerogelCond->param<double>("CurrentAerogel_SellF1");
  const double SellF2 = m_AerogelCond->param<double>("CurrentAerogel_SellF2");
  const double nAtFixedL = m_AerogelCond->param<double>("CurrentAerogel_nAtFixedLambda");

  //calculate scaling factor to match measured n(lambda)
  const double ephot = m_photMomWaveConv/Gaudi::Units::eV / fixedLambdaValue;
  const double term1 = SellF1/(SellE1*SellE1-ephot*ephot);
  const double term2 = SellF2/(SellE2*SellE2-ephot*ephot);
  const double rindex_square_minusone = term1 + term2;
  const double scalingfactor = (nAtFixedL*nAtFixedL-1.0)/rindex_square_minusone;

  for ( unsigned int ibin = 0; ibin < momVect.size(); ++ibin )
  {
    const double epho = momVect[ibin]/Gaudi::Units::eV;
    const double pfe  =
      SellF1 / ( SellE1 * SellE1 - epho * epho ) +
      SellF2 / ( SellE2 * SellE2 - epho * epho );
    const double curRindex = sqrt( 1.0 + pfe * scalingfactor );
    aTable.push_back( TabulatedProperty::Entry( epho*Gaudi::Units::eV, curRindex ) );
  }

  msg << MSG::DEBUG << "Table in TabulatedProperty " << tabProp->name()
      << " updated with " << momVect.size() << " bins" << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
// calculate Rayleigh scattering
//=========================================================================
StatusCode DeRichAerogelRadiator::
calcRayleigh (const std::vector<double>& momVect,
              const TabulatedProperty* tabProp )
{
  MsgStream msg( msgSvc(), myName() );

  // test the tab property pointer
  if ( !tabProp )
  {
    msg << MSG::ERROR << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  // get clarity
  const double clarity = m_AerogelCond->param<double>("CurrentAerogel_Clarity");

  // reset table
  TabulatedProperty* modTabProp = const_cast<TabulatedProperty*>( tabProp );
  TabulatedProperty::Table& aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  for ( unsigned int ibin = 0; ibin < momVect.size(); ++ibin )
  {
    const double epho       = momVect[ibin]/Gaudi::Units::eV;
    const double wAgel      = (m_photMomWaveConv/1000./Gaudi::Units::eV/Gaudi::Units::nanometer) /epho;
    const double pathlength = wAgel*wAgel*wAgel*wAgel/clarity*10;
    aTable.push_back( TabulatedProperty::Entry( epho*Gaudi::Units::eV, pathlength ) );
  }

  msg << MSG::DEBUG << "Table in TabulatedProperty " << tabProp->name()
      << " updated with " << momVect.size() << " bins" << endmsg;

  return StatusCode::SUCCESS;

}

//=========================================================================
// calculate Absorption
//=========================================================================
StatusCode DeRichAerogelRadiator::
calcAbsorption (const std::vector<double>& momVect,
                const TabulatedProperty* tabProp )
{
  MsgStream msg( msgSvc(), myName() );

  // test the tab property pointer
  if ( !tabProp ) {
    msg << MSG::ERROR << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  const double thickness = m_deRich1->param<double>("AerogelThickness");

  // get A constant
  const double constA = m_AerogelCond->param<double>("CurrentAerogel_Aconst");

  // reset table
  TabulatedProperty* modTabProp = const_cast<TabulatedProperty*>( tabProp );
  TabulatedProperty::Table& aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  const double pathlength = -thickness/std::log( constA );

  for ( unsigned int ibin = 0; ibin < momVect.size(); ++ibin ){
    const double epho = momVect[ibin]/Gaudi::Units::eV;
    aTable.push_back( TabulatedProperty::Entry( epho*Gaudi::Units::eV, pathlength ) );
  }

  msg << MSG::DEBUG << "Table in TabulatedProperty " << tabProp->name()
      << " updated with " << momVect.size() << " bins" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
