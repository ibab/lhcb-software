
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.cpp
 *
 *  Implementation file for tool : RichTabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.cpp,v 1.7 2005-06-23 15:17:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRayleighScatter.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRayleighScatter>          s_factory ;
const        IToolFactory& RichTabulatedRayleighScatterFactory = s_factory ;

// Standard constructor
RichTabulatedRayleighScatter::RichTabulatedRayleighScatter ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_rayScatL( 0 ) 
{

  declareInterface<IRichRayleighScatter>(this);

}

StatusCode RichTabulatedRayleighScatter::initialize() 
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get aerogel radiator
  const DeRichRadiator * aero = getDet<DeRichRadiator>( DeRichRadiatorLocation::Aerogel );

  // Get the rayleigh scattering length
  m_rayScatL = new Rich1DTabProperty( aero->rayleigh() );

  return sc;
}

StatusCode RichTabulatedRayleighScatter::finalize() 
{
  // clean up
  if ( m_rayScatL ) { delete m_rayScatL; m_rayScatL = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedRayleighScatter::photonScatteredProb( const RichRecSegment * segment,
                                                   const double energy ) const 
{

  // check this is aerogel
  if ( Rich::Aerogel != segment->trackSegment().radiator() ) return 0;

  // check energy is valid
  if ( energy <= 0 ) return 0;

  // check path length is valid
  const double path = segment->trackSegment().pathLength();
  if ( path <= 0 ) return 0;

  // compute and return prob
  const double scatLeng = (*m_rayScatL)[energy*eV];
  return ( 1 - (scatLeng/path)*(1.0-exp(-path/scatLeng)) );
}
