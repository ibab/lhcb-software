
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.cpp
 *
 *  Implementation file for tool : RichTabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.cpp,v 1.10 2006-03-17 15:54:46 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRayleighScatter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRayleighScatter>          s_factory ;
const        IToolFactory& RichTabulatedRayleighScatterFactory = s_factory ;

// Standard constructor
RichTabulatedRayleighScatter::RichTabulatedRayleighScatter ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichRecToolBase( type, name, parent )
{
  // interface
  declareInterface<IRichRayleighScatter>(this);
}

StatusCode RichTabulatedRayleighScatter::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get aerogel radiator
  // for aero, get the "0" file, as the multisolid does not have any properties
  // this whole scheme needs reworking to cope properly with different aerogel tiles.
  m_aero = getDet<DeRichAerogelRadiator>( DeRichRadiatorLocation::Aerogel+"T0:0" );

  return sc;
}

StatusCode RichTabulatedRayleighScatter::finalize()
{
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
  const double scatLeng = (*(m_aero->rayleigh()))[energy*eV];
  return ( 1 - (scatLeng/path)*(1.0-exp(-path/scatLeng)) );
}
