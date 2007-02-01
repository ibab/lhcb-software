
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.14 2007-02-01 17:51:10 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRefractiveIndex.h"

// From Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, TabulatedRefractiveIndex );

// Standard constructor
Rich::TabulatedRefractiveIndex::
TabulatedRefractiveIndex ( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_riches     ( Rich::NRiches ),
    m_deRads     ( Rich::NRadiatorTypes    ),
    m_refI       ( Rich::NRadiatorTypes, 0 ),
    m_refRMS     ( Rich::NRadiatorTypes, 0 )
{
  // interface
  declareInterface<IRefractiveIndex>(this);
}

StatusCode Rich::TabulatedRefractiveIndex::initialize()
{
  // Initialise base class
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get tools
  acquireTool( "RichDetParameters", m_detParams );

  // Get the RICH radiators
  // for aero, get the "0" file, as the multisolid does not have any properties
  // this whole scheme needs reworking to cope properly with different aerogel tiles.
  m_deRads[Rich::Aerogel] = getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel+"T0:0" );
  m_deRads[Rich::Rich1Gas]   = getDet<DeRichRadiator>( DeRichRadiatorLocation::Rich1Gas   );
  m_deRads[Rich::Rich2Gas]     = getDet<DeRichRadiator>( DeRichRadiatorLocation::Rich2Gas     );

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocation::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocation::Rich2 );

  // Register dependencies on DeRichRadiator objects to UMS

  // Rich1Gas
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::Rich1Gas,
                                  &TabulatedRefractiveIndex::updateRich1GasRefIndex );
  // Rich2Gas
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::Rich2Gas,
                                  &TabulatedRefractiveIndex::updateRich2GasRefIndex );
  // aerogel
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::Aerogel,
                                  &TabulatedRefractiveIndex::updateAerogelRefIndex );

  // force first updates
  sc = updMgrSvc()->update(this);
  if (sc.isFailure()) return Error ( "Failed first UMS update", sc );

  return sc;
}

StatusCode Rich::TabulatedRefractiveIndex::updateAerogelRefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::Aerogel);
  m_refRMS[Rich::Aerogel] = 0.488e-3; // temp fixup
  return sc;
}

StatusCode Rich::TabulatedRefractiveIndex::updateRich1GasRefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::Rich1Gas);
  m_refRMS[Rich::Rich1Gas] = 0.393e-4; // temp fixup
  return sc;
}

StatusCode Rich::TabulatedRefractiveIndex::updateRich2GasRefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::Rich2Gas);
  m_refRMS[Rich::Rich2Gas] = 0.123e-4; // temp fixup
  return sc;
}

StatusCode
Rich::TabulatedRefractiveIndex::updateRefIndex( const Rich::RadiatorType rad )
{

  if ( !m_deRads[rad]->refIndex() )
  {
    error() << "Null refractive index for " << rad << endreq;
    return StatusCode::FAILURE;
  }

  // average refractive index
  m_refI[rad] = refractiveIndex( rad,
                                 m_detParams->meanPhotonEnergy(rad) );

  // RMS values
  m_refRMS[rad] = m_deRads[rad]->refIndex()->rms(m_deRads[rad]->refIndex()->minX(),
                                                 m_deRads[rad]->refIndex()->maxX(),100);

  // printout
  info() << "Updated " << rad << " refractive index '"
         << m_deRads[rad]->refIndex()->tabProperty()->name() << "' with "
         << m_deRads[rad]->refIndex()->nDataPoints() << " data points" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode Rich::TabulatedRefractiveIndex::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

double Rich::TabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                        const double energy ) const
{
  return (*(m_deRads[rad]->refIndex()))[energy*Gaudi::Units::eV];
}

double Rich::TabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                        const double energyBot,
                                                        const double energyTop ) const
{
  const Rich::DetectorType rich = ( rad == Rich::Rich2Gas ? Rich::Rich2 : Rich::Rich1 );
  return refractiveIndex( rad,
                          m_riches[rich]->nominalHPDQuantumEff()->meanX(energyBot,energyTop) /
                          Gaudi::Units::eV );
}

double Rich::TabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad ) const
{
  return m_refI[rad];
}

double Rich::TabulatedRefractiveIndex::refractiveIndexRMS ( const Rich::RadiatorType rad ) const
{
  return m_refRMS[rad];
}
