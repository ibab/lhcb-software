
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.11 2006-03-02 15:29:52 jonrob Exp $
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
  m_deRads[Rich::C4F10]   = getDet<DeRichRadiator>( DeRichRadiatorLocation::C4F10   );
  m_deRads[Rich::CF4]     = getDet<DeRichRadiator>( DeRichRadiatorLocation::CF4     );

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocation::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocation::Rich2 );

  // Register dependencies on DeRichRadiator objects to UMS

  // C4F10
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::C4F10,
                                  &RichTabulatedRefractiveIndex::updateC4F10RefIndex );
  // CF4
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::CF4,
                                  &RichTabulatedRefractiveIndex::updateCF4RefIndex );
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

StatusCode RichTabulatedRefractiveIndex::updateC4F10RefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::C4F10);
  m_refRMS[Rich::C4F10] = 0.393e-4; // temp fixup
  return sc;
}

StatusCode RichTabulatedRefractiveIndex::updateCF4RefIndex()
{
  const StatusCode sc = updateRefIndex(Rich::CF4);
  m_refRMS[Rich::CF4] = 0.123e-4; // temp fixup
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
  const Rich::DetectorType rich = ( rad == Rich::CF4 ? Rich::Rich2 : Rich::Rich1 );
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
