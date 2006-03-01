
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.10 2006-03-01 17:12:25 papanest Exp $
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
    m_deRads     ( Rich::NRadiatorTypes    ),
    m_refI       ( Rich::NRadiatorTypes, 0 ),
    m_refRMS     ( Rich::NRadiatorTypes, 0 )
{
  // interface
  declareInterface<IRichRefractiveIndex>(this);
  // initialize
  m_refIndex[Rich::Aerogel] = 0;
  m_refIndex[Rich::C4F10]   = 0;
  m_refIndex[Rich::CF4]     = 0;
}

StatusCode RichTabulatedRefractiveIndex::initialize()
{
  // Initialise base class
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Get tools
  acquireTool( "RichDetParameters", m_detParams );

  // Get the RICH radiators
  m_deRads[Rich::Aerogel] = getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel );
  m_deRads[Rich::C4F10]   = getDet<DeRichRadiator>( DeRichRadiatorLocation::C4F10   );
  m_deRads[Rich::CF4]     = getDet<DeRichRadiator>( DeRichRadiatorLocation::CF4     );

  // Get Rich1
  const DeRich * rich1 = getDet<DeRich>( DeRichLocation::Rich1 );

  // Nominal HPD QE
  m_QE = new Rich1DTabProperty( rich1->nominalHPDQuantumEff() );

  // Register dependencies on DeRichRadiator objects to UMS
  // aerogel
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::Aerogel,
                                  &RichTabulatedRefractiveIndex::updateAerogelRefIndex );
  // C4F10
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::C4F10,
                                  &RichTabulatedRefractiveIndex::updateC4F10RefIndex );
  // CF4
  updMgrSvc()->registerCondition( this,
                                  DeRichRadiatorLocation::CF4,
                                  &RichTabulatedRefractiveIndex::updateCF4RefIndex );
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
  // create new interpolation object
  if ( m_refIndex[rad] ) { delete m_refIndex[rad]; }
  m_refIndex[rad] = m_deRads[rad]->refIndex();

  // average refractive index
  m_refI[rad] = refractiveIndex( rad,
                                 m_detParams->meanPhotonEnergy(rad) );

  // RMS values
  m_refRMS[rad] = m_refIndex[rad]->rms(m_refIndex[rad]->minX(),
                                       m_refIndex[rad]->maxX(),100);

  // printout
  info() << "Updated " << rad << " refractive index '"
         << m_deRads[rad]->refIndex()->tabProperty()->name() << "' with "
         << m_refIndex[rad]->nDataPoints() << " data points" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedRefractiveIndex::finalize()
{
  // Tidy up
  for ( RefractiveIndices::iterator iRef = m_refIndex.begin();
        iRef != m_refIndex.end(); ++iRef )
  {
    if ( *iRef ) { delete *iRef; *iRef = 0; }
  }
  if ( m_QE ) { delete m_QE; m_QE = 0; }

  // base class finalize
  return RichToolBase::finalize();
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energy ) const
{
  return (*m_refIndex[rad])[energy*eV];
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energyBot,
                                                      const double energyTop ) const
{
  return refractiveIndex( rad, m_QE->meanX(energyBot,energyTop)/eV );
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad ) const
{
  return m_refI[rad];
}

double RichTabulatedRefractiveIndex::refractiveIndexRMS ( const Rich::RadiatorType rad ) const
{
  return m_refRMS[rad];
}
