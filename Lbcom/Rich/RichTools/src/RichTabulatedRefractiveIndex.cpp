
//-----------------------------------------------------------------------------
/** @file RichTabulatedRefractiveIndex.cpp
 *
 * Implementation file for class : RichTabulatedRefractiveIndex
 *
 * CVS Log :-
 * $Id: RichTabulatedRefractiveIndex.cpp,v 1.4 2004-10-27 14:41:03 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/07/26 18:03:05  jonrob
 * Various improvements to the doxygen comments
 *
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
  : RichToolBase ( type, name, parent )
{

  declareInterface<IRichRefractiveIndex>(this);

}

StatusCode RichTabulatedRefractiveIndex::initialize() 
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;
 
  // Get tools
  acquireTool( "RichDetParameters", m_detParams );

  // Get the RICH radiators
  const DeRichRadiator * aero  = getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel );
  const DeRichRadiator * c4f10 = getDet<DeRichRadiator>( DeRichRadiatorLocation::C4F10   );
  const DeRichRadiator * cf4   = getDet<DeRichRadiator>( DeRichRadiatorLocation::CF4     );

  // Create tabulated property objects from XML information
  m_refIndex[Rich::Aerogel]  = new Rich1DTabProperty( aero->refIndex()  );
  m_refIndex[Rich::Rich1Gas] = new Rich1DTabProperty( c4f10->refIndex() );
  m_refIndex[Rich::Rich2Gas] = new Rich1DTabProperty( cf4->refIndex()   );

  // Get Rich1
  const DeRich * rich1 = getDet<DeRich>( DeRichLocation::Rich1 );

  // Nominal HPD QE 
  m_QE = new Rich1DTabProperty( rich1->nominalHPDQuantumEff() );

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedRefractiveIndex::finalize()
{
  // Tidy up
  for ( RefractiveIndices::iterator iRef = m_refIndex.begin();
        iRef != m_refIndex.end(); ++iRef ) {
    if ( *iRef ) { delete *iRef; *iRef = 0; }
  }
  if ( m_QE ) { delete m_QE; m_QE = 0; }

  // base class finalize
  return RichToolBase::finalize();
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energy )
{
  return (*m_refIndex[rad])[energy*eV];
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad,
                                                      const double energyBot,
                                                      const double energyTop )
{
  return refractiveIndex( rad, m_QE->meanX(energyBot,energyTop)/eV );
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType rad )
{
  return refractiveIndex( rad, m_detParams->meanPhotonEnergy(rad) );
}

