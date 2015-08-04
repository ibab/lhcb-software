
//---------------------------------------------------------------------------------
/** @file RichVaryGasPressureTemperatureAlg.cpp
 *
 * Implementation file for class : Rich::VaryGasPressureTemperatureAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2008-10-10
 */
//---------------------------------------------------------------------------------

// local
#include "RichVaryGasPressureTemperatureAlg.h"

//-----------------------------------------------------------------------------

using namespace Rich;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VaryGasPressureTemperatureAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VaryGasPressureTemperatureAlg::
VaryGasPressureTemperatureAlg( const std::string& name,
                               ISvcLocator* pSvcLocator )
  : Rich::AlgBase    ( name , pSvcLocator ),
    m_firstEvent     ( Rich::NRadiatorTypes, true ),
    m_radiators      ( Rich::NRadiatorTypes, (DeRichRadiator *)(NULL) ),
    m_nomPressure    ( Rich::NRadiatorTypes, 0.0  ),
    m_nomTemperature ( Rich::NRadiatorTypes, 0.0  ),
    m_nEvt           ( 0 )
{
  // JOs
  declareProperty( "PresVariationAmplitude", m_presVaryAmp    = 200   ); // in Pa (2 mbar)
  declareProperty( "PresVariationPeriod",    m_presVaryPeriod = 100   ); // # events
  declareProperty( "PresConstantShift",      m_presVaryShift  = 0     ); // in Pa
  declareProperty( "PresTolerance",          m_presTol        = 50    ); // in Pa (0.5 mbar)
  declareProperty( "TempVariationAmplitude", m_tempVaryAmp    = 1     ); // in K
  declareProperty( "TempVariationPeriod",    m_tempVaryPeriod = 100   ); // # of events
  declareProperty( "TempConstantShift",      m_tempVaryShift  = 0     ); // in K
  declareProperty( "TempTolerance",          m_tempTol        = 0.1   ); // in K
}

//=============================================================================
// Destructor
//=============================================================================
VaryGasPressureTemperatureAlg::~VaryGasPressureTemperatureAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VaryGasPressureTemperatureAlg::initialize()
{
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get the radiators
  m_radiators[Rich::Aerogel]  = NULL; // not used here
  m_radiators[Rich::Rich1Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich1Gas );
  m_radiators[Rich::Rich2Gas] = getDet<DeRichRadiator>( DeRichLocations::Rich2Gas );

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VaryGasPressureTemperatureAlg::execute()
{
  ++m_nEvt;
  return ( updateRadiator(Rich::Rich1Gas) && 
           updateRadiator(Rich::Rich2Gas) );
}

StatusCode
VaryGasPressureTemperatureAlg::updateRadiator( const Rich::RadiatorType rad )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Get the conditions
  Condition * pressureCond    = m_radiators[rad]->condition("GasPressure");
  Condition * temperatureCond = m_radiators[rad]->condition("GasTemperature");
  if ( !pressureCond || !temperatureCond )
  {
    sc = StatusCode::FAILURE;
  }
  else
  {

    const double currentPres = pressureCond->param<double>("CurrentPressure");
    const double currentTemp = temperatureCond->param<double>("CurrentTemperature");
    debug() << rad
            << " current temperature = " << currentTemp
            << " K, pressure = " << currentPres/Gaudi::Units::Pa << " Pa"
            << endmsg;

    // first event
    if ( m_firstEvent[rad] )
    {
      m_firstEvent[rad] = false;
      // get the nominal temps and pressures
      m_nomTemperature[rad] = currentTemp;
      m_nomPressure[rad]    = currentPres;
      info() << rad 
             << " nominal temperature = " << m_nomTemperature[rad]
             << " K, pressure = " << m_nomPressure[rad]/Gaudi::Units::Pa << " Pa"
             << endmsg;
    }

    // Vary the pressure
    const double newPres = (m_presVaryShift*Gaudi::Units::Pa) + m_nomPressure[rad] + 
      ( Gaudi::Units::Pa * m_presVaryAmp * std::sin(M_PI*2.0*m_nEvt/m_presVaryPeriod) );
    if ( fabs(newPres-currentPres) > m_presTol )
    {
      Condition newPressureCond;
      newPressureCond.addParam<double>("CurrentPressure",newPres);
      pressureCond->update(newPressureCond);
      updMgrSvc()->invalidate(pressureCond);
      sc = sc && updMgrSvc()->update(pressureCond);
    }

    // Vary the temperature
    const double newTemp = m_tempVaryShift + m_nomTemperature[rad] + 
      ( m_tempVaryAmp * std::sin(M_PI*2.0*m_nEvt/m_tempVaryPeriod) );
    if ( fabs(newTemp-currentTemp) > m_tempTol )
    {
      Condition newTempCond;
      newTempCond.addParam<double>("CurrentTemperature",newTemp);
      temperatureCond->update(newTempCond);
      updMgrSvc()->invalidate(temperatureCond);
      sc = sc && updMgrSvc()->update(temperatureCond);
    }

  }

  return sc;
}

//=============================================================================
