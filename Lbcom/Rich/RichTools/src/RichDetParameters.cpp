
//-----------------------------------------------------------------------------
/** @file RichDetParameters.cpp
 *
 * Implementation file for class : RichDetParameters
 *
 * CVS Log :-
 * $Id: RichDetParameters.cpp,v 1.7 2006-12-01 13:13:13 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichDetParameters.h"

DECLARE_TOOL_FACTORY( RichDetParameters );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichDetParameters::RichDetParameters( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichToolBase ( type, name , parent ),
    m_maxPhotEn  ( Rich::NRadiatorTypes ),
    m_minPhotEn  ( Rich::NRadiatorTypes ),
    m_meanPhotEn ( Rich::NRadiatorTypes )
{

  declareInterface<IRichDetParameters>(this);

  m_maxPhotEn[Rich::Aerogel] = 3.8;
  m_maxPhotEn[Rich::Rich1Gas]   = 7.0;
  m_maxPhotEn[Rich::Rich2Gas]     = 7.0;
  declareProperty( "MaxPhotonEnergy", m_maxPhotEn );

  m_minPhotEn[Rich::Aerogel] = 1.75;
  m_minPhotEn[Rich::Rich1Gas]   = 1.75;
  m_minPhotEn[Rich::Rich2Gas]     = 1.75;
  declareProperty( "MinPhotonEnergy", m_minPhotEn );

  m_meanPhotEn[Rich::Aerogel] = 2.89;
  m_meanPhotEn[Rich::Rich1Gas]   = 4.29;
  m_meanPhotEn[Rich::Rich2Gas]     = 4.34;
  declareProperty( "MeanPhotonEnergy", m_meanPhotEn );

}

StatusCode RichDetParameters::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Initialise the data. Eventually, some of this should code from XML or D-Base
  
  // Out radiator limits for a single HPD panel (unsigned)
  m_radOutLimLoc[Rich::Aerogel] = RadLimits ( 0,   625,  0,   600 );
  m_radOutLimLoc[Rich::Rich1Gas]   = RadLimits ( 0,   625,  180, 600 );
  m_radOutLimLoc[Rich::Rich2Gas]     = RadLimits ( 50,  690,  0,   696 );

  return sc;
}

StatusCode RichDetParameters::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

double RichDetParameters::maxPhotonEnergy( const Rich::RadiatorType rad ) const
{
  return m_maxPhotEn[rad];
}

double RichDetParameters::minPhotonEnergy( const Rich::RadiatorType rad ) const
{
  return m_minPhotEn[rad];
}

double RichDetParameters::meanPhotonEnergy( const Rich::RadiatorType rad ) const
{
  return m_meanPhotEn[rad];
}

const IRichDetParameters::RadLimits & 
RichDetParameters::AvAcceptOuterLimitsLocal( const Rich::RadiatorType rad ) const
{
  return m_radOutLimLoc[rad];
}
