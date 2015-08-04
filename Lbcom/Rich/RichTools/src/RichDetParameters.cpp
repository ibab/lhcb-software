
//-----------------------------------------------------------------------------
/** @file RichDetParameters.cpp
 *
 * Implementation file for class : Rich::DetParameters
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichDetParameters.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, DetParameters )

// General Rich namespace
namespace Rich
{

  //=============================================================================
  // Standard constructor, initializes variables
  //=============================================================================
  DetParameters::DetParameters( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : Rich::ToolBase ( type, name , parent  ),
      m_maxPhotEn    ( Rich::NRadiatorTypes ),
      m_minPhotEn    ( Rich::NRadiatorTypes ),
      m_meanPhotEn   ( Rich::NRadiatorTypes ),
      m_refSD        ( Rich::NRadiatorTypes ),
      m_radOutLimLoc ( Rich::NRadiatorTypes )
  {
    declareInterface<IDetParameters>(this);

    m_maxPhotEn[Rich::Aerogel]  = 4.0;
    m_maxPhotEn[Rich::Rich1Gas] = 7.0;
    m_maxPhotEn[Rich::Rich2Gas] = 7.0;
    declareProperty( "MaxPhotonEnergy", m_maxPhotEn );

    m_minPhotEn[Rich::Aerogel]  = 1.75;
    m_minPhotEn[Rich::Rich1Gas] = 1.75;
    m_minPhotEn[Rich::Rich2Gas] = 1.75;
    declareProperty( "MinPhotonEnergy", m_minPhotEn );

    m_meanPhotEn[Rich::Aerogel]  = 2.89;
    m_meanPhotEn[Rich::Rich1Gas] = 4.29;
    m_meanPhotEn[Rich::Rich2Gas] = 4.34;
    declareProperty( "MeanPhotonEnergy", m_meanPhotEn );

    m_refSD[Rich::Aerogel]  = 0.488e-3;
    m_refSD[Rich::Rich1Gas] = 0.393e-4;
    m_refSD[Rich::Rich2Gas] = 0.123e-4;
    declareProperty( "RefIndexSD", m_refSD );
  }

  StatusCode DetParameters::initialize()
  {
    // Initialise base class
    const StatusCode sc = Rich::ToolBase::initialize();
    if ( sc.isFailure() ) return sc;

    // Initialise the data. Eventually, some of this should come from a DataBase

    // Out radiator limits for a single HPD panel (unsigned)
    m_radOutLimLoc[Rich::Aerogel]  = RadLimits ( 0,   625,  0,   600 );
    m_radOutLimLoc[Rich::Rich1Gas] = RadLimits ( 0,   625,  180, 600 );
    m_radOutLimLoc[Rich::Rich2Gas] = RadLimits ( 50,  690,  0,   696 );

    return sc;
  }

  double DetParameters::maxPhotonEnergy( const Rich::RadiatorType rad ) const
  {
    return m_maxPhotEn[rad];
  }

  double DetParameters::minPhotonEnergy( const Rich::RadiatorType rad ) const
  {
    return m_minPhotEn[rad];
  }

  double DetParameters::meanPhotonEnergy( const Rich::RadiatorType rad ) const
  {
    return m_meanPhotEn[rad];
  }

  const IDetParameters::RadLimits &
  Rich::DetParameters::AvAcceptOuterLimitsLocal( const Rich::RadiatorType rad ) const
  {
    return m_radOutLimLoc[rad];
  }

  double DetParameters::refIndexSD( const Rich::RadiatorType rad ) const
  {
    return m_refSD[rad];
  }

}
