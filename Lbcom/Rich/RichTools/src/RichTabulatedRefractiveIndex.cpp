
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.12 2006-05-05 11:11:00 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRefractiveIndex.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRefractiveIndex>          s_factory ;
const        IToolFactory& RichTabulatedRefractiveIndexFactory = s_factory ;

// Standard constructor
RichTabulatedRefractiveIndex::RichTabulatedRefractiveIndex ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichToolBase ( type, name, parent ),
    m_riches     ( Rich::NRiches ),
    m_deRads     ( Rich::NRadiatorTypes    ),
    m_refI       ( Rich::NRadiatorTypes, 0 ),
    m_refRMS     ( Rich::NRadiatorTypes, 0 )
{
  // interface
  declareInterface<IRichRefractiveIndex>(this);
}

StatusCode RichTabulatedRefractiveIndex::initialize()
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
                                  &RichTabulatedRefractiveIndex::updateRich1GasRefIndex );
  // Rich2Gas
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::Rich2Gas,
                                  &RichTabulatedRefractiveIndex::updateRich2GasRefIndex );
  // aerogel
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::Aerogel,
                                  &RichTabulatedRefractiveIndex::updateAerogelRefIndex );

  // force first updates
  sc = updMgrSvc()->update(this);
  if (sc.isFailure()) return Error ( "Failed first UMS update", sc );

  return sc;
}

StatusCode RichTabulatedRefractiveIndex::updateAerogelRefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::Aerogel);
  m_refRMS[Rich::Aerogel] = 0.488e-3; // temp fixup
  return sc;
}

StatusCode RichTabulatedRefractiveIndex::updateRich1GasRefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::Rich1Gas);
  m_refRMS[Rich::Rich1Gas] = 0.393e-4; // temp fixup
  return sc;
}

StatusCode RichTabulatedRefractiveIndex::updateRich2GasRefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::Rich2Gas);
  m_refRMS[Rich::Rich2Gas] = 0.123e-4; // temp fixup
  return sc;
}

StatusCode
RichTabulatedRefractiveIndex::updateRefIndex( const Rich::RadiatorType rad )
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

StatusCode RichTabulatedRefractiveIndex::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energy ) const
{
  return (*(m_deRads[rad]->refIndex()))[energy*eV];
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energyBot,
                                                      const double energyTop ) const
{
  const Rich::DetectorType rich = ( rad == Rich::Rich2Gas ? Rich::Rich2 : Rich::Rich1 );
  return refractiveIndex( rad, 
                          m_riches[rich]->nominalHPDQuantumEff()->meanX(energyBot,energyTop)/eV );
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad ) const
{
  return m_refI[rad];
}

double RichTabulatedRefractiveIndex::refractiveIndexRMS ( const Rich::RadiatorType rad ) const
{
  return m_refRMS[rad];
}
