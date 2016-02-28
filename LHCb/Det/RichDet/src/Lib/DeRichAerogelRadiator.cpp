
//----------------------------------------------------------------------------
/** @file DeRichAerogelRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichAerogelRadiator
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
DeRichAerogelRadiator::DeRichAerogelRadiator(const std::string & name)
  : DeRichSingleSolidRadiator ( name )
{ }

//=============================================================================
// Destructor
//=============================================================================
DeRichAerogelRadiator::~DeRichAerogelRadiator() { }

//=========================================================================
// Retrieve Pointer to class defininition structure
//=========================================================================
const CLID& DeRichAerogelRadiator::classID()
{
  return CLID_DeRichAerogelRadiator;
}

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichAerogelRadiator::initialize ( )
{

  MsgStream msg = msgStream( "DeRichAerogelRadiator" );

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialize " << myName() << endmsg;

  StatusCode sc = DeRichSingleSolidRadiator::initialize();
  if ( sc.isFailure() ) return sc;

  m_photMomWaveConv = 1243.125*Gaudi::Units::nanometer*Gaudi::Units::eV;
  // Check on full tile vs subtile
  const auto tilenamePos = name().find("AerogelT");
  const auto subtilenamePos = name().find("Rich1AerogelSubTileDe");

  if ( tilenamePos != std::string::npos )
  {
    // This is full tile, so extract tile number from detector element name

    m_subTile = false;
    const auto pos = name().find(':');
    if ( std::string::npos == pos ){
      fatal() << "An Aerogel full tile without a number!" << endmsg;
      return StatusCode::FAILURE;
    }
    m_tileNumber = atoi( name().substr(pos+1).c_str() );

  }
  else if ( subtilenamePos !=  std::string::npos )
  {
    // this is a subtile, so extract the subtile copy number and the
    // corresponding full tile number and the subtile number inside the
    // full tile. The copy number is a unique number for each subtile and is within the range 0->299.

    m_subTile = true;
    const auto colpos = name().find(':');
    if ( colpos != std::string::npos ) {
      m_subtilecopynumber= atoi( name().substr(colpos+1).c_str() );
      m_subtileNumInTile = atoi( name().substr(colpos-2,2).c_str() );
      m_tileNumber = atoi( name().substr(colpos-4,2).c_str() );
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "DeRichAerogelRadiator Tile subtileNum and subtileCopy number "
                  << m_tileNumber <<"   "
                  << m_subtileNumInTile <<"   "<<m_subtilecopynumber<<endmsg;
    }
    else 
    {
      fatal()<< "An Aerogel sub tile tile without a number!" << endmsg;
      return StatusCode::FAILURE;
    }

  }
  else
  {
    fatal() << "An Aerogel radiator det elem without corresponding full tile or sub tile !" << endmsg;
    return StatusCode::FAILURE;

  }


  // configure refractive index updates

  // aerogel parameters from cond DB
  if ( hasCondition( "AerogelParameters" ) )
  {
    m_AerogelCond = condition( "AerogelParameters" );
    updMgrSvc()->registerCondition( this,
                                    m_AerogelCond.path(),
                                    &DeRichAerogelRadiator::updateProperties );
  }
  else
  {
    m_AerogelCond = 0;
    warning() << "Cannot load Condition AerogelParameters" << endmsg;
  }

  sc = updMgrSvc()->update(this);
  if ( sc.isFailure() )
  {
    error() << "First UMS update failed" << endmsg;
    return sc;
  }

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialisation Complete " << myName() << endmsg;

  // return
  return sc;
}


//=========================================================================
// Access on demand the DeRich1 detector element
//=========================================================================
DetectorElement* DeRichAerogelRadiator::deRich1() const
{
  if ( !m_deRich1 )
  {
    SmartDataPtr<DetectorElement> deRich1( dataSvc(), DeRichLocations::Rich1 );
    m_deRich1 = deRich1;
    if ( !m_deRich1 )
    {
      throw GaudiException( "Failed to load DeRich1 detector element at " +
                            DeRichLocations::Rich1,
                            "DeRichAerogelRadiator::deRich1()",
                            StatusCode::FAILURE );
    }
  }
  return m_deRich1;
}

//=========================================================================
// updateRefIndex
//=========================================================================
StatusCode DeRichAerogelRadiator::updateProperties ( )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Refractive index update triggered" << endmsg;

  // load various parameters
  const auto photonEnergyLowLimit     = deRich1()->param<double>("PhotonMinimumEnergyAerogel");
  const auto photonEnergyHighLimit    = deRich1()->param<double>("PhotonMaximumEnergyAerogel");
  const auto ckvPhotonEnergyLowLimit  = deRich1()->param<double>("PhotonCkvMinimumEnergyAerogel");
  const auto ckvPhotonEnergyHighLimit = deRich1()->param<double>("PhotonCkvMaximumEnergyAerogel");
  const unsigned int photonEnergyNumBins    = deRich1()->param<int>("PhotonEnergyNumBins");
  const unsigned int ckvPhotonEnergyNumBins = deRich1()->param<int>("CkvPhotonEnergyNumBins");

  if ( photonEnergyHighLimit    <  ckvPhotonEnergyHighLimit ||
       ckvPhotonEnergyLowLimit  <  photonEnergyLowLimit  )
  {
    error() << "Inadmissible photon energy limits "
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

  // Update interpolators in base class
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

  // test the tab property pointer
  if ( !tabProp )
  {
    error() << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  // reset table
  auto * modTabProp = const_cast<TabulatedProperty*>( tabProp );
  auto & aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  const auto fixedLambdaValue = deRich1()->param<double>("FixedLambdaValueForAerogel");

  const auto SellE1 = m_AerogelCond->param<double>("CurrentAerogel_SellE1");
  const auto SellE2 = m_AerogelCond->param<double>("CurrentAerogel_SellE2");
  const auto SellF1 = m_AerogelCond->param<double>("CurrentAerogel_SellF1");
  const auto SellF2 = m_AerogelCond->param<double>("CurrentAerogel_SellF2");
  const auto nAtFixedL = m_AerogelCond->param<double>("CurrentAerogel_nAtFixedLambda");

  //calculate scaling factor to match measured n(lambda)
  const auto ephot = m_photMomWaveConv/Gaudi::Units::eV / fixedLambdaValue;
  const auto term1 = SellF1/(SellE1*SellE1-ephot*ephot);
  const auto term2 = SellF2/(SellE2*SellE2-ephot*ephot);
  const auto rindex_square_minusone = term1 + term2;
  const auto scalingfactor = (nAtFixedL*nAtFixedL-1.0)/rindex_square_minusone;

  for ( unsigned int ibin = 0; ibin < momVect.size(); ++ibin )
  {
    const auto epho = momVect[ibin]/Gaudi::Units::eV;
    const auto pfe  =
      SellF1 / ( SellE1 * SellE1 - epho * epho ) +
      SellF2 / ( SellE2 * SellE2 - epho * epho );
    const double curRindex = sqrt( 1.0 + pfe * scalingfactor );
    aTable.emplace_back( TabulatedProperty::Entry( epho*Gaudi::Units::eV, curRindex ) );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Table in TabulatedProperty " << tabProp->name()
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

  // test the tab property pointer
  if ( !tabProp )
  {
    error() << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  // get clarity
  const auto clarity = m_AerogelCond->param<double>("CurrentAerogel_Clarity");

  // reset table
  auto * modTabProp = const_cast<TabulatedProperty*>( tabProp );
  auto & aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  for ( unsigned int ibin = 0; ibin < momVect.size(); ++ibin )
  {
    const auto epho       = momVect[ibin]/Gaudi::Units::eV;
    const auto wAgel      = (m_photMomWaveConv/1000./Gaudi::Units::eV/Gaudi::Units::nanometer) /epho;
    const auto pathlength = wAgel*wAgel*wAgel*wAgel/clarity*10;
    aTable.emplace_back( TabulatedProperty::Entry( epho*Gaudi::Units::eV, pathlength ) );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Table in TabulatedProperty " << tabProp->name()
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

  // test the tab property pointer
  if ( !tabProp )
  {
    error() << "NULL TabulatedProperty pointer" << endmsg;
    return StatusCode::FAILURE;
  }

  const auto thickness = deRich1()->param<double>("AerogelThickness");

  // get A constant
  const auto constA = m_AerogelCond->param<double>("CurrentAerogel_Aconst");

  // reset table
  auto * modTabProp = const_cast<TabulatedProperty*>( tabProp );
  auto & aTable = modTabProp->table();
  aTable.clear();
  aTable.reserve( momVect.size() );

  const auto pathlength = -thickness/std::log( constA );

  for ( unsigned int ibin = 0; ibin < momVect.size(); ++ibin )
  {
    const auto epho = momVect[ibin]/Gaudi::Units::eV;
    aTable.emplace_back( TabulatedProperty::Entry( epho*Gaudi::Units::eV, pathlength ) );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Table in TabulatedProperty " << tabProp->name()
            << " updated with " << momVect.size() << " bins" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
